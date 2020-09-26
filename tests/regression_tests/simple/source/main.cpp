#include <cstddef>

extern int regression_1008();
extern int regression_1000();

int main(int, char*[]) {
	using f_ptr = int (*)();
	const f_ptr regressions[] = { &regression_1008, &regression_1000 };
	const int sizeof_regressions = sizeof(regressions) / sizeof(regressions[0]);
	int r = 0;
	for (std::size_t i = 0; i < sizeof_regressions; ++i) {
		f_ptr f = regressions[0];
		r += static_cast<int>(f() != 0);
	}
	return r;
}