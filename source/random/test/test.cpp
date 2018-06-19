#include <iomanip>
#include "../../shogi.h"
#include "../int_board.h"
#include "../ex_board.h"
#include "test_rand.h"
#include "test_board.h"
#include "test.h"

// 詳細なデバッグ情報を出力するかのフラグ
constexpr bool debug_info_file_index_table = false;
constexpr bool debug_info_rank_index_table = false;


u64 error_count = 0;

// テストのmain関数
void test_main(Position& pos_) {
	test_file_index_table();
	test_rank_index_table();
	test_promotep();
	test__and();
	test__ninp();
	test__accumu();
	test_bitboard_to_intboard();
	accumu_rand();
#ifdef AVX512
	test__and2();
	test__ninp2();
	test__accumu2();
	test_bitboard_to_intboard2();
	test_set_lance();
#else
	test_set_king();
#endif
	test_rand_main();
	test_board_main(pos_);
	std::cout << "エラーカウントは「" << error_count << "」です。" << std::endl;
};

// file_index_table[]のテスト
void assert_file_index_table(const Square &sq, const int &expect) {
	if (expect != file_index_table[sq]) {
		std::cout << "Assert[file_index_table] -> Square: " << file_of(sq) << " != " << file_index_table[sq] << ", expect: " << expect << std::endl;
		std::cout << Bitboard(sq) << std::endl;
		++error_count;
	}
	else if (debug_info_file_index_table) {
		std::cout << "Square: " << sq << " == " << file_index_table[sq] << std::endl;
	}
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
	else if (debug_info_rank_index_table) {
		std::cout << "Square: " << sq << " == " << rank_index_table[sq] << std::endl;
	}
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
	IntBoard ninp_board = {
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
	assert__ninp(base_board, ninp_board, expect);
};

void assert__ninp(const IntBoard &base_board, const IntBoard &ninp_board, const IntBoard &expect) {
	IntBoard result1 = base_board;
	IntBoard result2 = ninp_board;
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
		std::cout << "Assert[bitboard_to_intboard] -> result: " << std::endl << result << "expect: " << std::endl << expect << std::endl;
		++error_count;
	}
};

void accumu_rand() {
	std::cout << "test: accumu_rand" << std::endl;
	IntBoard i2 = {
		10, 10, 10, 10, 10, 10, 10, 10, 10,
		10, 10, 10, 10, 10, 10, 10, 10, 10,
		10, 10, 10, 10, 10, 10, 10, 10, 10,
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 10, 0, 0, 0, 0
	};
	IntBoard i = {
		0, 10, 0, 0, 0, 0, 10, 10, 10,
		0, 0, 0, 0, 0, 0, 10, 10, 10,
		0, 0, 0, 0, 0, 0, 10, 10, 10,
		0, 0, 0, 0, 0, 0, 10, 10, 10,
		0, 0, 0, 0, 0, 0, 10, 10, 10,
		0, 0, 0, 0, 0, 0, 10, 10, 10,
		0, 0, 0, 0, 0, 0, 10, 10, 10,
		0, 0, 0, 0, 0, 0, 10, 10, 10,
		0, 0, 0, 0, 0, 0, 0, 0, 0
	};
	PBoard p(i);
	Square sq;
	int r;
	for (auto i = 0; i < 100; ++i) {
		r = p.accumu_rand();
		sq = sq_table[r];
		if (file_of(sq) >= 4) {
			std::cout << "r:" << r << std::endl;
			std::cout << "sq: " << (int)sq << std::endl;
			std::cout << sq << std::endl;
		}
	}
};

