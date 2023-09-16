#include <stdio.h>

#include "cringe.h"

#define BYTES_SIZE_0 16
#define BYTES_SIZE_1 5
#define BYTES_SIZE_2 20

void do_test();

int main(int argc, char* argv[])
{
    do_test();

    return 0;
}

void print_bytes(const byte_t* bytes, _uint64_t size, const char* prefix)
{
    printf("%s\n", prefix);
    _uint64_t i;
    for (i = 0; i < size; i++)
    {
        if (i && i % 16 == 0)
            printf("\n");
        printf("%02x ", bytes[i]);
    }
    printf("\n");
}

void do_test()
{
    byte_t bytes0[BYTES_SIZE_0] = { 0 };
    byte_t bytes1[BYTES_SIZE_1] = { 0 };
    byte_t bytes2[BYTES_SIZE_2] = { 0 };
    cringe_t cringe;

    cringe_init(&cringe);

    cringe_generate_random_bytes(&cringe, bytes0, BYTES_SIZE_0);
    print_bytes(bytes0, BYTES_SIZE_0, "bytes0[16]");
    
    putchar('\n');

    cringe_generate_random_bytes(&cringe, bytes1, BYTES_SIZE_1);
    print_bytes(bytes1, BYTES_SIZE_1, "bytes1[5]");
    
    putchar('\n');

    cringe_generate_random_bytes(&cringe, bytes2, BYTES_SIZE_2);
    print_bytes(bytes2, BYTES_SIZE_2, "bytes2[20]");
}
