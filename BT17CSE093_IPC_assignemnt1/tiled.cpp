#include <iostream>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <omp.h>
#include <cmath>

using namespace std;
int threads = 2;

void Mat_init(vector< vector<double> > &a,vector< vector<double> > &b, int n){
	// Initialize arrays.
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < n; ++j) {
			a[i][j] = (double)rand()/RAND_MAX*10;
			b[i][j] = (double)rand()/RAND_MAX*10;
			
		}
	}
}
	
void Mat_multy_seq(vector< vector<double> > &a,vector< vector<double> > &b, vector< vector<double> > &c, int n){
	// Compute matrix multiplication.
	// C <- C + A x B
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < n; ++j) {
			double temp  = 0;
			for (int k = 0; k < n; ++k) {
				temp += a[i][k] * b[k][j];
			}
			c[i][j]=temp;
		}
	}
}


void Mat_multy_Tiled(vector< vector<double> > &a,vector< vector<double> > &b, vector< vector<double> > &c, int n){
	int blockSize = 64;
	int ii,jj,kk,j,k;
	#pragma omp parallel num_threads(threads) shared(a,b,c,blockSize) private(ii,jj,kk,j,k)
	{
		#pragma omp for schedule(static)
		
		 for(int ii=0; ii<n; ii += blockSize){
			for(int jj=0; jj<n; jj += blockSize){
				for(int kk=0; kk<n; kk += blockSize){
					for (int i = ii; i < min(ii+blockSize, n); ++i) {
						for (int j = jj; j < min(jj+blockSize, n); ++j) {										
							for (int k = kk; k < min(kk+blockSize, n); k+=8) {
								c[i][j] +=
									 + a[i][k]*b[j][k]
									 + a[i][k+1]*b[j][k+1]
									 + a[i][k+2]*b[j][k+2]
									 + a[i][k+3]*b[j][k+3]
									 + a[i][k+4]*b[j][k+4]
									 + a[i][k+5]*b[j][k+5]
									 + a[i][k+6]*b[j][k+6]
									 + a[i][k+7]*b[j][k+7];
							}							
						}
					}
				}
			}
		}
	}
}


int main(){
	srand(time(0));
	
	int n =2048;
	double startTime = 0;
	double elapsedTime = 0;
	double seqTime = 0;
	double tiledTime = 0;
	
	vector< vector<double> > a(n, vector<double>(n)), b(n, vector<double>(n)), c(n, vector<double>(n));
	Mat_init(a, b, n);
	vector< vector<double> > btrans(n, vector<double>(n));

	
	startTime = time(0);
	Mat_multy_seq(a, b, c, n);
	elapsedTime = time(0)-startTime;
	seqTime = elapsedTime;
	cout<<"Naive multplication: "<<seqTime<<" seconds"<<endl;


	//printMat(c, n);
	
	startTime = time(0);
	Mat_multy_Tiled(a, btrans, c, n);
	elapsedTime = time(0)-startTime;
	tiledTime = elapsedTime;
	cout<<"Tiled multplication: "<<tiledTime<<" seconds"<<endl;
	//printMat(c, n);	
	
	cout<<"Speed up: "<<seqTime/tiledTime<<endl;
	
	
	
	
}
