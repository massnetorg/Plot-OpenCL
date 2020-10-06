
#define HALF_32BIT 2147483647
#define F1(x,y,z)   (bitselect(z,y,x))
#define F0(x,y,z)   (bitselect (x, y, ((x) ^ (z))))
#define mod(x,y) ((x)-((x)/(y)*(y)))
#define shr32(x,n) ((x) >> (n))
#define rotl32(a,n) rotate ((a), (n))

#define S0(x) (rotl32 ((x), 25u) ^ rotl32 ((x), 14u) ^ shr32 ((x),  3u))
#define S1(x) (rotl32 ((x), 15u) ^ rotl32 ((x), 13u) ^ shr32 ((x), 10u))
#define S2(x) (rotl32 ((x), 30u) ^ rotl32 ((x), 19u) ^ rotl32 ((x), 10u))
#define S3(x) (rotl32 ((x), 26u) ^ rotl32 ((x), 21u) ^ rotl32 ((x),  7u))

#define SHA256C00 0x428a2f98u
#define SHA256C01 0x71374491u
#define SHA256C02 0xb5c0fbcfu
#define SHA256C03 0xe9b5dba5u
#define SHA256C04 0x3956c25bu
#define SHA256C05 0x59f111f1u
#define SHA256C06 0x923f82a4u
#define SHA256C07 0xab1c5ed5u
#define SHA256C08 0xd807aa98u
#define SHA256C09 0x12835b01u
#define SHA256C0a 0x243185beu
#define SHA256C0b 0x550c7dc3u
#define SHA256C0c 0x72be5d74u
#define SHA256C0d 0x80deb1feu
#define SHA256C0e 0x9bdc06a7u
#define SHA256C0f 0xc19bf174u
#define SHA256C10 0xe49b69c1u
#define SHA256C11 0xefbe4786u
#define SHA256C12 0x0fc19dc6u
#define SHA256C13 0x240ca1ccu
#define SHA256C14 0x2de92c6fu
#define SHA256C15 0x4a7484aau
#define SHA256C16 0x5cb0a9dcu
#define SHA256C17 0x76f988dau
#define SHA256C18 0x983e5152u
#define SHA256C19 0xa831c66du
#define SHA256C1a 0xb00327c8u
#define SHA256C1b 0xbf597fc7u
#define SHA256C1c 0xc6e00bf3u
#define SHA256C1d 0xd5a79147u
#define SHA256C1e 0x06ca6351u
#define SHA256C1f 0x14292967u
#define SHA256C20 0x27b70a85u
#define SHA256C21 0x2e1b2138u
#define SHA256C22 0x4d2c6dfcu
#define SHA256C23 0x53380d13u
#define SHA256C24 0x650a7354u
#define SHA256C25 0x766a0abbu
#define SHA256C26 0x81c2c92eu
#define SHA256C27 0x92722c85u
#define SHA256C28 0xa2bfe8a1u
#define SHA256C29 0xa81a664bu
#define SHA256C2a 0xc24b8b70u
#define SHA256C2b 0xc76c51a3u
#define SHA256C2c 0xd192e819u
#define SHA256C2d 0xd6990624u
#define SHA256C2e 0xf40e3585u
#define SHA256C2f 0x106aa070u
#define SHA256C30 0x19a4c116u
#define SHA256C31 0x1e376c08u
#define SHA256C32 0x2748774cu
#define SHA256C33 0x34b0bcb5u
#define SHA256C34 0x391c0cb3u
#define SHA256C35 0x4ed8aa4au
#define SHA256C36 0x5b9cca4fu
#define SHA256C37 0x682e6ff3u
#define SHA256C38 0x748f82eeu
#define SHA256C39 0x78a5636fu
#define SHA256C3a 0x84c87814u
#define SHA256C3b 0x8cc70208u
#define SHA256C3c 0x90befffau
#define SHA256C3d 0xa4506cebu
#define SHA256C3e 0xbef9a3f7u
#define SHA256C3f 0xc67178f2u

