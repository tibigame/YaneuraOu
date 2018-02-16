#ifndef _GRAPHIC_BUTTON_H_
#define _GRAPHIC_BUTTON_H_
#include "graphic_common.h"

// OpenGLでボタンを扱うためのクラスです

#ifdef GLFW3

// ボタンとはある長方形の領域を持ち、ActionのFunctionTypeと関連付けられ、
// ビューに関するいくつかの状態を持ち、状態に応じたdrawを実現するオブジェクトである
// drawの実体はobject.cppに、状態による出し分けの制御はここで行う
// マウスコールバックなどは上流のmainで発行され、redux.cppがget_action()を実行するかを決定する
class Button {
private:
	double left, bottom, right, top;
	FunctionType ft; // 実行する関数のタイプ
public:
	bool is_visible = false; // 可視状態か
	bool is_enable = false; // 有効化状態か
	
	Button();
	~Button();

	double get_width(); // 幅を求めます
	double get_height(); // 高さを求めます
	double get_center(); // 幅の中央を求めます
	double get_middle(); // 高さの中央を求めます

	void init(double left_, double bottom_, double right_, double top_, FunctionType ft_); // 初期化を行う
	const Action get_action(double posx, double posy, const std::string &str_) const; // アクションを発行する
	void draw(std::string &str_);
};

#endif

#endif _GRAPHIC_BUTTON_H_
