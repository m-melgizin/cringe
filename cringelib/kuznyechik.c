#include "kuznyechik.h"

#if SSE2_AVAILABLE
// MMX intrinsics
#include <mmintrin.h>
// SSE2 intrinsics
#include <emmintrin.h>
#endif

// Pi' substitution table for nonlinear mapping as defined in section 4.1.
// of the reference document.
//
// Pi' = (Pi'(0), Pi'(1), ... , Pi'(255))
static const byte_t kuznyechik_pi[256] =
{
    0xfc, 0xee, 0xdd, 0x11, 0xcf, 0x6e, 0x31, 0x16, 0xfb, 0xc4, 0xfa,
    0xda, 0x23, 0xc5, 0x04, 0x4d, 0xe9, 0x77, 0xf0, 0xdb, 0x93, 0x2e,
    0x99, 0xba, 0x17, 0x36, 0xf1, 0xbb, 0x14, 0xcd, 0x5f, 0xc1, 0xf9,
    0x18, 0x65, 0x5a, 0xe2, 0x5c, 0xef, 0x21, 0x81, 0x1c, 0x3c, 0x42,
    0x8b, 0x01, 0x8e, 0x4f, 0x05, 0x84, 0x02, 0xae, 0xe3, 0x6a, 0x8f,
    0xa0, 0x06, 0x0b, 0xed, 0x98, 0x7f, 0xd4, 0xd3, 0x1f, 0xeb, 0x34,
    0x2c, 0x51, 0xea, 0xc8, 0x48, 0xab, 0xf2, 0x2a, 0x68, 0xa2, 0xfd,
    0x3a, 0xce, 0xcc, 0xb5, 0x70, 0x0e, 0x56, 0x08, 0x0c, 0x76, 0x12,
    0xbf, 0x72, 0x13, 0x47, 0x9c, 0xb7, 0x5d, 0x87, 0x15, 0xa1, 0x96, 
    0x29, 0x10, 0x7b, 0x9a, 0xc7, 0xf3, 0x91, 0x78, 0x6f, 0x9d, 0x9e,
    0xb2, 0xb1, 0x32, 0x75, 0x19, 0x3d, 0xff, 0x35, 0x8a, 0x7e, 0x6d,
    0x54, 0xc6, 0x80, 0xc3, 0xbd, 0x0d, 0x57, 0xdf, 0xf5, 0x24, 0xa9,
    0x3e, 0xa8, 0x43, 0xc9, 0xd7, 0x79, 0xd6, 0xf6, 0x7c, 0x22, 0xb9,
    0x03, 0xe0, 0x0f, 0xec, 0xde, 0x7a, 0x94, 0xb0, 0xbc, 0xdc, 0xe8,
    0x28, 0x50, 0x4e, 0x33, 0x0a, 0x4a, 0xa7, 0x97, 0x60, 0x73, 0x1e,
    0x00, 0x62, 0x44, 0x1a, 0xb8, 0x38, 0x82, 0x64, 0x9f, 0x26, 0x41,
    0xad, 0x45, 0x46, 0x92, 0x27, 0x5e, 0x55, 0x2f, 0x8c, 0xa3, 0xa5,
    0x7d, 0x69, 0xd5, 0x95, 0x3b, 0x07, 0x58, 0xb3, 0x40, 0x86, 0xac,
    0x1d, 0xf7, 0x30, 0x37, 0x6b, 0xe4, 0x88, 0xd9, 0xe7, 0x89, 0xe1,
    0x1b, 0x83, 0x49, 0x4c, 0x3f, 0xf8, 0xfe, 0x8d, 0x53, 0xaa, 0x90,
    0xca, 0xd8, 0x85, 0x61, 0x20, 0x71, 0x67, 0xa4, 0x2d, 0x2b, 0x09,
    0x5b, 0xcb, 0x9b, 0x25, 0xd0, 0xbe, 0xe5, 0x6c, 0x52, 0x59, 0xa6,
    0x74, 0xd2, 0xe6, 0xf4, 0xb4, 0xc0, 0xd1, 0x66, 0xaf, 0xc2, 0x39,
    0x4b, 0x63, 0xb6
};

