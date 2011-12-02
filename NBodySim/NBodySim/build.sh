#!/bin/bash
g++ -Wall -g main.cpp n2nbodysim.cpp -o nbody -I/opt/gpu_sdk/OpenCL/common/inc/ -lOpenCL
