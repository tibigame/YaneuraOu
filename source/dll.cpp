#ifdef DLL
#include "./random/random_board.h"

#include <sstream>
#include <iostream>

#include "shogi.h"
#include "position.h"
#include "search.h"
#include "thread.h"
#include "tt.h"

#include <cstring> // std::memcpy

#include "random/util/stat.h"
#include "dll.h"

using namespace std;

// 探索でbestmoveを保存する用のグローバル変数
string bestmove;

// char*へのコピーで用いるmemcpyのラッパー
void memcopy_wrap(char *dest, const char *src, size_t dest_size, size_t src_size) {
	size_t copy_size = src_size >= dest_size - 1 ? src_size : dest_size - 1; // 途中まででもコピーする
	memcpy(dest, src, copy_size);
	dest[copy_size] = '\0'; // 終端にnullを強制書き込み
}

// ユーザーの実験用に開放している関数。
// USI拡張コマンドで"user"と入力するとこの関数が呼び出される。
// "user"コマンドの後続に指定されている文字列はisのほうに渡される。
extern void user_test(Position& pos, std::istringstream& is);

// 探索開始局面(root)を格納するPositionクラス
Position pos;

// 局面を遡るためのStateInfoのlist。
StateListPtr states(new StateList(1));

// check sumを計算したとき、それを保存しておいてあとで次回以降、整合性のチェックを行なう。
u64 eval_sum_ = 0;

// 評価関数を読み込んだかのフラグ。これはevaldirの変更にともなってfalseにする。
bool load_eval_finished = false;


void init() {
	// --- 全体的な初期化
	USI::init(Options);
	Bitboards::init();
	Position::init();
	Search::init();
	Threads.set(Options["Threads"]);
	TT.resize(Options["Hash"]);
	Eval::init();
	position_sfen_init();

	int argc = 1;
	char *argv[1];
	USI::loop(argc, argv);
	// 生成して、待機させていたスレッドの停止
	Threads.set(0);
}

// ログファイルの書き出しのon
void start_logger() {
	start_logger(true);
}

// 思考エンジンの準備が出来たかの確認
unsigned __int64 isready() {
	// 評価関数の読み込みなど時間のかかるであろう処理はこのタイミングで行なう。
	// 起動時に時間のかかる処理をしてしまうと将棋所がタイムアウト判定をして、思考エンジンとしての認識をリタイアしてしまう。
	if (!load_eval_finished)
	{
		// 評価関数の読み込み
		Eval::load_eval();
		// チェックサムの計算と保存(その後のメモリ破損のチェックのため)
		eval_sum_ = Eval::calc_check_sum();
		load_eval_finished = true;
	}
	else
	{
		// メモリが破壊されていないかを調べるためにチェックサムを毎回調べる。
		if (eval_sum_ != Eval::calc_check_sum())
			return -1;
	}

	// isreadyに対してはreadyokを返すまで次のコマンドが来ないことは約束されているので
	// このタイミングで各種変数の初期化もしておく。

	TT.resize(Options["Hash"]);
	Search::clear();
	Time.availableNodes = 0;

	Threads.received_go_ponder = false;
	Threads.stop = false;

	// Positionコマンドが送られてくるまで評価値の全計算をしていないの気持ち悪いのでisreadyコマンドに対して
	// evalの値を返せるようにこのタイミングで平手局面で初期化してしまう。

	// 新しく渡す局面なので古いものは捨てて新しいものを作る。
	states = StateListPtr(new StateList(1));
	pos.set_hirate(&states->back(), Threads.main());

	return eval_sum_;
}

// オプションを設定する
void setoption(const char* name_c, const size_t name_length, const char* value_c, const size_t value_length) {
	string name = std::string(name_c);
	string value = std::string(value_c);
	if (Options.count(name))
		Options[name] = value;
	else {
		// USI_HashとUSI_Ponderは無視してやる。
		if (name != "USI_Hash" && name != "USI_Ponder")
			// この名前のoptionは存在しなかった
			sync_cout << "Error! : No such option: " << name << sync_endl;
	}
}

// オプションを取得する(USI独自拡張)
size_t getoption(const char* name_c, const size_t name_length, char* value_c, size_t value_length) {
	string name = std::string(name_c);
	string value;
	bool all = name == "";
	for (auto& o : Options)
	{
		// 大文字、小文字を無視して比較。また、nameが指定されていなければすべてのオプション設定の現在の値を表示。
		if ((!_stricmp(name.c_str(), o.first.c_str())) || all)
		{
			value += Options[o.first];
			if (!all)
				memcopy_wrap(value_c, value.c_str(), value_length, value.size());
				return value.size();
			value += "\n";
		}
	}
	if (all)
		memcopy_wrap(value_c, value.c_str(), value_length, value.size());
		return value.size();
	return 0;
}