// Inversed Pi' substitution box.
//
// Pi^(-1)' = (Pi^(-1)'(0), Pi^(-1)'(1), ... , Pi^(-1)'(255))
static const byte_t kuznyechik_pi_inv[256] =
{
    0xa5, 0x2d, 0x32, 0x8f, 0x0e, 0x30, 0x38, 0xc0, 0x54, 0xe6, 0x9e,
    0x39, 0x55, 0x7e, 0x52, 0x91, 0x64, 0x03, 0x57, 0x5a, 0x1c, 0x60, 
    0x07, 0x18, 0x21, 0x72, 0xa8, 0xd1, 0x29, 0xc6, 0xa4, 0x3f, 0xe0,
    0x27, 0x8d, 0x0c, 0x82, 0xea, 0xae, 0xb4, 0x9a, 0x63, 0x49, 0xe5,
    0x42, 0xe4, 0x15, 0xb7, 0xc8, 0x06, 0x70, 0x9d, 0x41, 0x75, 0x19,
    0xc9, 0xaa, 0xfc, 0x4d, 0xbf, 0x2a, 0x73, 0x84, 0xd5, 0xc3, 0xaf,
    0x2b, 0x86, 0xa7, 0xb1, 0xb2, 0x5b, 0x46, 0xd3, 0x9f, 0xfd, 0xd4,
    0x0f, 0x9c, 0x2f, 0x9b, 0x43, 0xef, 0xd9, 0x79, 0xb6, 0x53, 0x7f,
    0xc1, 0xf0, 0x23, 0xe7, 0x25, 0x5e, 0xb5, 0x1e, 0xa2, 0xdf, 0xa6,
    0xfe, 0xac, 0x22, 0xf9, 0xe2, 0x4a, 0xbc, 0x35, 0xca, 0xee, 0x78,
    0x05, 0x6b, 0x51, 0xe1, 0x59, 0xa3, 0xf2, 0x71, 0x56, 0x11, 0x6a,
    0x89, 0x94, 0x65, 0x8c, 0xbb, 0x77, 0x3c, 0x7b, 0x28, 0xab, 0xd2,
    0x31, 0xde, 0xc4, 0x5f, 0xcc, 0xcf, 0x76, 0x2c, 0xb8, 0xd8, 0x2e,
    0x36, 0xdb, 0x69, 0xb3, 0x14, 0x95, 0xbe, 0x62, 0xa1, 0x3b, 0x16,
    0x66, 0xe9, 0x5c, 0x6c, 0x6d, 0xad, 0x37, 0x61, 0x4b, 0xb9, 0xe3,
    0xba, 0xf1, 0xa0, 0x85, 0x83, 0xda, 0x47, 0xc5, 0xb0, 0x33, 0xfa,
    0x96, 0x6f, 0x6e, 0xc2, 0xf6, 0x50, 0xff, 0x5d, 0xa9, 0x8e, 0x17,
    0x1b, 0x97, 0x7d, 0xec, 0x58, 0xf7, 0x1f, 0xfb, 0x7c, 0x09, 0x0d,
    0x7a, 0x67, 0x45, 0x87, 0xdc, 0xe8, 0x4f, 0x1d, 0x4e, 0x04, 0xeb,
    0xf8, 0xf3, 0x3e, 0x3d, 0xbd, 0x8a, 0x88, 0xdd, 0xcd, 0x0b, 0x13,
    0x98, 0x02, 0x93, 0x80, 0x90, 0xd0, 0x24, 0x34, 0xcb, 0xed, 0xf4,
    0xce, 0x99, 0x10, 0x44, 0x40, 0x92, 0x3a, 0x01, 0x26, 0x12, 0x1a,
    0x48, 0x68, 0xf5, 0x81, 0x8b, 0xc7, 0xd6, 0x20, 0x0a, 0x08, 0x00,
    0x4c, 0xd7, 0x74
};

// Vector of constants used in linear transformation as defined in section
// 4.2. of the reference document.
static const byte_t kuznyechik_linear_vector[16] =
{
    0x94, 0x20, 0x85, 0x10, 0xc2, 0xc0, 0x01, 0xfb, 0x01, 0xc0, 0xc2,
    0x10, 0x85, 0x20, 0x94, 0x01
};

static byte_t gf256_mul_fast(gf256_ctx_t* gf256_ctx, byte_t a, byte_t b)
{
    _uint32_t c;

    if (a == 0 || b == 0)
        return 0;

    c = gf256_ctx->gf_multtable_log[a] + gf256_ctx->gf_multtable_log[b];

    return gf256_ctx->gf_multtable_exp[c % 255];
}

