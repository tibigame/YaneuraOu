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

const Bitboard& BitLancePromoteBlack = RANK1_BB;
const Bitboard& BitLancePromoteWhite = RANK9_BB;
const Bitboard BitKnightPromoteBlack = RANK1_BB | RANK2_BB;
const Bitboard BitKnightPromoteWhite = RANK8_BB | RANK9_BB;

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
	case RecheckReason::LANCE: os << "香"; break;
	case RecheckReason::ROOK: os << "飛"; break;
	case RecheckReason::BISHOP: os << "角"; break;
	case RecheckReason::CLEAR: os << "確認OK"; break;
	}
	return os;
};

CheckList::CheckList() {
}

CheckList::~CheckList() {
}

void CheckList::add(const Color c, const RecheckReason &reason, const Bitboard b) {
	switch (reason) {
		case RecheckReason::LANCE: {
			if (check_item_lance[(int)c].commit == ZERO_BB) { // 未代入
				check_item_lance[(int)c].color = (c == BLACK ? WHITE : BLACK);
				check_item_lance[(int)c].commit = b;
			}
			else {
				check_item_lance[(int)c].commit &= b; // ZERO_BBにはならない
			}
			break; 
		}
		case RecheckReason::ROOK: {
			Bitboard pre = check_item_rook[0].commit;
			if (pre == ZERO_BB) { // 未代入
				check_item_rook[0].commit = b;
				check_item_rook[0].color = c;
			}
			else if (pre & b & (check_item_rook[0].color == c)) { // 共通部分がある
				check_item_rook[0].commit &= b; // 同じ利きのライン
			}
			else { // 共通部分がない
				check_item_rook[1].commit = b; // 別の利きのライン
				check_item_rook[1].color = c;
			}
			break;
		}
		case RecheckReason::BISHOP: {
			Bitboard pre = check_item_bishop[0].commit;
			if (pre == ZERO_BB) { // 未代入
				check_item_bishop[0].commit = b;
				check_item_bishop[0].color = c;
			}
			else if (pre & b & (check_item_bishop[0].color == c)) { // 共通部分がある
				check_item_bishop[0].commit &= b; // 同じ利きのライン
			}
			else { // 共通部分がない
				check_item_bishop[1].commit = b; // 別の利きのライン
				check_item_bishop[1].color = c;
			}
			break;
		}
	}
};
