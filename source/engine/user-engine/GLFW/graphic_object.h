#ifndef _GRAPHIC_OBJECT_H_
#define _GRAPHIC_OBJECT_H_
#include "graphic_common.h"
#include "graphic_string.h"

// graphic_primitive.cppやgraphic_string.cppを利用して複雑なオブジェクトを描写する関数群です

#ifdef GLFW3
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
void draw_string(std::string str, GlString* gl_string,
	GLfloat line_height, GLfloat MAX_LINE_WIDTH, GLint MAX_LINE,
	GLfloat *mat_ambient, GLfloat *mat_diffuse, GLfloat *mat_specular, GLfloat z,
	GLfloat display_x, GLfloat display_y, double degree, GLfloat font_size);
void draw_button(GLfloat left, GLfloat bottom, GLfloat right, GLfloat top,
	GLfloat *mat_ambient, GLfloat *mat_diffuse, GLfloat *mat_specular, GLfloat z, std::string str, GLfloat left_offset);
#endif

#endif _GRAPHIC_OBJECT_H_
