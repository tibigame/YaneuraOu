#include "graphic_primitive.h"
#include "graphic_object.h"
#include "graphic_string.h"
#include "graphic_redux.h"


#ifdef GLFW3

std::mutex cmd_mtx; // cmdとの通信のためのミューテックス
std::mutex store_mtx; // Storeのキューにアクセスするためのミューテックス

Action::Action() {}

Action::Action(const FunctionType ft_, const std::string &str_) {
	ft = ft_;
	str = str_;
	index = 0;
	p = nullptr;
}

// 初期化を行う
void Button::init(double left_, double bottom_, double right_, double top_, FunctionType ft_) {
	left = left_;
	bottom = bottom_;
	right = right_;
	top = top_;
	ft = ft_;
}

// 渡されたposが自身の範囲内か
const bool Button::is_range(double posx, double posy) const {
	if (left < posx && posx < right && bottom < posy && posy < top) {
		return true;
	}
	return false;
}


Button::Button() {
}

Button::~Button() {
}

// アクションを発行する
const Action Button::get_action(const std::string &str_) const {
	return Action(ft, str_);
}

State::State() {
}

// TODO: この辺がうまく動いてない
// コピーコンストラクタ
State::State(const State &a) {
	is_render_pos = a.is_render_pos;
	info = a.info;
	std::copy(a.buttons.begin(), a.buttons.end(), back_inserter(buttons));
	copy(a.pos_, pos_);
}
// 代入演算子
State &State::operator=(const State &a)
{
	State t;
	t.info = a.info;
	t.is_render_pos = a.is_render_pos;
	std::copy(a.buttons.begin(), a.buttons.end(), back_inserter(t.buttons));
	copy(a.pos_, t.pos_);
	return t;
}

StateRaender::StateRaender(const State *s, const GLuint textureID_shogiboard_, GlString* gl_string_) {
	state = s;
	textureID_shogiboard = textureID_shogiboard_;
	gl_string = gl_string_;
}

// コンストラクタとデストラクタ
Store::Store() {
}

Store::~Store() {
}

void Store::init(GlString* gl_string_) {
	gl_string = gl_string_;
	draw_init(textureID_shogiboard);
}

// キューにactionを追加する
void Store::add_action_que(Action ac) {
	// storeのミューテックスを取得する
	std::lock_guard<std::mutex> lock(store_mtx);
	action_que.push(ac);
}

// actionを発行する
void Store::callback(const double posx, const double posy, const std::string &str) {
	std::vector<Button> temp;
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
	for (auto i = 0; i < buttons.size(); ++i) {
		if (
			buttons[i].is_range(posx, posy) && // ボタンがposxとposyの座標を含んでいるか
			buttons[i].is_visible && // ボタンが可視状態か
			buttons[i].is_enable // // ボタンが有効化状態か
			) {
			// Actionを発行する
			ac = (buttons[i].get_action(str));
			ac.index = i;
			return ac;
		}
	}
	return ac; // Action発行対象となるボタンが存在しなかった
}

const Action action_update_pos(const Position &new_pos_) {
	Action ac;
	ac.ft = FunctionType::POS_UPDATE;

	Position *p = static_cast<Position*>(_aligned_malloc(sizeof(Position), 32)); // ここで確保したメモリはreducerで解放すること
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
	State nextState;
	nextState.info = state.info;
	std::copy(state.buttons.begin(), state.buttons.end(), back_inserter(nextState.buttons));

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
			Position *source = static_cast<Position*>(action.p);
			copy(*source, nextState.pos_);
			nextState.is_render_pos = true;
			// ここで落ちる
			// free(static_cast<Position*>(action.p)); // Actionで確保したメモリを解放する
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
	copy(nextState.pos_, state.pos_);
}

// 現在のstateから描写に必要な情報を取り出す
StateRaender Store::provider() const {
	// 最新のstateを取り出し加工してrender()に渡す
	return StateRaender(&state, textureID_shogiboard, gl_string);
}

// providerを呼び出して描写を行う
void render(const StateRaender &state_render) {
	draw_shogiboard(const_cast<GLuint&>(state_render.textureID_shogiboard));
	draw_shogiboard_rank_file_number(state_render.gl_string);
	draw_info(state_render.state->info, state_render.gl_string);
	if (state_render.state->is_render_pos) {
		draw_board(state_render.state->pos_, state_render.gl_string);
		draw_hand(state_render.state->pos_, state_render.gl_string);
		draw_teban(state_render.state->pos_, state_render.gl_string);
	}
}

#endif
