#ifndef CRINGE_H
#define CRINGE_H

#include "kuznyechik.h"

typedef struct uint128
{
    _uint64_t low;
    _uint64_t high;
} uint128_t;

typedef struct cringe
{
    kuznyechik_ctx_t kuznyechik_ctx;
    uint128_t counter;
} cringe_t;

int cringe_init(cringe_t* cringe);

void cringe_generate_random_bytes(
    cringe_t* cringe,
    byte_t* out,
    _uint64_t size);

#endif // !CRINGE_H
