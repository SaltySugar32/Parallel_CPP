#include "vector_mod.h"
#include "test.h"
#include "performance.h"
#include <iostream>
#include <iomanip>
#include "num_threads.h"

int main2(int argc, char** argv)
{
	std::cout << "==Correctness tests. ";
	
	for (std::size_t iTest = 0; iTest < test_data_count; ++iTest)
	{
		std::cout <<"\ttest "<< iTest<<std::endl;
		if (test_data[iTest].result != vector_mod(test_data[iTest].dividend, test_data[iTest].dividend_size, test_data[iTest].divisor))
		{
			std::cout << "FAILURE==\ncorrect:"<< test_data[iTest].result<<"\ngot:"<< vector_mod(test_data[iTest].dividend, test_data[iTest].dividend_size, test_data[iTest].divisor)<<std::endl;
			//return -1;
		}
	}
	std::cout << "ok.==\n";
	std::cout << "==Performance tests. ";
	auto measurements = run_experiments();
	std::cout << "Done==\n";
	std::cout << std::setfill(' ') << std::setw(2) << "T:" << " |" << std::setw(3 + 2 * sizeof(IntegerWord)) << "Value:" << " | "
		<< std::setw(14) << "Duration, ms:" << " | Acceleration:\n";
	for (std::size_t T = 1; T <= measurements.size(); ++T)
	{
		std::cout << std::setw(2) << T << " | 0x" << std::setw(2 * sizeof(IntegerWord)) << std::setfill('0') << std::hex << measurements[T - 1].result;
		std::cout << " | " << std::setfill(' ') << std::setw(14) << std::dec << measurements[T - 1].time.count();
		std::cout << " | " << (static_cast<double>(measurements[0].time.count()) / measurements[T - 1].time.count()) << "\n";
	}

	return 0;
}
/*
	
*/
int main() {
	set_num_threads(8);
	IntegerWord v[] = { 1,2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0,
	1,2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0,
	1,2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0,
	1,2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0 };
	IntegerWord m = 0x100, r = vector_mod(v, 1, m);

	std::cout <<std::hex << r << std::endl;
	main2(1, 0);
	return 0;
}