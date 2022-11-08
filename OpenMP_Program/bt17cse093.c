#include<stdio.h>
#include <stdlib.h>
#include <omp.h>

long m_size;


void lu_factor(double **A, long n)
{
	printf("\nLU Factorization\n");

	long i,j,k,rows,mymin,mymax;
	int pid=0;
	int nprocs;

#pragma omp parallel shared(A,n,nprocs) private(i,j,k,pid,rows,mymin,mymax)
	{
#ifdef _OPENMP
		nprocs=omp_get_num_threads();
#endif

#ifdef _OPENMP
		pid=omp_get_thread_num();
#endif

		rows=n/nprocs;
		mymin=pid * rows;
		mymax=mymin + rows - 1;

		if(pid==nprocs-1 && (n-(mymax+1))>0)
			mymax=n-1;

		for(k=0;k<n;k++){
			if(k>=mymin && k<=mymax){
				#pragma omp for
				for(j=k+1;j<n;j++){
					A[k][j] = A[k][j]/A[k][k];
				}
			}

#pragma omp barrier
			for(i=(((k+1) > mymin) ? (k+1) : mymin);i<=mymax;i++){
				//#pragma omp for
				for(j=k+1;j<n;j++){
					A[i][j] = A[i][j] - A[i][k] * A[k][j];
				}
			}
		}
	}
}


double **make_matrix(long n)
{
	long i;
	double **m;
	m = (double**)malloc(n*sizeof(double*));
	for (i=0;i<n;i++)
		m[i] = (double*)malloc(n*sizeof(double));
	return m;
}


double **getMatrix(long n)
{
	double **m=make_matrix(n);
	{
	long i, j;
	for (i=0;i<n;i++){
		for (j=0;j<n;j++){
			if(i<=j )
				m[i][j]=i+1;
			else
				m[i][j]=j+1;

		}
	}
}

	return m;
}

int check(double **A, long n){
	long i, j;
	for (i=0;i<n;i++)
	{
		for (j=0;j<n;j++)
			if(A[i][j]!=1){
				return 0;
			}
	}
	return 1;
}

// only works for dynamic arrays:
void printmatrix(double **A, long n)
{
	printf("\n *************** MATRIX ****************\n\n");
	long i, j;
	for (i=0;i<n;i++)
	{
		for (j=0;j<n;j++)
			printf("%f ",A[i][j]);
		printf("\n");
	}
}
void free_matrix(double ** M, long n)
{
	long i;
	if (!M) return;
	for(i=0;i<n;i++)
		free(M[i]);
	free(M);
}
int main(int argc, char *argv[]){

	if(argc !=4){
		printf("Enter the size of matrix (N x N) where N = ");
		scanf("%lu",&m_size);
	}
	else{
		m_size=atol(argv[1]);
	}
	long num_threads=atol(argv[2]);
	if(num_threads<1){
		num_threads=4;
	}
	omp_set_num_threads(num_threads);

	double **matrix=getMatrix(m_size);

	printmatrix(matrix,m_size);

	lu_factor(matrix,m_size);

	printmatrix(matrix,m_size);


	printf("\n**********************************\n\n");
	printf("Size of Matrix :%lu \n",m_size);
	printf("Number of Process : %lu\n",num_threads);
	printf("%s",check(matrix,m_size)==1? "DECOMPOSE SUCCESSFULL\n":"DECOMPOSE FAIL\n");
	printf("\n**********************************\n\n");

	free_matrix(matrix,m_size);
	return 0;
}


