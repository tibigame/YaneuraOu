#include <iomanip>
#include "../../shogi.h"
#include "random_board.h"
#include "int_board.h"
#include "ex_board.h"
#include "test.h"

// 詳細なデバッグ情報を出力するか
//#define DEBUG_INFO

u64 error_count = 0;

// テストのmain関数
void test_main() {
#ifdef DEBUG_INFO
	std::cout << "DEBUG INFOを出力します。" << std::endl;
#else
	std::cout << "DEBUG INFOは出力しません。" << std::endl;
#endif
	test_file_index_table();
	test_rank_index_table();
	test_promotep();
	test__and();
	test__ninp();
	test__accumu();
	test_bitboard_to_intboard();
	test_effect();
	std::cout << "エラーカウントは「" << error_count << "」です。" << std::endl;
};

// file_index_table[]のテスト
void assert_file_index_table(const Square &sq, const int &expect) {
	if (expect != file_index_table[sq]) {
		std::cout << "Assert[file_index_table] -> Square: " << file_of(sq) << " != " << file_index_table[sq] << ", expect: " << expect << std::endl;
		std::cout << Bitboard(sq) << std::endl;
		++error_count;
	}
#ifdef DEBUG_INFO
	else {
		std::cout << "Square: " << sq << " == " << file_index_table[sq] << std::endl;
	}
#endif
};

void test_file_index_table() {
	std::cout << "test: file_index_table" << std::endl;
	assert_file_index_table(SQ_11, 0);
	assert_file_index_table(SQ_12, 0);
	assert_file_index_table(SQ_13, 0);
	assert_file_index_table(SQ_14, 0);
	assert_file_index_table(SQ_15, 0);
	assert_file_index_table(SQ_16, 0);
	assert_file_index_table(SQ_17, 0);
	assert_file_index_table(SQ_18, 0);
	assert_file_index_table(SQ_19, 0);
	assert_file_index_table(SQ_21, 1);
	assert_file_index_table(SQ_22, 1);
	assert_file_index_table(SQ_23, 1);
	assert_file_index_table(SQ_24, 1);
	assert_file_index_table(SQ_25, 1);
	assert_file_index_table(SQ_26, 1);
	assert_file_index_table(SQ_27, 1);
	assert_file_index_table(SQ_28, 1);
	assert_file_index_table(SQ_29, 1);
	assert_file_index_table(SQ_91, 8);
	assert_file_index_table(SQ_92, 8);
	assert_file_index_table(SQ_93, 8);
	assert_file_index_table(SQ_94, 8);
	assert_file_index_table(SQ_95, 8);
	assert_file_index_table(SQ_96, 8);
	assert_file_index_table(SQ_97, 8);
	assert_file_index_table(SQ_98, 8);
	assert_file_index_table(SQ_99, 8);
};

// rank_index_table[]のテスト
void assert_rank_index_table(const Square &sq, const int &expect) {
	if (expect != rank_index_table[sq]) {
		std::cout << "Assert[rank_index_table] -> Square: " << sq << " != " << rank_index_table[sq] << ", expect: " << expect << std::endl;
		std::cout << Bitboard(sq) << std::endl;
		++error_count;
	}
#ifdef DEBUG_INFO
	else {
		std::cout << "Square: " << sq << " == " << rank_index_table[sq] << std::endl;
	}
#endif
};

void test_rank_index_table() {
	std::cout << "test: rank_index_table" << std::endl;
	assert_rank_index_table(SQ_11, 0);
	assert_rank_index_table(SQ_21, 0);
	assert_rank_index_table(SQ_31, 0);
	assert_rank_index_table(SQ_41, 0);
	assert_rank_index_table(SQ_51, 0);
	assert_rank_index_table(SQ_61, 0);
	assert_rank_index_table(SQ_71, 0);
	assert_rank_index_table(SQ_81, 0);
	assert_rank_index_table(SQ_91, 0);
	assert_rank_index_table(SQ_12, 1);
	assert_rank_index_table(SQ_22, 1);
	assert_rank_index_table(SQ_32, 1);
	assert_rank_index_table(SQ_42, 1);
	assert_rank_index_table(SQ_52, 1);
	assert_rank_index_table(SQ_62, 1);
	assert_rank_index_table(SQ_72, 1);
	assert_rank_index_table(SQ_82, 1);
	assert_rank_index_table(SQ_92, 1);
	assert_rank_index_table(SQ_19, 8);
	assert_rank_index_table(SQ_29, 8);
	assert_rank_index_table(SQ_39, 8);
	assert_rank_index_table(SQ_49, 8);
	assert_rank_index_table(SQ_59, 8);
	assert_rank_index_table(SQ_69, 8);
	assert_rank_index_table(SQ_79, 8);
	assert_rank_index_table(SQ_89, 8);
	assert_rank_index_table(SQ_99, 8);
};

