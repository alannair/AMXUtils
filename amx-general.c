#include "amx.h"

int min(int a, int b)
{
  if (a<b) return a;
  return b;
}

void initialize(struct Matrix* mat, int zeros)
{
  int r = mat->totalrows;
  int c = mat->totalcols;

  mat->base_addr = (float*)malloc(sizeof(float)*r*c);

  for(int i=0; i < (r*c); ++i)
    (mat->base_addr)[i] = zeros ? 0.0 : (float)(rand() % 10) / 5;
}

void add(struct Matrix* A, struct Matrix* B)
{
  // add B to A
  assert(A->totalrows == B->totalrows);
  assert(A->totalcols == B->totalcols);
  assert(A->nrows == B->nrows);
  assert(A->ncols == B->ncols);

  float* baseA = A->base_addr + (A->startrow * A->totalcols) + A->startcol;
  float* baseB = B->base_addr + (B->startrow * B->totalcols) + B->startcol;

  for(int i=0; i < A->nrows; ++i)
  {
    float* rowA = baseA + i*(A->totalcols);
    float* rowB = baseB + i*(B->totalcols);
    for(int j=0; j < A->ncols; ++j)
    {
      *(rowA + j) += *(rowB + j);
    }
  }
}

void multiply(struct Matrix* A, struct Matrix* B, struct Matrix* C)
{
  int MAXROWS = 16;
  int MAXCOLS = 64 / sizeof(float);
  int MAXDIMS = min(MAXROWS, MAXCOLS);

  assert(A->ncols == B->nrows);

  if (A->nrows <= MAXROWS &&
      B->ncols <= MAXCOLS &&
      A->ncols <= MAXDIMS)
  {
    /* case 1
     * [a] x [b] = [ab]
     */

    struct tileconfig_t cfg;
    cfg.palette_id = 1;
    cfg.startRow = 0;

    __tile a = 0, b = 1, c = 2;
    cfg.cols[a] = A->ncols * sizeof(float);
    cfg.rows[a] = A->nrows;
    cfg.cols[b] = B->ncols * sizeof(float);
    cfg.rows[b] = B->nrows;
    cfg.cols[c] = C->ncols * sizeof(float);
    cfg.rows[c] = C->nrows;

    _tile_loadconfig(&cfg);

    float* addrA = A->base_addr + (A->startrow * A->totalcols) + A->startcol;
    float* addrB = B->base_addr + (B->startrow * B->totalcols) + B->startcol;
    float* addrC = C->base_addr + (C->startrow * C->totalcols) + C->startcol;

    int strideA = sizeof(float) * A->totalcols;
    int strideB = sizeof(float) * B->totalcols;
    int strideC = sizeof(float) * C->totalcols;

    _tile_loadd(a, addrA, strideA);
    _tile_loadd(b, addrB, strideB);
    _tile_dpbf16ps(c, a, b);
    _tile_stored(c, addrC, strideC);
    _tile_release();
  }
  else if (A->nrows > MAXROWS)
  {
    /* case 2
     * [a] x [c]  = [ac]
     * [b]          [bc]
     */
    struct Matrix A1, A2, C1, C2;

    A1.totalrows = A->totalrows;
    A1.totalcols = A->totalcols;
    A1.startrow = A->startrow;
    A1.startcol = A->startcol;
    A1.nrows = (A->nrows)/2; // the upper half of A
    A1.ncols = A->ncols;
    A1.base_addr = A->base_addr;

    A2.totalrows = A->totalrows;
    A2.totalcols = A->totalcols;
    A2.startrow = A->startrow + (A->nrows)/2; // the lower half of A
    A2.startcol = A->startcol;
    A2.nrows = A->nrows - (A->nrows)/2;
    A2.ncols = A->ncols;
    A2.base_addr = A->base_addr;

    C1.totalrows = C->totalrows;
    C1.totalcols = C->totalcols;
    C1.startrow = C->startrow;
    C1.startcol = C->startcol;
    C1.nrows = (C->nrows)/2; // the upper half of A
    C1.ncols = C->ncols;
    C1.base_addr = C->base_addr;

    C2.totalrows = C->totalrows;
    C2.totalcols = C->totalcols;
    C2.startrow = C->startrow + (C->nrows)/2; // the lower half of A
    C2.startcol = C->startcol;
    C2.nrows = C->nrows - (C->nrows)/2;
    C2.ncols = C->ncols;
    C2.base_addr = C->base_addr;

    multiply(&A1, B, &C1);
    multiply(&A2, B, &C2);
  }
  else if (B->ncols > MAXCOLS)
  {
    /* case 3
     * [a] x [b c] = [ab ac]
     */
    struct Matrix B1, B2, C1, C2;

    B1.totalrows = B->totalrows;
    B1.totalcols = B->totalcols;
    B1.startrow = B->startrow;
    B1.startcol = B->startcol;
    B1.nrows = B->nrows;
    B1.ncols = (B->ncols)/2; // the left half of B
    B1.base_addr = B->base_addr;

    B2.totalrows = B->totalrows;
    B2.totalcols = B->totalcols;
    B2.startrow = B->startrow;
    B2.startcol = B->startcol + (B->ncols)/2;
    B2.nrows = B->nrows;
    B2.ncols = B->ncols - (B->ncols)/2; // the right half of B
    B2.base_addr = B->base_addr;

    C1.totalrows = C->totalrows;
    C1.totalcols = C->totalcols;
    C1.startrow = C->startrow;
    C1.startcol = C->startcol;
    C1.nrows = C->nrows;
    C1.ncols = (C->ncols)/2; // the left half of C
    C1.base_addr = C->base_addr;

    C2.totalrows = C->totalrows;
    C2.totalcols = C->totalcols;
    C2.startrow = C->startrow; // the lower half of A
    C2.startcol = C->startcol + (C->ncols)/2;
    C2.nrows = C->nrows;
    C2.ncols = C->ncols - (C->ncols)/2;
    C2.base_addr = C->base_addr;

    multiply(A, &B1, &C1);
    multiply(A, &B2, &C2);
  }
  else if (A->ncols > MAXDIMS)
  {
    /* case 4
     * [a b] x [c] = [ac+bd]
     *         [d]
     */

    struct Matrix A1, A2, B1, B2, C1, C2;

    A1.totalrows = A->totalrows;
    A1.totalcols = A->totalcols;
    A1.startrow = A->startrow;
    A1.startcol = A->startcol;
    A1.nrows = A->nrows;
    A1.ncols = (A->ncols)/2;
    A1.base_addr = A->base_addr;

    A2.totalrows = A->totalrows;
    A2.totalcols = A->totalcols;
    A2.startrow = A->startrow;
    A2.startcol = A->startcol + (A->ncols)/2;
    A2.nrows = A->nrows;
    A2.ncols = A->ncols - (A->ncols)/2;
    A2.base_addr = A->base_addr;

    B1.totalrows = B->totalrows;
    B1.totalcols = B->totalcols;
    B1.startrow = B->startrow;
    B1.startcol = B->startcol;
    B1.nrows = (B->nrows)/2;
    B1.ncols = B->ncols;
    B1.base_addr = B->base_addr;

    B2.totalrows = B->totalrows;
    B2.totalcols = B->totalcols;
    B2.startrow = B->startrow + (B->nrows)/2;
    B2.startcol = B->startcol;
    B2.nrows = B->nrows - (B->nrows)/2;
    B2.ncols = B->ncols;
    B2.base_addr = B->base_addr;

    C1.totalrows = C->totalrows;
    C1.totalcols = C->totalcols;
    C1.startrow = C->startrow;
    C1.startcol = C->startcol;
    C1.nrows = C->nrows;
    C1.ncols = C->ncols;
    C1.base_addr = C->base_addr;

    C2.totalrows = C->totalrows;
    C2.totalcols = C->totalcols;
    C2.startrow = C->startrow;
    C2.startcol = C->startcol;
    C2.nrows = C->nrows;
    C2.ncols = C->ncols;
    C2.base_addr = NULL;
    initialize(&C2, 1);

    multiply(&A1, &B1, &C1);
    multiply(&A2, &B2, &C2);
    add(&C1, &C2);
  }
}

int main(int argc, char* argv[])
{
  srand(time(0));
  int x, y, z;
  x = atoi(argv[1]);
  y = atoi(argv[2]);
  z = atoi(argv[3]);

  struct Matrix A, B, C;

  A.totalrows = x;
  A.totalcols = y;
  A.startrow = 0;
  A.startcol = 0;
  A.nrows = x;
  A.ncols = y;
  initialize(&A, 0);

  B.totalrows = y;
  B.totalcols = z;
  B.startrow = 0;
  B.startcol = 0;
  B.nrows = y;
  B.ncols = z;
  initialize(&B, 0);

  A.totalrows = x;
  A.totalcols = z;
  A.startrow = 0;
  A.startcol = 0;
  A.nrows = x;
  A.ncols = z;
  initialize(&C, 1);

  multiply(&A, &B, &C);

  return 0;
}
