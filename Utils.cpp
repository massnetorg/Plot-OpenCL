//
// Created by Blink on 2020/10/5.
//

#include "Utils.h"

namespace Utils {
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

    unsigned int *getPubkeyHash(unsigned char *pubkey) {
        unsigned int tmp[8] = {0};
        unsigned int *hash = new unsigned int[8];
        {
            SHA256_CTX ctx;
            sha256_init(&ctx);
            sha256_update(&ctx, (BYTE *)pubkey, 33);
            sha256_final(&ctx, (BYTE *)tmp);
        }
        {
            SHA256_CTX ctx;
            sha256_init(&ctx);
            sha256_update(&ctx, (BYTE *)tmp, 32);
            sha256_final(&ctx, (BYTE *)hash);
        }
        return hash;
    }

    unsigned char hex2byte(char *h) {
        unsigned char result = 0;
        for (int i = 0; i < 2; i++) {
            if (h[i] >= 'a' && h[i] <= 'f') {
                h[i] = h[i] - 'a' + 10;
            } else if (h[i] >= 'A' && h[i] <= 'F') {
                h[i] = h[i] - 'A' + 10;
            } else if (h[i] >= '0' && h[i] <= '9') {
                h[i] = h[i] - '0';
            } else {
                std::cout << "invalid arguments" << std::endl;
                exit(1);
            }
            result = result * 16 + h[i];
        }
        return result;
    }
};