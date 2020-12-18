#include <cstddef>

extern int regression_1000();
extern int regression_1008();
extern int regression_1067();
extern int regression_1072();

using f_ptr = int();

int trampoline(f_ptr* f) {
	try {
		return f();
	}
	catch (...) {
	}
	return 1;
}

int main(int, char*[]) {
	f_ptr* regressions[] = { &regression_1008, &regression_1000, &regression_1067, &regression_1072 };
	const int sizeof_regressions = sizeof(regressions) / sizeof(regressions[0]);
	int r = 0;
	for (std::size_t i = 0; i < sizeof_regressions; ++i) {
		f_ptr* f = regressions[i];
		r += static_cast<int>(trampoline(f) != 0);
	}
	return r;
}