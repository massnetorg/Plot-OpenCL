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
    auto all_duration = duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "all: " << double(all_duration.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den << "s" << std::endl;
    /*
    unsigned int i;
    const int LIST_SIZE = 33554432;
    //const int LIST_SIZE = 10240;
    unsigned int *A = (unsigned int*)malloc(sizeof(unsigned int) * LIST_SIZE * 16);
    for (int i = 0; i < LIST_SIZE; i++) {
        for(unsigned int j = 0; j < 13; j++) {
            A[i * 16 + j] = 1094795585;
        }
        A[i * 16 + 12] = 1111638594;
        A[i * 16 + 13] = 1;
        A[i * 16 + 13] <<= 31;
        A[i * 16 + 14] = 0;
        A[i * 16 + 15] = 416;
    }
    //printDevices();
    // Load the kernel source code into the array source_str
    FILE *fp;
    char *source_str;
    size_t source_size;

    fp = fopen("sha256_mass.cl", "r");
    if (!fp) {
        fprintf(stderr, "Failed to load kernel.\n");
        exit(1);
    }
    source_str = (char*)malloc(MAX_SOURCE_SIZE);
    source_size = fread( source_str, 1, MAX_SOURCE_SIZE, fp);
    fclose( fp );

    // Get platform and device information
    cl_int ret;
    cl_device_id device_id = get_most_powerful_device();
    char* value;
    size_t valueSize;
    clGetDeviceInfo(device_id, CL_DEVICE_NAME, 0, NULL, &valueSize);
    value = (char*) malloc(valueSize);
    clGetDeviceInfo(device_id, CL_DEVICE_NAME, valueSize, value, NULL);
    printf("Device: %s\n", value);
    free(value);

    // Create an OpenCL context
    cl_context context = clCreateContext( NULL, 1, &device_id, NULL, NULL, &ret);

    // Create a command queue
    cl_command_queue command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

    // Create memory buffers on the device for each vector
    cl_mem a_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY,
                                      LIST_SIZE * sizeof(unsigned int) * 16, NULL, &ret);
    //cl_mem b_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY,
    //                                  LIST_SIZE * sizeof(int), NULL, &ret);
    cl_mem c_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
                                      LIST_SIZE * sizeof(unsigned int) * 8, NULL, &ret);

    // Copy the lists A and B to their respective memory buffers
    auto start = chrono::system_clock::now();
    ret = clEnqueueWriteBuffer(command_queue, a_mem_obj, CL_TRUE, 0,
                               LIST_SIZE * sizeof(unsigned int) * 16, A, 0, NULL, NULL);
    auto end = chrono::system_clock::now();
    auto duration = duration_cast<chrono::microseconds>(end - start);
    std::cout << "copy buffer: " << double(duration.count()) * chrono::microseconds::period::num / chrono::microseconds::period::den << "s" << std::endl;

    //ret = clEnqueueWriteBuffer(command_queue, b_mem_obj, CL_TRUE, 0,
    //                           LIST_SIZE * sizeof(int), B, 0, NULL, NULL);

    // Create a program from the kernel source
    cl_program program = clCreateProgramWithSource(context, 1,
                                                   (const char **)&source_str, (const size_t *)&source_size, &ret);

    // Build the program
    ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

    // Create the OpenCL kernel
    cl_kernel kernel = clCreateKernel(program, "sha256_mass", &ret);

    // Set the arguments of the kernel
    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&a_mem_obj);
    ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&c_mem_obj);
    //ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&c_mem_obj);

    // Execute the OpenCL kernel on the list
    size_t global_item_size = LIST_SIZE; // Process the entire lists
    size_t local_item_size = 64; // Divide work items into groups of 64
    start = chrono::system_clock::now();
    ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL,
                                 &global_item_size, &local_item_size, 0, NULL, NULL);
    clFinish(command_queue);
    end = chrono::system_clock::now();
    duration = duration_cast<chrono::microseconds>(end - start);
    std::cout << "compute: " << double(duration.count()) * chrono::microseconds::period::num / chrono::microseconds::period::den << "s" << std::endl;

    //std::this_thread::sleep_for(1s);
    // Read the memory buffer C on the device to the local variable C
    //ret = clEnqueueReadBuffer(command_queue, a_mem_obj, CL_TRUE, 0,
    //                          LIST_SIZE * sizeof(unsigned int), A, 0, NULL, NULL);
    unsigned int *C = (unsigned int*)malloc(sizeof(unsigned int) * LIST_SIZE * 8);
    ret = clEnqueueReadBuffer(command_queue, c_mem_obj, CL_TRUE, 0,
                              LIST_SIZE * sizeof(unsigned int) * 8, C, 0, NULL, NULL);

    // Display the result to the screen
    for(i = 0; i < 16; i++)
        printf("sha256(%u) = %u\n", i, C[i]);

    // Clean up
    ret = clFlush(command_queue);
    ret = clFinish(command_queue);
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(a_mem_obj);
    //ret = clReleaseMemObject(b_mem_obj);
    ret = clReleaseMemObject(c_mem_obj);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);
    free(A);
    // free(B);
    free(C);
     */
    return 0;
}