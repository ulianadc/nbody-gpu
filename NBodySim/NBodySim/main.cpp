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
    for (int i = 4096; i <= 4*4096; i += 4096) {
        sprintf(sysFileName, "%s/system%d.dat", SYSTEM_ROOT, i);
        sim.loadData(sysFileName);
        sim.run(1);
        sim.clearData();
    }
    
    // Load another system and simulate
//    sim.loadData(SYSTEM_ROOT "/system1.dat");
//    sim.run(1000);
//    sim.clearData();
    
    return 0;
}
