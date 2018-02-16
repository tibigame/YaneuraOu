#include "graphic_object.h"
#include "graphic_button.h"

// OpenGLでボタンを扱うためのクラスです

#ifdef GLFW3

ButtonInitializer::ButtonInitializer() {
}

ButtonInitializer::~ButtonInitializer() {
}

ButtonInitializer::ButtonInitializer(double left_, double bottom_, double right_, double top_, FunctionType ft_=FunctionType::NONE, std::string str_="",
	bool is_visible_=false, bool is_enable_=false) {
	left = left_;
	bottom = bottom_;
	right = right_;
	top = top_;
	ft = ft_;
	str = str_;
	is_visible = is_visible_;
	is_enable = is_enable_;
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

// アクションを発行する
const Action Button::get_action(double posx, double posy) const {
	if (
		!is_visible && !is_enable && // 可視でない または 有効でない
		left < posx && posx < right && bottom < posy && posy < top) { // posの範囲チェック
		return Action(ft, str);
	}
	return ActionNone; // 対象外のコールバックなのでNONEアクションを返す
}

void Button::draw() {
	draw_button(left, bottom, right, top,
		conv_GL_color(0, 0, 0, 255), conv_GL_color(255, 255, 255, 255), conv_GL_color(0, 0, 0, 255));
}

#endif
