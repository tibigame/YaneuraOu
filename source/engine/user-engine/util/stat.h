#ifndef _STAT_UTIL_H_
#define _STAT_UTIL_H_

using u64 = unsigned __int64;
using u32 = unsigned __int32;

// 統計関連のユーティリティ

double elorating(double winrate); // 勝率からレーティング差を計算する
double inv_elorating(double diff_rating); // レーティング差から勝率を求める
double integral_dnorm(double qn); // 正規分布の確率密度関数の累積がp以上となる値を計算する


// clopper_pearson法による二項分布近似
void clopper_pearson(const double k, const double n, const double alpha, double &lower, double &upper);

// レーティングの区間推定を行う
// win: 勝数, lose: 敗数, draw: 引分数,
// draw_half: 引き分けを1/2勝とするか (レーティング検証の結果からはTrueを推奨する)
// p: 有意水準 (0.95などを指定する)
// lower, upper: レーティング推定の下限値、上限値の結果を格納する場所
// print_flag: 結果を標準出力にprintするかのフラグ
void infar_rating(const u64 win, const u64 lose, const u64 draw,
	double &lower, double &upperconst, bool draw_half=true, const double p=0.95, bool print_flag=false);

// 対局シミュレーションを行います
// battle_num: 対局数
// diff_rating: レーティング差
// black_winrate: 互角のときの先手勝率
void sim_battle(const u64 battle_num=100, const double diff_rating=0.0, const double black_winrate=0.53);

// 対局シミュレーションガチャを行います
// n人が対局シミュレーションを行ったときに最も運の良い人、悪い人をピックアップします
// n人が同じ強さの評価関数を作ったときに誰かがこれは強い評価関数だと検定を通してしまう確率の評価に使います
// gacha_number: 対局シミュレーションを行う回数
// battle_num: 対局数
// diff_rating: レーティング差
// black_winrate: 互角のときの先手勝率
void sim_battle_gacha(const u64 gacha_number=100, const u64 battle_num=100,
	const double diff_rating=0.0, const double black_winrate=0.5);

// --------------
// テスト用の関数
// --------------
void test_stat();

#endif _STAT_UTIL_H_
