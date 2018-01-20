#include <iomanip>
#include "ntz.hpp"
#include "int_board.h"
#include "my_rand.h"

#define NTZ
// AVX512コードは現状遅い
// #define AVX512
#ifdef AVX512
#include <immintrin.h>
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
	int shifted = 0;
	int ntz_i;
	while (true) {
		ntz_i = ntz(p0); // 右端に立っているビットの位置を取得
		if (ntz_i == 64) { break; }// ビットがすべて0になるまでループ
		++ntz_i;
		shifted += ntz_i;
		result[index_table[shifted - 1]] = 0xffffffff;
		p0 >>= ntz_i; // 右端のビット1を落とすまで右シフト
	}
	shifted = 63;
	while (true) {
		ntz_i = ntz(p1); // 右端に立っているビットの位置を取得
		if (ntz_i == 64) { break; }// ビットがすべて0になるまでループ
		++ntz_i;
		shifted += ntz_i;
		result[index_table[shifted - 1]] = 0xffffffff;
		p1 >>= ntz_i; // 右端のビット1を落とすまで右シフト
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
#ifdef AVX512
	__m512i m;
	m = _mm512_and_epi32(_mm512_loadu_si512(&and_board[0]), _mm512_loadu_si512(&base_board[0]));
	_mm512_storeu_si512(&and_board[0], m);
	m = _mm512_and_epi32(_mm512_loadu_si512(&and_board[16]), _mm512_loadu_si512(&base_board[16]));
	_mm512_storeu_si512(&and_board[16], m);
	m = _mm512_and_epi32(_mm512_loadu_si512(&and_board[32]), _mm512_loadu_si512(&base_board[32]));
	_mm512_storeu_si512(&and_board[32], m);
	m = _mm512_and_epi32(_mm512_loadu_si512(&and_board[48]), _mm512_loadu_si512(&base_board[48]));
	_mm512_storeu_si512(&and_board[48], m);
	m = _mm512_and_epi32(_mm512_loadu_si512(&and_board[64]), _mm512_loadu_si512(&base_board[64]));
	_mm512_storeu_si512(&and_board[64], m);
	base_board[80] &= and_board[80];
#else
	for (auto i = 0; i < SQUARES_NUMBER; ++i) {
		base_board[i] &= and_board[i];
	}
#endif
};

// IntBoardが立っている部分を0にします。
void __ninp(IntBoard& base_board, IntBoard& ninp_board) {
#ifdef AVX512
	__m512i m;
	m = _mm512_andnot_epi32(_mm512_loadu_si512(&base_board[0]), _mm512_loadu_si512(&ninp_board[0]));
	_mm512_storeu_si512(&ninp_board[0], m);
	m = _mm512_andnot_epi32(_mm512_loadu_si512(&base_board[16]), _mm512_loadu_si512(&ninp_board[16]));
	_mm512_storeu_si512(&ninp_board[16], m);
	m = _mm512_andnot_epi32(_mm512_loadu_si512(&base_board[32]), _mm512_loadu_si512(&ninp_board[32]));
	_mm512_storeu_si512(&ninp_board[32], m);
	m = _mm512_andnot_epi32(_mm512_loadu_si512(&base_board[48]), _mm512_loadu_si512(&ninp_board[48]));
	_mm512_storeu_si512(&ninp_board[48], m);
	m = _mm512_andnot_epi32(_mm512_loadu_si512(&base_board[64]), _mm512_loadu_si512(&ninp_board[64]));
	_mm512_storeu_si512(&ninp_board[64], m);
	base_board[80] &= ~(ninp_board[80]);
#else
	for (auto i = 0; i < SQUARES_NUMBER; ++i) {
		base_board[i] &= ~(ninp_board[i]);
	}
#endif
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
#ifdef AVX512
	return accum_sum81_and_select_index(base_board, accumu);
#else
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
#endif
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
#ifdef AVX512
	__m128i out = _mm_load_si128((__m128i*)&in_array);
	out = _mm_add_epi32(out, _mm_slli_si128(out, 4));
	out = _mm_add_epi32(out, _mm_slli_si128(out, 8));
	_mm_store_si128((__m128i*)&accum_array[0], out); // 累計和を書き出す

	int *ptr = (int *)_aligned_malloc(128, 64); // 64バイト境界でメモリ確保
	ptr[0] = my_rand(accum_array[3]); // 剰余を計算
	__m128i r0 = _mm_shuffle_epi32(_mm_load_si128((__m128i*)&ptr[0]), 0); // 値をブロードキャスト
	__m128i cmp = _mm_cmpgt_epi32(out, r0); // 比較
	_mm_store_si128((__m128i*)&ptr[0], cmp);
	if (ptr[1]) {
		if (ptr[0]) {
			_aligned_free(ptr);
			return 0;
		}
		_aligned_free(ptr);
		return 1;
	}
	if (ptr[2]) {
		_aligned_free(ptr);
		return 2;
	}
	_aligned_free(ptr);
	return 3;
#else
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
#endif
};

