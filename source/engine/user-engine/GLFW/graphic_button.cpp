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
	std::string str_, bool is_visible_, bool is_enable_) {
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
float greenDiffuse[] = { 0.0, 1.0, 0.8, 1.0 };
void Button::draw() {
	draw_button(left, bottom, right, top, mat_ambient, mat_diffuse, mat_specular);
}

#endif
