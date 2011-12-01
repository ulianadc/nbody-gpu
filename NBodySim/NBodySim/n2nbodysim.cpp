//
//  n2nbodysim.cpp
//  NBodySim
//

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <time.h>
#include <vector>
#include "n2nbodysim.h"

N2NBodySim::N2NBodySim(const char *kernelPath)
    : mDataLoaded(false)
{
//    printf("DBG: Creating simulation object.\n");
    
    mErr = clGetPlatformIDs(1, &mPlatform, NULL);
    checkError(mErr, "clGetPlatformIDs");
               
    mErr = clGetDeviceIDs(mPlatform, CL_DEVICE_TYPE_GPU, 1, &mDeviceID, NULL);
    checkError(mErr, "clGetDeviceIDs");
    
    mContext = clCreateContext(0, 1, &mDeviceID, NULL, NULL, &mErr);
    checkError(mErr, "clCreateContext");
    
    mCommands = clCreateCommandQueue(mContext, mDeviceID, 0, &mErr);
    checkError(mErr, "clCreateCommandQueue");
    
    buildKernel(kernelPath);
}

N2NBodySim::~N2NBodySim()
{
//    printf("DBG: Destroying simulation object.\n");
    
    mErr = clFlush(mCommands);
    checkError(mErr, "clFlush");
    
    mErr = clFinish(mCommands);
    checkError(mErr, "clFinish");
    
    mErr = clReleaseKernel(mKernel);
    checkError(mErr, "clReleaseKernel");
    
    mErr = clReleaseProgram(mProgram);
    checkError(mErr, "clReleaseProgram");
    
    mErr = clReleaseCommandQueue(mCommands);
    checkError(mErr, "clReleaseCommandQueue");
    
    mErr = clReleaseContext(mContext);
    checkError(mErr, "clReleaseContext");
}

void N2NBodySim::loadData(const char *filePath)
{
    using namespace std;
    
    // Check for existing data
    if (mDataLoaded) {
        printf("E: Called loadData() on a data-populated simulation. First call clearData().\n");
        exit(EXIT_FAILURE);
    }
    
    // Flag indicates current data state
    mDataLoaded = true;
    
    // Open system file
    ifstream fh;
    fh.open(filePath, ios::in | ios::binary);
    if (!fh.is_open()) {
        printf("E: Unable to open %s (read-only)\n", filePath);
        exit(EXIT_FAILURE);
    }
    
    // Get file size and calc particle count
    fh.seekg(0, ios::end);
    mNumBodies = (int)(fh.tellg() / 4 / 7);
    fh.seekg(0, ios::beg);
    
    // Setup host state
    mHostState = new Body[mNumBodies];
    
    // Load particles
    for (int i = 0; i < mNumBodies; i++) {
        
        // Read in position
        fh.read((char *) &mHostState[i].posx, sizeof(float));
        fh.read((char *) &mHostState[i].posy, sizeof(float));
        fh.read((char *) &mHostState[i].posz, sizeof(float));
        
        // Read in velocity
        fh.read((char *) &mHostState[i].velx, sizeof(float));
        fh.read((char *) &mHostState[i].vely, sizeof(float));
        fh.read((char *) &mHostState[i].velz, sizeof(float));
        
        // Read in mass
        fh.read((char *) &mHostState[i].mass, sizeof(float));
        
    }
    fh.close();
    
    // Debug
    printf("Read in %lu particles from %s:\n", mNumBodies, filePath);
//    printHostState();
    
    // Set work group size
    if (mNumBodies > mMaxWorkGroupSize)
        mWorkGroupSize = mMaxWorkGroupSize;
    else
        mWorkGroupSize = mNumBodies;
    
    // Get device memory size
    mStateMemSize = mNumBodies * sizeof(Body);
    
    // Setup memory buffers
    mDevStateA = clCreateBuffer(mContext, CL_MEM_READ_WRITE, mStateMemSize, NULL, &mErr);
    checkError(mErr, "clCreateBuffer");
    mDevStateB = clCreateBuffer(mContext, CL_MEM_READ_WRITE, mStateMemSize, NULL, &mErr);
    checkError(mErr, "clCreateBuffer");
    
    // Write init state to device mem state A
    mErr = clEnqueueWriteBuffer(mCommands, mDevStateA, CL_TRUE, 0, mStateMemSize, mHostState, 0,
                                NULL, NULL);
    checkError(mErr, "clEnqueueWriteBuffer");
    // Also write to state B to set masses
    mErr = clEnqueueWriteBuffer(mCommands, mDevStateB, CL_TRUE, 0, mStateMemSize, mHostState, 0,
                                NULL, NULL);
    checkError(mErr, "clEnqueueWriteBuffer");
    
    // Set third kernel arg to system size
    mErr = clSetKernelArg(mKernel, 2, sizeof(size_t), &mNumBodies);
    checkError(mErr, "clSetKernelArg");
    
    // Initialize simulation iteration counter
    mSimIt = 0;
}

