#ifndef _RANDOMBOARD_H_
#define _RANDOMBOARD_H_

#include "../shogi.h"
#include "random_board.h"
#include "int_board.h"
#include "pboard.h"
#include "ex_board.h"
#include <list>

struct BoardInfo {
public:
	Square sq;
	Piece pi;

	BoardInfo(Square s, Piece p) {
		sq = s;
		pi = p;
	}
};

extern std::list<BoardInfo> list_bi;

std::string end_game_mate();


// テスト用の関数
void sq_test(); // sq列をBitBoardに変換してIntBoardに戻すテスト

void view();


Square set_b_king(Position& pos_);
Square set_w_king(Position& pos_, const Square& b_king);
void set_rook(Position& pos_, const Square &b_king, const Square &w_king,
	const Bitboard &b_king_bit, const Bitboard &w_king_bit, Bitboard &occupied, CheckList &checklist);
void set_bishop(Position& pos_, const Square &b_king, const Square &w_king,
	const Bitboard &b_king_bit, const Bitboard &w_king_bit, Bitboard &occupied, CheckList &checklist);
void set_lance(Position& pos_, const Square &b_king, const Square &w_king,
	const Bitboard &b_king_bit, const Bitboard &w_king_bit, Bitboard &occupied, CheckList &checklist);
void set_knight(Position& pos_, const Square &b_king, const Square &w_king,
	const Bitboard &b_king_bit, const Bitboard &w_king_bit, Bitboard &occupied);
void set_silver(Position& pos_, const Square &b_king, const Square &w_king,
	const Bitboard &b_king_bit, const Bitboard &w_king_bit, Bitboard &occupied);
void set_gold(Position& pos_, const Square &b_king, const Square &w_king,
	const Bitboard &b_king_bit, const Bitboard &w_king_bit, Bitboard &occupied);
bool set_pawn(Position& pos_, const Square &b_king, const Square &w_king,
	const Bitboard &b_king_bit, const Bitboard &w_king_bit, Bitboard &occupied, CheckList &checklist);

#endif _RANDOMBOARD_H_
