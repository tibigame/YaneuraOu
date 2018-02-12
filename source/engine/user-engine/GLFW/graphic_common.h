#ifndef _GRAPHIC_COMMON_H_
#define _GRAPHIC_COMMON_H_

// ここをコメントアウトするとグラフィック関連の関数が空になってGLFW3がなくてもビルドできます。
#define GLFW3

#ifdef GLFW3
#define GLFW_INCLUDE_GLU
#include "../include/GLFW/glfw3.h"
#include "../include/SDL_ttf.h"
#endif

#endif _GRAPHIC_COMMON_H_
