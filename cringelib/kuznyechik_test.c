#include "kuznyechik.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define BENCH_BUFSIZE 1024 * 1024 * 5
#define BENCH_ITER 5

static int do_test();
static void do_benchmarks();

int main(int argc, const char **argv)
{
    int status = do_test();
    switch (status)
    {
    case 0:
        printf("Test passed!\n");
        break;
    case 1:
        fprintf(stderr, "Test failed (encrypt)!\n");
        break;
    case 2:
        fprintf(stderr, "Test failed (decrypt)!\n");
    default:
        fprintf(stderr, "Test failed!\n");
        break;
    }

    if (status != 0)
        return EXIT_FAILURE;

    printf("\nBenchmarks:\n");
    do_benchmarks();

    return EXIT_SUCCESS;
}

// Test vectors are from the reference document.
ALIGN(16) static const byte_t key[32] = {
    0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
    0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
    0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef
};

ALIGN(16) static const byte_t plaintext[16] = {
    0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x00,
    0xff, 0xee, 0xdd, 0xcc, 0xbb, 0xaa, 0x99, 0x88
};

ALIGN(16) static const byte_t ciphertext[16] = {
    0x7f, 0x67, 0x9d, 0x90, 0xbe, 0xbc, 0x24, 0x30,
    0x5a, 0x46, 0x8d, 0x42, 0xb9, 0xd4, 0xed, 0xcd
};

static void print_block(const byte_t *block, const char *prefix)
{
    unsigned int i;

    printf("%s ", prefix);
    for (i = 0; i < 16; i++)
        printf("%02x", block[i]);
    putchar('\n');
}

static int do_test()
{
    ALIGN(16) byte_t buffer[16];

    kuznyechik_ctx_t kuznyechik_ctx;
    kuznyechik_init_ctx(&kuznyechik_ctx);
    kuznyechik_set_key(&kuznyechik_ctx, key);

    kuznyechik_encrypt(&kuznyechik_ctx, buffer, plaintext);
    print_block(buffer, "IMPL C:");
    print_block(ciphertext, "DOCS C:");
    if (memcmp(ciphertext, buffer, 16) != 0)
        return 1;

    kuznyechik_decrypt(&kuznyechik_ctx, buffer, buffer);
    print_block(buffer, "IMPL P:");
    print_block(plaintext, "DOCS P:");
    if (memcmp(plaintext, buffer, 16) != 0)
        return 2;

    return 0;
}

static double do_benchmark()
{
    kuznyechik_ctx_t kuznyechik_ctx;
    kuznyechik_init_ctx(&kuznyechik_ctx);
    ALIGN(16) byte_t buffer[BENCH_BUFSIZE];
    byte_t* ptr;
    byte_t* eptr;
    clock_t stime;

    ptr = buffer;
    eptr = buffer + sizeof(buffer);

    // Randomize the buffer
    while (ptr < eptr)
        *ptr++ = rand();

    ptr = buffer;
    kuznyechik_set_key(&kuznyechik_ctx, key);

    // Encrypt whole buffer
    stime = clock();
    while (ptr < eptr)
    {
        kuznyechik_encrypt(&kuznyechik_ctx, ptr, ptr);
        ptr += 16;
    }

    return (double)(clock() - stime) / CLOCKS_PER_SEC;
}

static void do_benchmarks()
{
    int i;
    double elapsed;
    double mbs;

    srand(time(NULL));

    for (i = 0; i < BENCH_ITER; i++)
    {
        elapsed = do_benchmark();
        mbs = (double)(BENCH_BUFSIZE) / (double)elapsed / 1024 / 1024;
		fprintf(stdout, "[%d/%d] Encryption speed: %f MB/s\n",
			i + 1, BENCH_ITER, mbs);
    }
}
