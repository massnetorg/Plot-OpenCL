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
#include <filesystem>
#include "Plotter.hpp"
#define MAX_SOURCE_SIZE (0x100000)


int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cout << "invalid arguments" << std::endl;
        exit(1);
    }
    std::string output_dir = std::string(argv[1]);
    int64_t ordinary = strtoll(argv[2], NULL, 10);
    char *pubkey_str = argv[3];
    std::cout << output_dir << " " << ordinary << " " << pubkey_str << std::endl;
    unsigned char *pubkey = new unsigned char[33];
    for (int i = 0; i < 33; i++) {
        pubkey[i] = Utils::hex2byte(pubkey_str + i * 2);
    }
    unsigned int *pubkeyHash = Utils::getPubkeyHash(pubkey);

    Plotter P;
    auto start = std::chrono::system_clock::now();
    P.makeHeader(pubkeyHash, pubkey);
    P.plot(pubkeyHash, 32);

    std::filesystem::path file_path(output_dir);
    file_path /= std::to_string(ordinary) + "_" + std::string(pubkey_str) + "_32.massdb";
    FILE *out_file = fopen(file_path.c_str(), "wb");
    fwrite(P.header, 1, 4096, out_file);
    for (int i = 0; i < 8; i++) {
        fwrite(P.B + i * (1LL << 29), sizeof(uint64_t), (1 << 29), out_file);
    }
    fclose(out_file);
    return 0;
}