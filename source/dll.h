#ifndef _DLL_H_
#define _DLL_H_
#ifdef DLL

extern "C" {
	__declspec(dllexport) void init(); // --- 全体的な初期化
	__declspec(dllexport) void start_logger(); // ログファイルの書き出しのon
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
		int depth=0, int nodes =0, int mate=0); // 探索してbestmoveを返す

	// ランダム局面生成
	__declspec(dllexport) size_t random_sfen(char* c_sfen, size_t bufsize);

	// debug
	__declspec(dllexport) void user(const char* c_user, const size_t length);
}

#endif
#endif
