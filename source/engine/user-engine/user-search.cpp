#include "../../extra/all.h"
#include "random_board.h"
#include "my_rand.h"
#include "./test/test.h"
#include "./util/stat.h"

#include <atltime.h>

// USI拡張コマンド"user"が送られてくるとこの関数が呼び出される。実験に使ってください。
void user_test(Position& pos_, istringstream& is)
{
	uint64_t loop_max = 100000000; // 1000万回
	is >> loop_max;
	cout << "Random Player test , loop_max = " << loop_max << endl;
	// sq_test();
	CFileTime cTimeStart, cTimeEnd;
	CFileTimeSpan cTimeSpan;
	cTimeStart = CFileTime::GetCurrentTime();           // 現在時刻
	const int loop_num = 100;
	for (auto i = 0; i < loop_num; ++i) {
		//end_game_mate(pos_);
	}
	cTimeEnd = CFileTime::GetCurrentTime();           // 現在時刻
	cTimeSpan = cTimeEnd - cTimeStart;
	std::cout << "処理時間:" << cTimeSpan.GetTimeSpan() / 10000 << "[ms]" << std::endl;
	if (cTimeSpan.GetTimeSpan() * 0.0001 > 0.000000001) {
		std::cout << "処理速度:" << loop_num / (cTimeSpan.GetTimeSpan() * 0.0001) << "[sfen/ms]" << std::endl;
	}
	test_main();
	test_stat();
	double a, b;
	infar_rating(2610, 1500, 251, a, b, true, 0.99, true);
	sim_battle(100000, 300, 0.6);
	sim_battle_gacha(1000000, 1000, 100, 0.6);
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
