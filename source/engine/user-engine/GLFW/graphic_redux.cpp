#include "graphic_primitive.h"
#include "graphic_object.h"
#include "graphic_redux.h"

#ifdef GLFW3

std::mutex cmd_mtx; // cmdとの通信のためのミューテックス
std::mutex store_mtx; // Storeのキューにアクセスするためのミューテックス

State::State() {
}

State::~State() {
}

// コピーコンストラクタ
State::State(const State &a) {
	is_render_pos = a.is_render_pos;
	info = a.info;
	std::copy(a.buttons.begin(), a.buttons.end(), back_inserter(buttons));
	pos_p = a.pos_p;
}
// 代入演算子
State &State::operator=(const State &a)
{
	State t;
	t.info = a.info;
	t.is_render_pos = a.is_render_pos;
	std::copy(a.buttons.begin(), a.buttons.end(), back_inserter(t.buttons));
	t.pos_p = a.pos_p;
	return t;
}

StateRender::StateRender(const State *s, const GLuint textureID_shogiboard_, GlString* gl_string_) {
	state = s;
	textureID_shogiboard = textureID_shogiboard_;
	gl_string = gl_string_;
}

// コンストラクタとデストラクタ
Store::Store() {
	gl_string = new GlString;
	state.pos_p = static_cast<Position*>(_aligned_malloc(sizeof(Position), 32)); // ここのポインタはmoveで変わっていく
}

Store::~Store() {
	delete gl_string;
	_aligned_free(static_cast<Position*>(state.pos_p));
}

void Store::init() {
	gl_string->font_init(); // フォントの読み込みと駒文字などのフォントキャッシュの生成
	draw_init(textureID_shogiboard);
}

// キューにactionを追加する
void Store::add_action_que(Action ac) {
	// storeのミューテックスを取得する
	std::lock_guard<std::mutex> lock(store_mtx);
	action_que.push(ac);
}

#include <stdlib.h>

// actionを発行する
void Store::callback(const double posx, const double posy, const std::string &str) {
	std::vector<Button> temp;

	char a[100];
	_itoa_s((int)posx, a, 10);
	std::string test = u8"posx = ";
	test += a;
	test += u8", poy = ";
	_itoa_s((int)posy, a, 10);
	test += a;
	test += u8"\n日本語テスト！aあ☆＠";
	add_action_que(action_update_info(test));
	return;

	std::copy(state.buttons.begin(), state.buttons.end(), back_inserter(temp));

	Action ac = action_callback(posx, posy, str, temp);
	if (ac.ft == FunctionType::NONE) {
		return;
	}
	add_action_que(ac); // キューにactionを追加する
}

// Actionを発行する
const Action action_callback(const double posx, const double posy, const std::string str, const std::vector<Button> buttons) {
	Action ac(FunctionType::NONE, "");
	for (auto i = 0; i < buttons.size(); ++i) { // 全てのボタンに対してアクションの発行を試みる
		ac = (buttons[i].get_action(posx, posy, str));
		if (ac.ft == FunctionType::NONE) { continue; } // posが範囲外がボタンが無効だったということ
		ac.index = i;
		return ac; // 最初に見つかったボタンのアクションを発行する
	}
	return ac; // Action発行対象となるボタンが存在しなかった
}

const Action action_update_pos(const Position &new_pos_) {
	Action ac;
	ac.ft = FunctionType::POS_UPDATE;

	Position *p = static_cast<Position*>(_aligned_malloc(sizeof(Position), 32)); // ここで確保したメモリはreducerで所有権が移される
	copy(new_pos_, *p);
	ac.p = static_cast<void*>(p);
	return ac;
}

const Action action_update_info(const std::string new_info) {
	Action ac(FunctionType::INFO_UPDATE, new_info);
	return ac;
}

// Actionを受けて新しいstateを作成する (stateは直接変更しない)
const State reducer(const Action &action, const State &state) {
	// stateをnextStateにコピーする
	// std::vector<Button>
	State nextState = state;

	// cmdのミューテックスを獲得する
	std::lock_guard<std::mutex> lock(cmd_mtx);

	// メインの処理を行う
	switch (action.ft) { // ここで個々のdispatcherを呼ぶ
		case FunctionType::IS_READY: {
			break;
		}
		case FunctionType::EXIT: {
			break;
		}
		case FunctionType::USER: {
			break;
		}
		case FunctionType::TEST: {
			nextState.info = action.str;
			break;
		}
		case FunctionType::INFO_UPDATE: {
			nextState.info = action.str;
			break;
		}
		case FunctionType::POS_UPDATE: {
			nextState.pos_p = static_cast<Position*>(action.p); // ポインタの付け替えでmoveする
			nextState.is_render_pos = true;
			_aligned_free(static_cast<Position*>(state.pos_p)); // 以前確保したメモリを解放する
		}
		default: {
			break;
		}
	}
	return nextState; // 新しいstateを返す (guiのミューテックスも解放される)
}

// Actionを実行する
void Store::exe_action_que() {
	if (action_que.empty()) { // キューが空なら何もしない
		return;
	}
	// キューが空でなければ1つActionを実行する
	store_mtx.lock(); // storeのミューテックスを取得する
	Action ac = action_que.front();
	action_que.pop();
	store_mtx.unlock(); // ミューテックスを解放する
	State nextState = reducer(ac, state); // reducerにdispatchする
	update_store(nextState); // stateを更新する
}

// stateを変更できる唯一のメソッドとすること
void Store::update_store(const State &nextState) {
	state.info = nextState.info;
	state.is_render_pos = nextState.is_render_pos;
	std::copy(nextState.buttons.begin(), nextState.buttons.end(), back_inserter(state.buttons));
	state.pos_p = nextState.pos_p;
}

// 現在のstateから描写に必要な情報を取り出す
StateRender Store::provider() const {
	// 最新のstateを取り出し加工してrender()に渡す
	return StateRender(&state, textureID_shogiboard, gl_string);
}

// providerから受け取ったStateを用いて描写を行う
void render(const StateRender &state_render) {
	draw_shogiboard(const_cast<GLuint&>(state_render.textureID_shogiboard));
	draw_shogiboard_rank_file_number(state_render.gl_string);
	draw_info_ex(state_render.state->info, state_render.gl_string);
	if (state_render.state->is_render_pos) {
		draw_board(*state_render.state->pos_p, state_render.gl_string);
		draw_hand(*state_render.state->pos_p, state_render.gl_string);
		draw_teban(*state_render.state->pos_p, state_render.gl_string);
	}
}

void render_loop_init() {
	draw_loop_init();
}

#endif
