#ifndef _GRAPHIC_GLFW3_H_
#define _GRAPHIC_GLFW3_H_
// GLFW3を使ってOpenGLでの描写を行います
// Shogi系の処理とOpenGLとを繋ぐクラスです
#include "graphic_common.h"
#include "graphic_redux.h"
#include <thread>

class Gui {
private:
#ifdef GLFW3
	std::thread gui_thread; // OpenGLのメインループを回すスレッド
	GLFWwindow* window; // 描写ウィンドウを表すポインタ

	void init(); // グラフィック系の初期化を行う
	void main(); // いわゆる描写系のメインループ
#endif
public:
	Gui();
	~Gui();
#ifdef GLFW3
	Store store; // 状態を管理する
#endif
	void create_thread(); // 新しいスレッドで初期化系を呼ばないといけない
};

extern Gui gui;

#endif _GRAPHIC_GLFW3_H_
