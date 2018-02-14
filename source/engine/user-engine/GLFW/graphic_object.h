#ifndef _GRAPHIC_OBJECT_H_
#define _GRAPHIC_OBJECT_H_
#include "graphic_common.h"
#include "graphic_string.h"

// graphic_primitive.cppやgraphic_string.cppを利用して複雑なオブジェクトを描写する関数群です

#ifdef GLFW3
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

void draw_shogiboard(const GLuint &textureID); // 将棋盤を描写します				 
void draw_shogiboard_rank_file_number(GlString* gl_string); // 段、筋数字の描写
void int_string(const int num, NumberType num_type,
	const GLfloat f_size, GLfloat display_x, GLfloat display_y, GLfloat offset, GlString* gl_string);
void board_piece_string(const Piece pc, const Square sq, GlString* gl_string, const Color c);
void draw_board(const Position &pos_, GlString* gl_string); // 盤面の駒文字を描写します
void hand_piece_string(const Piece pc, const int num, const Color c, GlString* gl_string,
	const GLfloat f_size, const GLfloat f_size_offset); // 手駒領域に文字1つを描写します
void draw_hand(const Position &pos_, GlString* gl_string); // 手駒の駒文字を描写します
void draw_teban(const Position &pos_, GlString* gl_string); // 手番と手数を描写します
void draw_info(const std::string &info_, GlString* gl_string); // info情報を出力します
#endif

#endif _GRAPHIC_OBJECT_H_
