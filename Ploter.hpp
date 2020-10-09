//
// Created by Blink on 2020/10/4.
//

#ifndef PLOT_OPENCL_PLOTER_HPP
#define PLOT_OPENCL_PLOTER_HPP
#include <iostream>
#include <OpenCL/cl.h>
#include <OpenCL/cl_platform.h>
#include <chrono>
#include <thread>
#include <vector>
#include <immintrin.h>
#include "util.h"
#include "thread_pool.h"
#define MAX_SOURCE_SIZE (0x100000)
#define MAX_TABLE_A_INPUT_SIZE 536870912 // 2GB of 4 Bytes block
class Ploter {
public:
    Ploter() {
        device_id = get_most_powerful_device();
        char* value;
        size_t valueSize;
        clGetDeviceInfo(device_id, CL_DEVICE_NAME, 0, NULL, &valueSize);
        value = (char*) malloc(valueSize);
        clGetDeviceInfo(device_id, CL_DEVICE_NAME, valueSize, value, NULL);
        printf("Device: %s\n", value);
        free(value);
        cl_ctx = clCreateContext(NULL, 1, &device_id, NULL, NULL, &cl_ret);
        cl_cmd_q = clCreateCommandQueue(cl_ctx, device_id, 0, &cl_ret);
        cl_input_obj = clCreateBuffer(cl_ctx, CL_MEM_READ_ONLY, sizeof(unsigned int), NULL, &cl_ret);
        cl_pubkey_hash_obj = clCreateBuffer(cl_ctx, CL_MEM_READ_ONLY, 8 * sizeof(unsigned int), NULL, &cl_ret);
        cl_output_obj = clCreateBuffer(cl_ctx, CL_MEM_READ_WRITE, MAX_TABLE_A_INPUT_SIZE * sizeof(unsigned int), NULL, &cl_ret);

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
        cl_prg = clCreateProgramWithSource(cl_ctx, 1, (const char **)&source_str, (const size_t *)&source_size, &cl_ret);
        cl_ret = clBuildProgram(cl_prg, 1, &device_id, NULL, NULL, NULL);
        cl_krnl_calc_table_a = clCreateKernel(cl_prg, "sha256_mass_table_a", &cl_ret);
        cl_krnl_calc_table_b = clCreateKernel(cl_prg, "sha256_mass_table_b", &cl_ret);
        cl_krnl_clean_buffer = clCreateKernel(cl_prg, "clean_buffer", &cl_ret);
        cl_ret = clSetKernelArg(cl_krnl_calc_table_a, 0, sizeof(cl_mem), (void *)&cl_input_obj);
        cl_ret = clSetKernelArg(cl_krnl_calc_table_a, 1, sizeof(cl_mem), (void *)&cl_pubkey_hash_obj);
        cl_ret = clSetKernelArg(cl_krnl_calc_table_a, 2, sizeof(cl_mem), (void *)&cl_output_obj);
        cl_ret = clSetKernelArg(cl_krnl_calc_table_b, 0, sizeof(cl_mem), (void *)&cl_pubkey_hash_obj);
        cl_ret = clSetKernelArg(cl_krnl_calc_table_b, 1, sizeof(cl_mem), (void *)&cl_output_obj);
        cl_ret = clSetKernelArg(cl_krnl_clean_buffer, 0, sizeof(cl_mem), (void *)&cl_output_obj);
        global_item_size = 4294967232;
        global_item_size_clean_buf = 536870912;
        global_item_size_calc_b = 536870912 >> 1;
        //global_item_size = 536870912;
        group_item_size = 64;
        //A = new unsigned int[1LL << 32];
        B = new uint64_t[1LL << 32];
        //B_rev = new unsigned int[1LL << 32];
    }

