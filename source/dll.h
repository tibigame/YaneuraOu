#ifndef _DLL_H_
#define _DLL_H_
#ifdef DLL

extern "C" {
	__declspec(dllexport) void init(); // --- 全体的な初期化
	__declspec(dllexport) unsigned __int64 isready(); // 思考エンジンの準備が出来たかの確認
	// オプションを設定する
	__declspec(dllexport) void setoption(
		const char* name_c, const size_t name_length, const char* value_c, const size_t value_length
	);
	// オプションを取得する(USI独自拡張)
	__declspec(dllexport) size_t getoption(
		const char* name_c, const size_t name_length, char* value_c, size_t value_length
	);

	// 現在の局面について評価関数を呼び出して、その値を返す。
	__declspec(dllexport) unsigned __int32 eval();

	// この局面のhash keyの値を出力する。
	__declspec(dllexport) unsigned __int64 key();


	// position
	__declspec(dllexport) void position(const char* c_position, const size_t length);
	__declspec(dllexport) size_t get_position(char* c_position, const size_t length);
	__declspec(dllexport) void matsuri(); // 指し手生成祭りの局面をセットする。


	// move
	__declspec(dllexport) size_t movelist(char* c_movelist, const size_t length);

	// mate
	__declspec(dllexport) int mated(); // この局面が詰んでいるかの判定

	// go
	__declspec(dllexport) size_t go(char* c_bestmove, const size_t length, int time=1000, int entering=24, int max_game_ply=0,
		int depth=0, int nodes=0, int mate=0); // 探索してbestmoveを返す

	// ランダム局面生成
	__declspec(dllexport) size_t random_sfen(char* c_sfen, size_t bufsize);

	// debug
	__declspec(dllexport) void start_logger(); // ログファイルの書き出しのon
	__declspec(dllexport) void user(const char* c_user, const size_t length);


	// Shogi statistics (将棋のための統計関数)

	// 統計関数
	__declspec(dllexport) double integral_dnorm(double qn); // 正規分布の確率密度関数の累積がp以上となる値を計算する
	__declspec(dllexport) double elorating(double winrate); // 勝率からレーティング差を計算する
	__declspec(dllexport) double inv_elorating(double diff_rating); // レーティング差から勝率を求める
	__declspec(dllexport) double eval_winrate(int16_t eval); // 評価値から勝率を求める
	__declspec(dllexport) double inv_eval_winrate(double winrate); // 勝率から評価値を求める

	// 乱数 (myrandの生成する乱数を利用します)
	__declspec(dllexport) unsigned __int32 rand_u32(); // u32乱数を返します
	__declspec(dllexport) unsigned __int64 rand_u64(); // u64乱数を返します
	__declspec(dllexport) unsigned __int32 rand_r(unsigned __int32 r); // r未満の乱数を返します
	// 確率p, length回のコイン投げを試行し、表なら1、裏なら0の結果列をc_bitに格納します。返り値は1がセットされた数。
	__declspec(dllexport) int binomial(const double p, char* c_bit, const size_t length);

	// 区間推定
	// clopper_pearson法による二項分布近似
	__declspec(dllexport) void clopper_pearson(const double k, const double n, const double alpha, double &lower, double &upper);
	// レーティングの区間推定を行う
	// win: 勝数, lose: 敗数, draw: 引分数,
	// lower, upper: レーティング推定の下限値、上限値の結果を格納する場所
	// draw_half: 引き分けを1/2勝とするか (レーティング検証の結果からはTrueを推奨する)
	// p: 有意水準 (0.95などを指定する)
	__declspec(dllexport) void infar_rating(const u64 win, const u64 lose, const u64 draw,
		double &lower, double &upper, const bool draw_half=true, const double p=0.95);

}

#endif
#endif
