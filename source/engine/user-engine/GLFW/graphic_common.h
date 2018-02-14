#ifndef _GRAPHIC_COMMON_H_
#define _GRAPHIC_COMMON_H_

// ここをコメントアウトするとグラフィック関連の関数が空になってGLFW3がなくてもビルドできます。
#define GLFW3

#ifdef GLFW3
#define GLFW_INCLUDE_GLU
#include "../include/GLFW/glfw3.h"
#include "../include/SDL_ttf.h"

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

#endif

#endif _GRAPHIC_COMMON_H_
