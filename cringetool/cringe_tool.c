#include <stdio.h>
#include <stdlib.h>

#include "argparse/argparse.h"

#include <cringe.h>

void print_hex(FILE* fp, const byte_t* bytes, int n_bytes, const char* delimiter)
{
    int i;
    for (i = 0; i < n_bytes; i++)
    {
        if (i && i % 16 == 0)
            fprintf(fp, "\n");
        fprintf(fp, "%02x%s", bytes[i], delimiter ? delimiter : "");
    }
    fprintf(fp, "\n");
}

int main(int argc, const char* argv[])
{
    int n_bytes = 128;
    const char* filename = NULL;
    int hex = 0;
    const char* delimiter = NULL;
    struct argparse_option options[] =
    {
        OPT_HELP(),
        OPT_GROUP("Options"),
        OPT_INTEGER('b', "bytes", &n_bytes, "number of bytes (default: 128)", NULL, 0, 0),
        OPT_STRING('f', "filename", &filename, "output file name", NULL, 0, 0),
        OPT_BOOLEAN('x', "hex", &hex, "output in hex", NULL, 0, 0),
        OPT_STRING('d', "delimiter", &delimiter, "output hex delimiter", NULL, 0, 0),
        OPT_END(),
    };
    const char* usages[] =
    {
        "-b <bytes> [-f <filename>] [-h] [-d <delimiter>]",
        NULL
    };

    struct argparse argparse;
    argparse_init(&argparse, options, usages, 0);
    argparse_describe(&argparse,
    "   _____ _____  _____ _   _  _____ ______ \n"
    "  / ____|  __ \\|_   _| \\ | |/ ____|  ____|\n"
    " | |    | |__) | | | |  \\| | |  __| |__   \n"
    " | |    |  _  /  | | | . ` | | |_ |  __|  \n"
    " | |____| | \\ \\ _| |_| |\\  | |__| | |____ \n"
    "  \\_____|_|  \\_\\_____|_| \\_|\\_____|______|\n"
    "                                          \n"
    "                                          \n"
    "CRINGE - Cryptographically Robust and Innovative Number Generator (Efficient)\n"
    "Cryptographically Secure PseudoRandom Number Generator (CSRNG)\n", NULL);
    argc = argparse_parse(&argparse, argc, argv);

    if (n_bytes <= 0)
        n_bytes = 128;

    byte_t* bytes = (byte_t*)malloc(n_bytes);
    if (!bytes)
        return 1;
    
    cringe_t cringe;
    cringe_init(&cringe);
    cringe_generate_random_bytes(&cringe, (byte_t*)bytes, n_bytes);    

    if (filename)
    {
        FILE* f = fopen(filename, hex ? "w" : "wb"); 
        
        if (!f)
        {
            free(bytes);
            return 1;
        }

        if (hex)
            print_hex(f, bytes, n_bytes, delimiter);
        else
            fwrite(bytes, 1, n_bytes, f);
        
        fclose(f);
    }
    else
        print_hex(stdout, bytes, n_bytes, delimiter);

    free(bytes);

    return 0;
}
