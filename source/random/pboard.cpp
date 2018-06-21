#include <iomanip>
#include "../thread.h"
#include "../shogi.h"
#include "pboard.h"
#include "int_board.h"
#include "ex_board.h"

PBoard::PBoard() {
}
PBoard::PBoard(const PBoard &p_board) {
	board = p_board.board;
	accum = p_board.accum;
}

#ifdef AVX512
// Arrayは+1でFILEが増える方、+9でRANKが増える方
PBoard::PBoard(const IntBoard2 init_board) {
	board = reverse(init_board); // 入力が逆転列なので元に戻す
	p_sum = __accumu(this->board, this->accum); // 累計加算を計算する
}
// IntBoardが立っていない部分を0にしてaccumを計算し直す
void PBoard::and_(IntBoard2& int_board) {
	__and(this->board, int_board);
};
// IntBoardが立っている部分を0にしてaccumを計算し直す
void PBoard::ninp(IntBoard2& int_board) {
	__ninp(this->board, int_board);
};
#else
// IntBoardが立っていない部分を0にしてaccumを計算し直す
void PBoard::and_(IntBoard& int_board) {
	__and(this->board, int_board);
};

// IntBoardが立っている部分を0にしてaccumを計算し直す
void PBoard::ninp(IntBoard& int_board) {
	__ninp(this->board, int_board);
};
#endif

// Bitboardが立っていない部分を0にする。
void PBoard::and_(Bitboard& bitboard) {
#ifdef AVX512
	__and(this->board, bitboard_to_intboard2(bitboard));
#else
	__and(this->board, bitboard_to_intboard(bitboard));
#endif
}

// Bitboardが立っている部分を0にする。
void PBoard::ninp(Bitboard& bitboard) {
#ifdef AVX512
	__ninp(this->board, bitboard_to_intboard2(bitboard));
#else
	__ninp(this->board, bitboard_to_intboard(bitboard));
#endif
}

// Arrayは+1でFILEが増える方、+9でRANKが増える方
PBoard::PBoard(const IntBoard init_board) {
	board = reverse(init_board); // 入力が逆転列なので元に戻す
	p_sum = __accumu(this->board, this->accum); // 累計加算を計算する
}

void PBoard::accumu() {
	p_sum = __accumu(this->board, this->accum);
};

int PBoard::accumu_rand() {
	return __accumu_rand(this->board, this->accum);
};

int PBoard::rand() {
	return __rand(this->board, this->accum);
};

bool PBoard::is_zero() {
#ifdef AVX512
	for (auto i = 0; i < 81; ++i) {
		if (board.p[i] != 0) {
			return false;
		}
	}
	return true;
#else
	return board == IntBoard_ZERO;
#endif
}

// 盤面を出力する。デバッグ用。
std::ostream& operator<<(std::ostream& os, const PBoard& pb) {
	os << reverse(pb.board);
	return os;
};