void test_set_king() {
	std::cout << "test: set_king" << std::endl;
	IntBoard i0 = {
		10, 10, 10, 10, 10, 10, 10, 10, 10,
		10, 10, 10, 10, 10, 10, 10, 10, 10,
		10, 10, 10, 10, 10, 10, 10, 10, 10,
		10, 10, 10, 10, 10, 10, 10, 10, 10,
		10, 10, 10, 10, 10, 10, 10, 10, 10,
		10, 10, 10, 10, 10, 10, 10, 10, 10,
		10, 10, 10, 10, 10, 10, 10, 10, 10,
		10, 10, 10, 10, 10, 10, 10, 10, 10,
		10, 10, 10, 10, 10, 10, 10, 10, 10
		};
	IntBoard i1 = {
		10, 10, 10, 10, 10, 10, 10, 10, 10,
		10, 10, 10, 10, 10, 10, 10, 10, 10,
		10, 10, 10, 10, 10, 10, 10, 10, 10,
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0
		};
	PBoard b_king_p(i0);
	PBoard w_king_p(i1);

	for (auto i = 0; i < 10000; ++i) {
		PBoard pb1 = b_king_p;
		Square sq1 = sq_table[pb1.rand()];
		PBoard pb2 = w_king_p;
		Bitboard bit_b_king(sq1);

		pb2.ninp(KingEffectBB[sq1] | bit_b_king); // 先手玉の9近傍を除く
		Bitboard x = KingEffectBB[sq1] | bit_b_king;
		Square sq2 = sq_table[pb2.accumu_rand()];
		Bitboard bit_w_king(sq2);

		assert_set_king(sq1, bit_b_king, sq2, bit_w_king, x, pb1, pb2);
	}
};

void assert_set_king(const Square &king1, const Bitboard &king1b, const Square &king2, const Bitboard &king2b,
	const Bitboard &x,
	const PBoard &p1, const PBoard &p2) {
	int b_file = (int)file_of(king1);
	int w_file = (int)file_of(king2);
	int b_rank = (int)rank_of(king1);
	int w_rank = (int)rank_of(king2);
	if (abs(b_file - w_file) <= 1 && abs(b_rank - w_rank) <= 1) {
		std::cout << "Assert[set_king] -> result: " << std::endl;
		std::cout << "king1: " << king1 << std::endl;
		std::cout << king1b << std::endl;
		std::cout << "king2: " << king2 << std::endl;
		std::cout << king2b << std::endl;
		std::cout << x << std::endl;
		std::cout << "p1: " << std::endl << p1 << std::endl;
		std::cout << "p2: " << std::endl << p2 << std::endl;
	}
};


void test_effect() {
	// std::cout << "test: effect" << std::endl;
};

#ifdef AVX512

void test__and2() {
	std::cout << "test: __and (AVX512)" << std::endl;
	IntBoard2 base_board({
		40, 20, 10, 20, 80, 150, 100, 420, 380,
		10, 10, 10, 20, 50, 150, 100, 350, 250,
		10, 10, 10, 10, 40, 60, 70, 180, 80,
		5, 32, 28, 10, 10, 10, 28, 100, 5,
		5, 32, 28, 10, 10, 10, 28, 100, 5,
		10, 80, 120, 80, 160, 80, 160, 240, 10,
		1, 1, 120, 40, 50, 40, 100, 50, 2,
		2, 10, 50, 80, 110, 150, 100, 820, 250,
		5, 10, 10, 20, 20, 20, 30, 250, 100
	});
	IntBoard2 and_board({
		0, 0, 0, -1, -1, -1, 0, 0, 0,
		0, 0, 0, -1, -1, -1, 0, 0, 0,
		0, 0, 0, 0, 0, 0, -1, -1, -1,
		0, 0, 0, 0, 0, 0, -1, -1, -1,
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		-1, -1, -1, -1, -1, -1, -1, -1, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1
	});
	IntBoard2 expect({
		0, 0, 0, 20, 80, 150, 0, 0, 0,
		0, 0, 0, 20, 50, 150, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 70, 180, 80,
		0, 0, 0, 0, 0, 0, 28, 100, 5,
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		10, 80, 120, 80, 160, 80, 160, 240, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 250,
		5, 10, 10, 20, 20, 20, 30, 250, 100
	});
	assert__and2(base_board, and_board, expect);
};

void assert__and2(const IntBoard2 &base_board, const IntBoard2 &and_board, const IntBoard2 &expect) {
	IntBoard2 result1 = base_board;
	IntBoard2 result2 = and_board;
	__and(result1, result2);
	if (result1 != expect) {
		std::cout << "Assert[__and (AVX512)] -> result: " << std::endl << result1 << std::endl << "expect: " << std::endl << expect << std::endl;
		++error_count;
	}
};

