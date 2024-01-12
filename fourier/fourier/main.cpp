#include<complex>
#include<iostream>
#include<fstream>
#include<numbers>
using std::complex;

void fft_seq(const complex<float>* input, complex<float>* output, std::size_t n, std::size_t step = 1) {
	// F(a) = a
	if (n == 1) {
		output[0] = input[0];
		return;
	}
	// vectorization here
	fft_seq(input, output, n / 2, step * 2);
	fft_seq(input+step, output + n/2, n / 2, step * 2);

	//									t++ ?
	for (std::size_t t = 0; t < n / 2; t++) {
		auto E = output[t];
		auto O = output[t + n / 2];
		auto W = std::polar(1.0f, -2.0f * std::numbers::pi_v<float> * t / n);
		output[t] = E + W * O;
		output[t + n/2] = E - W * O;
	}
}

void fft_par(const complex<float>* input, complex<float>* output, std::size_t n, std::size_t step = 1) {
	// F(a) = a
	if (n == 1) {
		output[0] = input[0];
		return;
	}
	// vectorization here
	// openmp 2.0
	#pragma omp task
	fft_par(input, output, n / 2, step * 2);

	#pragma omp task
	fft_seq(input + step, output + n / 2, n / 2, step * 2);
	
	#pragma omp taskwait

	for (std::size_t t = 0; t < n / 2; t++) {
		auto E = output[t];
		auto O = output[t + n / 2];
		auto W = std::polar(1.0f, -2.0f * std::numbers::pi_v<float> *t / n);
		output[t] = E + W * O;
		output[t + n / 2] = E - W * O;
	}
}

inline void to_csv(complex<float>* output, std::size_t n, std::string filename) //format CSV: RFC 4180
{
	using namespace std;
	ofstream myFile(filename+".csv");
	myFile << "real,imag";
	cout << "\n\t"<<filename<<"\n";
	for (size_t i = 0; i <= n; ++i) {
		myFile << "\n" << output[i].real() << ',' << output[i].imag();
		cout << output[i].real() << "," << output[i].imag() << "\n";
	}
}

int main() {
	constexpr std::size_t n = 128;
	//complex<float> output[n], input[n];
	
	auto input = std::make_unique<complex<float>[]>(n);
	auto output = std::make_unique<complex<float>[]>(n);
	

	for (std::size_t i = 0; i < n / 2; i++) {
		input[i] = 1.5 * i;
		input[n - 1 - i] = 1.5 * i;
	}

	std::cout << "\n\tSeq\n";
	fft_seq(input.get(), output.get(), n);
	to_csv(output.get(), n, "seq");

	fft_par(input.get(), output.get(), n);
	to_csv(output.get(), n, "par");
		
	return 0;
}