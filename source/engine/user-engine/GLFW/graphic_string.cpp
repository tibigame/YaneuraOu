#include "graphic_primitive.h"
#include "graphic_string.h"

// OpenGLで文字列を扱うためのクラスです

#ifdef GLFW3

constexpr char* font_filename = "resource/XANO-mincho-U32.ttf";
constexpr int fontcash_pt_size = 48;
constexpr SDL_Color font_color_black = { 6, 6, 6, 255 }; // 黒文字色
constexpr SDL_Color font_color_piece = { 13, 38, 38, 255 }; // 駒色
constexpr SDL_Color font_color_red = { 178, 28, 68, 255 }; // 成り駒色
constexpr char* sideToMove_b_str = u8"手番: 先手";
constexpr char* sideToMove_w_str = u8"手番: 後手";
constexpr char* gameply_str = u8"手数: ";

constexpr int powerOfTwo(int input) {
	int value = 1;
	while (value < input) {
		value <<= 1;
	}
	return value;
}

// コンストラクタとデストラクタ
GlString::GlString() {
}

GlString::~GlString() {
	font_close();
}

// フォントを初期化します
void GlString::font_init() {
	TTF_Init(); // 初期化
	font_ = TTF_OpenFont(font_filename, fontcash_pt_size); // フォントを読み込む
	create_texture_cash(u8"王", font_color_piece);
	create_texture_cash(u8"玉", font_color_piece);
	create_texture_cash(u8"飛", font_color_piece);
	create_texture_cash(u8"角", font_color_piece);
	create_texture_cash(u8"金", font_color_piece);
	create_texture_cash(u8"銀", font_color_piece);
	create_texture_cash(u8"桂", font_color_piece);
	create_texture_cash(u8"香", font_color_piece);
	create_texture_cash(u8"歩", font_color_piece);
	create_texture_cash(u8"龍", font_color_red);
	create_texture_cash(u8"馬", font_color_red);
	create_texture_cash(u8"全", font_color_red);
	create_texture_cash(u8"圭", font_color_red);
	create_texture_cash(u8"杏", font_color_red);
	create_texture_cash(u8"と", font_color_red);
	create_texture_cash(u8"先", font_color_black);
	create_texture_cash(u8"後", font_color_black);
	create_texture_cash(u8"手", font_color_black);
	create_texture_cash(u8"０", font_color_black);
	create_texture_cash(u8"１", font_color_black);
	create_texture_cash(u8"２", font_color_black);
	create_texture_cash(u8"３", font_color_black);
	create_texture_cash(u8"４", font_color_black);
	create_texture_cash(u8"５", font_color_black);
	create_texture_cash(u8"６", font_color_black);
	create_texture_cash(u8"７", font_color_black);
	create_texture_cash(u8"８", font_color_black);
	create_texture_cash(u8"９", font_color_black);
	create_texture_cash(u8"〇", font_color_black);
	create_texture_cash(u8"一", font_color_black);
	create_texture_cash(u8"二", font_color_black);
	create_texture_cash(u8"三", font_color_black);
	create_texture_cash(u8"四", font_color_black);
	create_texture_cash(u8"五", font_color_black);
	create_texture_cash(u8"六", font_color_black);
	create_texture_cash(u8"七", font_color_black);
	create_texture_cash(u8"八", font_color_black);
	create_texture_cash(u8"九", font_color_black);
	create_texture_cash(u8"十", font_color_black);
	create_texture_cash(sideToMove_b_str, font_color_black);
	create_texture_cash(sideToMove_w_str, font_color_black);
	create_texture_cash(gameply_str, font_color_black);
	create_texture_cash(u8"0", font_color_black);
	create_texture_cash(u8"1", font_color_black);
	create_texture_cash(u8"2", font_color_black);
	create_texture_cash(u8"3", font_color_black);
	create_texture_cash(u8"4", font_color_black);
	create_texture_cash(u8"5", font_color_black);
	create_texture_cash(u8"6", font_color_black);
	create_texture_cash(u8"7", font_color_black);
	create_texture_cash(u8"8", font_color_black);
	create_texture_cash(u8"9", font_color_black);
	create_texture_cash(u8"0", font_color_black);
	create_texture_cash(u8"-", font_color_black);
}

