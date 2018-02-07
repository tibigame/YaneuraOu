#include <iostream>
#include "position.h"

using namespace std;

constexpr int MAX_SFEN_SIZE = 128; // 128バイト (おそらくsfenはここに収まる)
constexpr char *sfen_ = "sfen ";
constexpr int PIECE_ENUM_NUM = 64; // 多分これぐらい
size_t piece_enum_str_size[PIECE_ENUM_NUM]; // 文字サイズを表す
char *piece_enum_str[PIECE_ENUM_NUM * 2]; // PIECE1個あたり2文字分を確保する
constexpr char CHAR0 = '0';

// Position::sfen_fast()関連の各種テーブルの初期化。
void position_sfen_init_helper(Piece pi, char* str, size_t length) {
	int x = static_cast<int>(pi);
	piece_enum_str_size[x] = length; // 長さ(1か2)を保存しておく
	memcpy(&piece_enum_str[2 * x], str, 2); // 常に2文字分をコピーする
}

void position_sfen_init() {
	memset(piece_enum_str, '\0', PIECE_ENUM_NUM * 2);
	position_sfen_init_helper(NO_PIECE, "-\0", 0);
	position_sfen_init_helper(B_PAWN, "P\0", 1);
	position_sfen_init_helper(B_PRO_PAWN, "+P", 2);
	position_sfen_init_helper(W_PAWN, "p\0", 1);
	position_sfen_init_helper(W_PRO_PAWN, "+p", 2);
	position_sfen_init_helper(B_LANCE, "L\0", 1);
	position_sfen_init_helper(B_PRO_LANCE, "+L", 2);
	position_sfen_init_helper(W_LANCE, "l\0", 1);
	position_sfen_init_helper(W_PRO_LANCE, "+l", 2);
	position_sfen_init_helper(B_KNIGHT, "N\0", 1);
	position_sfen_init_helper(B_PRO_KNIGHT, "+N", 2);
	position_sfen_init_helper(W_KNIGHT, "n\0", 1);
	position_sfen_init_helper(W_PRO_KNIGHT, "+n", 2);
	position_sfen_init_helper(B_SILVER, "S\0", 1);
	position_sfen_init_helper(B_PRO_SILVER, "+S", 2);
	position_sfen_init_helper(W_SILVER, "s\0", 1);
	position_sfen_init_helper(W_PRO_SILVER, "+s", 2);
	position_sfen_init_helper(B_GOLD, "G\0", 1);
	position_sfen_init_helper(W_GOLD, "g\0", 1);
	position_sfen_init_helper(B_BISHOP, "B\0", 1);
	position_sfen_init_helper(B_HORSE, "+B", 2);
	position_sfen_init_helper(W_BISHOP, "b\0", 1);
	position_sfen_init_helper(W_HORSE, "+b", 2);
	position_sfen_init_helper(B_ROOK, "R\0", 1);
	position_sfen_init_helper(B_DRAGON, "+R", 2);
	position_sfen_init_helper(W_ROOK, "r\0", 1);
	position_sfen_init_helper(W_DRAGON, "+r", 2);
	position_sfen_init_helper(B_KING, "K\0", 1);
	position_sfen_init_helper(W_KING, "k\0", 1);
	position_sfen_init_helper(PIECE_NB, "/\0", 1); // 「PIECE_NB」をRANKの境界文字として使う
}

constexpr char *hand_rb_b = "2RB2R2B"; // 手駒の飛角の文字列 (先手)
constexpr char *hand_rb_w = "2rb2r2b"; // 手駒の飛角の文字列 (後手)
constexpr uint32_t hand_rb_bitmask = 0x03300000; // 飛角のビットだけを残す
// Destにhandの飛角の文字列をrb_strからコピーする
// 返り値はコピーした文字数
inline int hand_rb_write(char* Dest, const Hand hand, const char *rb_str) {
	uint32_t rb = hand & hand_rb_bitmask;
	switch (rb) {
	case 0x00000000:
		return 0;
	case 0x00100000: // 角
		memcpy(Dest, &rb_str[2], 1);
		return 1;
	case 0x00200000: // 角2
		memcpy(Dest, &rb_str[5], 2);
		return 2;
	case 0x01000000: // 飛
		memcpy(Dest, &rb_str[1], 1);
		return 1;
	case 0x02000000: // 飛2
		memcpy(Dest, &rb_str[0], 2);
		return 2;
	case 0x01100000: // 飛角
		memcpy(Dest, &rb_str[1], 2);
		return 2;
	case 0x02100000: // 飛2角
		memcpy(Dest, &rb_str[0], 3);
		return 3;
	case 0x01200000: // 飛角2
		memcpy(Dest, &rb_str[4], 3);
		return 3;
	case 0x02200000: // 飛2角2
		memcpy(Dest, &rb_str[3], 4);
		return 4;
	}
	return 0; // ここに来ることはない
}
// Destにhandにmaskとshiftをして駒数を求めてから駒文字pieceを書き込む
// 返り値は書き込んだ文字数
inline int hand_gsnlp_write(char* Dest, const Hand hand, const uint32_t mask, const uint32_t shift, const char piece) {
	int n = (hand & mask) >> shift;
	if (n == 0) { // その駒を持っていないので何もしない
		return 0;
	}
	if (n >= 10) { // 10-18枚以上の場合
		*Dest = CHAR0 + 1;
		*(Dest + 1) = CHAR0 + n - 10;
		*(Dest + 2) = piece;
		return 3;
	}
	if (n != 1) { // 駒数と駒文字を書き込む
		*Dest = CHAR0 + n;
		*(Dest + 1) = piece;
		return 2;
	}
	else { // 駒文字を書き込む
		*Dest = piece;
		return 1;
	}
}

