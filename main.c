#include "amx.h"
#include <time.h>

void initialize(float* mat, int dim)
{
	for(int i=0; i<dim; ++i)
		for(int j=0; j<dim; ++j)
			*(mat + i*dim + j) = (rand()%10000)/1000;
}

void check(float* x, float* y, float* z, int dim)
{
	for(int i=0; i<dim; ++i)
		for(int j=0; j<dim; ++j)
		{
			float q=0.0;
			for(int k=0; k<dim; ++k)
				q += (*(x + dim*i + k)) * (*(y + dim*k + j));

			if (q != (*(z + dim*i + j)))
			{
				printf("ERROR: %d %d %f %f\n", i, j, q, *(z + dim*i + j));
			}
		}
}

void print(float* mat, int dim)
{
	for(int i=0; i<dim; ++i)
	{
		for(int j=0; j<dim; ++j)
		{
			printf("%f \t", *(mat + i*dim + j));
		}
		printf("\n");
	}
	printf("\n\n");
}

int main(int argc, char* argv[])
{
	int dim = atoi(argv[1]);
	float* X = (float*)malloc(dim*dim*sizeof(float));
	float* Y = (float*)malloc(dim*dim*sizeof(float));
	float* Z = (float*)malloc(dim*dim*sizeof(float));

	srand(time(0));
	initialize(X, dim);
	initialize(Y, dim);

	matrixMult(X, dim, Y, dim, Z);

	print(X, dim);
	print(Y, dim);
	print(Z, dim);

	check(X, Y, Z, dim);

	// printf("SIZE of FLOAT: %lu\n", sizeof(float));

	free(X);
	free(Y);
	free(Z);
	return 0;
}
