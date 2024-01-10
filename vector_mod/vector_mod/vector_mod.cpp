#include "vector_mod.h"
#include "config.h"
#include "mod_ops.h"
#include "num_threads.h"
#include <thread>
#include <vector>

#include <iostream>

std::unique_ptr<IntegerWord[]> get_powers(unsigned T, unsigned C, IntegerWord mod) {
	auto degrees_table = std::make_unique<IntegerWord[]>(C + T);
	degrees_table[0] = 1;
	for (std::size_t i = 1; i <+ C; ++i)
		degrees_table[i] = times_word(degrees_table[i - 1], mod);
	for (std::size_t i = 2; i <= T; ++i)
		degrees_table[C + i - 1] = mul_mod(degrees_table[C + i - 2], degrees_table[C], mod);
	return degrees_table;
}

IntegerWord vector_mod(const IntegerWord* V, size_t N, IntegerWord mod) {
	// ph
	const int CACHE_LINE = 64;
	auto C = CACHE_LINE / sizeof(IntegerWord);

	unsigned T = get_num_threads();
	auto powers_lut = get_powers(T, C, mod);

	auto partial_results = std::make_unique<IntegerWord[]>(T);
	auto worker_proc = [V, N, T, C, &powers_lut, mod, &partial_results](unsigned t) {

		IntegerWord R1 = 0;
		for (size_t iTrev = 0; iTrev < N * T; ++iTrev) {
			auto iT = N - 1 - iTrev;
			auto start = iT * T * C + t * C;
			if (start >= N)
				continue;
			auto end = start + C;
			if (end > N)
				end = N;
			IntegerWord R2 = 0;
			for (size_t ir = start; ir < end; ir++) {
				auto i = end - 1 - ir;
				// R2 = R2 * W + V[i];
				R2 = add_mod(times_word(R2, mod), V[i], mod);
			}
			// R1 = R1 * W ^ TC + R2;
			R1 = add_mod(mul_mod(R1, powers_lut[T * C], mod), R2, mod);
		}
		partial_results[t] = R1;
		};

	std::vector<std::thread> workers(T - 1);
	for (unsigned t = 1; t < T; ++t)
		workers[t - 1] = std::thread(worker_proc, t);

	worker_proc(0);

	for (auto& th : workers)
		th.join();

	IntegerWord result = 0;
	for (unsigned i = 0; i < T; i++) {
		result = add_mod(result, mul_mod(partial_results[i], powers_lut[i], mod), mod);
	}

	return result;
}

IntegerWord vector_mod_legacy(const IntegerWord* V, size_t N, IntegerWord mod) {
	// ph
	const int CACHE_LINE = 64;
	auto C = CACHE_LINE / sizeof(IntegerWord);
	
	unsigned T = get_num_threads();
	auto powers_lut = get_powers(T, C, mod);
	
	auto partial_results = std::make_unique<IntegerWord[]>(T);
	auto worker_proc = [V, N, T, C, mod, &partial_results](unsigned t) {

		size_t i_max = (N - t * C - 1) / (T * C) + 1;
		IntegerWord my_result = 0;
		for (ptrdiff_t i = i_max; i >= 0; i -= T * C) {
			size_t my_C = C;
			if (i > N) {
				my_C -= i_max - N;
				i = i_max - N;
			}
			
			for (size_t j = 0; j < my_C; j++) {
				// my_result = (my_result*W + V[i-1-j]) % mod
				my_result = add_mod(times_word(my_result, mod), V[i - 1 - j], mod);
			}
			partial_results[t] = my_result;
		}
	};
	
	std::vector<std::thread> workers(T - 1);
	for (unsigned t = 1; t < T; ++t) 
		workers[t - 1] = std::thread(worker_proc, t);

	worker_proc(0);

	for (auto& th : workers)
		th.join();

	IntegerWord result = 0;
	for (unsigned i = 0; i < T; i++) {
		std::cout << "\np_r["<<i<<"] = " << partial_results[i] << std::endl;
		result = add_mod(result, mul_mod(partial_results[i], powers_lut[i], mod), mod);
	}

	return result;
}

