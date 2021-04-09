#include "amx.h"

#define MAXROWS 16
#define MAXCOLS 16 // 64/sizeof(float)

void matrixMult(float* X, int xdim, float* Y, int ydim, float* result)
{
  assert(xdim == ydim);
  assert(sizeof(float) == 4);
  // assert xdim is a power of 2

  if (xdim <= MAXROWS)
  {
    // matrixmult_single_tile(X, xdim, Y, ydim, result);

    config[0] = 0x01;
    config[16] = (unsigned char)(4*xdim); // X bytes per row
    config[18] = (unsigned char)(4*xdim); // Y bytes per row
    config[20] = (unsigned char)(4*xdim); // result bytes per row
    config[48] = (unsigned char)xdim;
    config[49] = (unsigned char)xdim;
    config[50] = (unsigned char)xdim;

    _tile_loadconfig(config);
    __tile x = 0, y = 1, z = 2;
    _tile_loadd(x, X, 4*xdim);
    _tile_loadd(y, Y, 4*xdim);
    _tile_dpbf16ps(z, x, y);
    _tile_stored(z, result, 4*xdim);
    _tile_release();
  }
  else
  {
    float* x1 = (float*)malloc((xdim/2)*(xdim/2)*sizeof(float));
    float* x2 = (float*)malloc((xdim/2)*(xdim/2)*sizeof(float));
    float* x3 = (float*)malloc((xdim/2)*(xdim/2)*sizeof(float));
    float* x4 = (float*)malloc((xdim/2)*(xdim/2)*sizeof(float));

    float* y1 = (float*)malloc((xdim/2)*(xdim/2)*sizeof(float));
    float* y2 = (float*)malloc((xdim/2)*(xdim/2)*sizeof(float));
    float* y3 = (float*)malloc((xdim/2)*(xdim/2)*sizeof(float));
    float* y4 = (float*)malloc((xdim/2)*(xdim/2)*sizeof(float));

    float* r11 = (float*)malloc((xdim/2)*(xdim/2)*sizeof(float));
    float* r12 = (float*)malloc((xdim/2)*(xdim/2)*sizeof(float));
    float* r23 = (float*)malloc((xdim/2)*(xdim/2)*sizeof(float));
    float* r24 = (float*)malloc((xdim/2)*(xdim/2)*sizeof(float));
    float* r31 = (float*)malloc((xdim/2)*(xdim/2)*sizeof(float));
    float* r32 = (float*)malloc((xdim/2)*(xdim/2)*sizeof(float));
    float* r43 = (float*)malloc((xdim/2)*(xdim/2)*sizeof(float));
    float* r44 = (float*)malloc((xdim/2)*(xdim/2)*sizeof(float));

    for(int i=0; i<xdim/2; ++i)
    {
      for(int j=0; j<xdim/2; ++j)
      {
        *(x1 + i*(xdim/2) + j) = *(X + i*(xdim) + j);
        *(x2 + i*(xdim/2) + j) = *(X + i*(xdim) + j + xdim/2);
        *(x3 + i*(xdim/2) + j) = *(X + (i + xdim/2)*(xdim) + j);
        *(x4 + i*(xdim/2) + j) = *(X + (i + xdim/2)*(xdim) + j + (xdim/2));

        *(y1 + i*(xdim/2) + j) = *(Y + i*(xdim) + j);
        *(y2 + i*(xdim/2) + j) = *(Y + i*(xdim) + j + xdim/2);
        *(y3 + i*(xdim/2) + j) = *(Y + (i + xdim/2)*(xdim) + j);
        *(y4 + i*(xdim/2) + j) = *(Y + (i + xdim/2)*(xdim) + j + (xdim/2));
      }
    }

    matrixMult(x1, xdim/2, y1, xdim/2, r11);
    matrixMult(x1, xdim/2, y2, xdim/2, r12);
    matrixMult(x2, xdim/2, y3, xdim/2, r23);
    matrixMult(x2, xdim/2, y4, xdim/2, r24);
    matrixMult(x3, xdim/2, y1, xdim/2, r31);
    matrixMult(x3, xdim/2, y2, xdim/2, r32);
    matrixMult(x4, xdim/2, y3, xdim/2, r43);
    matrixMult(x4, xdim/2, y4, xdim/2, r44);

    for(int i=0; i<xdim/2; ++i)
    {
      for(int j=0; j<xdim/2; ++j)
      {
        *(result + i*xdim + j) = *(r11 + i*(xdim/2) +j) +
                                 *(r23 + i*(xdim/2) +j);
        *(result + i*xdim + j + xdim/2) = *(r12 + i*(xdim/2) +j) +
                                          *(r24 + i*(xdim/2) +j);
        *(result + (i + xdim/2)*xdim + j) = *(r31 + i*(xdim/2) +j) +
                                            *(r43 + i*(xdim/2) +j);
        *(result + (i + xdim/2)*xdim + j + xdim/2) = *(r32 + i*(xdim/2) +j) +
                                                     *(r44 + i*(xdim/2) +j);
      }
    }

    free(x1);
    free(x2);
    free(x3);
    free(x4);
    free(y1);
    free(y2);
    free(y3);
    free(y4);

    free(r11);
    free(r12);
    free(r23);
    free(r24);
    free(r31);
    free(r32);
    free(r43);
    free(r44);
  }
}
