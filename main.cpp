#include <iostream>
#ifdef __APPLE__
#include "OpenCL/opencl.h"
#else
#include "CL/cl.h"
#endif
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <fstream>
#include <thread>
#include <chrono>
#include "Ploter.hpp"
#define MAX_SOURCE_SIZE (0x100000)

// pubkeyHash : 256bit 8 * uint32

cl_device_id get_most_powerful_device() {
    int i, j;
    char* value;
    size_t valueSize;
    cl_uint platformCount;
    cl_platform_id* platforms;
    cl_uint deviceCount;
    cl_device_id* devices;
    cl_uint maxComputeUnits;

    // get all platforms
    clGetPlatformIDs(0, NULL, &platformCount);
    platforms = (cl_platform_id*) malloc(sizeof(cl_platform_id) * platformCount);
    clGetPlatformIDs(platformCount, platforms, NULL);
    int max_cu_num = 0;
    cl_device_id most_powerful_device;
    for (i = 0; i < platformCount; i++) {

        // get all devices
        clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_GPU, 0, NULL, &deviceCount);
        devices = (cl_device_id*) malloc(sizeof(cl_device_id) * deviceCount);
        clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_GPU, deviceCount, devices, NULL);

        // for each device print critical attributes
        for (j = 0; j < deviceCount; j++) {
            /*
            // print device name
            clGetDeviceInfo(devices[j], CL_DEVICE_NAME, 0, NULL, &valueSize);
            value = (char*) malloc(valueSize);
            clGetDeviceInfo(devices[j], CL_DEVICE_NAME, valueSize, value, NULL);
            printf("%d. Device: %s\n", j+1, value);
            free(value);

            // print hardware device version
            clGetDeviceInfo(devices[j], CL_DEVICE_VERSION, 0, NULL, &valueSize);
            value = (char*) malloc(valueSize);
            clGetDeviceInfo(devices[j], CL_DEVICE_VERSION, valueSize, value, NULL);
            printf(" %d.%d Hardware version: %s\n", j+1, 1, value);
            free(value);

            // print software driver version
            clGetDeviceInfo(devices[j], CL_DRIVER_VERSION, 0, NULL, &valueSize);
            value = (char*) malloc(valueSize);
            clGetDeviceInfo(devices[j], CL_DRIVER_VERSION, valueSize, value, NULL);
            printf(" %d.%d Software version: %s\n", j+1, 2, value);
            free(value);

            // print c version supported by compiler for device
            clGetDeviceInfo(devices[j], CL_DEVICE_OPENCL_C_VERSION, 0, NULL, &valueSize);
            value = (char*) malloc(valueSize);
            clGetDeviceInfo(devices[j], CL_DEVICE_OPENCL_C_VERSION, valueSize, value, NULL);
            printf(" %d.%d OpenCL C version: %s\n", j+1, 3, value);
            free(value);
             */
            // print parallel compute units
            clGetDeviceInfo(devices[j], CL_DEVICE_MAX_COMPUTE_UNITS,
                            sizeof(maxComputeUnits), &maxComputeUnits, NULL);
            if (maxComputeUnits > max_cu_num) {
                max_cu_num = maxComputeUnits;
                most_powerful_device = devices[j];
            }
            //printf(" %d.%d Parallel compute units: %d\n", j+1, 4, maxComputeUnits);

        }

        free(devices);

    }

    free(platforms);
    return most_powerful_device;
}
int main() {
    // Create the two input vectors
    unsigned int *pubkeyHash = new unsigned int[8];
    for (int i = 0 ; i < 8; i++) {
        pubkeyHash[i] = i * 2;
    }
    Ploter P;
    auto start = std::chrono::system_clock::now();
    P.calc_table_A(pubkeyHash, 32);
    int non_zero_num = 0;
    for (int i = 0; i < 1024000; i++) {
        if (P.B[i] != 0) {
            non_zero_num ++;
        }
    }
    std::cout << "non zero: " << non_zero_num << std::endl;
    auto end = std::chrono::system_clock::now();
    auto all_duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "all: " << double(all_duration.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den << "s" << std::endl;
    
    return 0;
}