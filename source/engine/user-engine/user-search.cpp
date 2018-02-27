#include "../../extra/all.h"
#include "random_board.h"
#include "my_rand.h"
#include "./test/test.h"
#include "./GLFW/graphic_main.h"
#include "./GLFW/graphic_redux.h"
#include "./util/i_to_u8.h"
#include "./io.h"
#include "../mate-engine/mate-search-func.h" 
#include "./mate_vector.h"

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
		loop_num = 5000000;
		time_output = true;
		cout << "ベンチマークの実行: " << loop_num << "局面を生成します。" << endl;
	} else if (cmd == "gen") {
		loop_num = 1;
		time_cout = false;
	} else {
		test_cout = true;
		loop_num = 2000;
		cout << "Random sfen test , loop_num = " << loop_num << endl;
	}

	CFileTime cTimeStart, cTimeEnd;
	CFileTimeSpan cTimeSpan;
	cTimeStart = CFileTime::GetCurrentTime(); // 現在時刻
	io.file_open();
	MateVector mv;
	for (auto i = 0; i < loop_num; ++i) {
		std::string sfen = end_game_mate();
		char mate_char;

		pos_.set_fast(sfen, pos_.state(), Threads[0]);
		std::string mate = mate_search_func2(pos_);
		mate_char = mate[0];
		if (mate_char == 'e') { // 合法局面でなかった
			continue;
		}
		if (mate_char == 'm') { // 詰み
			mv.set_origin(pos_.sideToMove, MATE_VECTOR_MATE);
		}
		else {
			if (mate_char == 'n') { // 不詰み
				mv.set_origin(pos_.sideToMove, MATE_VECTOR_NMATE);
			}
			// 不詰みのときはβ世界線の探索も行う
			pos_.worldline = Worldline::Beta;
			// 一応初期化しておく (どうせ探索の方がはるかに遅いのでここがボトルネックにはならない)
			pos_.set_fast(sfen, pos_.state(), Threads[0]);
			std::string mate = mate_search_func2(pos_);
			if (mate[0] == 'm') { // 詰み
				mv.set_beta(pos_.sideToMove, MATE_VECTOR_MATE);
			}
			else if (mate[0] == 'n') { // 不詰み
				mv.set_beta(pos_.sideToMove, MATE_VECTOR_NMATE);
			}
			else { // 不明
				mv.set_beta(pos_.sideToMove, MATE_VECTOR_UNKOWN);
			}
			pos_.worldline = Worldline::Alpha;
		}
		const std::vector<Piece> PieceList = { ROOK, BISHOP, GOLD, SILVER, KNIGHT, LANCE, PAWN };
		const std::vector<int> PieceMaxList = { 2, 2, 4, 4, 4, 4, 18 };

		if (mate_char != 'n') { // 詰み or 不明
			for (auto i = 0; i < 7; ++i) { // 自分の駒を減らして詰むかどうかを考える
				if (!hand_exists(pos_.hand[pos_.sideToMove], PieceList[i])) { // 駒を持っていない
					mv.set_minus(pos_.sideToMove, pos_.sideToMove, PieceList[i], mv.get_origin(pos_.sideToMove)); // originの値をコピーしておく
					continue;
				}
				pos_.set_fast(sfen, pos_.state(), Threads[0]);
				add_hand(pos_.hand[pos_.sideToMove], PieceList[i], -1);
				std::string mate = mate_search_func2(pos_);
				if (mate[0] == 'm') { // 詰み
					mv.set_minus(pos_.sideToMove, pos_.sideToMove, PieceList[i], MATE_VECTOR_MATE);
				}
				else if (mate[0] == 'n') { // 不詰み
					mv.set_minus(pos_.sideToMove, pos_.sideToMove, PieceList[i], MATE_VECTOR_NMATE);
				}
				else { // 不明
					mv.set_minus(pos_.sideToMove, pos_.sideToMove, PieceList[i], MATE_VECTOR_UNKOWN);
				}
			}
			for (auto i = 0; i < 7; ++i) { // 相手の駒を増やして詰むかどうかを考える
				if (hand_exists(pos_.hand[~pos_.sideToMove], PieceList[i])) { // 駒を持っている
					mv.set_plus(pos_.sideToMove, ~pos_.sideToMove, PieceList[i], mv.get_origin(pos_.sideToMove)); // originの値をコピーしておく
					continue;
				}
				pos_.set_fast(sfen, pos_.state(), Threads[0]);
				add_hand(pos_.hand[~pos_.sideToMove], PieceList[i], 1);
				std::string mate = mate_search_func2(pos_);
				if (mate[0] == 'm') { // 詰み
					mv.set_plus(pos_.sideToMove, ~pos_.sideToMove, PieceList[i], MATE_VECTOR_MATE);
				}
				else if (mate[0] == 'n') { // 不詰み
					mv.set_plus(pos_.sideToMove, ~pos_.sideToMove, PieceList[i], MATE_VECTOR_NMATE);
				}
				else { // 不明
					mv.set_plus(pos_.sideToMove, ~pos_.sideToMove, PieceList[i], MATE_VECTOR_UNKOWN);
				}
			}
		}
		if (mate_char != 'm') { // 不詰み or 不明
			for (auto i = 0; i < 7; ++i) { // 自分の駒を増やして詰むかどうかを考える
				if (hand_count(pos_.hand[pos_.sideToMove], PieceList[i]) >= PieceMaxList[i]) { // 駒をMAXまで持っている
					mv.set_plus(pos_.sideToMove, pos_.sideToMove, PieceList[i], mv.get_origin(pos_.sideToMove)); // originの値をコピーしておく
					continue;
				}
				pos_.set_fast(sfen, pos_.state(), Threads[0]);
				add_hand(pos_.hand[pos_.sideToMove], PieceList[i], 1);
				std::string mate = mate_search_func2(pos_);
				if (mate[0] == 'm') { // 詰み
					mv.set_plus(pos_.sideToMove, pos_.sideToMove, PieceList[i], MATE_VECTOR_MATE);
				}
				else if (mate[0] == 'n') { // 不詰み
					mv.set_plus(pos_.sideToMove, pos_.sideToMove, PieceList[i], MATE_VECTOR_NMATE);
				}
				else { // 不明
					mv.set_plus(pos_.sideToMove, pos_.sideToMove, PieceList[i], MATE_VECTOR_UNKOWN);
				}
			}
			for (auto i = 0; i < 7; ++i) { // 相手の駒を減らして詰むかどうかを考える
				if (!hand_exists(pos_.hand[~pos_.sideToMove], PieceList[i])) { // 駒を持っていない
					mv.set_minus(pos_.sideToMove, ~pos_.sideToMove, PieceList[i], mv.get_origin(pos_.sideToMove)); // originの値をコピーしておく
					continue;
				}
				pos_.set_fast(sfen, pos_.state(), Threads[0]);
				add_hand(pos_.hand[~pos_.sideToMove], PieceList[i], -1);
				std::string mate = mate_search_func2(pos_);
				if (mate[0] == 'm') { // 詰み
					mv.set_minus(pos_.sideToMove, ~pos_.sideToMove, PieceList[i], MATE_VECTOR_MATE);
				}
				else if (mate[0] == 'n') { // 不詰み
					mv.set_minus(pos_.sideToMove, ~pos_.sideToMove, PieceList[i], MATE_VECTOR_NMATE);
				}
				else { // 不明
					mv.set_minus(pos_.sideToMove, ~pos_.sideToMove, PieceList[i], MATE_VECTOR_UNKOWN);
				}
			}
		}

		sfen += u8",";
		sfen += mate;
		// io.add_que(std::move(sfen));
		if (i % 50 == 0) {
			std::cout << i << std::endl;
		}
	}
	io.file_flash();
	io.file_close();
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
#ifdef GLFW3
	gui.store.add_action_que(action_update_pos(pos_));
	if (time_output) {
		std::string output;
		output = u8"局面数: " + i_to_u8(loop_num);
		output += u8"\n処理時間: " + i_to_u8(int(cTimeSpan.GetTimeSpan() / 10000)) + u8"[ms]";
		if (cTimeSpan.GetTimeSpan() * 0.0001 > 0.000000001) {
			output += u8"\n処理速度: " + i_to_u8(int(loop_num / (cTimeSpan.GetTimeSpan() * 0.0001))) + u8"[sfen/ms]";
		}
		gui.store.add_action_que(action_update_info(output));
	}
	else {
		gui.store.add_action_que(action_update_info(mv.out()));
	}
#endif
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
