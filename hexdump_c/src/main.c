#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <ctype.h>

#define CHUNK_SIZE 16

int main(int argc, char *argv[]) {
    bool verbose = false;
    long skip = 0;
    long length = -1;
    int opt;
    while ((opt = getopt(argc, argv, "s:n:vh")) != -1) {
        switch (opt) {
            case 's':
                skip = atol(optarg);
                if (skip <= 0) {
                    fprintf(stderr,"Negative skip is not allowed");
                    return 1;
                }
                break;
            case 'n':
                length = atol(optarg);
                if (length <= 0) {
                    fprintf(stderr, "Negative length is not allowed");
                    return 1;
                }
                break;
            case 'v':
                verbose = true;
                break;
            case 'h':
                printf("hexdump_c [-s <offset>] [-n <length>] [-h (this message)] [-v (verbose)] [filename]\n");
                return 0;
            default:
                // already handled by getopt
            return 1;
        }
    }
    
    if (optind >= argc) {
        fprintf(stderr, "Expected filename\n");
        return 1;
    }

    const char *filename = argv[optind];
    if (verbose) {
        printf("skip=%ld, length=%ld, filename=%s\n",
            skip, length, filename);
    }

    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        fprintf(stderr, "Unable to open file\n");
        return 1;
    }

    if (skip != 0) {
        fseek(fp, skip, SEEK_SET);
    }

    unsigned long offset = ftell(fp);
    unsigned long total_read = 0;
    unsigned char buf[CHUNK_SIZE];
    size_t n = 0;
    for (;;) {
        size_t to_read = CHUNK_SIZE;
        
        if (length >= 0 && total_read + CHUNK_SIZE > (unsigned long)length) {
            to_read = length - total_read;
        }
        
        n = fread(buf, 1, to_read, fp);
        if (n == 0) break;
        total_read += n;

        printf("%08lx  ", offset);   
        for (size_t i = 0; i < CHUNK_SIZE; i++) {
            if (i == CHUNK_SIZE/2) printf(" ");
            (i < n) ? printf("%02x ", buf[i]) : printf("   ");
        }
        printf(" |");
        for (size_t i = 0; i < n; i++) {
            isprint(buf[i]) ? printf("%c", buf[i]) : printf("."); 
        }
        printf("|\n");

        
        offset = ftell(fp);
        if (length >= 0 && total_read >= (unsigned long)length) break;
    }
    printf("%08lx\n", offset);
    fclose(fp);
    return 0;
}