void N2NBodySim::run(int iterations)
{
    printf("Beginning simulation run (%d iteration(s)).\n", iterations);
    
    // State pointers
    cl_mem *curState, *nextState;
    
    // Timing
    clock_t startTime, endTime;
    startTime = clock();
    
    for (int i = 0; i < iterations; i++) {
        
        // Toggle between two copies of system within device mem
        if (mSimIt++ % 2 == 0) {
            curState = &mDevStateA;
            nextState = &mDevStateB;
        } else {
            curState = &mDevStateB;
            nextState = &mDevStateA;
        }
        
        // Set state args
        mErr = clSetKernelArg(mKernel, 0, sizeof(cl_mem), curState);
        checkError(mErr, "clSetKernelArg");
        mErr = clSetKernelArg(mKernel, 1, sizeof(cl_mem), nextState);
        checkError(mErr, "clSetKernelArg");
        
        // Run the darn thing...
        mErr = clEnqueueNDRangeKernel(mCommands, mKernel, 1, NULL, &mNumBodies, &mWorkGroupSize,
                                      0, NULL, NULL);
        checkError(mErr, "clEnqueueNDRangeKernel");
        
        // Wait for the kernel to finish
        clFinish(mCommands);
        
        // Read back next state results
        mErr = clEnqueueReadBuffer(mCommands, *nextState, CL_TRUE, 0, mStateMemSize, mHostState, 0,
                                   NULL, NULL);
        checkError(mErr, "clEnqueueReadBuffer");
        
    }
    
    endTime = clock();
    float timeElapsed = ((float) (endTime - startTime)) / CLOCKS_PER_SEC;
    
    printf("Simulation finished in %g seconds.\n", timeElapsed);
}

void N2NBodySim::clearData()
{
//    printf("DBG: Clearing stored system data.\n");
    
    mDataLoaded = false;
    
    // De-allocate device mem
    mErr = clReleaseMemObject(mDevStateA);
    checkError(mErr, "clReleaseMemObject");
    mErr = clReleaseMemObject(mDevStateB);
    checkError(mErr, "clReleaseMemObject");
    
    // De-allocate host mem
    delete mHostState;
    mNumBodies = 0;
}

void N2NBodySim::printHostState()
{
    for (int i = 0; i < mNumBodies; i++) {
        printf("   [%5d] = { (%g,%g,%g), (%g,%g,%g), %g }\n", i,
               mHostState[i].posx, mHostState[i].posy, mHostState[i].posz,
               mHostState[i].velx, mHostState[i].vely, mHostState[i].velz,
               mHostState[i].mass);
    }
}

