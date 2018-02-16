#ifndef _GRAPHIC_REDUX_H_
#define _GRAPHIC_REDUX_H_
// Reduxの概念を実装します

#include "graphic_common.h"
#include "graphic_string.h"
#include "graphic_button.h"
#include "../../../position.h"
#include <thread>
#include <vector>
#include <queue>

#ifdef GLFW3

extern std::mutex cmd_mtx; // cmdとの通信のためのミューテックス
extern std::mutex store_mtx; // Storeのキューにアクセスするためのミューテックス

// 状態を表現する構造体
class State {
	public:
	std::vector<Button> buttons;
	Position *pos_p; // Position構造体を受け取る
	bool is_render_pos = false; // pos_を描写するかのフラグ
	std::string info = u8" "; // 汎用の情報出力用の文字列を格納します

	State();
	~State();
	State(const State &a); // コピーコンストラクタ
	State &operator=(const State &a);
};

// Renderに渡す状態を表現する構造体
// Stateの部分情報にReducerからの変更が不要な情報を合わせる
// GUIはシングルスレッドなのでコピーでなくポインタで渡してよい
struct StateRender {
	const State *state;
	GLuint textureID_shogiboard; // 将棋盤のテクスチャID
	GlString* gl_string; // 文字のテクスチャIDを管理するクラスへのポインタ
	StateRender(const State *s, const GLuint textureID_shogiboard_, GlString* gl_string_);
};

// Stateを管理するクラス
class Store {
private:
	std::queue<Action> action_que; // Actionを格納するキュー
	GLuint textureID_shogiboard;
	GlString* gl_string;
public:
	void update_store(const State &nextState); // stateを更新する
	State state;

	Store();
	~Store();

	void init();

	void callback(const double posx, const double posy, const std::string &str);
	void add_action_que(Action ac); // キューにactionを追加する
	void exe_action_que(); // キューのアクションを実行する

	StateRender provider() const; // 現在のstateから描写に必要な情報を取り出す
};

// Actionを発行する
const Action action_callback(const double posx, const double posy, const std::string str, const std::vector<Button> buttons);
const Action action_update_info(const std::string new_info);
const Action action_update_pos(const Position &new_pos_);

// Actionを受けて新しいStateを作成する (Stateは直接変更しない)
const State reducer(const Action &action, const State &state);

// providerから受け取ったStateを用いて描写を行う
void render(const StateRender &state_render);
void render_loop_init();

#endif

#endif _GRAPHIC_REDUX_H_
