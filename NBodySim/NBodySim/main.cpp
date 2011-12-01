//
//  main.cpp
//  NBodySim
//

// http://www.thebigblob.com/getting-started-with-opencl-and-gpu-computing/
// http://www.browndeertechnology.com/docs/BDT_OpenCL_Tutorial_NBody-rev3.html#algorithm

#include <stdio.h>
#include "n2nbodysim.h"

#define KERNEL_ROOT "."
#define SYSTEM_ROOT "../Systems"

int main (int argc, const char * argv[])
{
    // Create simulation object with kernel.cl
    N2NBodySim sim(KERNEL_ROOT "/kernel.cl");
    
    // Load system from file and simulate
    char sysFileName[256];
    for (int i = (1<<14); i <= 16*(1<<14); i += (1<<14)) {
        sprintf(sysFileName, "%s/system%06d.dat", SYSTEM_ROOT, i);
        sim.loadData(sysFileName);
        sim.run(10);
        sim.clearData();
    }
    
    return 0;
}
