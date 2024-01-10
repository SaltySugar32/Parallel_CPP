#include <thread>

using namespace std;

static size_t num_threads = thread::hardware_concurrency(); //omp_get_num_procs()
size_t get_num_threads() {
	return num_threads;
}
void set_num_threads(size_t T) {
	num_threads = T;
}