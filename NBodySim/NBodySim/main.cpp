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
    
    // Output results in MATLAB format
    printf("%% N^2 Simulation Results\n\n");
    printf("runs = [\n");
    
    // Load system from file and simulate
    char sysFileName[256];
    for (int i = (1<<13); i <= (1<<13); i += (1<<17)) {
        sprintf(sysFileName, "%s/system%06d.dat", SYSTEM_ROOT, i);
        for (int i = 0; i <= 10; i += 2) {
            sim.loadData(sysFileName);
            sim.run(10, (i == 0 ? 1 : i));
            sim.clearData();
        }
    }
    
    printf("\t];\n");
    
    return 0;
}
