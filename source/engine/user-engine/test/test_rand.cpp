#include "test_rand.h"
#include "test.h"

// 詳細なデバッグ情報を出力するかのフラグ
constexpr bool debug_info_uniform = false;
constexpr bool debug_info_piece_existence_rand = false;
constexpr bool debug_info_accumu_rand = false;
constexpr bool debug_info_accumu_rand_avx512 = false;
constexpr bool debug_info_is_promoted_rand = false;
constexpr double permit_randomness = 0.05;
constexpr u64 random_number_per_test = 160000000;

void test_rand_main() {
	std::cout << "乱数のテストを行います" << std::endl;
	std::cout << "テスト当たりの乱数の個数： " << random_number_per_test << std::endl;
	test_uniform();
	test_piece_existence_rand();
	test_accumu_rand();
	test_accumu_rand_avx512();
	test_is_promoted_rand();
};

// 一様乱数のテスト
void test_uniform() {
	std::cout << "test_uniform" << std::endl;
	std::array<u64, 16> count;
	for (auto i = 0; i < 16; ++i) {
		count[i] = 0;
	}
	u64 loopnum = random_number_per_test;
	for (auto i = 0; i < loopnum; ++i) {
		++count[myrand.rand() % 16];
	}
	u64 max = count[0], min = count[0];
	for (auto i = 1; i < 16; ++i) {
		if (max < count[i]) { max = count[i]; }
		if (min > count[i]) { min = count[i]; }
	}
	double randomness = 16.0 * (max - min) / loopnum;
	if (randomness > permit_randomness) {
		std::cout << "randomness: " << randomness << std::endl;
		++error_count;
	}
	if (debug_info_uniform || randomness > permit_randomness) {
		std::cout << "16の剰余でのバラツキ: " << 16.0 * (max - min) / loopnum << std::endl;
		std::cout << "乱数: " << loopnum << "個, " << "最大カウント: " << max << "個, 最小カウント: " << min << "個" << std::endl;
	}
};

// 駒配置の乱数のテスト
void test_piece_existence_rand() {
	std::cout << "test_piece_existence" << std::endl;
	std::array<u64, 4> count;
	for (auto i = 0; i < 4; ++i) {
		count[i] = 0;
	}
	PieceExistence t;
	u64 loopnum = random_number_per_test;
	int a = 70, b = 80, c = 49, d = 1;
	double sum = a + b + c + d;
	for (auto i = 0; i < loopnum; ++i) {
		t = piece_existence_rand(a, b, c, d);
		switch (t) {
		case PieceExistence::B_Board: ++count[0]; break;
		case PieceExistence::W_Board: ++count[1]; break;
		case PieceExistence::B_Hand: ++count[2]; break;
		case PieceExistence::W_Hand: ++count[3]; break;
		}
	}
	double r[4];
	r[0] = ((a / sum) - (count[0] / (double)loopnum)) / (a / sum);
	r[1] = ((b / sum) - (count[1] / (double)loopnum)) / (b / sum);
	r[2] = ((c / sum) - (count[2] / (double)loopnum)) / (c / sum);
	r[3] = ((d / sum) - (count[3] / (double)loopnum)) / (d / sum);
	double randomness = r[0];
	if (randomness < r[1]) { randomness = r[1]; }
	if (randomness < r[2]) { randomness = r[2]; }
	if (randomness < r[3]) { randomness = r[3]; }
	if (randomness > permit_randomness) {
		std::cout << "randomness: " << randomness << std::endl;
		++error_count;
	}
	if (debug_info_accumu_rand || randomness > permit_randomness) {
		std::cout << "駒配置の期待値: " << a / sum << ", " << b / sum << ", " <<
			c / sum << ", " << d / sum << ", " << std::endl;
		std::cout << "結果: " << count[0] / (double)loopnum << ", " << count[1] / (double)loopnum << ", " <<
			count[2] / (double)loopnum << ", " << count[3] / (double)loopnum << std::endl;
	}
};

typedef std::array<double, SQUARES_NUMBER> TestDoubleBoard;