    ~Ploter() {
        cl_ret = clFlush(cl_cmd_q);
        cl_ret = clFinish(cl_cmd_q);
        cl_ret = clReleaseKernel(cl_krnl_calc_table_a);
        cl_ret = clReleaseKernel(cl_krnl_clean_buffer);
        cl_ret = clReleaseKernel(cl_krnl_calc_table_b);
        cl_ret = clReleaseProgram(cl_prg);
        cl_ret = clReleaseMemObject(cl_input_obj);
        cl_ret = clReleaseMemObject(cl_output_obj);
        cl_ret = clReleaseMemObject(cl_pubkey_hash_obj);
        cl_ret = clReleaseCommandQueue(cl_cmd_q);
        cl_ret = clReleaseContext(cl_ctx);
    }
    void calc_table_A(unsigned int *pubkeyHash, unsigned int bl) {
        cl_ret = clEnqueueWriteBuffer(cl_cmd_q, cl_pubkey_hash_obj, CL_TRUE, 0, 8 * sizeof(unsigned int), pubkeyHash, 0, NULL, NULL);
        unsigned int MASS_Prefix = 1296126803; // "MASS"
        unsigned int max_block_size = MAX_TABLE_A_INPUT_SIZE; // 2GB of 64 bytes block
        int block_nums = (1LL << bl) / (uint64_t)max_block_size;
        // Use OpenMP to accel
        std::cout << "block_nums: " << block_nums << std::endl;
        auto start = std::chrono::system_clock::now();
        auto end = std::chrono::system_clock::now();
        auto copy_buffer_duration = duration_cast<std::chrono::microseconds>(start - start);
        auto compute_duration = duration_cast<std::chrono::microseconds>(start - start);
        auto store_table_a_duration = duration_cast<std::chrono::microseconds>(start - start);
        auto compute_b_duration = duration_cast<std::chrono::microseconds>(start - start);
        //unsigned int *zeros = new unsigned int[MAX_TABLE_A_INPUT_SIZE];
        //memset (zeros, 0, sizeof(unsigned int) * MAX_TABLE_A_INPUT_SIZE);
        thread_pool make_table_b_thread_pool(4, 2, B);
        make_table_b_thread_pool.start();
        for (int j = 0; j < block_nums; j++) {
            A = new unsigned int[1 << 29];
            B_rev = new unsigned int[1 << 29];
            unsigned int offset = MAX_TABLE_A_INPUT_SIZE * j;
            // copy input buffer to device
            //unsigned int zero = 0;
            //cl_ret = clEnqueueWriteBuffer(cl_cmd_q, cl_output_obj, CL_TRUE, 0, sizeof(unsigned int) * MAX_TABLE_A_INPUT_SIZE, zeros, 0, NULL, NULL);
            //cl_ret = clEnqueueFillBuffer(cl_cmd_q, cl_output_obj, &zero, sizeof(unsigned int), 0, sizeof(unsigned int) * (size_t)MAX_TABLE_A_INPUT_SIZE, 0, NULL, NULL);
            start = std::chrono::system_clock::now();
            cl_ret = clEnqueueWriteBuffer(cl_cmd_q, cl_input_obj, CL_TRUE, 0, sizeof(unsigned int), &offset, 0, NULL, NULL);

            cl_ret = clEnqueueNDRangeKernel(cl_cmd_q, cl_krnl_clean_buffer, 1, NULL,
                                            &global_item_size_clean_buf, &group_item_size, 0, NULL, NULL);
            clFinish(cl_cmd_q);
            // calc
            cl_ret = clEnqueueNDRangeKernel(cl_cmd_q, cl_krnl_calc_table_a, 1, NULL,
                                            &global_item_size, &group_item_size, 0, NULL, NULL);
            clFinish(cl_cmd_q);
            end = std::chrono::system_clock::now();
            compute_duration += duration_cast<std::chrono::microseconds>(end - start);
            // copy output buffer to host
            start = std::chrono::system_clock::now();
            cl_ret = clEnqueueReadBuffer(cl_cmd_q, cl_output_obj, CL_TRUE, 0,
                                         MAX_TABLE_A_INPUT_SIZE * sizeof(unsigned int), A, 0, NULL, NULL);
            end = std::chrono::system_clock::now();
            copy_buffer_duration += duration_cast<std::chrono::microseconds>(end - start);
            start = std::chrono::system_clock::now();
            cl_ret = clEnqueueNDRangeKernel(cl_cmd_q, cl_krnl_calc_table_b, 1, NULL,
                                            &global_item_size_calc_b, &group_item_size, 0, NULL, NULL);
            clFinish(cl_cmd_q);
            end = std::chrono::system_clock::now();
            compute_b_duration += duration_cast<std::chrono::microseconds>(end - start);
            start = std::chrono::system_clock::now();
            cl_ret = clEnqueueReadBuffer(cl_cmd_q, cl_output_obj, CL_TRUE, 0,
                                         MAX_TABLE_A_INPUT_SIZE * sizeof(unsigned  int), B_rev, 0, NULL, NULL);
            clFinish(cl_cmd_q);
            end = std::chrono::system_clock::now();
            copy_buffer_duration += duration_cast<std::chrono::microseconds>(end - start);

            make_table_b_thread_pool.add_task(std::make_pair(A, B_rev));
            /*
            start = std::chrono::system_clock::now();
            make_table_B(A, B_rev);
            end = std::chrono::system_clock::now();
            auto make_table_b_duration = duration_cast<std::chrono::microseconds>(end - start);
            std::cout << "make_table_b: " << double(make_table_b_duration.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den << "s" << std::endl;
            */
            /*
            for (unsigned int i = 0; i < MAX_TABLE_A_INPUT_SIZE - 4; i++) {
                //_mm_prefetch(&A[output[i + 1]], _MM_HINT_NTA);
                _mm_prefetch(&A[output[i + 2]], _MM_HINT_NTA);
                A[output[i]] = offset + i;
            }
            A[output[MAX_TABLE_A_INPUT_SIZE - 4]] = offset + MAX_TABLE_A_INPUT_SIZE - 4;
            A[output[MAX_TABLE_A_INPUT_SIZE - 3]] = offset + MAX_TABLE_A_INPUT_SIZE - 3;
            A[output[MAX_TABLE_A_INPUT_SIZE - 2]] = offset + MAX_TABLE_A_INPUT_SIZE - 2;
            A[output[MAX_TABLE_A_INPUT_SIZE - 1]] = offset + MAX_TABLE_A_INPUT_SIZE - 1;
             */
            //write_table_A(output);

            printf("%d\n", j);
            /*
            int wrong = 0;
            int nonzero = 0;
            for(unsigned int i = 0; i < 100000; i++) {
                if (A[i] != 0) {
                    nonzero ++;
                    if ((offset + i) % 2 == 0) {
                        if (!verify(A[i], pubkeyHash, (offset + i) >> 1)) {
                            wrong ++;
                        }
                    } else {
                        if (!verify(A[i], pubkeyHash, ~((offset + i - 1) >> 1))) {
                            wrong ++;
                        }
                    }
                }
                //printf("sha256(%u) = %u\n", i, A[offset + i]);
            }
            std::cout << "wrong: " << wrong << std::endl;
            std::cout << "nonzero: " << nonzero << std::endl;
            */
            //delete[] A;
            //delete[] B_rev;
        }
        make_table_b_thread_pool.set_terminate();
        //make_table_b_thread_pool.join();
        std::cout << "compute: " << double(compute_duration.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den << "s" << std::endl;
        std::cout << "copy_buffer: " << double(copy_buffer_duration.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den << "s" << std::endl;
        std::cout << "compute_b: " << double(compute_b_duration.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den << "s" << std::endl;
    }
    uint64_t *B;
private:
    cl_device_id get_most_powerful_device() {
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
        for (int i = 0; i < platformCount; i++) {

            // get all devices
            clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_GPU, 0, NULL, &deviceCount);
            devices = (cl_device_id*) malloc(sizeof(cl_device_id) * deviceCount);
            clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_GPU, deviceCount, devices, NULL);

            // for each device print critical attributes
            for (int j = 0; j < deviceCount; j++) {
                clGetDeviceInfo(devices[j], CL_DEVICE_MAX_COMPUTE_UNITS,
                                sizeof(maxComputeUnits), &maxComputeUnits, NULL);
                if (maxComputeUnits > max_cu_num) {
                    max_cu_num = maxComputeUnits;
                    most_powerful_device = devices[j];
                }
            }

            free(devices);

        }

        free(platforms);
        return most_powerful_device;
    }

    void make_table_B(unsigned int *A, unsigned int *B_rev) {
        unsigned int end = 1 << 29;
        uint64_t x = 0, xp = 0;
        uint64_t pos = 0;
        for (unsigned int i = 0; i < end; i += 2) {
            x = A[i];
            xp = A[i + 1];
            if (x == 0 || xp == 0) {
                continue;
            }
            B[B_rev[i]] = x << 32U | xp;
            B[B_rev[i + 1]] = xp << 32U | x;
        }

        delete[] A;
        delete[] B_rev;
    }

    void write_table_A(unsigned int *output) {
        std::atomic_flag lock[256];
        for (int i = 0; i < 256; i++) {
            lock[i].clear(std::memory_order_release);
        }
        int thread_num = 6;
        std::vector<std::thread *> thread_p;
        auto w_f = [&](unsigned int offset, unsigned int length, unsigned int *out, unsigned int *A) {
            unsigned int end = offset + length;
            unsigned int block_size = (1LL << 32) / 256;
            for (unsigned int i = offset; i < end; i++) {
                int block = out[i] / block_size;
                while (lock[block].test_and_set(std::memory_order_acquire));
                A[out[i]] = i;
                lock[block].clear(std::memory_order_release);
            }
        };
        for (int i = 0; i < thread_num; i++) {
            unsigned int length = (MAX_TABLE_A_INPUT_SIZE / thread_num);
            unsigned int offset = i * length;
            thread_p.emplace_back(new std::thread(w_f, offset, length, output, A));
        }
        for (int i = 0; i < thread_num; i++) {
            thread_p[i]->join();
            delete thread_p[i];
        }
    }
    cl_int cl_ret;
    cl_device_id device_id;
    cl_context cl_ctx;
    cl_command_queue cl_cmd_q;
    cl_mem cl_input_obj, cl_output_obj, cl_pubkey_hash_obj;
    cl_program cl_prg;
    cl_kernel cl_krnl_calc_table_a, cl_krnl_clean_buffer, cl_krnl_calc_table_b;
    size_t global_item_size;
    size_t group_item_size;
    size_t global_item_size_clean_buf, global_item_size_calc_b;
    unsigned int *A;
    unsigned int *B_rev;
};


#endif //PLOT_OPENCL_PLOTER_HPP
