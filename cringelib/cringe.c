#include "cringe.h"

static _uint32_t cringe_next = 1;

void cringe_srand(_uint32_t seed)
{
    cringe_next = seed;
}

_uint32_t cringe_rand()
{
    cringe_next = cringe_next * 1103515245 + 12345;
    return (_uint32_t)(cringe_next / 65536) % 32768;
}

_uint64_t cringe_rand64()
{
    return (_uint64_t)cringe_rand() | ((_uint64_t)cringe_rand() << 32);
}

void* cringe_memcpy(void* dst, const void* src, _uint64_t size)
{
    byte_t* d = (byte_t*)dst;
    const byte_t* s = (byte_t*)src;
    while (size--)
        *d++ = *s++;
    return dst;
}

void inc_uint128_t(uint128_t* x)
{
    x->low++;
    if (x->low == 0)
        x->high++;
}

int cringe_init(cringe_t* cringe)
{
    kuznyechik_init_ctx(&cringe->kuznyechik_ctx);
    cringe->counter.low = 0;
    cringe->counter.high = 0;

    _uint64_t key[2] = { cringe_rand64(), cringe_rand64() };
    kuznyechik_set_key(&cringe->kuznyechik_ctx, (byte_t*)key);

    return 0;
}

void cringe_generate_random_bytes(
    cringe_t* cringe,
    byte_t* out,
    _uint64_t size)
{
    byte_t buffer[16];
    _uint32_t i;
    _uint32_t size_div_16 = size >> 4; // size / 16
    _uint32_t size_mod_16 = size - (size_div_16 << 4); // size % 16

    for (i = 0; i < size_div_16; i++)
    {
        kuznyechik_encrypt(&cringe->kuznyechik_ctx, buffer, (byte_t*)(&cringe->counter));
        (void)cringe_memcpy(out + (i << 4), buffer, 16);
        inc_uint128_t(&cringe->counter);
    }

    if (size_mod_16 > 0)
    {
        kuznyechik_encrypt(&cringe->kuznyechik_ctx, buffer, (byte_t*)(&cringe->counter));
        (void)cringe_memcpy(out + (size - size_mod_16), buffer, size_mod_16);
        inc_uint128_t(&cringe->counter);
    }

}
