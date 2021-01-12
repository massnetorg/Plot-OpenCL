//
// Created by Blink on 2020/10/4.
//

#include "Plotter.hpp"

Plotter::Plotter() {
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
    header = new unsigned char[4096];
    //B_rev = new unsigned int[1LL << 32];
}

Plotter::~Plotter() {
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
    delete[] B;
    delete[] header;
}

void Plotter::plot(unsigned int *pubkeyHash, unsigned int bl) {
    cl_ret = clEnqueueWriteBuffer(cl_cmd_q, cl_pubkey_hash_obj, CL_TRUE, 0, 8 * sizeof(unsigned int), pubkeyHash, 0, NULL, NULL);
    unsigned int MASS_Prefix = 1296126803; // "MASS"
    unsigned int max_block_size = MAX_TABLE_A_INPUT_SIZE; // 2GB of 64 bytes block
    int block_nums = (1LL << bl) / (uint64_t)max_block_size;
    auto start = std::chrono::system_clock::now();
    auto end = std::chrono::system_clock::now();
    auto copy_buffer_duration = std::chrono::duration_cast<std::chrono::microseconds>(start - start);
    auto compute_duration = std::chrono::duration_cast<std::chrono::microseconds>(start - start);
    auto store_table_a_duration = std::chrono::duration_cast<std::chrono::microseconds>(start - start);
    auto compute_b_duration = std::chrono::duration_cast<std::chrono::microseconds>(start - start);
    int cores = std::thread::hardware_concurrency();
    TableBFiller make_table_b_thread_pool(2, cores / 2, B);
    make_table_b_thread_pool.start();
    for (int j = 0; j < block_nums; j++) {
        A = new unsigned int[1 << 29];
        B_rev = new unsigned int[1 << 29];
        unsigned int offset = MAX_TABLE_A_INPUT_SIZE * j;
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
        compute_duration += std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        // copy output buffer to host
        start = std::chrono::system_clock::now();
        cl_ret = clEnqueueReadBuffer(cl_cmd_q, cl_output_obj, CL_TRUE, 0,
                                     MAX_TABLE_A_INPUT_SIZE * sizeof(unsigned int), A, 0, NULL, NULL);
        end = std::chrono::system_clock::now();
        copy_buffer_duration += std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        start = std::chrono::system_clock::now();
        cl_ret = clEnqueueNDRangeKernel(cl_cmd_q, cl_krnl_calc_table_b, 1, NULL,
                                        &global_item_size_calc_b, &group_item_size, 0, NULL, NULL);
        clFinish(cl_cmd_q);
        end = std::chrono::system_clock::now();
        compute_b_duration += std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        start = std::chrono::system_clock::now();
        cl_ret = clEnqueueReadBuffer(cl_cmd_q, cl_output_obj, CL_TRUE, 0,
                                     MAX_TABLE_A_INPUT_SIZE * sizeof(unsigned  int), B_rev, 0, NULL, NULL);
        clFinish(cl_cmd_q);
        end = std::chrono::system_clock::now();
        copy_buffer_duration += std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        make_table_b_thread_pool.add_task(std::make_pair(A, B_rev));
    }
    make_table_b_thread_pool.set_terminate();
    std::cout << "compute: " << double(compute_duration.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den << "s" << std::endl;
    std::cout << "copy_buffer: " << double(copy_buffer_duration.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den << "s" << std::endl;
    std::cout << "compute_b: " << double(compute_b_duration.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den << "s" << std::endl;
}

cl_device_id Plotter::get_most_powerful_device() {
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

void Plotter::makeHeader(unsigned int *pubkeyHash, unsigned char *pubkey)  {
    static const unsigned char filecode[32] = {
            0x52, 0xa7, 0xad, 0x74, 0xc4, 0x92, 0x9d, 0xec,
            0x7b, 0x5c, 0x8d, 0x46, 0xcc, 0x3b, 0xaf, 0xa8,
            0x1f, 0xc9, 0x61, 0x29, 0x28, 0x3b, 0x3a, 0x69,
            0x23, 0xcd, 0x12, 0xf4, 0x1a, 0x30, 0xb3, 0xac
    };
    memset(header, 0, 4096);
    // filecode
    memcpy(header, filecode, 32);
    // version
    uint64_t *version = reinterpret_cast<uint64_t *>(header + 32);
    *version = 1;
    // bitlength
    header[40] = 32;
    // type
    header[41] = 2;
    // checkpoint
    uint64_t *checkpoint = reinterpret_cast<uint64_t *>(header + 42);
    *checkpoint = (1LL << 31);
    // pubkeyhash
    memcpy(header + 50, pubkeyHash, 32);
    // pubkey
    memcpy(header + 82, pubkey, 33);
}