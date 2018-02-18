#ifndef _GRAPHIC_COMMON_H_
#define _GRAPHIC_COMMON_H_

// ここをコメントアウトするとグラフィック関連の関数が空になってGLFW3がなくてもビルドできます。
#define GLFW3

#ifdef GLFW3
#define GLFW_INCLUDE_GLU
#include "../include/GLFW/glfw3.h"
#include "../include/SDL_ttf.h"
#include <string>

// 座標と各種オブジェクトのサイズを規定する
constexpr float board_size = 9.f; // 将棋盤の升目部分のサイズ
constexpr float board_border = 0.5f; // 将棋盤の升目から境界までのサイズ
constexpr float shogiboard_size = board_size + 2 * board_border; // 将棋盤全体のサイズ
constexpr float hand_inner_margin = 0.5f; // 駒台と将棋盤の間のマージン
constexpr float hand_size = 0.7f; // 将棋盤左右に置く駒台のサイズ
constexpr float hand_outer_margin = 0.5f; // 駒台外側のマージン
constexpr float total_width = shogiboard_size + 2 * (hand_inner_margin + hand_size + hand_outer_margin); // 画面の横幅
constexpr float shogiboard_top_margin = 1.f; // 将棋盤から上側のマージン
constexpr float shogiboard_bottom_margin = 0.5f; // 将棋盤から下側のマージン
constexpr float string_size = 2.5f; // 文字エリアのサイズ
constexpr float string_bottom_margin = 0.5f; // 文字エリアから下側のマージン
constexpr float total_height = shogiboard_size + shogiboard_top_margin + shogiboard_bottom_margin
+ string_size + string_bottom_margin; // 画面の縦幅

constexpr float view_right = board_border + hand_inner_margin + hand_size + hand_outer_margin;
constexpr float view_left = view_right - total_width;
constexpr float view_top = board_border + shogiboard_top_margin;
constexpr float view_bottom = view_top - total_height;

constexpr float shogiboard_line_heap = 0.09; // 将棋盤の線を盛り上げる

constexpr int window_width = 1000; // ウィンドウの横幅
constexpr int window_height = static_cast<int>(window_width * total_height / total_width); // ウィンドウの縦幅

// 実行する関数のタイプ
enum class FunctionType
{
	IS_READY, // isready
	EXIT, // 終了
	USER, // USER
	ADD_BUTTON, // ボタンの追加
	UPDATE_INFO, // infoの更新
	UPDATE_POS, // pos_の更新
	BENCH, // ベンチマークの実行
	GEN_RANDOM_SFEN, // ランダム局面を1つ生成する
	TEST, // テスト関数
	NONE
};

// Actionの定義 (Actionは関数のタイプとstringからなる)
struct Action {
	FunctionType ft;
	std::string str;
	int index;
	void* p;
	Action::Action();
	Action(const FunctionType ft_, const std::string &str);
};

inline Action::Action() {}

inline Action::Action(const FunctionType ft_, const std::string &str_) {
	ft = ft_;
	str = str_;
	index = 0;
	p = nullptr;
}


const Action ActionNone = Action(FunctionType::NONE, "");

struct GlColor {
	union {
		GLfloat c[4];
		GLfloat red;
		GLfloat green;
		GLfloat blue;
		GLfloat alpha;
	};
	inline GlColor() {};
	inline const GLfloat* operator()()
	{
		return c;
	}
	inline GlColor(unsigned char red_, unsigned char green_, unsigned char blue_, unsigned char alpha_) {
		red = red_ / 255.f;
		green = green_ / 255.f;
		blue = blue_ / 255.f;
		alpha = alpha_ / 255.f;
	}
};

// RGBをGLの色情報に変換します
inline GLfloat *conv_GL_color(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha) {
	GLfloat c[] = { red / 255.f , green / 255.f , blue / 255.f , alpha / 255.f };
	return c;
}

inline void set_glColor(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha) {
	glColor4f(red / 255.f, green / 255.f, blue / 255.f, alpha / 255.f);
}

inline void set_glColor(GLfloat *GL_c) {
	glColor4f(GL_c[0], GL_c[1], GL_c[2], GL_c[3]);
}

// 色の定義
extern GLfloat GL_COLOR_ZERO[];
extern GLfloat GL_COLOR_MAX[];
extern GLfloat GL_COLOR_BUTTON[];
extern GLfloat GL_COLOR_BUTTON_ALPHA_WHITE[];

#endif

#endif _GRAPHIC_COMMON_H_
