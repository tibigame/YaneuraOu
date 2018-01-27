#include <cmath>
#include <vector>
#include <iostream>
#include <iomanip>
#include "../my_rand.h"
#include "stat.h"

// 統計関連のユーティリティ

// 勝率からレーティング差を計算する
double elorating(double winrate) {
	const double rating_max = SHRT_MAX;
	const double rating_min = SHRT_MIN;
	if (winrate >= 1.0) {
		return rating_max;
	}
	if (winrate <= 0.0) {
		return rating_min;
	}
	const double rating = 400.0 * log10(winrate / (1.0 - winrate));
	if (rating >= rating_max) {
		return rating_max;
	}
	if (rating <= rating_min) {
		return rating_min;
	}
	return rating;
}

// レーティング差から勝率を求める
double inv_elorating(double diff_rating) {
	return 1.0 / (1.0 + std::pow(10.0, -diff_rating / 400.0));
}

constexpr double PONA_CONST = 600.0;
// 評価値から勝率を求める
double eval_winrate(int16_t eval) {
	return 1.0 / (1.0 + std::exp(- eval / PONA_CONST));
}

// 勝率から評価値を求める
double inv_eval_winrate(double winrate) {
	if (winrate - 1.0 >= 0.0) { return 32768; }
	if (winrate <= 0.0) { return -32768; }
	return -PONA_CONST * log(1.0 / winrate - 1.0);
}

constexpr double MATH_PI = 3.141592653589793238462643383279;
// 正規分布の確率密度関数
inline double dnorm(double x) {
	return std::exp(-x*x / 2.0) / std::sqrt(2.0*MATH_PI);
};

// 正規分布の確率密度関数の累積がp以上となる値を計算する
double integral_dnorm(double qn)
{
	static double b[11] = { 1.570796288,     0.03706987906,  -0.8364353589e-3,
		-0.2250947176e-3, 0.6841218299e-5, 0.5824238515e-5,
		-0.104527497e-5,  0.8360937017e-7,-0.3231081277e-8,
		0.3657763036e-10,0.6936233982e-12 };
	double w1, w3;
	int i;

	if (qn < 0. || 1. < qn)
	{
		fprintf(stderr, "Error : qn <= 0 or qn >= 1  in pnorm()!\n");
		return 0.;
	}
	if (qn == 0.5)	return 0.;

	w1 = qn;
	if (qn > 0.5)	w1 = 1. - w1;
	w3 = -log(4. * w1 * (1. - w1));
	w1 = b[0];
	for (i = 1; i < 11; i++)	w1 += (b[i] * pow(w3, (double)i));
	if (qn > 0.5)	return sqrt(w1 * w3);
	return -sqrt(w1 * w3);
}

// ベータ関数
double beta(double a, double b) {
	return std::tgamma(a) * std::tgamma(b) / (std::tgamma(a + b));
};

// ベータ分布の確率密度関数
double beta_p(double x, double a, double b) {
	return std::pow(x, a - 1.0) * std::pow(1.0 - x, b - 1.0) / beta(a, b);
};

class BetaDistribution {
	double eps;
	double val_a, val_b;

	//Ix : regularized incomplete beta function
	double Ix(double x, double a, double b) {
		if (a <= 0) return -1;
		if (b <= 0) {
			if (x < 1) return 0;
			if (fabs(x - 1) < eps) return 1.0;
			return -1;
		}

		if (x > (a + 1) / (a + b + 2)) return 1 - Ix(1 - x, b, a);
		if (x <= 0) return 0;
		double p1 = 0, q1 = 1;
		double p2 = exp(a*log(x) + b*log(1 - x) + lgamma(a + b) - lgamma(a) - lgamma(b)) / a, q2 = 1;
		double prev, d;
		for (int k = 0; k<500;) {
			prev = p2;
			d = -((a + k) * (a + b + k) * x) / ((a + 2 * k) * (a + 2 * k + 1));
			p1 = p1*d + p2;
			q1 = q1*d + q2;
			k++;
			d = (k * (b - k) * x) / ((a + 2 * k - 1) * (a + 2 * k));
			p2 = p2*d + p1;
			q2 = q2*d + q1;
			if (fabs(q2) < eps) {
				q2 = 1e+9;
				continue;
			}
			p1 /= q2;
			q1 /= q2;
			p2 /= q2;
			q2 = 1;
			if (fabs(p2 - prev) < eps) return p2;
		}
		return -1;
	}
public:
	BetaDistribution(double val_a, double val_b) :
		val_a(val_a),
		val_b(val_b),
		eps(1e-5)
	{}

