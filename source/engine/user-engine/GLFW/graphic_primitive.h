#ifndef _GRAPHIC_PRIMITIVE_H_
#define _GRAPHIC_PRIMITIVE_H_
#include "graphic_common.h"

// OpenGLをラッパーして基礎的なグラフィック描写を行う関数群です

#ifdef GLFW3
void draw_init(GLuint &textureID);
void draw_loop_init();
GLfloat *conv_GL_color(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha=255);
void set_glColor(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha=255);
void set_glColor(GLfloat *GL_c);
void draw_line(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLfloat width,
	GLfloat *mat_ambient, GLfloat *mat_diffuse, GLfloat *mat_specular, GLfloat z=0.f);
void draw_rect(GLfloat left, GLfloat bottom, GLfloat right, GLfloat top,
	GLfloat *mat_ambient, GLfloat *mat_diffuse, GLfloat *mat_specular, GLfloat z=0.f,
	GLfloat display_x=0.f, GLfloat display_y=0.f, double degree=0.0, bool is_texture = false, GLuint textureID=0);
void draw_circle(GLfloat x, GLfloat y, GLfloat r,
	GLfloat *mat_ambient, GLfloat *mat_diffuse, GLfloat *mat_specular, GLfloat z=0.f);

void draw_rect_ex(GLfloat left, GLfloat bottom, GLfloat right, GLfloat top,
	GLfloat *mat_ambient, GLfloat *mat_diffuse, GLfloat *mat_specular, GLfloat z = 0.f,
	GLfloat display_x = 0.f, GLfloat display_y = 0.f, double degree = 0.0, GLfloat scale = 1.f, GLfloat string_offset = 0.f,
	bool is_texture = false, GLuint textureID = 0);

void draw_pentagon_ex(GLfloat left, GLfloat bottom, GLfloat right, GLfloat top,
	GLfloat *mat_ambient, GLfloat *mat_diffuse, GLfloat *mat_specular, GLfloat z = 0.f,
	GLfloat display_x = 0.f, GLfloat display_y = 0.f, double degree = 0.0, GLfloat scale = 1.f, GLfloat string_offset = 0.f,
	bool is_texture = false, GLuint textureID = 0);
#endif

#endif _GRAPHIC_PRIMITIVE_H_
