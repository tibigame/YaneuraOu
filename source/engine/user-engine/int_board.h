#ifndef _INTBOARD_H_
#define _INTBOARD_H_

#include <array>
#include "../../position.h"
#include "../../bitboard.h"
#include "my_rand.h"
#include "ex_board.h"

// AVX512コードは10%ほど速い
#define AVX512
#ifdef AVX512
#include <immintrin.h>
#endif

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

IntBoard bitboard_to_intboard(const Bitboard &bit_board); // BitboardからIntBoardを返す
IntBoard reverse(const IntBoard prev);// IntBoardの左右を反転する
IntBoard reverse_vertical(const IntBoard prev); // IntBoardの上下を反転する
IntBoard reverse_123(const IntBoard prev); // IntBoardを180°回転する
std::ostream& operator<<(std::ostream& os, const IntBoard& board);
// 累計和を計算して乱数に従って駒の配置を選択
PieceExistence piece_existence_rand(const int b_board_p, const int w_board_p, const int b_hand_p, const int w_hand_p);

// 成りの確率
typedef std::array<double, 9> PromoteP;
// PromotePを反転する
constexpr PromoteP reverse(const PromoteP p) {
	return { p[8], p[7], p[6], p[5], p[4], p[3], p[2], p[1], p[0] };
};

// is_promoted_randの補助関数
constexpr u32 support_is_promoted_rand(const Square sq, const PromoteP &p) {
	return static_cast<u32>(UINT_MAX * p[rank_index_table[sq]]);
};
// 成りかどうかを確率的に判定します
// sq: 対象となる駒の位置、p: 1段目から9段目の成り確率。0(不成)～1(確定成)までの値を手番を考慮して入れること)
// 返り値は成り判定ならtrue、不成り判定ならfalse。
struct is_promoted_rand_func {
	is_promoted_rand_func() {
	}
	bool operator()(const Square sq, const PromoteP &p) {
		return support_is_promoted_rand(sq, p) > myrand.rand(); // 乱数をそのまま使える形式にするためにu32の最大値で正規化する
	};
};
extern is_promoted_rand_func is_promoted_rand;

#ifdef AVX512

struct alignas(64) IntBoard2 {
	union {
		u32 p[81];
		__m512i m[5];
	};
	IntBoard2() {
	}
	IntBoard2& operator = (const IntBoard2& rhs) {
		_mm512_store_si512(&this->m[0], rhs.m[0]);
		_mm512_store_si512(&this->m[1], rhs.m[1]);
		_mm512_store_si512(&this->m[2], rhs.m[2]);
		_mm512_store_si512(&this->m[3], rhs.m[3]);
		_mm512_store_si512(&this->m[4], rhs.m[4]);
		p[80] = rhs.p[80];
		return *this;
	}
	IntBoard2& operator = (IntBoard& i) {
		for (auto j = 0; j < 81; ++j) {
			p[j] = i[j];
		}
		return *this;
	}
	bool operator != (const IntBoard2& i) {
		for (auto j = 0; j < 81; ++j) {
			if (p[j] != i.p[j]) { // 要素が1つでも違う
				return true;
			}
		}
		return false;
	}
	IntBoard2(const IntBoard &i) {
		for (auto j = 0; j < 81; ++j) {
			p[j] = i[j];
		}
	};
};

const IntBoard2 IntBoard2_ZERO(IntBoard_ZERO);

// Bitが立っている升が1でそれ以外は0
struct bitboard_to_intboard2_func {
	bitboard_to_intboard2_func() {}
	IntBoard2 operator()(const Bitboard &bit_board) {
		u64 p0 = bit_board.p[0];
		u64 p1 = bit_board.p[1];
		IntBoard2 result = IntBoard2_ZERO; // 0初期化
		// result[index_table[shifted - 1]]と参照したいがここで減算したくないので
		int shifted = -1; // shifted = 0でなく -1で初期化
		int ntz_i;
		while (p0) { // ビットがすべて0になるまでループ
			ntz_i = static_cast<int>(_tzcnt_u64(p0)); // 右端に立っているビットの位置を取得
			p0 >>= (++ntz_i); // 右端のビット1を落とすまで右シフト
			shifted += ntz_i;
			result.p[index_table[shifted]] = 0xffffffff;
		}
		shifted = 62; // 上記と同じ理由で shifted = 63 でなく 62で初期化
		while (p1) { // ビットがすべて0になるまでループ
			ntz_i = static_cast<int>(_tzcnt_u64(p1)); // 右端に立っているビットの位置を取得
			p1 >>= (++ntz_i); // 右端のビット1を落とすまで右シフト
			shifted += ntz_i;
			result.p[index_table[shifted]] = 0xffffffff;
		}
		return result;
	}
};
extern bitboard_to_intboard2_func bitboard_to_intboard2;

IntBoard reverse(const IntBoard2 prev);
std::ostream& operator<<(std::ostream& os, const IntBoard2& board);
void __and(IntBoard2& base_board, IntBoard2& and_board);
void __ninp(IntBoard2& base_board, IntBoard2& ninp_board);
int __accumu(IntBoard2& base_board, IntBoard2& accumu);
int __accumu_rand(IntBoard2& base_board, IntBoard2& accumu);
int __rand(IntBoard2& base_board, IntBoard2& accumu);
#endif

#endif _INTBOARD_H_