void N2NBodySim::buildKernel(const char *filePath)
{
//    printf("DBG: Loading and building device kernel.\n");
    
    // Open kernel source
    FILE *fh = fopen(filePath, "r");
    if (!fh) {
        printf("E: Unable to open %s (read-only)\n", filePath);
        exit(EXIT_FAILURE);
    }
    fseek(fh, 0, SEEK_END);
    long fileSize = ftell(fh);
    fseek(fh, 0, SEEK_SET);
    char *buf = new char[fileSize+1];
    
    // Read it in
    size_t tmp = fread(buf, 1, fileSize, fh);
    
    // Check for read error
    if (tmp != fileSize)
    {
        printf("E: Failure to completely read %s\n", filePath);
        exit(EXIT_FAILURE);
    }
    
    // Close the file
    fclose(fh);
    
    // Terminate string
    buf[fileSize] = '\0';
    
    // Create program
    mProgram = clCreateProgramWithSource(mContext, 1, (const char **) &buf,
                                         (const unsigned long *) &fileSize, &mErr);
    checkError(mErr, "clCreateProgramWithSource");
    
    // De-allocate memory for file contents
    delete [] buf;
    
    mErr = clBuildProgram(mProgram, 0, NULL, NULL, NULL, NULL);
    if (mErr != CL_SUCCESS)
    {
        size_t len;
        char buf[2048];
        clGetProgramBuildInfo(mProgram, mDeviceID, CL_PROGRAM_BUILD_LOG, sizeof(buf), buf, &len);
        printf("E: %s\n", buf);
    }
    checkError(mErr, "clBuildProgram");
    
    // Actually create the kernel from the program
    mKernel = clCreateKernel(mProgram, "nbody", &mErr);
    checkError(mErr, "clCreateKernel");
    
    // Get max work group size for the device
    mErr = clGetKernelWorkGroupInfo(mKernel, mDeviceID, CL_KERNEL_WORK_GROUP_SIZE,
                                    sizeof(mMaxWorkGroupSize), &mMaxWorkGroupSize, NULL);
    checkError(mErr, "clGetKernelWorkGroupInfo");
//    printf("DBG: mMaxWorkGroupSize = %lu\n", mMaxWorkGroupSize);
}

void N2NBodySim::checkError(cl_int err, const char  *functionName)
{
    // If function was not successful, print the error
    if (err != CL_SUCCESS) {
        printf("E: %s() => [%d] %s\n", functionName, err, clErrorToString(err));
        exit(EXIT_FAILURE);
    }
}

const char *N2NBodySim::clErrorToString(cl_int error)
{
    // Array of all errors
    static const char *errorString[] = {
        "CL_SUCCESS",
        "CL_DEVICE_NOT_FOUND",
        "CL_DEVICE_NOT_AVAILABLE",
        "CL_COMPILER_NOT_AVAILABLE",
        "CL_MEM_OBJECT_ALLOCATION_FAILURE",
        "CL_OUT_OF_RESOURCES",
        "CL_OUT_OF_HOST_MEMORY",
        "CL_PROFILING_INFO_NOT_AVAILABLE",
        "CL_MEM_COPY_OVERLAP",
        "CL_IMAGE_FORMAT_MISMATCH",
        "CL_IMAGE_FORMAT_NOT_SUPPORTED",
        "CL_BUILD_PROGRAM_FAILURE",
        "CL_MAP_FAILURE",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "CL_INVALID_VALUE",
        "CL_INVALID_DEVICE_TYPE",
        "CL_INVALID_PLATFORM",
        "CL_INVALID_DEVICE",
        "CL_INVALID_CONTEXT",
        "CL_INVALID_QUEUE_PROPERTIES",
        "CL_INVALID_COMMAND_QUEUE",
        "CL_INVALID_HOST_PTR",
        "CL_INVALID_MEM_OBJECT",
        "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR",
        "CL_INVALID_IMAGE_SIZE",
        "CL_INVALID_SAMPLER",
        "CL_INVALID_BINARY",
        "CL_INVALID_BUILD_OPTIONS",
        "CL_INVALID_PROGRAM",
        "CL_INVALID_PROGRAM_EXECUTABLE",
        "CL_INVALID_KERNEL_NAME",
        "CL_INVALID_KERNEL_DEFINITION",
        "CL_INVALID_KERNEL",
        "CL_INVALID_ARG_INDEX",
        "CL_INVALID_ARG_VALUE",
        "CL_INVALID_ARG_SIZE",
        "CL_INVALID_KERNEL_ARGS",
        "CL_INVALID_WORK_DIMENSION",
        "CL_INVALID_WORK_GROUP_SIZE",
        "CL_INVALID_WORK_ITEM_SIZE",
        "CL_INVALID_GLOBAL_OFFSET",
        "CL_INVALID_EVENT_WAIT_LIST",
        "CL_INVALID_EVENT",
        "CL_INVALID_OPERATION",
        "CL_INVALID_GL_OBJECT",
        "CL_INVALID_BUFFER_SIZE",
        "CL_INVALID_MIP_LEVEL",
        "CL_INVALID_GLOBAL_WORK_SIZE",
    };
    
    // Number of errors
    const int errorCount = sizeof(errorString) / sizeof(errorString[0]);
    
    // Get index of error in string array
    const int index = -error;
    
    // Return string if index within array bounds
    if (index >= 0 && index < errorCount)
        return errorString[index];
    else
        return "";
}