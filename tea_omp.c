// Tiny Encryption Algorithm
// Adapated from Wikipedia, which was in turn adapted from http://citeseer.ist.psu.edu/viewdoc/download;jsessionid=C08E8409ADF484095568965A1EBF3E5E?doi=10.1.1.45.281&rep=rep1&type=pdf
// Written by Jackson Goerner (jgoe0001@student.monash.edu)

#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <omp.h>

const uint32_t const_key[4] = {5678123, 21673, 656172392, 9872};

void encrypt_2 (uint32_t v[2], const uint32_t k[4]) {
    uint32_t v0=v[0], v1=v[1], sum=0, i;           /* set up */
    uint32_t delta=0x9E3779B9;                     /* a key schedule constant */
    uint32_t k0=k[0], k1=k[1], k2=k[2], k3=k[3];   /* cache key */
    for (i=0; i<32; i++) {                         /* basic cycle start */
        sum += delta;
        v0 += ((v1<<4) + k0) ^ (v1 + sum) ^ ((v1>>5) + k1);
        v1 += ((v0<<4) + k2) ^ (v0 + sum) ^ ((v0>>5) + k3);
    }                                              /* end cycle */
    v[0]=v0; v[1]=v1;
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 1000000;
    int res;
    do {
        res = nanosleep(&ts, &ts);
    } while (res);
}

void decrypt_2 (uint32_t v[2], const uint32_t k[4]) {
    uint32_t v0=v[0], v1=v[1], sum=0xC6EF3720, i;  /* set up; sum is 32*delta */
    uint32_t delta=0x9E3779B9;                     /* a key schedule constant */
    uint32_t k0=k[0], k1=k[1], k2=k[2], k3=k[3];   /* cache key */
    for (i=0; i<32; i++) {                         /* basic cycle start */
        v1 -= ((v0<<4) + k2) ^ (v0 + sum) ^ ((v0>>5) + k3);
        v0 -= ((v1<<4) + k0) ^ (v1 + sum) ^ ((v1>>5) + k1);
        sum -= delta;
    }                                              /* end cycle */
    v[0]=v0; v[1]=v1;
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 1000000;
    int res;
    do {
        res = nanosleep(&ts, &ts);
    } while (res);
}

void char_encrypt(char* string, uint32_t* output, int string_size) {
    #pragma omp parallel for default(shared) schedule(static) num_threads(4)
    for (int i=0; i<string_size/2; i++) {
        uint32_t out[2];
        out[0] = string[2*i];
        out[1] = (2*i+1 == string_size) ? '0' : string[2*i+1];
        encrypt_2(out, const_key);
        output[2*i] = out[0];
        if (2*i+1 != string_size) output[2*i+1] = out[1];
    }
}

void char_decrypt(uint32_t* encrypted, char* output, int string_size) {
    #pragma omp parallel for default(shared) schedule(static) num_threads(4)
    for (int i=0; i<string_size/2; i++) {
        uint32_t out[2];
        out[0] = encrypted[2*i];
        out[1] = (2*i+1 == string_size) ? '0' : encrypted[2*i+1];
        decrypt_2(out, const_key);
        output[2*i] = out[0];
        if (2*i+1 != string_size) output[2*i+1] = out[1];
    }
}

int main() {

    char in[512] = "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567891234567890123456789012345678901234567890";
    uint32_t crypt[512];
    char out[512];
    clock_t start = clock();
    char_encrypt(in, crypt, 512);
    char_decrypt(crypt, out, 512);
    clock_t end = clock();
    printf("Took %d.\n", end - start);

    return 0;
}

