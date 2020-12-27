#include <iostream>
#include <fstream>
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
#include "Plotter.hpp"
#define MAX_SOURCE_SIZE (0x100000)


int main() {
    std::ifstream in_file("hash_key", std::ios::binary);
    unsigned int *pubkeyHash = new unsigned int[8];
    unsigned char *pubkey = new unsigned char[33];
    in_file.read((char *)pubkeyHash, 32);
    in_file.read((char *)pubkey, 33);

    Plotter P;
    auto start = std::chrono::system_clock::now();
    P.makeHeader(pubkeyHash, pubkey);
    P.plot(pubkeyHash, 32);
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
    std::ofstream out_file("table_b", std::ios::binary);
    out_file.write((char *)(P.header), 4096);
    out_file.write((char *)(P.B), sizeof(uint64_t) * (1LL << 32));
    return 0;
}