#ifndef KUZNYECHIK_H
#define KUZNYECHIK_H

// #if !defined(__STD_VERSION__) || (__STD_VERSION__ < 199901L)
// #error At least C99 is required
// #endif

#ifndef DISABLE_SSE2
#define TRY_SSE2 1
#endif

#if defined(TRY_SSE2)
    #if defined(__SSE2__) || defined(_MSC_VER) && (defined(_M_X64) || defined(_M_IX86) && _M_IX86_FP >= 2)
        #define SSE2_AVAILABLE 1
    #else
        #define SSE2_AVAILABLE 0
        #warning SSE2 not available
    #endif
#else
    #define SSE2_AVAILABLE 0
#endif

typedef unsigned char byte_t;
typedef unsigned long _uint32_t;
typedef unsigned long long _uint64_t;

#ifndef ALIGN
#define ALIGN(n) __attribute__((aligned(n)))
#endif

typedef struct gf256_ctx
{
    byte_t gf_multtable_log[256];
    byte_t gf_multtable_exp[256];
} gf256_ctx_t;

typedef struct kuznyechik_keys
{
    _uint64_t ek[10][2];
    _uint64_t dk[10][2];
} kuznyechik_keys_t;

typedef struct kuznyechik_ctx
{
    gf256_ctx_t gf256_ctx;

#if SSE2_AVAILABLE
    ALIGN(16) byte_t T_SL[16 * 256 * 16];
    ALIGN(16) byte_t T_IL[16 * 256 * 16];
    ALIGN(16) byte_t T_ISL[16 * 256 * 16];
#else
    ALIGN(16) _uint64_t T_SL[16][256][2];
    ALIGN(16) _uint64_t T_IL[16][256][2];
    ALIGN(16) _uint64_t T_ISL[16][256][2];
#endif

    ALIGN(16) kuznyechik_keys_t keys;
} kuznyechik_ctx_t;

int kuznyechik_init_ctx(kuznyechik_ctx_t* kuznyechik_ctx);

int kuznyechik_set_key(
    kuznyechik_ctx_t* kuznyechik_ctx,
    const byte_t *key);

void kuznyechik_encrypt(
    kuznyechik_ctx_t* kuznyechik_ctx,
    byte_t* out,
    const byte_t* in);

void kuznyechik_decrypt(
    kuznyechik_ctx_t* kuznyechik_ctx,
    byte_t* out,
    const byte_t* in);

#endif // !KUZNYECHIK_H
