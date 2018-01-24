#include "ex_board.h"

const Bitboard FILE1_3 = FILE1_BB | FILE2_BB | FILE3_BB; // 盤面右側
const Bitboard FILE4_6 = FILE4_BB | FILE5_BB | FILE6_BB; // 盤面中央
const Bitboard FILE7_9 = FILE7_BB | FILE8_BB | FILE9_BB; // 盤面左側

const Bitboard RANK1_3 = RANK1_BB | RANK2_BB | RANK3_BB; // 盤面上段
const Bitboard RANK4_6 = RANK4_BB | RANK5_BB | RANK6_BB; // 盤面中段
const Bitboard RANK7_9 = RANK7_BB | RANK8_BB | RANK9_BB; // 盤面下段

const Bitboard& BitRight = FILE1_3;
const Bitboard& BitCenter = FILE4_6;
const Bitboard& BitLeft = FILE7_9;
const Bitboard& BitUpper = RANK1_3;
const Bitboard& BitMiddle = RANK4_6;
const Bitboard& BitLower = RANK7_9;

std::ostream& operator<<(std::ostream& os, const PieceExistence& pe) {
	switch (pe) {
	case PieceExistence::B_Board: os << "先手盤上"; break;
	case PieceExistence::W_Board: os << "後手盤上"; break;
	case PieceExistence::B_Hand: os << "先手駒台"; break;
	case PieceExistence::W_Hand: os << "後手駒台"; break;
	}
	return os;
};

std::ostream& operator<<(std::ostream& os, const RecheckReason& rr) {
	switch (rr) {
	case RecheckReason::None: os << "確認不要"; break;
	case RecheckReason::B_Rook: os << "先手の飛車"; break;
	case RecheckReason::W_Rook: os << "後手の飛車"; break;
	case RecheckReason::B_BISHOP: os << "先手の角"; break;
	case RecheckReason::W_BISHOP: os << "後手の角"; break;
	case RecheckReason::B_LANCE: os << "先手の香"; break;
	case RecheckReason::W_LANCE: os << "後手の香"; break;
	}
	return os;
};

std::ostream& operator<<(std::ostream& os, const CheckList& cl) {
	for (auto i = 0; i<9; ++i) {
		if (cl.check_item[i].reason != RecheckReason::None) {
			std::cout << cl.check_item[0].sq << ": " << cl.check_item[0].reason;
		}
	}
	return os;
};

CheckList::CheckList() {
}

CheckList::~CheckList() {
}

void CheckList::add(const Square &sq, const RecheckReason &reason) {
	check_item[index].sq = sq;
	check_item[index].reason = reason;
	++index;
}
