#include <iomanip>
#include "int_board.h"
#include "my_rand.h"

#define NTZ
#ifdef NTZ
#include "ntz.hpp"
#endif

// 0～mod-1までの乱数を生成する
MyRand myrand;;
inline u32 my_rand() {
	return myrand.rand();
};
// 剰余は遅いので、rand() % modの代替として乗算 + シフトを使う
inline u32 my_rand(const u32 mod) {
	return (uint64_t)myrand.rand() * (uint64_t)mod >> 32;
};

// BitBoardに対応したIntBoardを返す
// Bitが立っている升が1でそれ以外は0
IntBoard bitboard_to_intboard(const Bitboard bit_board) {
#ifdef NTZ
	u64 p0 = bit_board.p[0];
	u64 p1 = bit_board.p[1];
	IntBoard result = IntBoard_ZERO; // 0初期化
	// result[index_table[shifted - 1]]と参照したいがここで減算したくないので
	int shifted = -1; // shifted = 0でなく -1で初期化
	int ntz_i;
	while (true) {
		ntz_i = ntz(p0); // 右端に立っているビットの位置を取得
		if (ntz_i++ == 64) { break; }// ビットがすべて0になるまでループ
		p0 >>= ntz_i; // 右端のビット1を落とすまで右シフト
		shifted += ntz_i;
		result[index_table[shifted]] = 0xffffffff;
	}
	shifted = 62; // 上記と同じ理由で shifted = 63 でなく 62で初期化
	while (true) {
		ntz_i = ntz(p1); // 右端に立っているビットの位置を取得
		if (ntz_i++ == 64) { break; }// ビットがすべて0になるまでループ
		p1 >>= ntz_i; // 右端のビット1を落とすまで右シフト
		shifted += ntz_i;
		result[index_table[shifted]] = 0xffffffff;
	}
	return result;
#else
	u64 p0 = bit_board.p[0];
	u64 p1 = bit_board.p[1];
	IntBoard result = IntBoard_ZERO; // 0初期化
	int rank = 0;
	int file = 0;
	for (auto i = 0; i < 63; ++i) {
		if (p0 & 1) {
			result[rank * 9 + file] = 0xffffffff;
		}
		p0 >>= 1;
		++rank;
		if (rank == 9) {
			rank = 0;
			++file;
		}
	}
	for (auto i = 0; i < 18; ++i) {
		if (p1 & 1) {
			result[rank * 9 + file] = 0xffffffff;
		}
		p1 >>= 1;
		++rank;
		if (rank == 9) {
			rank = 0;
			++file;
		}
	}
	return result;
#endif
}

// IntBoardが立っていない部分を0にします。
void __and(IntBoard& base_board, IntBoard& and_board) {
	for (auto i = 0; i < SQUARES_NUMBER; ++i) {
		base_board[i] &= and_board[i];
	}
};

// IntBoardが立っている部分を0にします。
void __ninp(IntBoard& base_board, IntBoard& ninp_board) {
	for (auto i = 0; i < SQUARES_NUMBER; ++i) {
		base_board[i] &= ~(ninp_board[i]);
	}
};

// IntBoardの左右を反転する
// ソースコード上は現実の将棋盤のような位置に数字を書きたいが、
// 内部状態は逆なので変数に入れるときにこの関数を介するようにする。
IntBoard reverse(const IntBoard prev) {
	IntBoard result;
	for (auto rank = 0; rank < 9; ++rank) { // 9段分を処理する
		for (auto file = 0; file < 9; ++file) {
			result[rank * 9 + file] = prev[rank * 9 + 8 - file];
		}
	}
	return result;
};

// 盤面を出力する。デバッグ用。
constexpr int line_width = 5;
std::ostream& operator<<(std::ostream& os, const IntBoard& board) {
	for (auto rank = 0; rank < 9; ++rank) {
		for (auto file = 0; file < 9; ++file) {
			os << std::setw(line_width) << std::right << board[rank * 9 + file];
		}
		os << std::endl;
	}
	return os;
};