// 現状は使われていないコード
int accum_sum81_and_select_index(IntBoard &board, IntBoard &accum) {
#ifdef AVX512
	// 累積和の計算
	__m512i iz = _mm512_loadu_si512(&Int_ZERO16);
	__m512i in0 = _mm512_loadu_si512(&board[0]);
	in0 = _mm512_add_epi32(in0, _mm512_alignr_epi32(in0, iz, 16 - 1));
	in0 = _mm512_add_epi32(in0, _mm512_alignr_epi32(in0, iz, 16 - 2));
	in0 = _mm512_add_epi32(in0, _mm512_alignr_epi32(in0, iz, 16 - 4));
	in0 = _mm512_add_epi32(in0, _mm512_alignr_epi32(in0, iz, 16 - 8));
	_mm512_storeu_si512(&accum[0], in0);
	__m512i sum0 = _mm512_set1_epi32(accum[15]);
	__m512i in1 = _mm512_loadu_si512(&board[16]);
	in1 = _mm512_add_epi32(in1, _mm512_alignr_epi32(in1, iz, 16 - 1));
	in1 = _mm512_add_epi32(in1, _mm512_alignr_epi32(in1, iz, 16 - 2));
	in1 = _mm512_add_epi32(in1, _mm512_alignr_epi32(in1, iz, 16 - 4));
	in1 = _mm512_add_epi32(in1, _mm512_alignr_epi32(in1, iz, 16 - 8));
	in1 = _mm512_add_epi32(in1, sum0);
	_mm512_storeu_si512(&accum[16], in1);
	__m512i sum1 = _mm512_set1_epi32(accum[31]);
	__m512i in2 = _mm512_loadu_si512(&board[32]);
	in2 = _mm512_add_epi32(in2, _mm512_alignr_epi32(in2, iz, 16 - 1));
	in2 = _mm512_add_epi32(in2, _mm512_alignr_epi32(in2, iz, 16 - 2));
	in2 = _mm512_add_epi32(in2, _mm512_alignr_epi32(in2, iz, 16 - 4));
	in2 = _mm512_add_epi32(in2, _mm512_alignr_epi32(in2, iz, 16 - 8));
	in2 = _mm512_add_epi32(in2, sum1);
	_mm512_storeu_si512(&accum[32], in2);
	__m512i sum2 = _mm512_set1_epi32(accum[47]);
	__m512i in3 = _mm512_loadu_si512(&board[48]);
	in3 = _mm512_add_epi32(in3, _mm512_alignr_epi32(in3, iz, 16 - 1));
	in3 = _mm512_add_epi32(in3, _mm512_alignr_epi32(in3, iz, 16 - 2));
	in3 = _mm512_add_epi32(in3, _mm512_alignr_epi32(in3, iz, 16 - 4));
	in3 = _mm512_add_epi32(in3, _mm512_alignr_epi32(in3, iz, 16 - 8));
	in3 = _mm512_add_epi32(in3, sum2);
	_mm512_storeu_si512(&accum[48], in3);
	__m512i sum3 = _mm512_set1_epi32(accum[63]);
	__m512i in4 = _mm512_loadu_si512(&board[64]);
	in4 = _mm512_add_epi32(in4, _mm512_alignr_epi32(in4, iz, 16 - 1));
	in4 = _mm512_add_epi32(in4, _mm512_alignr_epi32(in4, iz, 16 - 2));
	in4 = _mm512_add_epi32(in4, _mm512_alignr_epi32(in4, iz, 16 - 4));
	in4 = _mm512_add_epi32(in4, _mm512_alignr_epi32(in4, iz, 16 - 8));
	in4 = _mm512_add_epi32(in4, sum3);
	_mm512_storeu_si512(&accum[64], in4);
	accum[80] = accum[79] + board[80];
	// sumが求まるので乱数の剰余を求める
	int r = my_rand(accum[80]);
	if (r > accum[79]) { // 81マス目
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
#else
	return 0;
#endif
};