const std::string Position::sfen_fast() const
{
	char *str = static_cast<char*>(_aligned_malloc(MAX_SFEN_SIZE, 64));
	char *p = str + 5; // 文字列操作用のポインタ
	memcpy(str, sfen_, 5); // 接頭辞
	// ここから盤面の文字列
	char empty_cnt = 0;
	int enum_int;
	size_t copy_size;
	const Piece piece[89] = { // sfen変換対象として処理していく駒の配列
		board[72], board[63], board[54], board[45], board[36], board[27], board[18], board[9], board[0], PIECE_NB,
		board[73], board[64], board[55], board[46], board[37], board[28], board[19], board[10], board[1], PIECE_NB,
		board[74], board[65], board[56], board[47], board[38], board[29], board[20], board[11], board[2], PIECE_NB,
		board[75], board[66], board[57], board[48], board[39], board[30], board[21], board[12], board[3], PIECE_NB,
		board[76], board[67], board[58], board[49], board[40], board[31], board[22], board[13], board[4], PIECE_NB,
		board[77], board[68], board[59], board[50], board[41], board[32], board[23], board[14], board[5], PIECE_NB,
		board[78], board[69], board[60], board[51], board[42], board[33], board[24], board[15], board[6], PIECE_NB,
		board[79], board[70], board[61], board[52], board[43], board[34], board[25], board[16], board[7], PIECE_NB,
		board[80], board[71], board[62], board[53], board[44], board[35], board[26], board[17], board[8]
	};
	for (auto i = 0; i < 89; ++i) {
		enum_int = static_cast<int>(piece[i]);
		copy_size = piece_enum_str_size[enum_int];
		if (copy_size) { // 何らかの駒が存在した
			if (empty_cnt) { // 0でないということは空升がいくつかあったということ
				*p = CHAR0 + empty_cnt; // 空升の数をコピーする
				++p;
				empty_cnt = 0;
			}
			memcpy(p, &piece_enum_str[enum_int * 2], copy_size);
			p += copy_size;
		}
		else { // 文字サイズが0ということは空升だったということ
			++empty_cnt;
		}
	}
	
	// ここから手番の文字列
	memcpy(p, sideToMove == WHITE ? " w " : " b ", 3);
	p += 3;
	
	// ここから手駒の文字列
	if (hand[BLACK] || hand[WHITE]) { // 手駒なしのとき
		memcpy(p, "- ", 2);
		p += 2;
	}
	else {
		p += hand_rb_write(p, hand[BLACK], hand_rb_b);
		p += hand_gsnlp_write(p, hand[BLACK], PIECE_BIT_MASK2[GOLD], PIECE_BITS[GOLD], 'G');
		p += hand_gsnlp_write(p, hand[BLACK], PIECE_BIT_MASK2[SILVER], PIECE_BITS[SILVER], 'S');
		p += hand_gsnlp_write(p, hand[BLACK], PIECE_BIT_MASK2[KNIGHT], PIECE_BITS[KNIGHT], 'N');
		p += hand_gsnlp_write(p, hand[BLACK], PIECE_BIT_MASK2[LANCE], PIECE_BITS[LANCE], 'L');
		p += hand_gsnlp_write(p, hand[BLACK], PIECE_BIT_MASK2[PAWN], PIECE_BITS[PAWN], 'P');
		p += hand_rb_write(p, hand[WHITE], hand_rb_w);
		p += hand_gsnlp_write(p, hand[WHITE], PIECE_BIT_MASK2[GOLD], PIECE_BITS[GOLD], 'g');
		p += hand_gsnlp_write(p, hand[WHITE], PIECE_BIT_MASK2[SILVER], PIECE_BITS[SILVER], 's');
		p += hand_gsnlp_write(p, hand[WHITE], PIECE_BIT_MASK2[KNIGHT], PIECE_BITS[KNIGHT], 'n');
		p += hand_gsnlp_write(p, hand[WHITE], PIECE_BIT_MASK2[LANCE], PIECE_BITS[LANCE], 'l');
		p += hand_gsnlp_write(p, hand[WHITE], PIECE_BIT_MASK2[PAWN], PIECE_BITS[PAWN], 'p');
		*p = ' ';
		++p;
	}

	// ここから手数の文字列
	int gp = gamePly;
	if (gp < 10) { // 10未満
		*p = CHAR0 + gp;
		++p;
		*p = '\0';
		return std::string(str);
	}
	div_t r = div(gp, 10);
	if (r.quot < 10) { // 100未満
		*p = CHAR0 + r.quot;
		++p;
		*p = CHAR0 + r.rem;
		++p;
		*p = '\0';
		return std::string(str);
	}
	if (r.quot < 100) { // 1000未満
		*(p + 2) = CHAR0 + r.rem;
		r = div(r.quot, 10);
		*p = CHAR0 + r.quot;
		++p;
		*p = CHAR0 + r.rem;
		p+=2;
		*p = '\0';
		return std::string(str);
	}
	// 手数が10000以上になることはないだろう
	*(p + 3) = CHAR0 + r.rem;
	r = div(r.quot, 10);
	*(p + 2) = CHAR0 + r.rem;
	r = div(r.quot, 10);
	*p = CHAR0 + r.quot;
	++p;
	*p = CHAR0 + r.rem;
	p += 3;
	*p = '\0';
	return std::string(str);
}