	//Int_0^Q { dPhi(x) } >= p
	double quantile(double p) {
		double lb = 0.0, ub = 1.0;
		for (int i = 0; i<200; i++) {
			double m = (lb + ub) / 2.0;
			if (Ix(m, val_a, val_b) < p) lb = m;
			else ub = m;
		}
		return lb;
	}
};

// # clopper_pearson法による二項分布近似
void clopper_pearson(const double k, const double n, const double alpha,
	double &lower, double &upper) {
	double alpha2 = (1 - alpha) / 2.0;
	BetaDistribution b_lower(k, n - k + 1.0);
	lower = b_lower.quantile(alpha2);
	BetaDistribution b_upper(k + 1.0, n - k);
	upper = b_upper.quantile(1.0 - alpha2);
};

void print_result(const double lower, const double upper, const double alpha, const double sigma) {
	double lower2 = elorating(lower);
	double upper2 = elorating(upper);
	std::cout << "result" << std::endl;
	std::cout << sigma << "σ: R(" << 100*alpha << "%): " << lower2 << " (" << 100.0 * inv_elorating(lower2) << "%) ～ "
		<< upper2 << " (" << 100.0 * inv_elorating(upper2) << "%) "
		<< "Range: " << upper2 - lower2 << std::endl;
};

void infar_rating(const u64 win, const u64 lose, const u64 draw, double &lower, double &upper,
	const bool draw_half, const double p, bool print_flag) {
	double alpha = 1.0 - ((1.0 - p) / 2.0);
	double sigma = integral_dnorm(alpha);
	double win2, lose2, draw2, match;
	if (draw_half) { // 引き分けを0.5勝として計算する場合 (推奨)
		draw2 = static_cast<double>(draw);
		win2 = static_cast<double>(win + draw2);
		lose2 = static_cast<double>(lose + draw2);
		match = static_cast<double>(win + lose + draw2 * 2);
	}
	else {
		win2 = static_cast<double>(win);
		lose2 = static_cast<double>(lose);
		match = static_cast<double>(win + lose);
	}
	clopper_pearson(win2, match, p, lower, upper);
	if (print_flag) {
		print_result(lower, upper, p, sigma);
	}
};

// 対局シミュレーションのmain関数です
// battle_num: 対局数
// diff_rating: レーティング差
// black_winrate: 互角のときの先手勝率
void sim_battle_main(u64 battle_num, double diff_rating, double black_winrate,
	double &win, double &lose, double &result_winrate) {
	double correction_rating = elorating(black_winrate); // 先手のときのレート補正
	double calc_winrate = inv_elorating(diff_rating);
	double calc_winrate_black = inv_elorating(diff_rating + correction_rating);
	double calc_winrate_white = inv_elorating(diff_rating - correction_rating);
	u32 win_rand_black = static_cast<u32>(UINT_MAX * calc_winrate_black);
	u32 win_rand_white = static_cast<u32>(UINT_MAX * calc_winrate_white);

	win = 0, lose = 0;
	u64 battle_num_set = battle_num / 2;
	for (auto i = 0; i < battle_num_set; ++i) {
		if (win_rand_black >= myrand.rand()) {
			++win;
		}
		else {
			++lose;
		}
		if (win_rand_white >= myrand.rand()) {
			++win;
		}
		else {
			++lose;
		}
	}
	result_winrate = win / static_cast<double>(win + lose);
};