static byte_t gf256_mul_slow(gf256_ctx_t* gf256_ctx, byte_t a, byte_t b)
{
    byte_t c = 0;

    while (b)
    {
        if (b & 1)
            c ^= a;
        a = (a << 1) ^ (a & 0x80 ? 0xc3 : 0x00);
        b >>= 1;
    }
    return c;
}

static void gf256_init_tables(gf256_ctx_t* gf256_ctx)
{
    _uint32_t c = 1;
    _uint32_t i;

    for (i = 0; i < 256; i++)
    {
        gf256_ctx->gf_multtable_log[c] = i;
        gf256_ctx->gf_multtable_exp[i] = c;
        c = gf256_mul_slow(gf256_ctx, c, 0x03);
    }
}

static void kuznyechik_linear(kuznyechik_ctx_t* kuznyechik_ctx, byte_t* a)
{
    byte_t c;
    int i;
    int j;

    for (i = 16; i > 0; i--)
    {
        c = a[15];
        for (j = 14; j >= 0; j--)
        {
            a[j + 1] = a[j];
            c ^= gf256_mul_fast(
                &kuznyechik_ctx->gf256_ctx, a[j], kuznyechik_linear_vector[j]);
        }
        a[0] = c;
    }
}

static void kuznyechik_linear_inv(kuznyechik_ctx_t* kuznyechik_ctx, byte_t* a)
{
    byte_t c;
    int i;
    int j;

    for (i = 16; i > 0; i--)
    {
        c = a[0];
        for (j = 0; j < 15; j++)
        {
            a[j] = a[j + 1];
            c ^= gf256_mul_fast(
                &kuznyechik_ctx->gf256_ctx, a[j], kuznyechik_linear_vector[j]);
        }
        a[15] = c;
    }
}

#if SSE2_AVAILABLE
ALIGN(16) const byte_t sse2_bitmask[16] =
{
    0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff,
    0x00, 0xff, 0x00, 0xff,
};
#endif

// Platform-dependent loading a working state from an array of bytes and
// storage of output data in an output buffer. If SSE optimization enabled,
// data are loaded into a single 128-bit vector, when not, a vector of two
// 64-bit integers is used instead.
#if SSE2_AVAILABLE
#define LOAD(out, in) \
    out = *((__m128i*)in);
#define STORE(out, in) \
    *((__m128i*)out) = in;
#else
#define LOAD(out, in)            \
    out[0] = ((_uint64_t*)in)[0]; \
    out[1] = ((_uint64_t*)in)[1];
#define STORE(out, in)           \
    ((_uint64_t*)out)[0] = in[0]; \
    ((_uint64_t*)out)[1] = in[1];
#endif

// Applying a lookup table - version optimized for SSE2. There things
// are a little bit tricky. SSE2 is only able to extract 16-bit values
// from a vector. For that reason, we have to split our vector and
// obtain the values by applying a bit-mask and shift to right (for
// even values).
#if SSE2_AVAILABLE
#define XOR_LOOKUP(T, a, b)                                                            \
    addr1 = _mm_and_si128(*(__m128i*)sse2_bitmask, a);                                 \
    addr2 = _mm_andnot_si128(*(__m128i*)sse2_bitmask, a);                              \
                                                                                       \
    addr1 = _mm_srli_epi16(addr1, 4);                                                  \
    addr2 = _mm_slli_epi16(addr2, 4);                                                  \
                                                                                       \
    b = _mm_load_si128((const void*)(T + _mm_extract_epi16(addr2, 0)));                \
    b = _mm_xor_si128(b, *(const __m128i*)(T + _mm_extract_epi16(addr1, 0) + 0x1000)); \
    b = _mm_xor_si128(b, *(const __m128i*)(T + _mm_extract_epi16(addr2, 1) + 0x2000)); \
    b = _mm_xor_si128(b, *(const __m128i*)(T + _mm_extract_epi16(addr1, 1) + 0x3000)); \
    b = _mm_xor_si128(b, *(const __m128i*)(T + _mm_extract_epi16(addr2, 2) + 0x4000)); \
    b = _mm_xor_si128(b, *(const __m128i*)(T + _mm_extract_epi16(addr1, 2) + 0x5000)); \
    b = _mm_xor_si128(b, *(const __m128i*)(T + _mm_extract_epi16(addr2, 3) + 0x6000)); \
    b = _mm_xor_si128(b, *(const __m128i*)(T + _mm_extract_epi16(addr1, 3) + 0x7000)); \
    b = _mm_xor_si128(b, *(const __m128i*)(T + _mm_extract_epi16(addr2, 4) + 0x8000)); \
    b = _mm_xor_si128(b, *(const __m128i*)(T + _mm_extract_epi16(addr1, 4) + 0x9000)); \
    b = _mm_xor_si128(b, *(const __m128i*)(T + _mm_extract_epi16(addr2, 5) + 0xa000)); \
    b = _mm_xor_si128(b, *(const __m128i*)(T + _mm_extract_epi16(addr1, 5) + 0xb000)); \
    b = _mm_xor_si128(b, *(const __m128i*)(T + _mm_extract_epi16(addr2, 6) + 0xc000)); \
    b = _mm_xor_si128(b, *(const __m128i*)(T + _mm_extract_epi16(addr1, 6) + 0xd000)); \
    b = _mm_xor_si128(b, *(const __m128i*)(T + _mm_extract_epi16(addr2, 7) + 0xe000)); \
    b = _mm_xor_si128(b, *(const __m128i*)(T + _mm_extract_epi16(addr1, 7) + 0xf000));

