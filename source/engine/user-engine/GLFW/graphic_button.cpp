#include "graphic_object.h"
#include "graphic_button.h"

// OpenGLでボタンを扱うためのクラスです

#ifdef GLFW3

ButtonInitializer::ButtonInitializer() {
}

ButtonInitializer::~ButtonInitializer() {
}

ButtonInitializer::ButtonInitializer(double left_, double bottom_, double right_, double top_, FunctionType ft_,
	GLfloat *mat_ambient_, GLfloat *mat_diffuse_, GLfloat *mat_specular_,
	std::string str_, bool is_visible_, bool is_enable_, GLfloat left_offset_) {
	left = left_;
	bottom = bottom_;
	right = right_;
	top = top_;
	ft = ft_;
	str = str_;
	is_visible = is_visible_;
	is_enable = is_enable_;
	mat_ambient = mat_ambient_;
	mat_diffuse = mat_diffuse_;
	mat_specular = mat_specular_;
	left_offset = left_offset_;
}

Button::Button() {
}

Button::~Button() {
}

// 初期化を行う
Button::Button(ButtonInitializer button_initializer) {
	left = button_initializer.left;
	bottom = button_initializer.bottom;
	right = button_initializer.right;
	top = button_initializer.top;
	ft = button_initializer.ft;
	str = button_initializer.str;
	is_visible = button_initializer.is_visible;
	is_enable = button_initializer.is_enable;
	mat_ambient = button_initializer.mat_ambient;
	mat_diffuse = button_initializer.mat_diffuse;
	mat_specular = button_initializer.mat_specular;
	left_offset = button_initializer.left_offset;
	// TODO: 座標変換を行いウィンドウの座標を求める
	// 計算と全然合わないのでそれっぽい位置になるように補正
	window_left = ((-0.3f + left * 1.08f + (board_size + board_border + hand_inner_margin + hand_size + hand_outer_margin)) / total_width) * window_width * 1.05f;
	window_right = ((-0.3f + right * 1.08f + (board_size + board_border + hand_inner_margin + hand_size + hand_outer_margin)) / total_width) * window_width * 1.05f;
	window_top = ((-top * 1.1f + (board_border + shogiboard_top_margin)) / total_height) * window_height * 1.1f;
	window_bottom = ((-bottom * 1.1f + (board_border + shogiboard_top_margin)) / total_height) * window_height * 1.1f;
}

double Button::get_width() { // 幅を求めます
	return right - left;
}
double Button::get_height() {// 高さを求めます
	return top - bottom;
}
double Button::get_center() { // 幅の中央を求めます
	return (left + right) * 0.5;
}
double Button::get_middle() { // 高さの中央を求めます
	return (top + bottom) * 0.5;
}

#include <stdlib.h>

// アクションを発行する
const Action Button::get_action(double posx, double posy) const {
	if (
		is_visible && is_enable && // 可視 かつ 有効
		window_left < posx && posx < window_right && window_top < posy && posy < window_bottom) { // posの範囲チェック
		return Action(ft, str);
	}
	return ActionNone; // 対象外のコールバックなのでNONEアクションを返す

	char a[100];
	std::string test = u8"window_left = ";
	_itoa_s((int)window_left, a, 10);
	test += a;
	test += u8", window_right = ";
	_itoa_s((int)window_right, a, 10);
	test += a;
	_itoa_s((int)posx, a, 10);
	test += u8", posx = ";
	test += a;
	test += u8",\nwindow_top = ";
	_itoa_s((int)window_top, a, 10);
	test += a;
	test += u8", window_bottom = ";
	_itoa_s((int)window_bottom, a, 10);
	test += a;
	_itoa_s((int)posy, a, 10);
	test += u8", posy = ";
	test += a;
	test += u8"\n日本語テスト！aあ☆＠";


	return Action(FunctionType::UPDATE_INFO, test);
	return ActionNone; // 対象外のコールバックなのでNONEアクションを返す
}

void Button::draw() {
	draw_button(left, bottom, right, top, mat_ambient, mat_diffuse, mat_specular, 0.6f, str, left_offset);
}

#endif