// 累計和を求める
int __accumu(IntBoard& base_board, IntBoard& accumu) {
	int p_sum = 0;
	for (auto i = 0; i < SQUARES_NUMBER; ++i) {
		p_sum += base_board[i];
		accumu[i] = p_sum;
	}
	return p_sum;
};

// __accumuと__randを両方行いたい時向け
int __accumu_rand(IntBoard& base_board, IntBoard& accumu) {
	int p_sum = 0;
	for (auto i = 0; i < SQUARES_NUMBER; ++i) {
		p_sum += base_board[i];
		accumu[i] = p_sum;
	}
	int r = my_rand(p_sum);
	// 累計加算の値が初めてr以上となるようなインデックスを求める
	int min = 0;
	int middle = SQUARES_NUMBER >> 1;
	int max = SQUARES_NUMBER;
	// ただの二分探索
	while (max - min > 1) {
		// middleが大きい時
		if (accumu[middle] >= r) {
			max = middle;
			middle = (min + max) >> 1;
		}
		// middleが小さい時
		else {
			min = middle;
			middle = (min + max) >> 1;
		}
	}
	return max;
};

// 累計和のIntBoardを利用してbase_boardの確率分布に従ったインデックスを返す
int __rand(IntBoard& base_board, IntBoard& accumu) {
	int r = my_rand(accumu[SQUARES_NUMBER - 1]);
	// 累計加算の値が初めてr以上となるようなインデックスを求める
	int min = 0;
	int middle = SQUARES_NUMBER >> 1;
	int max = SQUARES_NUMBER;
	// ただの二分探索
	while (max - min > 1) {
		if (accumu[middle] >= r) { // middleが大きい時
			max = middle;
			middle = (min + max) >> 1;
		} // middleが小さい時
		else {
			min = middle;
			middle = (min + max) >> 1;
		}
	}
	return max;
};

// 累積和を計算します
// 累積和を計算して乱数に従ってインデックスを選択
int accum_sum4_and_select_index(std::array<int, 4> &in_array, std::array<int, 4> &accum_array) {
	accum_array[0] = in_array[0];
	accum_array[1] = accum_array[0] + in_array[1];
	accum_array[2] = accum_array[1] + in_array[2];
	accum_array[3] = accum_array[2] + in_array[3];
	int r = my_rand(accum_array[3]);
	if (accum_array[1] >= r) {
		if (accum_array[0] >= r) {
			return 0;
		}
		return 1;
	}
	if (accum_array[2] >= r) {
		return 2;
	}
	return 3;
};

#ifdef AVX512
IntBoard2 bitboard_to_intboard2(const Bitboard bit_board) {
	return IntBoard2(bitboard_to_intboard(bit_board));
}

IntBoard reverse(const IntBoard2 prev) {
	IntBoard result;
	for (auto rank = 0; rank < 9; ++rank) { // 9段分を処理する
		for (auto file = 0; file < 9; ++file) {
			result[rank * 9 + file] = prev.p[rank * 9 + 8 - file];
		}
	}
	return result;
};
std::ostream& operator<<(std::ostream& os, const IntBoard2& board) {
	IntBoard t;
	for (auto j = 0; j < 81; ++j) {
		t[j] = board.p[j];
	}
	for (auto rank = 0; rank < 9; ++rank) {
		for (auto file = 0; file < 9; ++file) {
			os << std::setw(line_width) << std::right << t[rank * 9 + file];
		}
		os << std::endl;
	}
	return os;
};


// IntBoardが立っていない部分を0にします。
void __and(IntBoard2& base_board, IntBoard2& and_board) {
	base_board.m[0] = _mm512_and_epi32(base_board.m[0], and_board.m[0]);
	base_board.m[1] = _mm512_and_epi32(base_board.m[1], and_board.m[1]);
	base_board.m[2] = _mm512_and_epi32(base_board.m[2], and_board.m[2]);
	base_board.m[3] = _mm512_and_epi32(base_board.m[3], and_board.m[3]);
	base_board.m[4] = _mm512_and_epi32(base_board.m[4], and_board.m[4]);
	base_board.p[80] &= and_board.p[80];
};