// Applying a lookup table - portable version. If the target machine
// does not have any SSE instruction set, we use this fallback code.
#else
#define XOR_LOOKUP_HALF(T, a, b, i)                 \
    b[i]  = T[ 0][(((byte_t*)&a[0])[0]) & 0xff][i]; \
    b[i] ^= T[ 1][(((byte_t*)&a[0])[1]) & 0xff][i]; \
    b[i] ^= T[ 2][(((byte_t*)&a[0])[2]) & 0xff][i]; \
    b[i] ^= T[ 3][(((byte_t*)&a[0])[3]) & 0xff][i]; \
    b[i] ^= T[ 4][(((byte_t*)&a[0])[4]) & 0xff][i]; \
    b[i] ^= T[ 5][(((byte_t*)&a[0])[5]) & 0xff][i]; \
    b[i] ^= T[ 6][(((byte_t*)&a[0])[6]) & 0xff][i]; \
    b[i] ^= T[ 7][(((byte_t*)&a[0])[7]) & 0xff][i]; \
    b[i] ^= T[ 8][(((byte_t*)&a[1])[0]) & 0xff][i]; \
    b[i] ^= T[ 9][(((byte_t*)&a[1])[1]) & 0xff][i]; \
    b[i] ^= T[10][(((byte_t*)&a[1])[2]) & 0xff][i]; \
    b[i] ^= T[11][(((byte_t*)&a[1])[3]) & 0xff][i]; \
    b[i] ^= T[12][(((byte_t*)&a[1])[4]) & 0xff][i]; \
    b[i] ^= T[13][(((byte_t*)&a[1])[5]) & 0xff][i]; \
    b[i] ^= T[14][(((byte_t*)&a[1])[6]) & 0xff][i]; \
    b[i] ^= T[15][(((byte_t*)&a[1])[7]) & 0xff][i]

#define XOR_LOOKUP(T, a, b)      \
    XOR_LOOKUP_HALF(T, a, b, 0); \
    XOR_LOOKUP_HALF(T, a, b, 1)
#endif

#if SSE2_AVAILABLE
#define X(a, k) \
    a = _mm_xor_si128(a, *((__m128i*)&k))
#else
#define X(a, k)   \
    a[0] ^= k[0]; \
    a[1] ^= k[1]
#endif

#define SL(a, b) \
    XOR_LOOKUP(kuznyechik_ctx->T_SL, a, b)

#define IL(a, b) \
    XOR_LOOKUP(kuznyechik_ctx->T_IL, a, b)

#define ISL(a, b) \
    XOR_LOOKUP(kuznyechik_ctx->T_ISL, a, b)

