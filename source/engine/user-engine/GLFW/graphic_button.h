#ifndef _GRAPHIC_BUTTON_H_
#define _GRAPHIC_BUTTON_H_
#include "graphic_common.h"

// OpenGLでボタンを扱うためのクラスです

#ifdef GLFW3

// ボタンの初期化を行うのに必要なデータの構造体
struct ButtonInitializer {
	double left;
	double bottom;
	double right;
	double top;
	FunctionType ft;
	std::string str;
	bool is_visible;
	bool is_enable;
	ButtonInitializer();
	~ButtonInitializer();
	ButtonInitializer(double left_, double bottom_, double right_, double top_, FunctionType ft_, std::string str_,
		bool is_visible_, bool is_enable_);
};

// ボタンとはある長方形の領域を持ち、ActionのFunctionTypeと関連付けられ、
// ビューに関するいくつかの状態を持ち、状態に応じたdrawを実現するオブジェクトである
// drawの実体はobject.cppに、状態による出し分けの制御はここで行う
// マウスコールバックなどは上流のmainで発行され、redux.cppがget_action()を実行するかを決定する
class Button {
private:
	double left, bottom, right, top;
	FunctionType ft; // 実行する関数のタイプ
	std::string str;
public:
	bool is_visible; // 可視状態か
	bool is_enable; // 有効化状態か
	
	Button();
	~Button();
	Button(ButtonInitializer button_initializer);

	double get_width(); // 幅を求めます
	double get_height(); // 高さを求めます
	double get_center(); // 幅の中央を求めます
	double get_middle(); // 高さの中央を求めます

	const Action get_action(double posx, double posy) const; // アクションを発行する
	void draw();
};

#endif

#endif _GRAPHIC_BUTTON_H_
