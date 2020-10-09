//
// Created by Blink on 2020/10/5.
//

#ifndef PLOT_OPENCL_UTIL_H
#define PLOT_OPENCL_UTIL_H

#include "sha256.h"
#include <memory>
#include <iostream>

namespace {
    bool verify(unsigned int idx, unsigned int *pubkey_hash, unsigned int v) {
        SHA256_CTX ctx;
        static unsigned int hash[8] = {0};
        sha256_init(&ctx);
        unsigned int W[11];
        W[0] = 0x5353414d;
        memcpy(&W[1], pubkey_hash, sizeof(unsigned int) * 8);
        W[9] = idx;
        W[10] = 0;
        sha256_update(&ctx, (BYTE *)W, 44);
        sha256_final(&ctx, (BYTE *)hash);
        if (hash[0] != (unsigned int)v) {
            std::cout << hash[0] << " " << v << std::endl;
            return false;
        }
        return true;
    }

}

#endif //PLOT_OPENCL_UTIL_H