__constant uint k_sha256[64] =
{
SHA256C00, SHA256C01, SHA256C02, SHA256C03,
SHA256C04, SHA256C05, SHA256C06, SHA256C07,
SHA256C08, SHA256C09, SHA256C0a, SHA256C0b,
SHA256C0c, SHA256C0d, SHA256C0e, SHA256C0f,
SHA256C10, SHA256C11, SHA256C12, SHA256C13,
SHA256C14, SHA256C15, SHA256C16, SHA256C17,
SHA256C18, SHA256C19, SHA256C1a, SHA256C1b,
SHA256C1c, SHA256C1d, SHA256C1e, SHA256C1f,
SHA256C20, SHA256C21, SHA256C22, SHA256C23,
SHA256C24, SHA256C25, SHA256C26, SHA256C27,
SHA256C28, SHA256C29, SHA256C2a, SHA256C2b,
SHA256C2c, SHA256C2d, SHA256C2e, SHA256C2f,
SHA256C30, SHA256C31, SHA256C32, SHA256C33,
SHA256C34, SHA256C35, SHA256C36, SHA256C37,
SHA256C38, SHA256C39, SHA256C3a, SHA256C3b,
SHA256C3c, SHA256C3d, SHA256C3e, SHA256C3f,
};

#define SHA256_STEP(F0a,F1a,a,b,c,d,e,f,g,h,x,K)  \
{                                               \
  h += K;                                       \
  h += x;                                       \
  h += S3 (e);                           \
  h += F1a (e,f,g);                              \
  d += h;                                       \
  h += S2 (a);                           \
  h += F0a (a,b,c);                              \
}

#define SHA256_EXPAND(x,y,z,w) (S1 (x) + y + S0 (z) + w)
unsigned int SWAP (unsigned int val)
{
    return (rotate(((val) & 0x00FF00FF), 24U) | rotate(((val) & 0xFF00FF00), 8U));
}

