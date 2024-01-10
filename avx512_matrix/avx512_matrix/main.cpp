#include <iostream>
#include <immintrin.h>
using namespace std;

// basic sequence
void matrix_add_seq(const double* A, const double* B, unsigned n, double* M) {
	for (unsigned int row = 0; row < n; row++)
		for (unsigned int col = 0; col < n; col++) {
			unsigned index = row * n + col;
			M[index] = A[index] + B[index];
		}
}

// Intel AVX512
void matrix_add_avx512(const double* A, const double* B, unsigned n, double* M) {
	unsigned zmm_size = sizeof(__m512d) / sizeof(double);
	for (unsigned int row = 0; row < n; row++){
		for (unsigned int col = 0; col < n / zmm_size; col++) {
			unsigned index = row * n + col * zmm_size;
			__m512d v = _mm512_loadu_pd(&A[index]);
			__m512d u = _mm512_loadu_pd(&B[index]);
			__m512d z = _mm512_add_pd(v, u);
			_mm512_storeu_pd(&M[index], z);
		}
	}
}

/*

		[3]
[7,8,9]	[6] = 21+48+81=102+48=150
		[9]

*/

/* First col, then row, in order to keep data local
* 
* order of adding values to M
* [1 4 7]				[1 2 3]
* [2 5 8] better then	[4 5 6]
* [3 6 9]				[7 8 9]
* Assuming we write a matrix as an array A = [1,2,4,5,7,8] //	[1 4 7]
*																[2 5 8]
*/
void matrix_mul_seq(const double* A, const double* B, size_t n, double* M) {
	for (unsigned int col = 0; col < n; col++) {
		for (unsigned int row = 0; row < n; row++) {
			double temp = 0;
			for (unsigned int g = 0; g < n; g++)
				temp += A[g * n + row] * B[col * n + g];
			M[col * n + row] = temp;
		}
	}
}

void matrix_mul_avx512(const double* A, const double* B, size_t n, double* M) {
	unsigned zmm_size = sizeof(__m512d) / sizeof(double);
	for (unsigned int col = 0; col < n; col++) {
		for (unsigned int row = 0; row < n / zmm_size; row++) {
			__m512d temp = _mm512_setzero_pd();
			for (unsigned int g = 0; g < n; g++) {
				__m512d a = _mm512_loadu_pd(&A[g * n + row * zmm_size]);
				__m512d b = _mm512_set1_pd(B[col * n + g]);
				__m512d c = _mm512_mul_pd(a, b);
				temp = _mm512_add_pd(temp, c);
			}
			_mm512_storeu_pd(&M[col * n + row * zmm_size], temp);
		}
	}
}

template<typename T>
void matrix_print(const T* A, unsigned n, string name="M") {
	cout << "---------	Matrix " << name << "	---------" << endl;
	for (unsigned row = 0; row < n; row++) {
		for(unsigned col=0;col<n; col++)
			cout << A[row * n + col] << '\t';
		cout << endl;
	}
}

int main()
{
	//  assume n mod 16 = 0
	const unsigned n = 8;
	double A[n * n];
	double B[n * n];
	for (int i = 0; i < n * n; i++) {
		A[i] = i;
		B[i] = 63-i;
	}
	double M[n * n];
	/*
	matrix_add_avx512(A, B, n, M);
	matrix_print(M, n);

	matrix_add_seq(A, B, n, M);
	matrix_print(M, n);
	*/

	matrix_mul_seq(B, A, n, M);
	matrix_print(M, n);
	
	matrix_mul_avx512(B, A, n, M);
	matrix_print(M, n);
	
	return 0;
}