#define IS(a)                             \
    do                                    \
    {                                     \
        byte_t* c = ((byte_t*)&a);        \
        c[ 0] = kuznyechik_pi_inv[c[ 0]]; \
        c[ 1] = kuznyechik_pi_inv[c[ 1]]; \
        c[ 2] = kuznyechik_pi_inv[c[ 2]]; \
        c[ 3] = kuznyechik_pi_inv[c[ 3]]; \
        c[ 4] = kuznyechik_pi_inv[c[ 4]]; \
        c[ 5] = kuznyechik_pi_inv[c[ 5]]; \
        c[ 6] = kuznyechik_pi_inv[c[ 6]]; \
        c[ 7] = kuznyechik_pi_inv[c[ 7]]; \
        c[ 8] = kuznyechik_pi_inv[c[ 8]]; \
        c[ 9] = kuznyechik_pi_inv[c[ 9]]; \
        c[10] = kuznyechik_pi_inv[c[10]]; \
        c[11] = kuznyechik_pi_inv[c[11]]; \
        c[12] = kuznyechik_pi_inv[c[12]]; \
        c[13] = kuznyechik_pi_inv[c[13]]; \
        c[14] = kuznyechik_pi_inv[c[14]]; \
        c[15] = kuznyechik_pi_inv[c[15]]; \
    } while (0)

static void kuznyechik_wipe_keys(kuznyechik_keys_t* keys)
{
    int i;

    for (i = 9; i >= 0; i--)
    {
        keys->ek[i][0] = 0x0000000000000000;
        keys->ek[i][1] = 0x0000000000000000;
        keys->dk[i][0] = 0x0000000000000000;
        keys->dk[i][1] = 0x0000000000000000;
    }
}

int kuznyechik_init_ctx(kuznyechik_ctx_t* kuznyechik_ctx)
{
    _uint32_t i;
    _uint32_t j;
    byte_t* ptr;
    #if SSE2_AVAILABLE
    _uint32_t pos = 0;
    #endif

    gf256_init_tables(&kuznyechik_ctx->gf256_ctx);

    byte_t* t_sl = (byte_t*)kuznyechik_ctx->T_SL;
    byte_t* t_il = (byte_t*)kuznyechik_ctx->T_IL;
    byte_t* t_isl = (byte_t*)kuznyechik_ctx->T_ISL;

    for (i = 0; i < 16 * 256 * 16; i++)
    {
        t_sl[i] = 0;
        t_il[i] = 0;
        t_isl[i] = 0;
    }

    for (i = 0; i < 16; i++)
    {
        for (j = 0; j < 256; j++)
        {

            // Pi' substitution and linear transformation
            #if SSE2_AVAILABLE
            ptr = kuznyechik_ctx->T_SL + pos;
            #else
            ptr = (byte_t*)kuznyechik_ctx->T_SL[i][j];
            #endif

            ptr[i] = kuznyechik_pi[j];
            kuznyechik_linear(kuznyechik_ctx, ptr);

            // Inversed Pi' substitution and inversed linear transformation
            #if SSE2_AVAILABLE
            ptr = kuznyechik_ctx->T_ISL + pos;
            #else
            ptr = (byte_t*)kuznyechik_ctx->T_ISL[i][j];
            #endif

            ptr[i] = kuznyechik_pi_inv[j];
            kuznyechik_linear_inv(kuznyechik_ctx, ptr);

            // Inversed linear transformation
            #if SSE2_AVAILABLE
            ptr = kuznyechik_ctx->T_IL + pos;
            #else
            ptr = (byte_t*)kuznyechik_ctx->T_IL[i][j];
            #endif

            ptr[i] = j;
            kuznyechik_linear_inv(kuznyechik_ctx, ptr);

            #if SSE2_AVAILABLE
            pos += 16;
            #endif
        }
    }

    return 0;
}

