//
// Created by Blink on 2020/10/5.
//

#ifndef PLOT_OPENCL_UTILS_H
#define PLOT_OPENCL_UTILS_H

#include "sha256.h"
#include <memory>
#include <cstring>
#include <iostream>

namespace Utils {
    bool verify(unsigned int idx, unsigned int *pubkey_hash, unsigned int v);

    unsigned char hex2byte(char *h);

    unsigned int *getPubkeyHash(unsigned char *pubkey);
}

#endif //PLOT_OPENCL_UTILS_H
