//
//  main.cpp
//  NBodySim
//

// http://www.thebigblob.com/getting-started-with-opencl-and-gpu-computing/
// http://www.browndeertechnology.com/docs/BDT_OpenCL_Tutorial_NBody-rev3.html#algorithm

#include <iostream>
#include "n2nbodysim.h"

#define KERNEL_ROOT "."
#define SYSTEM_ROOT "."

int main (int argc, const char * argv[])
{
    // Create simulation object with kernel.cl
    N2NBodySim sim(KERNEL_ROOT "/kernel.cl");
    
    // Load system from file and simulate (10 iterations)
    sim.loadData(SYSTEM_ROOT "/system0.dat");
    sim.run(10);
    sim.clearData();
    
    // Load another system and simulate
    sim.loadData(SYSTEM_ROOT "/system1.dat");
    sim.run(2);
    sim.clearData();
    
    return 0;
}
