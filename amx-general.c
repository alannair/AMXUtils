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

  for(int i=0; i<(totalrows*totalcols); ++i)
    (mat->base_addr)[i] = zeros ? 0.0 : (float)(rand() % 10) / 5;
}

void matmul(struct Matrix* A, struct Matrix* B, struct Matrix* C)
{
  struct tileconfig_t cfg;
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
  }
  else if (A->nrows > MAXROWS)
  {
    /* case 2
     * [a] x [c]  = [ac]
     * [b]          [bc]
     */
  }
  else if (B->ncols > MAXCOLS)
  {
    /* case 3
     * [a] x [b c] = [ab ac]
     */
  }
  else if (A->ncols > MAXDIMS)
  {
    /* case 4
     * [a b] x [c] = [ac+bd]
     *         [d]
     */
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
