//
// Created by Blink on 2020/10/4.
//

#ifndef PLOT_OPENCL_PLOTTER_HPP
#define PLOT_OPENCL_PLOTTER_HPP
#include <iostream>
#ifdef __APPLE__
#include "OpenCL/opencl.h"
#else
#include "CL/cl.h"
#endif
#include <chrono>
#include <thread>
#include <vector>
#include <immintrin.h>
#include "util.h"
#include "thread_pool.h"
#define MAX_SOURCE_SIZE (0x100000)
#define MAX_TABLE_A_INPUT_SIZE 536870912 // 2GB of 4 Bytes block
class Plotter {
public:
    explicit Plotter();

    ~Plotter();

    void plot(unsigned int *pubkeyHash, unsigned int bl);

    void makeHeader(unsigned int *pubkeyHash, unsigned char *pubkey) {
        static std::string filecode = "DC9081169821110F139A7FC8486D74F3749653DD3F60F127F8F0F14571AE3F6D";
        memset(header, 0, 4096);
        // filecode
        memcpy(header, filecode.data(), 32);
        // version
        uint64_t *version = reinterpret_cast<uint64_t *>(header + 32);
        *version = 1;
        // bitlength
        header[40] = 32;
        // type
        header[41] = 1;
        // checkpoint
        uint64_t *checkpoint = reinterpret_cast<uint64_t *>(header + 42);
        *checkpoint = (1LL << 31);
        // pubkeyhash
        memcpy(header + 50, pubkeyHash, 32);
        // pubkey
        memcpy(header + 82, pubkey, 33);
    }
    uint64_t *B;
    unsigned char *header;
private:
    cl_device_id get_most_powerful_device();

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


#endif //PLOT_OPENCL_PLOTTER_HPP