// フォントの解放をします
void GlString::font_close() {
	TTF_CloseFont(font_); // フォントを破棄
	TTF_Quit();
}

// 1文字のu8 stringを受け取ってテクスチャIDとの連想配列を作ります
void GlString::create_texture_cash(const char* u8_char, SDL_Color font_color) {
	std::string s(u8_char);
	// ラスタライズしたビットマップを取得
	SDL_Surface* surface = TTF_RenderUTF8_Blended(font_, u8_char, font_color);
	int width_ = surface->w;
	int height_ = surface->h;

	// サーフェスの大きさを２のｎ乗に拡張する
	int w = powerOfTwo(width_);
	int h = powerOfTwo(height_);
	GLfloat left_ = 0.0;
	GLfloat top_ = 0.0;
	GLfloat right_ = (GLfloat)width_ / w;
	GLfloat bottom_ = (GLfloat)height_ / h;

	SDL_Surface *image = SDL_CreateRGBSurface(
		SDL_SWSURFACE,
		w, h,
		32,
		// OpenGL RGBAのマスクをする
#if SDL_BYTEORDER==SDL_LIL_ENDIAN
		0x000000FF,
		0x0000FF00,
		0x00FF0000,
		0xFF000000
#else
		0xFF000000,
		0x00FF0000,
		0x0000FF00,
		0x000000FF
#endif
	);

	if (image == NULL) {
		return;
	}

	// テクスチャイメージにサーフェスをコピーする
	SDL_Rect area;
	area.x = 0;
	area.y = 0;
	area.w = width_;
	area.h = height_;
	SDL_BlitSurface(surface, &area, image, &area);

	// OpenGLテクスチャを生成する
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, image->pixels);
	SDL_FreeSurface(image);

	// テクスチャを拡大・縮小する方法の指定 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	mp[s] = textureID; // 連想配列に登録
}

// 任意の文字に対応するテクスチャIDを取得します。なければ作成します。
GLuint GlString::create_and_get_texture_id(const char* u8_char) {
	std::string s(u8_char);
	auto itr = mp.find(s);
	if (itr == mp.end()) { // キーが設定されていない
		create_texture_cash(u8_char, font_color_black);
	}
	return mp[s];
}

// 文字に対応するテクスチャIDを取得します。なければfalseを返します
bool GlString::get_texture_id(const char* u8_char, GLuint &textureID) {
	std::string s(u8_char);
	auto itr = mp.find(s);
	if (itr == mp.end()) { // キーが設定されていない
		return false;
	}
	textureID = mp[s];
	return true;
}

// Pieceに対応するテクスチャIDを取得します。なければfalseを返します
bool GlString::get_texture_id(const Piece pc, GLuint &textureID) {
	std::string s;
	switch (pc) {
		case B_KING: s = u8"玉"; break;
		case W_KING: s = u8"王"; break;
		case B_ROOK: case W_ROOK: s = u8"飛"; break;
		case B_BISHOP: case W_BISHOP: s = u8"角"; break;
		case B_GOLD: case W_GOLD: s = u8"金"; break;
		case B_SILVER: case W_SILVER: s = u8"銀"; break;
		case B_KNIGHT: case W_KNIGHT: s = u8"桂"; break;
		case B_LANCE: case W_LANCE: s = u8"香"; break;
		case B_PAWN: case W_PAWN: s = u8"歩"; break;
		case B_DRAGON: case W_DRAGON: s = u8"龍"; break;
		case B_HORSE: case W_HORSE: s = u8"馬"; break;
		case B_PRO_SILVER: case W_PRO_SILVER: s = u8"全"; break;
		case B_PRO_KNIGHT: case W_PRO_KNIGHT: s = u8"圭"; break;
		case B_PRO_LANCE: case W_PRO_LANCE: s = u8"杏"; break;
		case B_PRO_PAWN: case W_PRO_PAWN: s = u8"と"; break;
	}

	auto itr = mp.find(s);
	if (itr == mp.end()) { // キー設定されていない
		return false;
	}
	textureID = mp[s];
	return true;
}

