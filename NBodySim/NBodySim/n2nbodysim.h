//
//  n2nbodysim.h
//  NBodySim
//

#ifndef NBodySim_n2nbodysim_h
#define NBodySim_n2nbodysim_h

#ifdef __APPLE__
    #include <OpenCL/opencl.h>
#else
    #include <CL/cl.h>
#endif

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

// N^2 N-Body Simulation
class N2NBodySim {
    
public:
    
    // Constructors
    N2NBodySim(const char *filePath);
    
    // Destructor
    ~N2NBodySim();
    
    // Load system data
    void loadData(const char *filePath);
    
    // Start simulation
    void run(unsigned int iterations, unsigned int iterationsPerWriteBack = 1);
    
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
    unsigned long mMaxWorkGroupSize;
    
    // Device memory 
    cl_mem mDevStateA;
    cl_mem mDevStateB;
    
    // Host memory
    Body *mHostState;
    
    // Data info
    bool mDataLoaded;
    unsigned long mNumBodies;
    unsigned long mStateMemSize;
    unsigned long mWorkGroupSize;
    
    // Sim state
    unsigned int mSimIt;
    
    // Debuging variables
    cl_int mErr;
    cl_event mEvent;
    
};

#endif
