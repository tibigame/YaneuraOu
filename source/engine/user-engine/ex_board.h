#ifndef _EXBOARD_H_
#define _EXBOARD_H_

#include "../../bitboard.h"

// 駒がどこに存在するか
enum class PieceExistence
{
	B_Board,
	W_Board,
	B_Hand,
	W_Hand
};
std::ostream& operator<<(std::ostream& os, const PieceExistence& pe);

extern const Bitboard FILE1_3; // 盤面右側
extern const Bitboard FILE4_6; // 盤面中央
extern const Bitboard FILE7_9; // 盤面左側

extern const Bitboard RANK1_3; // 盤面上段
extern const Bitboard RANK4_6; // 盤面中段
extern const Bitboard RANK7_9; // 盤面下段

extern const Bitboard& BitRight;
extern const Bitboard& BitCenter;
extern const Bitboard& BitLeft;
extern const Bitboard& BitUpper;
extern const Bitboard& BitMiddle;
extern const Bitboard& BitLower;

#endif _EXBOARD_H_