// Promote関連の関数のテスト
void test_promotep() {
	std::cout << "test: promotep" << std::endl;
	PromoteP p = { 0.75, 0.8, 0.7, 0.2, 0.15, 0.1, 0.1, 0.1, 0.05 };
	PromoteP p_reverse = { 0.05, 0.1, 0.1, 0.1, 0.15, 0.2, 0.7, 0.8, 0.75 };
	assert_reverse_promotep(p, p_reverse);
	test_support_is_promoted_rand(SQ_12, p);
	test_support_is_promoted_rand(SQ_12, p_reverse);
	test_support_is_promoted_rand(SQ_13, p);
	test_support_is_promoted_rand(SQ_31, p);
	test_support_is_promoted_rand(SQ_58, p);
	test_support_is_promoted_rand(SQ_89, p_reverse);
	test_support_is_promoted_rand(SQ_91, p_reverse);
	test_support_is_promoted_rand(SQ_99, p_reverse);
};

void assert_reverse_promotep(const PromoteP &p, const PromoteP &expect) {
	PromoteP result = reverse(p);
	for (auto i = 0; i < 9; ++i) {
		if (result[i] != expect[i]) {
			std::cout << "Assert[reverse PromoteP] -> result[ " << i << "]: " << result[i] << ", expect: " << expect[i] << std::endl;
			++error_count;
		}
	}
};

void test_support_is_promoted_rand(const Square &sq, const PromoteP &p) {
	int rank = rank_of(sq);
	double p_double = p[rank];
	u32 ex = static_cast<u32>(UINT_MAX * p_double);
	assert_support_is_promoted_rand(sq, p, ex);
};

void assert_support_is_promoted_rand(const Square &sq, const PromoteP &p, const u32 &expect) {
	u32 result = support_is_promoted_rand(sq, p);
	if (result != expect) {
		std::cout << "Assert[support_is_promoted_rand] -> result: " << result << ", expect: " << expect << std::endl;
		++error_count;
	}
};

void test__and() {
	std::cout << "test: __and" << std::endl;
	IntBoard base_board = {
		40, 20, 10, 20, 80, 150, 100, 420, 380,
		10, 10, 10, 20, 50, 150, 100, 350, 250,
		10, 10, 10, 10, 40, 60, 70, 180, 80,
		5, 32, 28, 10, 10, 10, 28, 100, 5,
		5, 32, 28, 10, 10, 10, 28, 100, 5,
		10, 80, 120, 80, 160, 80, 160, 240, 10,
		1, 1, 120, 40, 50, 40, 100, 50, 2,
		2, 10, 50, 80, 110, 150, 100, 820, 250,
		5, 10, 10, 20, 20, 20, 30, 250, 100
	};
	IntBoard and_board = {
		0, 0, 0, -1, -1, -1, 0, 0, 0,
		0, 0, 0, -1, -1, -1, 0, 0, 0,
		0, 0, 0, 0, 0, 0, -1, -1, -1,
		0, 0, 0, 0, 0, 0, -1, -1, -1,
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		-1, -1, -1, -1, -1, -1, -1, -1, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1
	};
	IntBoard expect = {
		0, 0, 0, 20, 80, 150, 0, 0, 0,
		0, 0, 0, 20, 50, 150, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 70, 180, 80,
		0, 0, 0, 0, 0, 0, 28, 100, 5,
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		10, 80, 120, 80, 160, 80, 160, 240, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 250,
		5, 10, 10, 20, 20, 20, 30, 250, 100
	};
	assert__and(base_board, and_board, expect);
};

void assert__and(const IntBoard &base_board, const IntBoard &and_board, const IntBoard &expect) {
	IntBoard result1 = base_board;
	IntBoard result2 = and_board;
	__and(result1, result2);
	if (result1 != expect) {
		std::cout << "Assert[__and] -> result: " << std::endl << result1 << std::endl << "expect: " << std::endl << expect << std::endl;
		++error_count;
	}
};

void test__ninp() {
	std::cout << "test: __ninp" << std::endl;
	IntBoard base_board = {
		40, 20, 10, 20, 80, 150, 100, 420, 380,
		10, 10, 10, 20, 50, 150, 100, 350, 250,
		10, 10, 10, 10, 40, 60, 70, 180, 80,
		5, 32, 28, 10, 10, 10, 28, 100, 5,
		5, 32, 28, 10, 10, 10, 28, 100, 5,
		10, 80, 120, 80, 160, 80, 160, 240, 10,
		1, 1, 120, 40, 50, 40, 100, 50, 2,
		2, 10, 50, 80, 110, 150, 100, 820, 250,
		5, 10, 10, 20, 20, 20, 30, 250, 100
	};
	IntBoard and_board = {
		0, 0, 0, -1, -1, -1, 0, 0, 0,
		0, 0, 0, -1, -1, -1, 0, 0, 0,
		0, 0, 0, 0, 0, 0, -1, -1, -1,
		0, 0, 0, 0, 0, 0, -1, -1, -1,
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		-1, -1, -1, -1, -1, -1, -1, -1, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1
	};
	IntBoard expect = {
		40, 20, 10, 0, 0, 0, 100, 420, 380,
		10, 10, 10, 0, 0, 0, 100, 350, 250,
		10, 10, 10, 10, 40, 60, 0, 0, 0,
		5, 32, 28, 10, 10, 10, 0, 0, 0,
		5, 32, 28, 10, 10, 10, 28, 100, 5,
		0, 0, 0, 0, 0, 0, 0, 0, 10,
		1, 1, 120, 40, 50, 40, 100, 50, 2,
		2, 10, 50, 80, 110, 150, 100, 820, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0
	};
	assert__ninp(base_board, and_board, expect);
};