// 数字に対応するテクスチャIDを取得します。なければfalseを返します
bool GlString::get_texture_id(const int num, NumberType num_type, GLuint &textureID) {
	std::string s;
	if (num_type == NumberType::Chinese) { // 漢数字
		switch (num) {
			case 0: s = u8"〇"; break;
			case 1: s = u8"一"; break;
			case 2: s = u8"二"; break;
			case 3: s = u8"三"; break;
			case 4: s = u8"四"; break;
			case 5: s = u8"五"; break;
			case 6: s = u8"六"; break;
			case 7: s = u8"七"; break;
			case 8: s = u8"八"; break;
			case 9: s = u8"九"; break;
			case 10: s = u8"十"; break;
			default: return false;
		}
	}
	else if (num_type == NumberType::Alabic) { // アラビア数字
		switch (num) {
			case 0: s = u8"０"; break;
			case 1: s = u8"１"; break;
			case 2: s = u8"２"; break;
			case 3: s = u8"３"; break;
			case 4: s = u8"４"; break;
			case 5: s = u8"５"; break;
			case 6: s = u8"６"; break;
			case 7: s = u8"７"; break;
			case 8: s = u8"８"; break;
			case 9: s = u8"９"; break;
			default: return false;
		}
	}
	else if (num_type == NumberType::HalfAlabic) { // 半角数字
		switch (num) {
		case 0: s = u8"0"; break;
		case 1: s = u8"1"; break;
		case 2: s = u8"2"; break;
		case 3: s = u8"3"; break;
		case 4: s = u8"4"; break;
		case 5: s = u8"5"; break;
		case 6: s = u8"6"; break;
		case 7: s = u8"7"; break;
		case 8: s = u8"8"; break;
		case 9: s = u8"9"; break;
		default: return false;
		}
	}
	auto itr = mp.find(s);
	if (itr == mp.end()) { // キーが設定されていない
		return false;
	}
	textureID = mp[s];
	return true;
}

// 手番文字列に対応するテクスチャIDを取得します。なければfalseを返します
bool GlString::get_texture_id(const Color c, GLuint &textureID) {
	std::string s = c == BLACK ? sideToMove_b_str : sideToMove_w_str;
	auto itr = mp.find(s);
	if (itr == mp.end()) { // キーが設定されていない
		return false;
	}
	textureID = mp[s];
	return true;
}

// 手数文字列に対応するテクスチャIDを取得します。なければfalseを返します
bool GlString::get_texture_id(const int gameply, GLuint &textureID) {
	std::string s = gameply_str;
	auto itr = mp.find(s);
	if (itr == mp.end()) { // キーが設定されていない
		return false;
	}
	textureID = mp[s];
	return true;
}


StringLength::StringLength(std::string &str) {
	lenUtf8(str);
}

StringLength::~StringLength() {
}

static inline int lenByte(unsigned char c)
{
	if ((c >= 0x00) && (c <= 0x7f))
	{
		return 1;
	}
	else if ((c >= 0xc2) && (c <= 0xdf))
	{
		return 2;
	}
	else if ((c >= 0xe0) && (c <= 0xef))
	{
		return 3;
	}
	else if ((c >= 0xf0) && (c <= 0xf7))
	{
		return 4;
	}
	return 0;
}

int StringLength::lenUtf8(std::string &str) {
	auto _pstr = str.c_str();
	int i = 0;
	int len = 0;
	int len1_count = 0;
	int temp_length = 0;
	while (_pstr[i] != '\0')
	{
		++len;
		temp_length = lenByte(_pstr[i]);
		if (temp_length == 1) { ++len1_count; }
		i += temp_length;
	}
	length = len;
	render_length = len - len1_count / 2;
	return len;
}

// UTF8文字列を1文字ずつ取り出すルーチン
std::tuple<std::string, int> utf8_next_char(std::string str, size_t str_size, int pos)
{
	using namespace std;
	unsigned char c;

	string buf;
	int buf_size;

	if (pos >= str_size) {
		return make_tuple(u8"?", pos);
	}

	c = str[pos];

	if (c < 0x80) {
		buf_size = 1;
	}
	else if (c < 0xE0) {
		buf_size = 2;
	}
	else if (c < 0xF0) {
		buf_size = 3;
	}
	else {
		buf_size = 4;
	}

	buf = str.substr(pos, buf_size);

	return  make_tuple(buf, buf_size);
}

#endif
