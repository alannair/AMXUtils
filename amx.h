#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <immintrin.h>
#include <time.h>

unsigned char config[64];

struct Matrix
{
  int totalrows;
  int totalcols;
  int startrow;
  int startcol;
  int nrows;
  int ncols;
  float* base_addr;
};

void initialize(struct Matrix* mat);

struct tileconfig_t
{
  uint8_t  palette_id;
  uint8_t  startRow;
  uint8_t  reserved[14];
  uint16_t cols[16];
  uint8_t  rows[16];
};

void matmul(struct Matrix*, struct Matrix*, struct Matrix*);