// 現在の局面について評価関数を呼び出して、その値を返す。
unsigned __int32 eval() {
	return Eval::compute_eval(pos);
}

// この局面のhash keyの値を出力する。
unsigned __int64 key() {
	return pos.state()->key();
}

void position(const char* c_position, const size_t length) {
	istringstream is = std::istringstream(c_position);

	Move m;
	string token, sfen;

	is >> token;

	if (token == "startpos")
	{
		// 初期局面として初期局面のFEN形式の入力が与えられたとみなして処理する。
		sfen = SFEN_HIRATE;
		is >> token; // もしあるなら"moves"トークンを消費する。
	}
	// 局面がfen形式で指定されているなら、その局面を読み込む。
	// UCI(チェスプロトコル)ではなくUSI(将棋用プロトコル)だとここの文字列は"fen"ではなく"sfen"
	// この"sfen"という文字列は省略可能にしたいので..
	else {
		if (token != "sfen")
			sfen += token + " ";
		while (is >> token && token != "moves")
			sfen += token + " ";
	}

	// 新しく渡す局面なので古いものは捨てて新しいものを作る。
	states = StateListPtr(new StateList(1));
	pos.set(sfen, &states->back(), Threads.main());

	// 指し手のリストをパースする(あるなら)
	while (is >> token && (m = move_from_usi(pos, token)) != MOVE_NONE)
	{
		// 1手進めるごとにStateInfoが積まれていく。これは千日手の検出のために必要。
		states->emplace_back();
		if (m == MOVE_NULL) // do_move に MOVE_NULL を与えると死ぬので
			pos.do_null_move(states->back());
		else
			pos.do_move(m, states->back());
	}
}

size_t get_position(char* c_position, const size_t length) {
	std::string src = pos.sfen_fast();
	memcopy_wrap(c_position, src.c_str(), length, src.size());
	if (src.size() < length) {
		return src.size();
	}
	return 0;
}

// 指し手生成祭りの局面をセットする。
void matsuri() {
	pos.set("l6nl/5+P1gk/2np1S3/p1p4Pp/3P2Sp1/1PPb2P1P/P5GS1/R8/LN4bKL w GR5pnsg 1", &states->back(), Threads.main());
}

size_t movelist(char* c_movelist, const size_t length) {
	std::string str_movelist;
	for (auto m : MoveList<LEGAL_ALL>(pos)) {
		str_movelist += to_usi_string(m.move);
		str_movelist += ' ';
	}
	str_movelist.pop_back();

	memcopy_wrap(c_movelist, str_movelist.c_str(), length, str_movelist.size());
	if (str_movelist.size() < length) {
		return str_movelist.size();
	}
	return 0;
}

// この局面が詰んでいるかの判定
int mated() {
	return pos.is_mated() ? 1 : -1;
}

// Threads.start_thinkingを呼ぶ
size_t go(char* c_bestmove, const size_t length, int time, int entering, int max_game_ply, int depth, int nodes, int mate) {
	Search::LimitsType limits;
	string token;
	bool ponderMode = false;

	// 思考開始時刻の初期化。なるべく早い段階でこれをしておかないとサーバー時間との誤差が大きくなる。
	Time.reset();

	// 入玉ルール
	switch (entering)
	{
	case 27: {limits.enteringKingRule = EnteringKingRule::EKR_27_POINT; break; }
	case 0: {limits.enteringKingRule = EnteringKingRule::EKR_NONE; break; }
	case 1: {limits.enteringKingRule = EnteringKingRule::EKR_TRY_RULE; break; }
	default:
		limits.enteringKingRule = EnteringKingRule::EKR_24_POINT;
		break;
	}
	limits.enteringKingRule = EnteringKingRule::EKR_24_POINT;


	// 終局(引き分け)になるまでの手数
	limits.max_game_ply = max_game_ply;

	// エンジンオプションによる探索制限(0なら無制限)
	if (Options["DepthLimit"] >= 0) limits.depth = (int)Options["DepthLimit"];
	if (Options["NodesLimit"] >= 0) limits.nodes = (u64)Options["NodesLimit"];

	// 先手、後手の残り時間。[ms]
	limits.time[WHITE] = 0;
	limits.time[BLACK] = 0;

	// フィッシャールール時における時間
	limits.inc[WHITE] = 0;
	limits.inc[BLACK] = 0;

	limits.rtime = 0;

	// USIプロトコルでは、これが先手後手同じ値だと解釈する。
	limits.byoyomi[BLACK] = limits.byoyomi[WHITE] = time;

	// この探索深さで探索を打ち切る
	if (depth != 0) {
		limits.depth = depth;
	}

	// この探索ノード数で探索を打ち切る
	if (nodes != 0) {
		limits.nodes = nodes;
	}

	// mateが0でないなら詰将棋での詰み探索
	if (mate != 0) {
		limits.mate = mate;
	}

	Threads.start_thinking(pos, states, limits, ponderMode);

	memcopy_wrap(c_bestmove, bestmove.c_str(), length, bestmove.size());
	if (bestmove.size() < length) {
		return bestmove.size();
	}
	return 0;
}