static void sha256_main(__global unsigned int *W, __global unsigned int *hash) {
    // TODO: SWAP in host
    //for (int i = 0; i < 13; i++) {
    //    W[i] = SWAP(W[i]);
    //}

    unsigned int a = 0x6a09e667;
    unsigned int b = 0xbb67ae85;
    unsigned int c = 0x3c6ef372;
    unsigned int d = 0xa54ff53a;
    unsigned int e = 0x510e527f;
    unsigned int f = 0x9b05688c;
    unsigned int g = 0x1f83d9ab;
    unsigned int h = 0x5be0cd19;

    unsigned int w0_t = SWAP(W[0]);
    unsigned int w1_t = SWAP(W[1]);
    unsigned int w2_t = SWAP(W[2]);
    unsigned int w3_t = SWAP(W[3]);
    unsigned int w4_t = SWAP(W[4]);
    unsigned int w5_t = SWAP(W[5]);
    unsigned int w6_t = SWAP(W[6]);
    unsigned int w7_t = SWAP(W[7]);
    unsigned int w8_t = SWAP(W[8]);
    unsigned int w9_t = SWAP(W[9]);
    unsigned int wa_t = SWAP(W[10]);
    unsigned int wb_t = SWAP(W[11]);
    unsigned int wc_t = SWAP(W[12]);
    unsigned int wd_t = W[13];
    unsigned int we_t = W[14];
    unsigned int wf_t = W[15];

#define ROUND_EXPAND(i)                           \
  {                                                 \
    w0_t = SHA256_EXPAND (we_t, w9_t, w1_t, w0_t);  \
    w1_t = SHA256_EXPAND (wf_t, wa_t, w2_t, w1_t);  \
    w2_t = SHA256_EXPAND (w0_t, wb_t, w3_t, w2_t);  \
    w3_t = SHA256_EXPAND (w1_t, wc_t, w4_t, w3_t);  \
    w4_t = SHA256_EXPAND (w2_t, wd_t, w5_t, w4_t);  \
    w5_t = SHA256_EXPAND (w3_t, we_t, w6_t, w5_t);  \
    w6_t = SHA256_EXPAND (w4_t, wf_t, w7_t, w6_t);  \
    w7_t = SHA256_EXPAND (w5_t, w0_t, w8_t, w7_t);  \
    w8_t = SHA256_EXPAND (w6_t, w1_t, w9_t, w8_t);  \
    w9_t = SHA256_EXPAND (w7_t, w2_t, wa_t, w9_t);  \
    wa_t = SHA256_EXPAND (w8_t, w3_t, wb_t, wa_t);  \
    wb_t = SHA256_EXPAND (w9_t, w4_t, wc_t, wb_t);  \
    wc_t = SHA256_EXPAND (wa_t, w5_t, wd_t, wc_t);  \
    wd_t = SHA256_EXPAND (wb_t, w6_t, we_t, wd_t);  \
    we_t = SHA256_EXPAND (wc_t, w7_t, wf_t, we_t);  \
    wf_t = SHA256_EXPAND (wd_t, w8_t, w0_t, wf_t);  \
  }

#define ROUND_STEP(i)                                                                   \
  {                                                                                       \
    SHA256_STEP (F0, F1, a, b, c, d, e, f, g, h, w0_t, k_sha256[i +  0]); \
    SHA256_STEP (F0, F1, h, a, b, c, d, e, f, g, w1_t, k_sha256[i +  1]); \
    SHA256_STEP (F0, F1, g, h, a, b, c, d, e, f, w2_t, k_sha256[i +  2]); \
    SHA256_STEP (F0, F1, f, g, h, a, b, c, d, e, w3_t, k_sha256[i +  3]); \
    SHA256_STEP (F0, F1, e, f, g, h, a, b, c, d, w4_t, k_sha256[i +  4]); \
    SHA256_STEP (F0, F1, d, e, f, g, h, a, b, c, w5_t, k_sha256[i +  5]); \
    SHA256_STEP (F0, F1, c, d, e, f, g, h, a, b, w6_t, k_sha256[i +  6]); \
    SHA256_STEP (F0, F1, b, c, d, e, f, g, h, a, w7_t, k_sha256[i +  7]); \
    SHA256_STEP (F0, F1, a, b, c, d, e, f, g, h, w8_t, k_sha256[i +  8]); \
    SHA256_STEP (F0, F1, h, a, b, c, d, e, f, g, w9_t, k_sha256[i +  9]); \
    SHA256_STEP (F0, F1, g, h, a, b, c, d, e, f, wa_t, k_sha256[i + 10]); \
    SHA256_STEP (F0, F1, f, g, h, a, b, c, d, e, wb_t, k_sha256[i + 11]); \
    SHA256_STEP (F0, F1, e, f, g, h, a, b, c, d, wc_t, k_sha256[i + 12]); \
    SHA256_STEP (F0, F1, d, e, f, g, h, a, b, c, wd_t, k_sha256[i + 13]); \
    SHA256_STEP (F0, F1, c, d, e, f, g, h, a, b, we_t, k_sha256[i + 14]); \
    SHA256_STEP (F0, F1, b, c, d, e, f, g, h, a, wf_t, k_sha256[i + 15]); \
  }

    ROUND_STEP (0);

    ROUND_EXPAND();
    ROUND_STEP(16);

    ROUND_EXPAND();
    ROUND_STEP(32);

    ROUND_EXPAND();
    ROUND_STEP(48);

    hash[0] = SWAP(a + 0x6a09e667);
    hash[1] = SWAP(b + 0xbb67ae85);
    hash[2] = SWAP(c + 0x3c6ef372);
    hash[3] = SWAP(d + 0xa54ff53a);
    hash[4] = SWAP(e + 0x510e527f);
    hash[5] = SWAP(f + 0x9b05688c);
    hash[6] = SWAP(g + 0x1f83d9ab);
    hash[7] = SWAP(h + 0x5be0cd19);
}