void assert__ninp(const IntBoard &base_board, const IntBoard &and_board, const IntBoard &expect) {
	IntBoard result1 = base_board;
	IntBoard result2 = and_board;
	__ninp(result1, result2);
	if (result1 != expect) {
		std::cout << "Assert[__ninp] -> result: " << std::endl << result1 << std::endl << "expect: " << std::endl << expect << std::endl;
		++error_count;
	}
};

void test__accumu() {
	std::cout << "test: __accumu" << std::endl;
	IntBoard base_board = {
		40, 20, 10, 20, 80, 150, 100, 420, 380,
		10, 10, 10, 20, 50, 150, 100, 350, 250,
		10, 10, 10, 10, 40, 60, 70, 180, 80,
		5, 32, 28, 10, 10, 10, 28, 100, 5,
		5, 32, 28, 10, 10, 10, 28, 100, 5,
		10, 80, 120, 80, 160, 80, 160, 240, 10,
		1, 1, 120, 40, 50, 40, 100, 50, 2,
		2, 10, 50, 80, 110, 150, 100, 820, 250,
		5, 10, 10, 20, 20, 20, 30, 250, 100
	};
	IntBoard accumu_board;
	IntBoard expect = {
		40, 60, 70, 90, 170, 320, 420, 840, 1220,
		1230, 1240, 1250, 1270, 1320, 1470, 1570, 1920, 2170,
		2180, 2190, 2200, 2210, 2250, 2310, 2380, 2560, 2640,
		2645, 2677, 2705, 2715, 2725, 2735, 2763, 2863, 2868,
		2873, 2905, 2933, 2943, 2953, 2963, 2991, 3091, 3096,
		3106, 3186, 3306, 3386, 3546, 3626, 3786, 4026, 4036,
		4037, 4038, 4158, 4198, 4248, 4288, 4388, 4438, 4440,
		4442, 4452, 4502, 4582, 4692, 4842, 4942, 5762, 6012,
		6017, 6027, 6037, 6057, 6077, 6097, 6127, 6377, 6477
	};
	assert__accumu(base_board, accumu_board, expect);
};

void assert__accumu(const IntBoard &base_board, const IntBoard &accumu_board, const IntBoard &expect) {
	IntBoard result1 = base_board;
	IntBoard result2 = accumu_board;
	__accumu(result1, result2);
	if (result2 != expect) {
		std::cout << "Assert[__accumu] -> result: " << std::endl << result2 << std::endl << "expect: " << std::endl << expect << std::endl;
		++error_count;
	}
};

void test_bitboard_to_intboard() {
	std::cout << "test: bitboard_to_intboard" << std::endl;
	IntBoard intboard_bitright = {
		-1, -1, -1, 0, 0, 0, 0, 0, 0,
		-1, -1, -1, 0, 0, 0, 0, 0, 0,
		-1, -1, -1, 0, 0, 0, 0, 0, 0,
		-1, -1, -1, 0, 0, 0, 0, 0, 0,
		-1, -1, -1, 0, 0, 0, 0, 0, 0,
		-1, -1, -1, 0, 0, 0, 0, 0, 0,
		-1, -1, -1, 0, 0, 0, 0, 0, 0,
		-1, -1, -1, 0, 0, 0, 0, 0, 0,
		-1, -1, -1, 0, 0, 0, 0, 0, 0
	};
	IntBoard intboard_bitmiddle = {
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		-1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1,
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0
	};
	assert_bitboard_to_intboard(BitRight, intboard_bitright);
	assert_bitboard_to_intboard(BitMiddle, intboard_bitmiddle);
};
void assert_bitboard_to_intboard(const Bitboard &bit_board, const IntBoard &expect) {
	IntBoard result = bitboard_to_intboard(bit_board);
	if (result != expect) {
		std::cout << "Assert[bitboard_to_intboard] -> result: " << std::endl << result << "expect: " << expect << std::endl;
		++error_count;
	}
};


void test_effect() {
	// std::cout << "test: effect" << std::endl;
};

