#ifdef DLL
#include "./random/random_board.h"

extern "C" {
	__declspec(dllexport) int test(int a, int b);
	__declspec(dllexport) void init();
	__declspec(dllexport) int random_sfen(char* c_sfen, size_t bufsize);
}

int test(int a, int b) {
	return a + b;
}

void init() {
	Bitboards::init();
	Position::init();

	position_sfen_init();
}

int random_sfen(char* c_sfen, size_t bufsize) {
	std::string src = random_board();
	if (src.size() < bufsize) {
		strncpy(c_sfen, src.c_str(), bufsize - 1);
		*(c_sfen + bufsize - 1) = '\0';
		return src.size();
	}
	return 0;
}


#endif
