/******************************************************************************
* 
* dense_mm.c
* 
* This program implements a dense matrix multiply and can be used as a
* hypothetical workload. 
*
* Usage: This program takes a single input describing the size of the matrices
*        to multiply. For an input of size N, it computes A*B = C where each
*        of A, B, and C are matrices of size N*N. Matrices A and B are filled
*        with random values. 
*
* Written Sept 6, 2015 by David Ferry
******************************************************************************/

#include <stdio.h>  //For printf()
#include <stdlib.h> //for exit() and atoi()
#include <signal.h> //for signal handling

const int num_expected_args = 2;
const unsigned sqrt_of_UINT32_MAX = 65536;
int sig_counter = 0; //global signal counter

void rpi_signal_handler(int rpiSig); //function definition

int main( int argc, char* argv[] ){

	unsigned index, row, col; //loop indicies
	unsigned matrix_size, squared_size;
	double *A, *B, *C;
	//signal handling variables
	struct sigaction rpi_sigs;
	rpi_sigs.sa_handler = rpi_signal_handler;
	rpi_sigs.sa_flags = SA_RESTART;
	//signal handling variables - end
	int ret = sigaction(SIGINT, &rpi_sigs, NULL); //registering signal handler
	if (ret < 0)
	{
		perror("could not register sigaction");
		return 1;
	}
	
	
	if( argc != num_expected_args ){
		printf("Usage: ./dense_mm <size of matrices>\n");
		exit(-1);
	}

	matrix_size = atoi(argv[1]);
	
	if( matrix_size > sqrt_of_UINT32_MAX ){
		printf("ERROR: Matrix size must be between zero and 65536!\n");
		exit(-1);
	}

	squared_size = matrix_size * matrix_size;

	printf("Generating matrices...\n");

	A = (double*) malloc( sizeof(double) * squared_size );
	B = (double*) malloc( sizeof(double) * squared_size );
	C = (double*) malloc( sizeof(double) * squared_size );

	for( index = 0; index < squared_size; index++ ){
		A[index] = (double) rand();
		B[index] = (double) rand();
		C[index] = 0.0;
	}

	printf("Multiplying matrices...\n");

	for( row = 0; row < matrix_size; row++ ){
		for( col = 0; col < matrix_size; col++ ){
			for( index = 0; index < matrix_size; index++){
			C[row*matrix_size + col] += A[row*matrix_size + index] *B[index*matrix_size + col];
			}	
		}
	}

	printf("Multiplication done!\n");
	printf("Number of times SIGINT called:  %d\n", sig_counter);


	return 0;
}

void rpi_signal_handler(int rpiSig)
{
	sig_counter++;
}
