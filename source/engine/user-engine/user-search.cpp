#include "../../extra/all.h"
#include "random_board.h"
#include "my_rand.h"
#include "./test/test.h"
#include "./GLFW/graphic_main.h"
#include "./GLFW/graphic_redux.h"

#include <stdlib.h>
#include <atltime.h>


// USI拡張コマンド"user"が送られてくるとこの関数が呼び出される。実験に使ってください。
void user_test(Position& pos_, istringstream& is)
{
	int loop_num;
	bool test_cout = false;
	bool time_cout = true;
	bool time_output = false;
	std::string cmd;
	is >> cmd;
	if (cmd == "bench") {
		loop_num = 10000000;
		time_output = true;
		cout << "ベンチマークの実行: " << loop_num << "局面を生成します。" << endl;
	} else if (cmd == "gen") {
		loop_num = 1;
		time_cout = false;
	} else {
		test_cout = true;
		loop_num = 10;
		cout << "Random sfen test , loop_num = " << loop_num << endl;
	}

	CFileTime cTimeStart, cTimeEnd;
	CFileTimeSpan cTimeSpan;
	cTimeStart = CFileTime::GetCurrentTime(); // 現在時刻
	for (auto i = 0; i < loop_num; ++i) {
		end_game_mate(pos_);
	}
	cTimeEnd = CFileTime::GetCurrentTime(); // 現在時刻
	cTimeSpan = cTimeEnd - cTimeStart;

	if (time_cout) {
		std::cout << "処理時間:" << cTimeSpan.GetTimeSpan() / 10000 << "[ms]" << std::endl;
		if (cTimeSpan.GetTimeSpan() * 0.0001 > 0.000000001) {
			std::cout << "処理速度:" << loop_num / (cTimeSpan.GetTimeSpan() * 0.0001) << "[sfen/ms]" << std::endl;
		}
	}
	if (test_cout) {
		myrand.cout();
		view();
	}
	if (time_output) {
		char a[100];
		std::string output;
		output = u8"局面数: ";
		_itoa_s(loop_num, a, 10);
		output += a;
		output += u8"\n処理時間: ";
		_itoa_s(int(cTimeSpan.GetTimeSpan() / 10000), a, 10);
		output += a;
		output += u8"[ms]";
		if (cTimeSpan.GetTimeSpan() * 0.0001 > 0.000000001) {
			output += u8"\n処理速度: ";
			_itoa_s(int(loop_num / (cTimeSpan.GetTimeSpan() * 0.0001)), a, 10);
			output += a;
			output += u8"[sfen/ms]";
		}
		gui.store.add_action_que(action_update_info(output));
	}
	else {
		gui.store.add_action_que(action_update_info(pos_.sfen_fast(true)));
	}
	gui.store.add_action_que(action_update_pos(pos_));
}

#ifdef USER_ENGINE

// USIに追加オプションを設定したいときは、この関数を定義すること。
// USI::init()のなかからコールバックされる。
void USI::extra_option(USI::OptionsMap & o)
{
}

// 起動時に呼び出される。時間のかからない探索関係の初期化処理はここに書くこと。
void Search::init()
{
}

// isreadyコマンドの応答中に呼び出される。時間のかかる処理はここに書くこと。
void  Search::clear()
{
}

// 探索開始時に呼び出される。
// この関数内で初期化を終わらせ、slaveスレッドを起動してThread::search()を呼び出す。
// そのあとslaveスレッドを終了させ、ベストな指し手を返すこと。
void MainThread::think()
{
  // 例)
  //  for (auto th : Threads.slaves) th->start_searching();
  //  Thread::search();
  //  for (auto th : Threads.slaves) th->wait_for_search_finished();
}

// 探索本体。並列化している場合、ここがslaveのエントリーポイント。
// MainThread::search()はvirtualになっていてthink()が呼び出されるので、MainThread::think()から
// この関数を呼び出したいときは、Thread::search()とすること。
void Thread::search()
{
}

#endif // USER_ENGINE
