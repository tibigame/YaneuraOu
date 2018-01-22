﻿#include <iomanip>
#include "../../thread.h"
#include "../../shogi.h"
#include "random_board.h"
#include "int_board.h"
#include "ex_board.h"

// 盤面を出力する。デバッグ用。
std::ostream& operator<<(std::ostream& os, const PBoard& pb) {
	os << reverse(pb.board);
	return os;
};

#ifdef AVX512
// Arrayは+1でFILEが増える方、+9でRANKが増える方
PBoard::PBoard(const IntBoard2 init_board) {
	board = reverse(init_board); // 入力が逆転列なので元に戻す
	p_sum = __accumu(this->board, this->accum); // 累計加算を計算する
}
// IntBoardが立っていない部分を0にしてaccumを計算し直す
void PBoard::and(IntBoard2& int_board) {
	__and(this->board, int_board);
};
// IntBoardが立っている部分を0にしてaccumを計算し直す
void PBoard::ninp(IntBoard2& int_board) {
	__ninp(this->board, int_board);
};
#else
// IntBoardが立っていない部分を0にしてaccumを計算し直す
void PBoard::and(IntBoard& int_board) {
	__and(this->board, int_board);
};

// IntBoardが立っている部分を0にしてaccumを計算し直す
void PBoard::ninp(IntBoard& int_board) {
	__ninp(this->board, int_board);
};
#endif

PBoard::PBoard() {
}
// Arrayは+1でFILEが増える方、+9でRANKが増える方
PBoard::PBoard(const IntBoard init_board) {
	board = reverse(init_board); // 入力が逆転列なので元に戻す
	p_sum = __accumu(this->board, this->accum); // 累積加算を計算する
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

// 空の盤面で初期化します
void set_blank(Position& pos_) {
	pos_.set_blank();
};

// -----------------------------------
//     玉の配置確率を定義する
// -----------------------------------
const IntBoard b_king_p_intboard = {
	10, 10, 8, 6, 5, 6, 8, 10, 10,
	10, 10, 8, 6, 5, 6, 8, 10, 10,
	10, 10, 8, 6, 5, 6, 8, 10, 10,
	40, 32, 28, 10, 10, 10, 28, 32, 40,
	40, 32, 28, 10, 10, 10, 28, 32, 40,
	40, 32, 28, 10, 10, 10, 28, 32, 40,
	230, 150, 120, 40, 50, 40, 100, 150, 200,
	200, 820, 120, 350, 400, 300, 100, 250, 150,
	380, 150, 380, 140, 250, 140, 250, 150, 200
};
const PBoard b_king_p(b_king_p_intboard);
// 先手玉をpdで指定された確率で配置します(返り値は配置された場所のSquare)
Square set_b_king(Position& pos_) {
	PBoard pb = b_king_p;
	Square sq = sq_table[pb.rand()];
	pos_.put_piece(sq, B_KING);
	return sq;
};

const IntBoard w_king_p_intboard = reverse_123(b_king_p_intboard);
const PBoard w_king_p(w_king_p_intboard);
// 後手玉をpdで指定された確率で配置します(返り値は配置された場所のSquare)
Square set_w_king(Position& pos_, const Square& b_king) {
	PBoard pb2 = w_king_p;
	pb2.ninp(bitboard_to_intboard(kingEffect(b_king) | b_king)); // 先手玉の9近傍を除く
	Square sq = sq_table[pb2.accumu_rand()];
	pos_.put_piece(sq, W_KING);
	pos_.update_kingSquare();
	return sq;
};

// 飛車を配置します
void set_rook(Position& pos_, const Square b_king, const Square w_king) {
};

void end_game_mate(Position& pos_) {
	set_blank(pos_);
	Square sq_b_king = set_b_king(pos_);
	Square sq_w_king = set_w_king(pos_, sq_b_king);
	set_rook(pos_, sq_b_king, sq_w_king);
	pos_.update_bitboards();
};

// -----------------------------------------------------------------
// ここからテスト用の関数
// -----------------------------------------------------------------

// sq列をBitBoardに変換してIntBoardに戻すテスト
void sq_test() {
	for (auto i = 0; i < 81; ++i) {
		std::cout << "index: " << i << std::endl;
		Square sq = sq_table[i];
		Bitboard bb(sq);
		std::cout << bitboard_to_intboard(bb) << std::endl;
	}
};