// PBoard乱数のテスト
void test_accumu_rand() {
	std::cout << "test_accumu_rand" << std::endl;
	IntBoard p_intboard = {
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
	IntBoard accumu;
	IntBoard count = IntBoard_ZERO;
	int t;
	u64 loopnum = random_number_per_test;
	double sum = 0.0;
	for (auto i = 0; i < SQUARES_NUMBER; ++i) {
		sum += p_intboard[i];
	}
	TestDoubleBoard expect;
	for (auto i = 0; i < SQUARES_NUMBER; ++i) {
		expect[i] = p_intboard[i] / sum;
	}
	for (auto i = 0; i < loopnum; ++i) {
		t = __accumu_rand(p_intboard, accumu);
		++count[t];
	}
	double count_sum = 0.0;
	for (auto i = 0; i < SQUARES_NUMBER; ++i) {
		count_sum += count[i];
	}
	TestDoubleBoard result;
	for (auto i = 0; i < SQUARES_NUMBER; ++i) {
		result[i] = count[i] / count_sum;
	}
	double randomness;
	for (auto i = 0; i < SQUARES_NUMBER; ++i) {
		randomness = (expect[i] - result[i]) / expect[i];
		if (randomness > permit_randomness) {
			std::cout << "randomness: " << randomness << std::endl;
			++error_count;
		}
		if (debug_info_accumu_rand || randomness > permit_randomness) {
			std::cout << "期待値: " << expect[i] << ", 結果: " << result[i] << std::endl;
		}
	}
}

// PBoard乱数のテスト (AVX512)
void test_accumu_rand_avx512() {
#ifdef AVX512
	std::cout << "test_accumu_rand (AVX512)" << std::endl;
	IntBoard2 p_intboard({
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
	IntBoard2 accumu;
	IntBoard count = IntBoard_ZERO;
	int t;
	u64 loopnum = random_number_per_test;
	double sum = 0.0;
	for (auto i = 0; i < SQUARES_NUMBER; ++i) {
		sum += p_intboard.p[i];
	}
	TestDoubleBoard expect;
	for (auto i = 0; i < SQUARES_NUMBER; ++i) {
		expect[i] = p_intboard.p[i] / sum;
	}
	for (auto i = 0; i < loopnum; ++i) {
		t = __accumu_rand(p_intboard, accumu);
		++count[t];
	}
	double count_sum = 0.0;
	for (auto i = 0; i < SQUARES_NUMBER; ++i) {
		count_sum += count[i];
	}
	TestDoubleBoard result;
	for (auto i = 0; i < SQUARES_NUMBER; ++i) {
		result[i] = count[i] / count_sum;
	}
	double randomness;
	for (auto i = 0; i < SQUARES_NUMBER; ++i) {
		randomness = (expect[i] - result[i]) / expect[i];
		if (randomness > permit_randomness) {
			std::cout << "randomness: " << randomness << std::endl;
			++error_count;
		}
		if (debug_info_accumu_rand_avx512 || randomness > permit_randomness) {
			std::cout << "期待値: " << expect[i] << ", 結果: " << result[i] << std::endl;
		}
	}
#endif
}

// is_promoted_randのテスト
void test_is_promoted_rand() {
	std::cout << "test_is_promoted_rand" << std::endl;
	PromoteP p = { 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.0, 1.0 };
	Square sq[9] = { SQ_11, SQ_22, SQ_33, SQ_44, SQ_55, SQ_66, SQ_77, SQ_88, SQ_99 };
	u64 loopnum = random_number_per_test;
	bool t;
	u64 count_true, count_false;
	double expect, result;
	double randomness;
	for (auto j = 0; j < 9; ++j) {
		count_true = 0, count_false = 0;
		for (auto i = 0; i < loopnum; ++i) {
			t = is_promoted_rand(sq[j], p);
			if (t) { ++count_true; }
			else { ++count_false; }
		}
		expect = p[j];
		result = (double)count_true / (count_true + count_false);
		randomness = (expect - result) / expect;
		if (randomness > permit_randomness) {
			std::cout << "randomness: " << randomness << std::endl;
			++error_count;
		}
		if (debug_info_is_promoted_rand || randomness > permit_randomness) {
			std::cout << "p[" << j << "] 期待値: " << expect << ", 結果: " << result << std::endl;
		}
		if (j == 7 && count_true != 0) {
			std::cout << "成りカウントが0で成りが発生しました: " << count_true << std::endl;
			++error_count;
		}
		if (j == 8 && count_false != 0) {
			std::cout << "成りカウントが1で不成が発生しました: " << count_false << std::endl;
			++error_count;
		}
	}
}
