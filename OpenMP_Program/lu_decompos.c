#include<stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>

#define IDX(i,j) (size*(i) + (j))
long size;
/** 
 * Decomposes a matrix in-situ into its L and U components
 * @param matrix pointer to the matrix
 * @param size   size of the matrix
 */
void decompose_matrix(double *matrix, int size)
{
    /* in situ LU decomposition */
    int k;

    //LU-decomposition based on Gaussian Elimination
    // - Arranged so that the multiplier doesn't have to be computed multiple times
    for(k = 0; k < size-1; k++){ //iterate over rows/columns for elimination
        int row;
        // The "multiplier" is the factor by which a row is multiplied when
        //  being subtracted from another row.
#pragma omp parallel for private(row) shared(matrix)
        for(row = k + 1; row < size; row++){
            int col;

            // the multiplier only depends on (k,row),
            // it is invariant with respect to col
            double factor = matrix[IDX(row,k)]/matrix[IDX(k,k)];

            //Eliminate entries in sub matrix (subtract rows)
            for(col = k + 1; col < size; col++){ //column
                matrix[IDX(row,col)] = matrix[IDX(row,col)] - factor*matrix[IDX(k,col)];
            }

            matrix[IDX(row,k)] = factor;
        }
    }
}

#define EPSILON (1.0E-3)

/*inline int min(int a, int b){
    if(a < b)
        return a;
    else
        return b;
}
*/
/** 
 * Checks if the computation of l*u is the same as matrix
 * @param lu     pointer to the lu matrix
 * @param matrix pointer to the matrix
 * @param size   size of the matrix
 * @return       true if l*u=matrix, false otherwise
 */

int check_matrix(double *lu, double *matrix, int size)
{
    /* return l*u == matrix */

    int row;
    double s = 0.0;

    //Compute l*u element-wise and compare those elements to the original matrix.
    //The deviation of all elements is aggregated in `s`. The matrices are equal
    //  when this aggregated deviation is zero (or close to it)
    //The multiplication itself is a naïve matrix multiplication, modified to
    //  work with the embedding of L and U in `lu`.
#pragma omp parallel for private(row) reduction(+: s)
    for(row = 0; row < size; row++){ // row of matrix
        int col, k;
        for(col = 0; col < size; col++){ // column of matrix
            double r = 0.0;
            int limit = min(row-1,col);
            for(k = 0; k <= limit; k++){ // index in sum
                double l = lu[IDX(row,k)]; // has 1's on the diagonal
                double u = lu[IDX(k,col)];
                r += l*u;
            }
            //The diagonal of L is treated separately to keep the previous loop simple
            if(col >= row){
                double l = 1.0;
                double u = lu[IDX(row,col)];
                r += l*u;
            }

            //Check whether there is a difference between L*U and the original matrix.
            r = r - matrix[IDX(row,col)];
            r = fabs(r);
            if(r < EPSILON)
                r = 0.0;

            s += r;
        }
    }

    return s < EPSILON;
}
double *make2dmatrix(long n)
{
	long i;
	double *m;
	m = (double*)malloc(n*sizeof(double));
	/*for (i=0;i<n;i++)
		m[i] = (double)malloc(n*sizeof(double));*/
	return m;
}


double *getMatrix(long n)
{
	double *m=make2dmatrix(n);;
	long i,j, k;
	for(i=0;i<n;i++){
		for(j=i;j<n;j++){
			if(i==j){
				k=i+1;
				m[IDX(i,j)]=4*k-3;
			}
			else{
				m[IDX(i,j)]=m[IDX(i,i)]+1;
				m[IDX(j,i)]=m[IDX(i,i)]+1;
			}
		}
	}
	
	return m;
}
void printmatrix(double *A, long n)
{
	printf("\n *************** MATRIX ****************\n\n");
	long i, j;
	for (i=0;i<n;i++)
	{
		for (j=0;j<n;j++)
			printf("%f ",A[IDX(i,j)]);
		printf("\n");
	}
}
int main(int argc, char *argv[]){

	if(argc !=4){
		printf("Enter the size of matrix (N x N) where N = ");
		scanf("%lu",&size);

	}
	else{
		size=atol(argv[1]);
	}
	long num_threads=atol(argv[2]);
	if(num_threads<1){
		num_threads=4;
	}
	omp_set_num_threads(num_threads);

	double *matrix=getMatrix(size);

	printmatrix(matrix,size);


	decompose_matrix(matrix,size);

	printmatrix(matrix,size);


	return 0;
}
