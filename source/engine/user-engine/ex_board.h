#ifndef _EXBOARD_H_
#define _EXBOARD_H_

#include "../../bitboard.h"

// 駒がどこに存在するか
enum class PieceExistence
{
	B_Board, // 先手盤上
	W_Board, // 後手盤上
	B_Hand, // 先手駒台
	W_Hand // 後手駒台
};
std::ostream& operator<<(std::ostream& os, const PieceExistence& pe);

extern const Bitboard FILE1_3; // 盤面右側
extern const Bitboard FILE4_6; // 盤面中央
extern const Bitboard FILE7_9; // 盤面左側

extern const Bitboard RANK1_3; // 盤面上段
extern const Bitboard RANK4_6; // 盤面中段
extern const Bitboard RANK7_9; // 盤面下段

extern const Bitboard& BitRight; // 盤面右側
extern const Bitboard& BitCenter; // 盤面中央
extern const Bitboard& BitLeft; // 盤面左側
extern const Bitboard& BitUpper; // 盤面上段
extern const Bitboard& BitMiddle; // 盤面中段
extern const Bitboard& BitLower; // 盤面下段

// Squareを直接fileにするテーブル。配列インデックス用に0開始
constexpr int file_index_table[82] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 1, 1, 1, 1,
	2, 2, 2, 2, 2, 2, 2, 2, 2,
	3, 3, 3, 3, 3, 3, 3, 3, 3,
	4, 4, 4, 4, 4, 4, 4, 4, 4,
	5, 5, 5, 5, 5, 5, 5, 5, 5,
	6, 6, 6, 6, 6, 6, 6, 6, 6,
	7, 7, 7, 7, 7, 7, 7, 7, 7,
	8, 8, 8, 8, 8, 8, 8, 8, 8, 9
};
// Squareを直接rankにするテーブル。配列インデックス用に0開始
constexpr int rank_index_table[82] = {
	0, 1, 2, 3, 4, 5, 6, 7, 8,
	0, 1, 2, 3, 4, 5, 6, 7, 8,
	0, 1, 2, 3, 4, 5, 6, 7, 8,
	0, 1, 2, 3, 4, 5, 6, 7, 8,
	0, 1, 2, 3, 4, 5, 6, 7, 8,
	0, 1, 2, 3, 4, 5, 6, 7, 8,
	0, 1, 2, 3, 4, 5, 6, 7, 8,
	0, 1, 2, 3, 4, 5, 6, 7, 8,
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9
};

// 盤面の配置再確認の理由
enum class RecheckReason
{
	None = 0, // 確認不要
	B_Rook, // 先手の飛車
	W_Rook, // 後手の飛車
	B_BISHOP, // 先手の角
	W_BISHOP, // 後手の角
	B_LANCE, // 先手の香
	W_LANCE // 後手の香
};
// 盤面再チェック用の項目を表す構造体
struct CheckItem {
	Square sq = SQ_NB;
	RecheckReason reason = RecheckReason::None;
};
// 盤面再チェック用のリストを管理するクラス
// write onceで使い捨てです
// 走査する時はcheck_itemをNoneでないまでループしてください。
class CheckList {
public:
	CheckItem check_item[9]; // 飛2角2香4+NULL用の9個
	int index = 0;
	CheckList();
	~CheckList();
	void add(const Square &sq, const RecheckReason &reason);
};

// 後手の玉が先手の飛車の利きにいる
inline bool is_effect_b_rook(const Square &w_king, const Square &b_rook) {
	return (file_index_table[b_rook] == file_index_table[w_king]) || (rank_index_table[b_rook] == rank_index_table[w_king]);
};
// 先手の玉が後手の飛車の利きにいる (実質同じ関数)
inline bool is_effect_w_rook(const Square &b_king, const Square &w_rook) {
	return (file_index_table[w_rook] == file_index_table[b_king]) || (rank_index_table[w_rook] == rank_index_table[b_king]);
};

std::ostream& operator<<(std::ostream& os, const RecheckReason& pe);

#endif _EXBOARD_H_