static void sha256_main_table_b(unsigned int x, unsigned int xp, __global const unsigned int *pubkey_hash, __global unsigned int* hash) {
    unsigned int a = 0x6a09e667;
    unsigned int b = 0xbb67ae85;
    unsigned int c = 0x3c6ef372;
    unsigned int d = 0xa54ff53a;
    unsigned int e = 0x510e527f;
    unsigned int f = 0x9b05688c;
    unsigned int g = 0x1f83d9ab;
    unsigned int h = 0x5be0cd19;
    unsigned int w0_t = 0x4d415353; // "MASS"
    unsigned int w1_t = SWAP(pubkey_hash[0]);
    unsigned int w2_t = SWAP(pubkey_hash[1]);
    unsigned int w3_t = SWAP(pubkey_hash[2]);
    unsigned int w4_t = SWAP(pubkey_hash[3]);
    unsigned int w5_t = SWAP(pubkey_hash[4]);
    unsigned int w6_t = SWAP(pubkey_hash[5]);
    unsigned int w7_t = SWAP(pubkey_hash[6]);
    unsigned int w8_t = SWAP(pubkey_hash[7]);
    unsigned int w9_t = SWAP(x);
    unsigned int wa_t = 0;
    unsigned int wb_t = SWAP(xp);
    unsigned int wc_t = 0;
    unsigned int wd_t = 1 << 31;
    unsigned int we_t = 0;
    unsigned int wf_t = 416;

#define ROUND_EXPAND(i)                           \
  {                                                 \
    w0_t = SHA256_EXPAND (we_t, w9_t, w1_t, w0_t);  \
    w1_t = SHA256_EXPAND (wf_t, wa_t, w2_t, w1_t);  \
    w2_t = SHA256_EXPAND (w0_t, wb_t, w3_t, w2_t);  \
    w3_t = SHA256_EXPAND (w1_t, wc_t, w4_t, w3_t);  \
    w4_t = SHA256_EXPAND (w2_t, wd_t, w5_t, w4_t);  \
    w5_t = SHA256_EXPAND (w3_t, we_t, w6_t, w5_t);  \
    w6_t = SHA256_EXPAND (w4_t, wf_t, w7_t, w6_t);  \
    w7_t = SHA256_EXPAND (w5_t, w0_t, w8_t, w7_t);  \
    w8_t = SHA256_EXPAND (w6_t, w1_t, w9_t, w8_t);  \
    w9_t = SHA256_EXPAND (w7_t, w2_t, wa_t, w9_t);  \
    wa_t = SHA256_EXPAND (w8_t, w3_t, wb_t, wa_t);  \
    wb_t = SHA256_EXPAND (w9_t, w4_t, wc_t, wb_t);  \
    wc_t = SHA256_EXPAND (wa_t, w5_t, wd_t, wc_t);  \
    wd_t = SHA256_EXPAND (wb_t, w6_t, we_t, wd_t);  \
    we_t = SHA256_EXPAND (wc_t, w7_t, wf_t, we_t);  \
    wf_t = SHA256_EXPAND (wd_t, w8_t, w0_t, wf_t);  \
  }

#define ROUND_STEP(i)                                                                   \
  {                                                                                       \
    SHA256_STEP (F0, F1, a, b, c, d, e, f, g, h, w0_t, k_sha256[i +  0]); \
    SHA256_STEP (F0, F1, h, a, b, c, d, e, f, g, w1_t, k_sha256[i +  1]); \
    SHA256_STEP (F0, F1, g, h, a, b, c, d, e, f, w2_t, k_sha256[i +  2]); \
    SHA256_STEP (F0, F1, f, g, h, a, b, c, d, e, w3_t, k_sha256[i +  3]); \
    SHA256_STEP (F0, F1, e, f, g, h, a, b, c, d, w4_t, k_sha256[i +  4]); \
    SHA256_STEP (F0, F1, d, e, f, g, h, a, b, c, w5_t, k_sha256[i +  5]); \
    SHA256_STEP (F0, F1, c, d, e, f, g, h, a, b, w6_t, k_sha256[i +  6]); \
    SHA256_STEP (F0, F1, b, c, d, e, f, g, h, a, w7_t, k_sha256[i +  7]); \
    SHA256_STEP (F0, F1, a, b, c, d, e, f, g, h, w8_t, k_sha256[i +  8]); \
    SHA256_STEP (F0, F1, h, a, b, c, d, e, f, g, w9_t, k_sha256[i +  9]); \
    SHA256_STEP (F0, F1, g, h, a, b, c, d, e, f, wa_t, k_sha256[i + 10]); \
    SHA256_STEP (F0, F1, f, g, h, a, b, c, d, e, wb_t, k_sha256[i + 11]); \
    SHA256_STEP (F0, F1, e, f, g, h, a, b, c, d, wc_t, k_sha256[i + 12]); \
    SHA256_STEP (F0, F1, d, e, f, g, h, a, b, c, wd_t, k_sha256[i + 13]); \
    SHA256_STEP (F0, F1, c, d, e, f, g, h, a, b, we_t, k_sha256[i + 14]); \
    SHA256_STEP (F0, F1, b, c, d, e, f, g, h, a, wf_t, k_sha256[i + 15]); \
  }


    ROUND_STEP (0);
    ROUND_EXPAND();
    ROUND_STEP(16);

    ROUND_EXPAND();
    ROUND_STEP(32);

    ROUND_EXPAND();
    ROUND_STEP(48);

    hash[0] = SWAP(a + 0x6a09e667);
}

