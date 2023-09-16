# cringe
Cryptographically Robust and Innovative Number Generator (Efficient)

```
   _____ _____  _____ _   _  _____ ______ 
  / ____|  __ \|_   _| \ | |/ ____|  ____|
 | |    | |__) | | | |  \| | |  __| |__   
 | |    |  _  /  | | | . ` | | |_ |  __|  
 | |____| | \ \ _| |_| |\  | |__| | |____ 
  \_____|_|  \_\_____|_| \_|\_____|______|

```

# cringelib
Cryptographically Secure Random Number Generator library based on the Kuznyechik block cipher algorithm. It is fast and secure. (GOST R 34.12-2015 aka [RFC 7801](https://www.rfc-editor.org/info/rfc7801))

## Usage

### Kuznyechik usage example
```c
#include <cringelib/kuznyechik.h>

byte_t buffer[16];

kuznyechik_ctx_t kuznyechik_ctx;
kuznyechik_init_ctx(&kuznyechik_ctx);
kuznyechik_set_key(&kuznyechik_ctx, key);

kuznyechik_encrypt(&kuznyechik_ctx, buffer, plaintext);
kuznyechik_decrypt(&kuznyechik_ctx, buffer, ciphertext);
```

### CRINGE usage example
```c
#include <cringelib/cringe.h>

byte_t bytes[BYTES_SIZE];

cringe_t cringe;
cringe_init(&cringe);

cringe_generate_random_bytes(&cringe, bytes, BYTES_SIZE);
```

# cringetool

```
Usage: -b <bytes> [-f <filename>] [-h] [-d <delimiter>]
   _____ _____  _____ _   _  _____ ______ 
  / ____|  __ \|_   _| \ | |/ ____|  ____|
 | |    | |__) | | | |  \| | |  __| |__   
 | |    |  _  /  | | | . ` | | |_ |  __|  
 | |____| | \ \ _| |_| |\  | |__| | |____ 
  \_____|_|  \_\_____|_| \_|\_____|______|
                                          
                                          
CRINGE - Cryptographically Robust and Innovative Number Generator (Efficient)
Cryptographically Secure PseudoRandom Number Generator (CSRNG)


    -h, --help                show this help message and exit

Options
    -b, --bytes=<int>         number of bytes (default: 128)
    -f, --filename=<str>      output file name
    -x, --hex                 output in hex
    -d, --delimiter=<str>     output hex delimiter
```

Provides a command-line tool for generating cryptographically secure random numbers. And save them to a file.