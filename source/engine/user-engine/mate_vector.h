#ifndef _MATE_VECTOR_H_
#define _MATE_VECTOR_H_

#include "../../shogi.h"

// 1:詰みあり、0:不明、-1:詰みなし
constexpr char MATE_VECTOR_MATE = 1;
constexpr char MATE_VECTOR_UNKOWN = 0;
constexpr char MATE_VECTOR_NMATE = -1;

inline char* mate_vector_string(char x) {
	switch (x) {
		case MATE_VECTOR_MATE: return u8"+";
		case MATE_VECTOR_NMATE: return u8"-";
	}
	return u8"0";
}

struct HandVector {
	char p[7]; // rbgsklpの順
	void set(char x);
	void set(Piece pi, char x);
};

bool operator==(const HandVector& t1, const HandVector& t2);
bool operator!=(const HandVector& t1, const HandVector& t2);

// 盤面の詰みベクトルを表現するクラス

class MateVector
{
private:
	// それぞれBLACKとWHITE
	char origin[2]; // 現局面の値
	char beta[2]; // β世界線での値
	// 1次元目がどちらの手番用か2次元目がBLACK側の駒台かWHITE側の駒台かを表す
	HandVector plus_hv[2][2]; // 駒台の増加による値
	HandVector minus_hv[2][2]; // 駒台の減少による値

public:
	MateVector();
	~MateVector();

	char get_origin(Color c);

	void set_origin(Color c, char x);
	void set_beta(Color c, char x);
	void set_plus(Color c, Color hand, Piece pi, char x);
	void set_minus(Color c, Color hand, Piece pi, char x);

	// 出力する。デバッグ用。
	std::string out() const;
	friend std::ostream& operator<<(std::ostream& os, const MateVector& mv);
};

#endif _MATE_VECTOR_H_