// ランダム局面生成
size_t random_sfen(char* c_sfen, size_t bufsize) {
	std::string src = random_board();
	memcopy_wrap(c_sfen, src.c_str(), bufsize, src.size());
	if (src.size() < bufsize) {
		return src.size();
	}
	return 0;
}

// debug
void user(const char* c_user, const size_t length) {
	istringstream is = std::istringstream(c_user);
	user_test(pos, is);
}


// Shogi statistics (将棋のための統計関数)

// 統計関数

// 正規分布の確率密度関数の累積がp以上となる値を計算する
double integral_dnorm(double qn) {
	return ShogiStat::integral_dnorm(qn);
}

// 勝率からレーティング差を計算する
double elorating(double winrate) {
	return ShogiStat::elorating(winrate);
}

// レーティング差から勝率を求める
double inv_elorating(double diff_rating) {
	return ShogiStat::inv_elorating(diff_rating);
}

// 評価値から勝率を求める
double eval_winrate(int16_t eval) {
	return ShogiStat::eval_winrate(eval);
}

// 勝率から評価値を求める
double inv_eval_winrate(double winrate) {
	return ShogiStat::inv_eval_winrate(winrate);
}

// 乱数 (myrandの生成する乱数を利用します)

// u32乱数を返します
unsigned __int32 rand_u32() {
	return myrand.rand();
}

// u64乱数を返します
unsigned __int64 rand_u64() {
	unsigned __int64 temp = myrand.rand();
	temp <<= 32;
	temp += myrand.rand();
	return temp;
}

// r未満の乱数を返します
unsigned __int32 rand_r(unsigned __int32 r) {
	return myrand.rand_m(r);
}

// 確率p, length回のコイン投げを試行し、表なら1、裏なら0の結果列をc_bitに格納します。返り値は1がセットされた数。
int binomial(const double p, char* c_bit, const size_t length) {
	int result = 0;
	for (auto i = 0; i < length; ++i) {
		c_bit[i] = static_cast<int>(myrand.rand_b(p)); // boolからintへのキャストで0, 1になる
		if (c_bit[i] == 1) {
			++result;
		}
	}
	return result;
}

// 区間推定
// clopper_pearson法による二項分布近似
void clopper_pearson(const double k, const double n, const double alpha, double &lower, double &upper) {
	ShogiStat::clopper_pearson(k, n, alpha, lower, upper);
}

void print_result(const double lower, const double upper, const double alpha, const double sigma) {
	double lower2 = elorating(lower);
	double upper2 = elorating(upper);
	std::cout << "result" << std::endl;
	std::cout << sigma << "σ: R(" << 100 * alpha << "%): " << lower2 << " (" << 100.0 * inv_elorating(lower2) << "%) ～ "
		<< upper2 << " (" << 100.0 * inv_elorating(upper2) << "%) "
		<< "Range: " << upper2 - lower2 << std::endl;
};

// レーティングの区間推定を行う
// win: 勝数, lose: 敗数, draw: 引分数,
// lower, upper: レーティング推定の下限値、上限値の結果を格納する場所
// draw_half: 引き分けを1/2勝とするか (レーティング検証の結果からはTrueを推奨する)
// p: 有意水準 (0.95などを指定する)
void infar_rating(const u64 win, const u64 lose, const u64 draw,
	double &lower, double &upper, const bool draw_half, const double p) {
	
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
	
	lower = elorating(lower);
	upper = elorating(upper);
}

#endif
