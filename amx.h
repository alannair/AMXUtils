#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <immintrin.h>

unsigned char config[64];

void matrixMult(float* X, int xdim,
                float* Y, int ydim,
                float* result);
