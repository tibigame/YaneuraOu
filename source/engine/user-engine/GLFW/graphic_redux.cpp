#include "graphic_primitive.h"
#include "graphic_object.h"
#include "graphic_redux.h"
#include "mtx.h"
#include "../../../shogi.h"

#ifdef GLFW3

State::State() {
}

State::~State() {
}

// コピーコンストラクタ
State::State(const State &a) {
	is_render_pos = a.is_render_pos;
	info = a.info;
	buttons = a.buttons;
	pos_p = a.pos_p;
}
// 代入演算子
State &State::operator=(const State &a)
{
	State t;
	t.info = a.info;
	t.is_render_pos = a.is_render_pos;
	t.buttons = a.buttons;
	t.pos_p = a.pos_p;
	return t;
}

void State::add_button(const ButtonInitializer &button_initializer) {
	buttons->push_back(Button(button_initializer)); // 末尾に追加していく
}

StateRender::StateRender(const State *s, const GLuint textureID_shogiboard_) {
	state = s;
	textureID_shogiboard = textureID_shogiboard_;
}

// コンストラクタとデストラクタ
Store::Store() {
	gl_string = new GlString;
	state.buttons = new std::vector<Button>;
	state.pos_p = static_cast<Position*>(_aligned_malloc(sizeof(Position), 32)); // ここのポインタはmoveで変わっていく
}

Store::~Store() {
	delete gl_string;
	delete state.buttons;
	_aligned_free(state.pos_p);
}

void Store::init() {
	gl_string->font_init(); // フォントの読み込みと駒文字などのフォントキャッシュの生成
	draw_init(textureID_shogiboard);
	// ボタン類の初期化
	add_button(ButtonInitializer(-8.5f, 0.6f, -5.5f, 1.2f, FunctionType::IS_READY,
		GL_COLOR_ZERO, GL_COLOR_BUTTON, GL_COLOR_ZERO, u8"IS READY", true, true));
	add_button(ButtonInitializer(-3.5f, 0.6f, -1.5f, 1.2f, FunctionType::USER,
		GL_COLOR_ZERO, GL_COLOR_BUTTON, GL_COLOR_ZERO, u8"USER", true, true));
	add_button(ButtonInitializer(-0.5f, 0.6f, 1.5f, 1.2f, FunctionType::EXIT,
		GL_COLOR_ZERO, GL_COLOR_BUTTON, GL_COLOR_ZERO, u8"EXIT", true, true));
}

void Store::add_button(const ButtonInitializer &button_initializer) {
	state.add_button(button_initializer);
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
	Action ac = action_callback(posx, posy, str, *state.buttons);
	if (ac.ft == FunctionType::NONE) {
		return;
	}
	add_action_que(ac); // キューにactionを追加する
	return;

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
}
#include "graphic_main.h"
// Actionを発行する
const Action action_callback(const double posx, const double posy, const std::string str, const std::vector<Button> &buttons) {
	Action ac(FunctionType::NONE, "");
	for (auto i = 0; i < buttons.size(); ++i) { // 全てのボタンに対してアクションの発行を試みる
		ac = (buttons[i].get_action(posx, posy));
		if (ac.ft == FunctionType::NONE) { continue; } // posが範囲外かボタンが無効だったということ
		ac.index = i;
		return ac; // 最初に見つかったボタンのアクションを発行する
	}
	return ac; // Action発行対象となるボタンが存在しなかった
}

const Action action_add_button(const ButtonInitializer &button_initializer) {
	Action ac;
	ac.ft = FunctionType::ADD_BUTTON;

	ButtonInitializer *p = static_cast<ButtonInitializer*>(_aligned_malloc(sizeof(ButtonInitializer), 32)); // ここで確保したメモリはreducerで所有権が移される
	*p = button_initializer;
	ac.p = static_cast<void*>(p);
	return ac;
}

const Action action_update_pos(const Position &new_pos_) {
	Action ac;
	ac.ft = FunctionType::UPDATE_POS;

	Position *p = static_cast<Position*>(_aligned_malloc(sizeof(Position), 32)); // ここで確保したメモリはreducerで所有権が移される
	copy(new_pos_, *p);
	ac.p = static_cast<void*>(p);
	return ac;
}

const Action action_update_info(const std::string new_info) {
	Action ac(FunctionType::UPDATE_INFO, new_info);
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
			std::string cmd = "isready";
			cmds.push(cmd);
			break;
		}
		case FunctionType::EXIT: { // 現状うまく終了できない
			std::string cmd = "quit";
			cmds.push(cmd);
			//exit(0);
			break;
		}
		case FunctionType::USER: {
			std::string cmd = "user";
			cmds.push(cmd);
			break;
		}
		case FunctionType::TEST: {
			break;
		}
		case FunctionType::ADD_BUTTON: {
			nextState.add_button(*static_cast<ButtonInitializer*>(action.p));
			_aligned_free(static_cast<ButtonInitializer*>(action.p)); // 渡されたメモリをここで解放する
			break;
		}
		case FunctionType::UPDATE_INFO: {
			nextState.info = action.str;
			break;
		}
		case FunctionType::UPDATE_POS: {
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
	state.buttons = nextState.buttons;
	state.pos_p = nextState.pos_p;
}

// 現在のstateから描写に必要な情報を取り出す
StateRender Store::provider() const {
	// 最新のstateを取り出し加工してrender()に渡す
	return StateRender(&state, textureID_shogiboard);
}

// providerから受け取ったStateを用いて描写を行う
void render(const StateRender &state_render) {
	draw_shogiboard(const_cast<GLuint&>(state_render.textureID_shogiboard));
	draw_shogiboard_rank_file_number(gl_string);
	draw_info(state_render.state->info, gl_string);
	if (state_render.state->is_render_pos) {
		draw_board(*state_render.state->pos_p, gl_string);
		draw_hand(*state_render.state->pos_p, gl_string);
		draw_teban(*state_render.state->pos_p, gl_string);
	}
	for (auto button : *state_render.state->buttons) {
		button.draw();
	}
}

void render_loop_init() {
	draw_loop_init();
}

#endif
