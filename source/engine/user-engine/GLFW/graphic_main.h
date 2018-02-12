#ifndef _GRAPHIC_GLFW3_H_
#define _GRAPHIC_GLFW3_H_
// GLFW3を使ってOpenGLでの描写を行います
// Shogi系の処理とOpenGLとを繋ぐクラスです
#include "graphic_common.h"
#include "graphic_string.h"
#include "../../../position.h"
#include <thread>

class Gui {
private:
#ifdef GLFW3
	std::thread gui_thread; // OpenGLのメインループを回すスレッド
	GLFWwindow* window; // 描写ウィンドウを表すポインタ

	GlString* gl_string; // 文字列を管理する
	Position pos_; // Position構造体を受け取る
	bool is_render_pos = false; // pos_を描写するかのフラグ
	std::string info = u8" "; // 汎用の情報出力用の文字列を格納します

	GLuint textureID_shogiboard; // 将棋盤のテクスチャIDを格納する

	void init(); // グラフィック系の初期化を行う
	void main(); // いわゆる描写系のメインループ
#endif
public:
	Gui();
	~Gui();
	void create_thread(); // 新しいスレッドで初期化系を呼ばないといけない
	void set_pos(const Position &p); // 新しいPositionをコピーしてセットする
	void set_info(const std::string &str); // 新しいstringをコピーしてセットする
	void mouse(const double xpos, const double ypos); //マウスがクリックされたときのウィンドウのx座標とy座標の位置を受け取ります
};

extern Gui gui;

#endif _GRAPHIC_GLFW3_H_