void test__ninp2() {
	std::cout << "test: __ninp (AVX512)" << std::endl;
	IntBoard2 base_board({
		40, 20, 10, 20, 80, 150, 100, 420, 380,
		10, 10, 10, 20, 50, 150, 100, 350, 250,
		10, 10, 10, 10, 40, 60, 70, 180, 80,
		5, 32, 28, 10, 10, 10, 28, 100, 5,
		5, 32, 28, 10, 10, 10, 28, 100, 5,
		10, 80, 120, 80, 160, 80, 160, 240, 10,
		1, 1, 120, 40, 50, 40, 100, 50, 2,
		2, 10, 50, 80, 110, 150, 100, 820, 250,
		5, 10, 10, 20, 20, 20, 30, 250, 100
	});
	IntBoard2 ninp_board({
		0, 0, 0, -1, -1, -1, 0, 0, 0,
		0, 0, 0, -1, -1, -1, 0, 0, 0,
		0, 0, 0, 0, 0, 0, -1, -1, -1,
		0, 0, 0, 0, 0, 0, -1, -1, -1,
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		-1, -1, -1, -1, -1, -1, -1, -1, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1
	});
	IntBoard2 expect({
		40, 20, 10, 0, 0, 0, 100, 420, 380,
		10, 10, 10, 0, 0, 0, 100, 350, 250,
		10, 10, 10, 10, 40, 60, 0, 0, 0,
		5, 32, 28, 10, 10, 10, 0, 0, 0,
		5, 32, 28, 10, 10, 10, 28, 100, 5,
		0, 0, 0, 0, 0, 0, 0, 0, 10,
		1, 1, 120, 40, 50, 40, 100, 50, 2,
		2, 10, 50, 80, 110, 150, 100, 820, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0
	});
	assert__ninp2(base_board, ninp_board, expect);
};

void assert__ninp2(const IntBoard2 &base_board, const IntBoard2 &ninp_board, const IntBoard2 &expect) {
	IntBoard2 result1 = base_board;
	IntBoard2 result2 = ninp_board;
	__ninp(result1, result2);
	if (result1 != expect) {
		std::cout << "Assert[__ninp (AVX512)] -> result: " << std::endl << result1 << std::endl << "expect: " << std::endl << expect << std::endl;
		++error_count;
	}
};

void test__accumu2() {
	std::cout << "test: __accumu (AVX512)" << std::endl;
	IntBoard2 base_board({
		40, 20, 10, 20, 80, 150, 100, 420, 380,
		10, 10, 10, 20, 50, 150, 100, 350, 250,
		10, 10, 10, 10, 40, 60, 70, 180, 80,
		5, 32, 28, 10, 10, 10, 28, 100, 5,
		5, 32, 28, 10, 10, 10, 28, 100, 5,
		10, 80, 120, 80, 160, 80, 160, 240, 10,
		1, 1, 120, 40, 50, 40, 100, 50, 2,
		2, 10, 50, 80, 110, 150, 100, 820, 250,
		5, 10, 10, 20, 20, 20, 30, 250, 100
	});
	IntBoard2 accumu_board;
	IntBoard2 expect({
		40, 60, 70, 90, 170, 320, 420, 840, 1220,
		1230, 1240, 1250, 1270, 1320, 1470, 1570, 1920, 2170,
		2180, 2190, 2200, 2210, 2250, 2310, 2380, 2560, 2640,
		2645, 2677, 2705, 2715, 2725, 2735, 2763, 2863, 2868,
		2873, 2905, 2933, 2943, 2953, 2963, 2991, 3091, 3096,
		3106, 3186, 3306, 3386, 3546, 3626, 3786, 4026, 4036,
		4037, 4038, 4158, 4198, 4248, 4288, 4388, 4438, 4440,
		4442, 4452, 4502, 4582, 4692, 4842, 4942, 5762, 6012,
		6017, 6027, 6037, 6057, 6077, 6097, 6127, 6377, 6477
	});
	assert__accumu2(base_board, accumu_board, expect);
};

void assert__accumu2(const IntBoard2 &base_board, const IntBoard2 &accumu_board, const IntBoard2 &expect) {
	IntBoard2 result1 = base_board;
	IntBoard2 result2 = accumu_board;
	__accumu(result1, result2);
	if (result2 != expect) {
		std::cout << "Assert[__accumu (AVX512)] -> result: " << std::endl << result2 << std::endl << "expect: " << std::endl << expect << std::endl;
		++error_count;
	}
};

