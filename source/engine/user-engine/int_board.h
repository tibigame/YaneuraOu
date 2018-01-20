#ifndef _INTBOARD_H_
#define _INTBOARD_H_

#include <array>
#include "../../position.h"
#include "../../bitboard.h"

constexpr int SQUARES_NUMBER = 81; // 将棋盤のマス目の数

typedef std::array<int, SQUARES_NUMBER> IntBoard;
const IntBoard IntBoard_ZERO = {
	0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0
};
const std::array<int, 16> Int_ZERO16 = {
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0
};

constexpr Square sq_table[81] = {
	SQ_11, SQ_21, SQ_31, SQ_41, SQ_51, SQ_61, SQ_71, SQ_81, SQ_91,
	SQ_12, SQ_22, SQ_32, SQ_42, SQ_52, SQ_62, SQ_72, SQ_82, SQ_92,
	SQ_13, SQ_23, SQ_33, SQ_43, SQ_53, SQ_63, SQ_73, SQ_83, SQ_93,
	SQ_14, SQ_24, SQ_34, SQ_44, SQ_54, SQ_64, SQ_74, SQ_84, SQ_94,
	SQ_15, SQ_25, SQ_35, SQ_45, SQ_55, SQ_65, SQ_75, SQ_85, SQ_95,
	SQ_16, SQ_26, SQ_36, SQ_46, SQ_56, SQ_66, SQ_76, SQ_86, SQ_96,
	SQ_17, SQ_27, SQ_37, SQ_47, SQ_57, SQ_67, SQ_77, SQ_87, SQ_97,
	SQ_18, SQ_28, SQ_38, SQ_48, SQ_58, SQ_68, SQ_78, SQ_88, SQ_98,
	SQ_19, SQ_29, SQ_39, SQ_49, SQ_59, SQ_69, SQ_79, SQ_89, SQ_99
};

constexpr int index_table[81] = {
	0, 9, 18, 27, 36, 45, 54, 63, 72,
	1, 10, 19, 28, 37, 46, 55, 64, 73,
	2, 11, 20, 29, 38, 47, 56, 65, 74,
	3, 12, 21, 30, 39, 48, 57, 66, 75,
	4, 13, 22, 31, 40, 49, 58, 67, 76,
	5, 14, 23, 32, 41, 50, 59, 68, 77,
	6, 15, 24, 33, 42, 51, 60, 69, 78,
	7, 16, 25, 34, 43, 52, 61, 70, 79,
	8, 17, 26, 35, 44, 53, 62, 71, 80
};

void __and(IntBoard& base_board, IntBoard& and_board);
void __ninp(IntBoard& base_board, IntBoard& ninp_board);
int __accumu(IntBoard& base_board, IntBoard& accumu);
int __accumu_rand(IntBoard& base_board, IntBoard& accumu);
int __rand(IntBoard& base_board, IntBoard& accumu);

IntBoard bitboard_to_intboard(const Bitboard bit_board); // BitboardからIntBoardを返す
IntBoard reverse(const IntBoard prev);
std::ostream& operator<<(std::ostream& os, const IntBoard& board);

int accum_sum4_and_select_index(std::array<int, 4> &in_array, std::array<int, 4> &accum_array);
int accum_sum81_and_select_index(IntBoard &board, IntBoard &accum);

#endif _INTBOARD_H_
