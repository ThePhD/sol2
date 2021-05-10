#include <cstddef>

using f_ptr = unsigned int();

extern unsigned int regression_1000();
extern unsigned int regression_1008();
extern unsigned int regression_1067();
extern unsigned int regression_1072();
extern unsigned int regression_1087();
extern unsigned int regression_1095();
extern unsigned int regression_1096();
extern unsigned int regression_1149();
extern unsigned int regression_1192();

static f_ptr* const regression_tests_regressions[]
     = { &regression_1008, &regression_1000, &regression_1067, &regression_1072, &regression_1087, &regression_1095, &regression_1096, &regression_1149, &regression_1192 };
static const int regression_tests_sizeof_regressions = sizeof(regression_tests_regressions) / sizeof(regression_tests_regressions[0]);

int trampoline(f_ptr* f) {
	try {
		return f();
	}
	catch (...) {
	}
	return 1;
}

int main(int, char*[]) {
	int r = 0;
	for (std::size_t i = 0; i < regression_tests_sizeof_regressions; ++i) {
		f_ptr* f = regression_tests_regressions[i];
		r += static_cast<int>(trampoline(f) != 0u);
	}
	return r;
}
