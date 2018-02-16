#ifndef _GRAPHIC_STRING_H_
#define _GRAPHIC_STRING_H_
#include "graphic_common.h"
#include "../../../shogi.h"
#include <map>

// OpenGLで文字列を扱うためのクラスです

#ifdef GLFW3

// 数字の種類
enum class NumberType
{
	Chinese, // 漢数字
	Alabic, // 全角数字
	HalfAlabic // 半角数字
};

class GlString {
private:
	std::map<std::string, GLuint> mp; // u8_stringとtextureIDとの連想配列
	TTF_Font* font_;
public:
	GlString();
	~GlString();

	void font_init(); // フォントを初期化します
	void font_close(); // フォントの解放をします
	void create_texture_cash(const char* u8_char, SDL_Color font_color); // フォントのテクスチャキャッシュを作成します
	GLuint create_and_get_texture_id(const char* u8_char);  // 任意の文字に対応するテクスチャIDを取得します。なければ作成します。
	bool get_texture_id(const char* u8_char, GLuint &textureID); // 文字に対応するテクスチャIDを取得します。なければfalseを返します
	bool get_texture_id(const Piece pc, GLuint &textureID); // Pieceに対応するテクスチャIDを取得します。なければfalseを返します
	bool get_texture_id(const int num, NumberType num_type, GLuint &textureID); // 数字に対応するテクスチャIDを取得します。なければfalseを返します
	bool get_texture_id(const Color c, GLuint &textureID); // 手番文字列に対応するテクスチャIDを取得します。なければfalseを返します
	bool get_texture_id(const int gameply, GLuint &textureID); // 手数文字列に対応するテクスチャIDを取得します。なければfalseを返します
};

class StringLength {
public:
	StringLength(std::string &str);
	~StringLength();
	int length=0;
	int render_length = 0;

	int lenUtf8(std::string &str);
};

std::tuple<std::string, int> utf8_next_char(std::string str, size_t str_size, int pos);

#endif

#endif _GRAPHIC_STRING_H_