static void sha256_main_table_a(__global const unsigned int *offset, unsigned int idx, __global const unsigned int *pubkey_hash, __global unsigned int *hash) {
    unsigned int a = 0x6a09e667;
    unsigned int b = 0xbb67ae85;
    unsigned int c = 0x3c6ef372;
    unsigned int d = 0xa54ff53a;
    unsigned int e = 0x510e527f;
    unsigned int f = 0x9b05688c;
    unsigned int g = 0x1f83d9ab;
    unsigned int h = 0x5be0cd19;
    unsigned int offset_ = offset[0];
    unsigned int w0_t = 0x4d415353; // "MASS"
    unsigned int w1_t = SWAP(pubkey_hash[0]);
    unsigned int w2_t = SWAP(pubkey_hash[1]);
    unsigned int w3_t = SWAP(pubkey_hash[2]);
    unsigned int w4_t = SWAP(pubkey_hash[3]);
    unsigned int w5_t = SWAP(pubkey_hash[4]);
    unsigned int w6_t = SWAP(pubkey_hash[5]);
    unsigned int w7_t = SWAP(pubkey_hash[6]);
    unsigned int w8_t = SWAP(pubkey_hash[7]);
    unsigned int w9_t = SWAP(idx);
    //unsigned int w9_t = SWAP(idx + offset_);
    unsigned int wa_t = 0;
    unsigned int wb_t = 1 << 31;
    unsigned int wc_t = 0;
    unsigned int wd_t = 0;
    unsigned int we_t = 0;
    unsigned int wf_t = 352;

#define ROUND_EXPAND(i)                           \
  {                                                 \
    w0_t = SHA256_EXPAND (we_t, w9_t, w1_t, w0_t);  \
    w1_t = SHA256_EXPAND (wf_t, wa_t, w2_t, w1_t);  \
    w2_t = SHA256_EXPAND (w0_t, wb_t, w3_t, w2_t);  \
    w3_t = SHA256_EXPAND (w1_t, wc_t, w4_t, w3_t);  \
    w4_t = SHA256_EXPAND (w2_t, wd_t, w5_t, w4_t);  \
    w5_t = SHA256_EXPAND (w3_t, we_t, w6_t, w5_t);  \
    w6_t = SHA256_EXPAND (w4_t, wf_t, w7_t, w6_t);  \
    w7_t = SHA256_EXPAND (w5_t, w0_t, w8_t, w7_t);  \
    w8_t = SHA256_EXPAND (w6_t, w1_t, w9_t, w8_t);  \
    w9_t = SHA256_EXPAND (w7_t, w2_t, wa_t, w9_t);  \
    wa_t = SHA256_EXPAND (w8_t, w3_t, wb_t, wa_t);  \
    wb_t = SHA256_EXPAND (w9_t, w4_t, wc_t, wb_t);  \
    wc_t = SHA256_EXPAND (wa_t, w5_t, wd_t, wc_t);  \
    wd_t = SHA256_EXPAND (wb_t, w6_t, we_t, wd_t);  \
    we_t = SHA256_EXPAND (wc_t, w7_t, wf_t, we_t);  \
    wf_t = SHA256_EXPAND (wd_t, w8_t, w0_t, wf_t);  \
  }

#define ROUND_STEP(i)                                                                   \
  {                                                                                       \
    SHA256_STEP (F0, F1, a, b, c, d, e, f, g, h, w0_t, k_sha256[i +  0]); \
    SHA256_STEP (F0, F1, h, a, b, c, d, e, f, g, w1_t, k_sha256[i +  1]); \
    SHA256_STEP (F0, F1, g, h, a, b, c, d, e, f, w2_t, k_sha256[i +  2]); \
    SHA256_STEP (F0, F1, f, g, h, a, b, c, d, e, w3_t, k_sha256[i +  3]); \
    SHA256_STEP (F0, F1, e, f, g, h, a, b, c, d, w4_t, k_sha256[i +  4]); \
    SHA256_STEP (F0, F1, d, e, f, g, h, a, b, c, w5_t, k_sha256[i +  5]); \
    SHA256_STEP (F0, F1, c, d, e, f, g, h, a, b, w6_t, k_sha256[i +  6]); \
    SHA256_STEP (F0, F1, b, c, d, e, f, g, h, a, w7_t, k_sha256[i +  7]); \
    SHA256_STEP (F0, F1, a, b, c, d, e, f, g, h, w8_t, k_sha256[i +  8]); \
    SHA256_STEP (F0, F1, h, a, b, c, d, e, f, g, w9_t, k_sha256[i +  9]); \
    SHA256_STEP (F0, F1, g, h, a, b, c, d, e, f, wa_t, k_sha256[i + 10]); \
    SHA256_STEP (F0, F1, f, g, h, a, b, c, d, e, wb_t, k_sha256[i + 11]); \
    SHA256_STEP (F0, F1, e, f, g, h, a, b, c, d, wc_t, k_sha256[i + 12]); \
    SHA256_STEP (F0, F1, d, e, f, g, h, a, b, c, wd_t, k_sha256[i + 13]); \
    SHA256_STEP (F0, F1, c, d, e, f, g, h, a, b, we_t, k_sha256[i + 14]); \
    SHA256_STEP (F0, F1, b, c, d, e, f, g, h, a, wf_t, k_sha256[i + 15]); \
  }

    ROUND_STEP (0);

    ROUND_EXPAND();
    ROUND_STEP(16);

    ROUND_EXPAND();
    ROUND_STEP(32);

    ROUND_EXPAND();
    ROUND_STEP(48);

    b = SWAP(a + 0x6a09e667);
    if (b > HALF_32BIT) {
        b = ~b << 1 | 1;
    } else {
        b <<= 1;
    }
    if (b > offset_ && b < offset_ + 536870912) {
        hash[b - offset_] = idx;
    }

    //hash[0] = SWAP(a + 0x6a09e667);
}

__kernel void sha256_mass_table_a(__global const unsigned int *offset, __global const unsigned int *pubkey_hash, __global unsigned int *output_buf) {
    unsigned int idx = get_global_id(0);
    //output_buf[idx] = idx;
    //sha256_main_table_a(offset, idx, pubkey_hash, output_buf + idx);
    sha256_main_table_a(offset, idx, pubkey_hash, output_buf);
}

__kernel void sha256_mass_table_b(__global unsigned int *pubkey_hash, __global unsigned int *buf) {
    unsigned int offset = get_global_id(0) << 1;
    unsigned int x = buf[offset];
    unsigned int xp = buf[offset + 1];
    sha256_main_table_b(x, xp, pubkey_hash, buf + offset);
    sha256_main_table_b(xp, x, pubkey_hash, buf + offset + 1);
}

__kernel void sha256_mass(__global unsigned int *input_buf, __global unsigned int *output_buf) {
    unsigned int idx = get_global_id(0);
    sha256_main(input_buf + idx * 16, output_buf + idx * 8);
}

__kernel void clean_buffer(__global unsigned int *buf) {
    unsigned int idx = get_global_id(0);
    buf[idx] = 0;
}