// IntBoardが立っている部分を0にします。
void __ninp(IntBoard2& base_board, IntBoard2& ninp_board) {
	base_board.m[0] = _mm512_andnot_epi32(base_board.m[0], ninp_board.m[0]);
	base_board.m[1] = _mm512_andnot_epi32(base_board.m[1], ninp_board.m[1]);
	base_board.m[2] = _mm512_andnot_epi32(base_board.m[2], ninp_board.m[2]);
	base_board.m[3] = _mm512_andnot_epi32(base_board.m[3], ninp_board.m[3]);
	base_board.m[4] = _mm512_andnot_epi32(base_board.m[4], ninp_board.m[4]);
	base_board.p[80] &= ~(ninp_board.p[80]);
};

// 累計和を求める
int __accumu(IntBoard2& base_board, IntBoard2& accumu) {
	int p_sum = 0;
	for (auto i = 0; i < SQUARES_NUMBER; ++i) {
		p_sum += base_board.p[i];
		accumu.p[i] = p_sum;
	}
	return p_sum;
};

// __accumuと__randを両方行いたい時向け
int __accumu_rand(IntBoard2& base_board, IntBoard2& accumu) {
	// 累積和の計算
	__m512i iz;
	iz =_mm512_xor_epi32(iz, iz); // 0にする
	__m512i in0 = base_board.m[0];
	in0 = _mm512_add_epi32(in0, _mm512_alignr_epi32(in0, iz, 16 - 1));
	in0 = _mm512_add_epi32(in0, _mm512_alignr_epi32(in0, iz, 16 - 2));
	in0 = _mm512_add_epi32(in0, _mm512_alignr_epi32(in0, iz, 16 - 4));
	in0 = _mm512_add_epi32(in0, _mm512_alignr_epi32(in0, iz, 16 - 8));
	accumu.m[0] = in0;
	__m512i sum0 = _mm512_set1_epi32(accumu.p[15]);
	__m512i in1 = base_board.m[1];
	in1 = _mm512_add_epi32(in1, _mm512_alignr_epi32(in1, iz, 16 - 1));
	in1 = _mm512_add_epi32(in1, _mm512_alignr_epi32(in1, iz, 16 - 2));
	in1 = _mm512_add_epi32(in1, _mm512_alignr_epi32(in1, iz, 16 - 4));
	in1 = _mm512_add_epi32(in1, _mm512_alignr_epi32(in1, iz, 16 - 8));
	in1 = _mm512_add_epi32(in1, sum0);
	accumu.m[1] = in1;
	__m512i sum1 = _mm512_set1_epi32(accumu.p[31]);
	__m512i in2 = base_board.m[2];
	in2 = _mm512_add_epi32(in2, _mm512_alignr_epi32(in2, iz, 16 - 1));
	in2 = _mm512_add_epi32(in2, _mm512_alignr_epi32(in2, iz, 16 - 2));
	in2 = _mm512_add_epi32(in2, _mm512_alignr_epi32(in2, iz, 16 - 4));
	in2 = _mm512_add_epi32(in2, _mm512_alignr_epi32(in2, iz, 16 - 8));
	in2 = _mm512_add_epi32(in2, sum1);
	accumu.m[2] = in2;
	__m512i sum2 = _mm512_set1_epi32(accumu.p[47]);
	__m512i in3 = base_board.m[3];
	in3 = _mm512_add_epi32(in3, _mm512_alignr_epi32(in3, iz, 16 - 1));
	in3 = _mm512_add_epi32(in3, _mm512_alignr_epi32(in3, iz, 16 - 2));
	in3 = _mm512_add_epi32(in3, _mm512_alignr_epi32(in3, iz, 16 - 4));
	in3 = _mm512_add_epi32(in3, _mm512_alignr_epi32(in3, iz, 16 - 8));
	in3 = _mm512_add_epi32(in3, sum2);
	accumu.m[3] = in3;
	__m512i sum3 = _mm512_set1_epi32(accumu.p[63]);
	__m512i in4 = base_board.m[4];
	in4 = _mm512_add_epi32(in4, _mm512_alignr_epi32(in4, iz, 16 - 1));
	in4 = _mm512_add_epi32(in4, _mm512_alignr_epi32(in4, iz, 16 - 2));
	in4 = _mm512_add_epi32(in4, _mm512_alignr_epi32(in4, iz, 16 - 4));
	in4 = _mm512_add_epi32(in4, _mm512_alignr_epi32(in4, iz, 16 - 8));
	in4 = _mm512_add_epi32(in4, sum3);
	accumu.m[4] = in4;
	accumu.p[80] = accumu.p[79] + base_board.p[80];
	// sumが求まるので乱数の剰余を求める
	int r = my_rand(accumu.p[80]);
	if (r > accumu.p[79]) { // 81マス目
		return 80;
	}
	// SIMDで比較を行い、累積和を超えるインデックスを導出
	__m512i r0 = _mm512_set1_epi32(r);
	__mmask16 cmp0, cmp1, cmp2, cmp3, cmp4;
	u16 cmp0u, cmp1u, cmp2u, cmp3u, cmp4u;
	cmp0 = _mm512_cmpgt_epi32_mask(in0, r0);
	cmp1 = _mm512_cmpgt_epi32_mask(in1, r0);
	cmp2 = _mm512_cmpgt_epi32_mask(in2, r0);
	cmp3 = _mm512_cmpgt_epi32_mask(in3, r0);
	cmp4 = _mm512_cmpgt_epi32_mask(in4, r0);
	cmp0u = __popcnt16(~_mm512_mask2int(cmp0));
	cmp1u = __popcnt16(~_mm512_mask2int(cmp1));
	cmp2u = __popcnt16(~_mm512_mask2int(cmp2));
	cmp3u = __popcnt16(~_mm512_mask2int(cmp3));
	cmp4u = __popcnt16(~_mm512_mask2int(cmp4));
	return cmp0u + cmp1u + cmp2u + cmp3u + cmp4u;
};