void test_bitboard_to_intboard2() {
	std::cout << "test: bitboard_to_intboard2 (AVX512)" << std::endl;
	IntBoard2 intboard_bitright({
		-1, -1, -1, 0, 0, 0, 0, 0, 0,
		-1, -1, -1, 0, 0, 0, 0, 0, 0,
		-1, -1, -1, 0, 0, 0, 0, 0, 0,
		-1, -1, -1, 0, 0, 0, 0, 0, 0,
		-1, -1, -1, 0, 0, 0, 0, 0, 0,
		-1, -1, -1, 0, 0, 0, 0, 0, 0,
		-1, -1, -1, 0, 0, 0, 0, 0, 0,
		-1, -1, -1, 0, 0, 0, 0, 0, 0,
		-1, -1, -1, 0, 0, 0, 0, 0, 0
	});
	IntBoard2 intboard_bitmiddle({
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		-1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1,
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0
	});
	assert_bitboard_to_intboard2(BitRight, intboard_bitright);
	assert_bitboard_to_intboard2(BitMiddle, intboard_bitmiddle);
};
void assert_bitboard_to_intboard2(const Bitboard &bit_board, const IntBoard2 &expect) {
	IntBoard2 result = bitboard_to_intboard2(bit_board);
	if (result != expect) {
		std::cout << "Assert[bitboard_to_intboard (AVX512)] -> result: " << std::endl << result << "expect: " << std::endl << expect << std::endl;
		++error_count;
	}
};


void test_set_lance() {
	std::cout << "test: set_lance (AVX512)" << std::endl;
	IntBoard2 i0({
		10, 10, 10, 10, 10, 10, 10, 10, 10,
		10, 10, 10, 10, 10, 10, 10, 10, 10,
		10, 10, 10, 10, 10, 10, 10, 10, 10,
		10, 10, 10, 10, 10, 10, 10, 10, 10,
		10, 10, 10, 10, 10, 10, 10, 10, 10,
		10, 10, 10, 10, 10, 10, 10, 10, 10,
		10, 10, 10, 10, 10, 10, 10, 10, 10,
		10, 10, 10, 10, 10, 10, 10, 10, 10,
		10, 10, 10, 10, 10, 10, 10, 10, 10
		});
	IntBoard2 i1({
		10, 10, 10, 10, 10, 10, 10, 10, 10,
		10, 10, 10, 10, 10, 10, 10, 10, 10,
		10, 10, 10, 10, 10, 10, 10, 10, 10,
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0
		});
	PBoard p0(i0);
	PBoard p1(i1);
	assert_set_lance(p1, SQ_51, WHITE, BitLancePromoteBlack);
	assert_set_lance(p1, SQ_52, WHITE, BitLancePromoteBlack);
	assert_set_lance(p1, SQ_11, WHITE, BitLancePromoteBlack);
	assert_set_lance(p1, SQ_12, WHITE, BitLancePromoteBlack);
	assert_set_lance(p1, SQ_91, WHITE, BitLancePromoteBlack);
	assert_set_lance(p1, SQ_92, WHITE, BitLancePromoteBlack);
	for (auto i = 0; i < 100000; ++i) {
		assert_set_lance(p1, sq_table[p0.rand()], WHITE, BitLancePromoteBlack);
	}
};
void assert_set_lance(const PBoard &p, const Square &e_king, const Color e_c, const Bitboard confirm_promote) {
	PBoard pb = p;
	pb.ninp(bitboard_to_intboard2(PawnEffectBB[e_king][e_c] | e_king | (cross00StepEffectBB[e_king] & confirm_promote)));

	pb.accumu(); // 香は例外が発生することがあるので一旦累積和だけ計算しておく
	Square sq;
gen_piece_pos: // 駒位置生成ポイントを表すステートメントラベル
	sq = sq_table[pb.accumu_rand()]; // 香の位置を確定させる
	if (cross00StepEffectBB[e_king] & confirm_promote & sq) { // この位置だと香でも成香でも合法にならない
		std::cout << "Assert[set_lance (AVX512)] -> result: " << std::endl;
		std::cout << "e_king: " << e_king << std::endl;
		std::cout << "PawnEffectBB[e_king][e_c]" << std::endl;
		std::cout << PawnEffectBB[e_king][e_c] << std::endl;
		std::cout << "cross00StepEffectBB[e_king] & confirm_promote" << std::endl;
		std::cout << (cross00StepEffectBB[e_king] & confirm_promote) << std::endl;
		std::cout << "sq: " << sq << std::endl;
		std::cout << Bitboard(sq) << std::endl;
		std::cout << "pb" << std::endl;
		std::cout << pb << std::endl;
		goto gen_piece_pos; // 条件を満たさなかったので駒位置生成からやり直す
	}
};

#endif