int kuznyechik_set_key(kuznyechik_ctx_t* kuznyechik_ctx, const byte_t *key)
{
    _uint32_t i;
    _uint32_t j;
    _uint64_t x[4];
    _uint64_t z[2];
    byte_t c[16];

    kuznyechik_wipe_keys(&kuznyechik_ctx->keys);

    x[0] = ((_uint64_t*)key)[0];
    x[1] = ((_uint64_t*)key)[1];
    x[2] = ((_uint64_t*)key)[2];
    x[3] = ((_uint64_t*)key)[3];

    kuznyechik_ctx->keys.ek[0][0] = x[0];
    kuznyechik_ctx->keys.ek[0][1] = x[1];
    kuznyechik_ctx->keys.ek[1][0] = x[2];
    kuznyechik_ctx->keys.ek[1][1] = x[3];

    for (i = 1; i <= 32; i++)
    {
        ((_uint64_t*)c)[0] = 0;
        ((_uint64_t*)c)[1] = 0;
        c[15] = i;
        kuznyechik_linear(kuznyechik_ctx, c);

        z[0] = x[0] ^ ((_uint64_t*)c)[0];
        z[1] = x[1] ^ ((_uint64_t*)c)[1];

        for (j = 0; j < 16; j++)
            ((byte_t*)z)[j] = kuznyechik_pi[((byte_t*)z)[j]];

        kuznyechik_linear(kuznyechik_ctx, (byte_t*)z);

        z[0] ^= x[2];
        z[1] ^= x[3];

        x[2] = x[0];
        x[3] = x[1];

        x[0] = z[0];
        x[1] = z[1];

        if ((i & 7) == 0)
        {
            kuznyechik_ctx->keys.ek[(i >> 2)][0] = x[0];
            kuznyechik_ctx->keys.ek[(i >> 2)][1] = x[1];
            kuznyechik_ctx->keys.ek[(i >> 2) + 1][0] = x[2];
            kuznyechik_ctx->keys.ek[(i >> 2) + 1][1] = x[3];
        }
    }

    for (i = 0; i < 10; i++)
    {
        kuznyechik_ctx->keys.dk[i][0] = kuznyechik_ctx->keys.ek[i][0];
        kuznyechik_ctx->keys.dk[i][1] = kuznyechik_ctx->keys.ek[i][1];
        if (i > 0)
            kuznyechik_linear_inv(kuznyechik_ctx, (byte_t*)&(kuznyechik_ctx->keys.dk[i]));
    }

    return 0;
}

void kuznyechik_encrypt(kuznyechik_ctx_t* kuznyechik_ctx, byte_t* out, const byte_t* in)
{
#if SSE2_AVAILABLE
    __m128i a;
    __m128i b;
    __m128i addr1;
    __m128i addr2;
#else
    _uint64_t a[2];
    _uint64_t b[2];
#endif

    LOAD(a, in);

    X(a, kuznyechik_ctx->keys.ek[0]);
    SL(a, b);
    X(b, kuznyechik_ctx->keys.ek[1]);
    SL(b, a);
    X(a, kuznyechik_ctx->keys.ek[2]);
    SL(a, b);
    X(b, kuznyechik_ctx->keys.ek[3]);
    SL(b, a);
    X(a, kuznyechik_ctx->keys.ek[4]);
    SL(a, b);
    X(b, kuznyechik_ctx->keys.ek[5]);
    SL(b, a);
    X(a, kuznyechik_ctx->keys.ek[6]);
    SL(a, b);
    X(b, kuznyechik_ctx->keys.ek[7]);
    SL(b, a);
    X(a, kuznyechik_ctx->keys.ek[8]);
    SL(a, b);
    X(b, kuznyechik_ctx->keys.ek[9]);

    STORE(out, b);
}

void kuznyechik_decrypt(kuznyechik_ctx_t* kuznyechik_ctx, byte_t *out, const byte_t *in)
{
#if SSE2_AVAILABLE
    __m128i a;
    __m128i b;
    __m128i addr1;
    __m128i addr2;
#else
    _uint64_t a[2];
    _uint64_t b[2];
#endif

    LOAD(a, in);

    IL(a, b);
    X(b, kuznyechik_ctx->keys.dk[9]);
    ISL(b, a);
    X(a, kuznyechik_ctx->keys.dk[8]);
    ISL(a, b);
    X(b, kuznyechik_ctx->keys.dk[7]);
    ISL(b, a);
    X(a, kuznyechik_ctx->keys.dk[6]);
    ISL(a, b);
    X(b, kuznyechik_ctx->keys.dk[5]);
    ISL(b, a);
    X(a, kuznyechik_ctx->keys.dk[4]);
    ISL(a, b);
    X(b, kuznyechik_ctx->keys.dk[3]);
    ISL(b, a);
    X(a, kuznyechik_ctx->keys.dk[2]);
    ISL(a, b);
    X(b, kuznyechik_ctx->keys.dk[1]);
    IS(b);
    X(b, kuznyechik_ctx->keys.dk[0]);

    STORE(out, b);
}
