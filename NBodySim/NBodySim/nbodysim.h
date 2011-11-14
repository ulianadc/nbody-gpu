//
//  nbodysim.h
//  NBodySim
//

#ifndef _NBODYSIM_H_
#define _NBODYSIM_H_

#include <OpenCL/opencl.h>

// Represents a single particle in the system
struct Body {
    
    // Position
    float posx;
    float posy;
    float posz;
    
    // Velocity
    float velx;
    float vely;
    float velz;
    
    // Mass
    float mass;
};

// Main simulation class
class NBodySim {
    
public:
    
    // Constructors
    NBodySim(const char *filePath);
    
    // Destructor
    ~NBodySim();
    
    // Load system data
    void loadData(const char *filePath);
    
    // Start simulation
    void run(int iterations);
    
    // Clear all system data
    void clearData();
    
    // Print host state
    void printHostState();
    
private:
    
    // Load and build the kernel
    void buildKernel(const char *filePath);
    
    // Print useful info and exit if err indicates failure
    void checkError(cl_int err, const char *function);
    
    // Returns a string representation of the cl_int error
    const char *clErrorToString(cl_int error);
    
    // Important
    cl_platform_id mPlatform;
    cl_device_id mDeviceID;
    cl_context mContext;
    cl_command_queue mCommands;
    cl_program mProgram;
    cl_kernel mKernel;
    
    // Other device info
    size_t mMaxWorkGroupSize;
    
    // Device memory 
    cl_mem mDevStateA;
    cl_mem mDevStateB;
    
    // Host memory
    Body *mHostState;
    
    // Data info
    bool mDataLoaded;
    size_t mNumBodies;
    size_t mStateMemSize;
    size_t mWorkGroupSize;
    
    // Sim state
    unsigned int mSimIt;
    
    // Debuging variables
    cl_int mErr;
    cl_event mEvent;
    
};

#endif