// 累計和のIntBoardを利用してbase_boardの確率分布に従ったインデックスを返す
int __rand(IntBoard2& base_board, IntBoard2& accumu) {
	int r = my_rand(accumu.p[80]);
	if (r > accumu.p[79]) { // 81マス目
		return 80;
	}
	// SIMDで比較を行い、累積和を超えるインデックスを導出
	__m512i in0 = accumu.m[0];
	__m512i in1 = accumu.m[1];
	__m512i in2 = accumu.m[2];
	__m512i in3 = accumu.m[3];
	__m512i in4 = accumu.m[4];
	__m512i r0 = _mm512_set1_epi32(r);
	__mmask16 cmp0, cmp1, cmp2, cmp3, cmp4;
	u16 cmp0u, cmp1u, cmp2u, cmp3u, cmp4u;
	cmp0 = _mm512_cmpgt_epi32_mask(in0, r0);
	cmp1 = _mm512_cmpgt_epi32_mask(in1, r0);
	cmp2 = _mm512_cmpgt_epi32_mask(in2, r0);
	cmp3 = _mm512_cmpgt_epi32_mask(in3, r0);
	cmp4 = _mm512_cmpgt_epi32_mask(in4, r0);
	cmp0u = __popcnt16(~_mm512_mask2int(cmp0));
	cmp1u = __popcnt16(~_mm512_mask2int(cmp1));
	cmp2u = __popcnt16(~_mm512_mask2int(cmp2));
	cmp3u = __popcnt16(~_mm512_mask2int(cmp3));
	cmp4u = __popcnt16(~_mm512_mask2int(cmp4));
	return cmp0u + cmp1u + cmp2u + cmp3u + cmp4u;
};
#endif
