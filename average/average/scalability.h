#pragma once
#include <chrono>
#include <vector>
#include <iostream>
#include <thread>
#include <fstream>

size_t get_num_threads();
void set_num_threads(size_t T);

template <class F, class ... P>
double get_time(F f, P...params)
{
	using namespace std;
	using namespace chrono;
	auto t1 = steady_clock::now();
	f(params...);
	auto t2 = steady_clock::now();
	return duration_cast<milliseconds>(t2 - t1).count();
}

struct scalability {
	std::vector<double> times, speedups;
};
template <class F, class ... P>
scalability get_speedups(F f, P...params) {
	using namespace std;
	scalability s;
	set_num_threads(1);
	s.times.push_back(get_time(f, params...));
	s.speedups.push_back(1.0);
	for (size_t T = 2; T <= thread::hardware_concurrency(); ++T) {
		s.times.push_back(get_time(f, params...));
		s.speedups.push_back(s.times[0] / s.times.back());
	}
	return s;
}

inline void to_csv(const scalability& scal, std::string filename) //format CSV: RFC 4180
{
	using namespace std;
	ofstream myFile(filename);
	myFile << "Threads,Speedups";
	cout << "Threads,Speedups";
	for (size_t T = 1; T <= scal.times.size(); ++T) {
		myFile << "\n" << T << ',' << /*scal.times[T-1] << ',' <<*/ scal.speedups[T - 1];
		cout << "\n" << T << ',' << /*scal.times[T-1] << ',' <<*/ scal.speedups[T - 1];
	}
}