// 対局シミュレーションを行います
void sim_battle(const u64 battle_num, const double diff_rating, const double black_winrate) {
	double correction_rating = elorating(black_winrate); // 先手のときのレート補正
	double calc_winrate = inv_elorating(diff_rating);
	double calc_winrate_black = inv_elorating(diff_rating + correction_rating);
	double calc_winrate_white = inv_elorating(diff_rating - correction_rating);
	u32 win_rand_black = static_cast<u32>(UINT_MAX * calc_winrate_black);
	u32 win_rand_white = static_cast<u32>(UINT_MAX * calc_winrate_white);
	std::cout << "対局シミュレーション" << std::endl;
	std::cout << "対局数: " << battle_num << std::endl;
	std::cout << "レーティング差: " << diff_rating << std::endl;
	std::cout << "互角のときの先手勝率: " << 100.0 * black_winrate << "%" << std::endl;
	std::cout << "理論勝率 (先手勝率 50%時): " << 100.0 * calc_winrate << "%" << std::endl;
	std::cout << "理論勝率 (指定先手勝率時): " << 50.0 * (calc_winrate_black + calc_winrate_white) << "%" << std::endl;
	double win, lose, result_winrate;
	sim_battle_main(battle_num, diff_rating, black_winrate, win, lose, result_winrate);
	std::cout << std::endl;
	std::cout << "---------結果-----------" << std::endl;
	std::cout << "　勝数: " << win << ", 敗数: " << lose << std::endl;
	std::cout << "　勝率: " << 100.0 * result_winrate << "%" << std::endl;
	std::cout << "------------------------" << std::endl;
};

void sim_battle_gacha(const u64 gacha_number, const u64 battle_num,
	const double diff_rating, const double black_winrate) {
	std::cout << "対局シミュレーションガチャ" << std::endl;
	std::cout << "ガチャ回数: " << gacha_number <<  "回" << std::endl;
	std::vector<double> win(gacha_number, 0);
	std::vector<double> lose_array(gacha_number, 0);
	std::vector<double> result_winrate_array(gacha_number, 0);
	for (auto i = 0; i < gacha_number; ++i) {
		sim_battle_main(battle_num, diff_rating, black_winrate, win[i], lose_array[i], result_winrate_array[i]);
	}
	double correction_rating = elorating(black_winrate);
	double calc_winrate = inv_elorating(diff_rating);
	double calc_winrate_black = inv_elorating(diff_rating + correction_rating);
	double calc_winrate_white = inv_elorating(diff_rating - correction_rating);

	std::sort(win.begin(), win.end());
	std::cout << std::endl;
	std::cout << "---------結果-----------" << std::endl;
	std::cout << "　　理論勝率: ";
	std::cout << 50.0 * (calc_winrate_black + calc_winrate_white) << "%" << std::endl;
	std::cout << "　最小の勝率: ";
	std::cout << 100.0 * win[0] / battle_num << "%" << std::endl;
	std::cout << "　　　　5%点: ";
	std::cout << 100.0 * win[gacha_number / 20] / battle_num << "%" << std::endl;
	std::cout << "　　　中央値: ";
	std::cout << 100.0 * win[gacha_number / 2] / battle_num << "%" << std::endl;
	std::cout << "　　　 95%点: ";
	std::cout << 100.0 * win[gacha_number / 20 * 19] / battle_num << "%" << std::endl;
	std::cout << "　最大の勝率: ";
	std::cout << 100.0 * win[gacha_number - 1] / battle_num << "%" << std::endl;
	std::cout << "------------------------" << std::endl;
};

// --------------
// テスト用の関数
// --------------

void test_stat() {
	// elorating
	std::cout << "elorating" << std::endl;
	std::cout << "0: " << elorating(0) << std::endl;
	std::cout << "0.1: " << elorating(0.1) << std::endl;
	std::cout << "0.4: " << elorating(0.4) << std::endl;
	std::cout << "0.5: " << elorating(0.5) << std::endl;
	std::cout << "0.6: " << elorating(0.6) << std::endl;
	std::cout << "1: " << elorating(1) << std::endl;
	// mybeta
	double a = 0.5, b = 100, p = 0.95;
	BetaDistribution mybeta(a, b);
	std::cout << mybeta.quantile(p) << std::endl;

	double lower, upper;
	std::cout << "infar" << std::endl;
	infar_rating(120, 80, 0, lower, upper, true, 0.95449973610364158, true);

	std::cout << "integral_dnorm" << std::endl;
	std::cout << "0.90: " << integral_dnorm(0.90) << std::endl;
	std::cout << "0.95: " << integral_dnorm(0.95) << std::endl;
	std::cout << "0.9545: " << integral_dnorm(0.9545) << std::endl;
	std::cout << "0.99: " << integral_dnorm(0.99) << std::endl;

	std::cout << "inv_elorating" << std::endl;
	std::cout << "100: " << inv_elorating(100) << std::endl;
	std::cout << "-800: " << inv_elorating(-800) << std::endl;
};
