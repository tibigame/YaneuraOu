#include "mate_vector.h"

bool operator==(const HandVector& t1, const HandVector& t2) {
	for (auto i = 0; i < 7; ++i) {
		if (t1.p[i] != t2.p[i]) {
			return false; // 1つでも異なればfalse
		}
	}
	return true;
}

bool operator!=(const HandVector& t1, const HandVector& t2) {
	return !(t1 == t2);
}

void HandVector::set(char x) {
	for (auto i = 0; i < 7; ++i) {
		p[i] = x;
	}
}

void HandVector::set(Piece pi, char x) {
	switch (pi) {
		case ROOK:{
			p[0] = x;
			return;
		}
		case BISHOP: {
			p[1] = x;
			return;
		}
		case GOLD: {
			p[2] = x;
			return;
		}
		case SILVER: {
			p[3] = x;
			return;
		}
		case KNIGHT: {
			p[4] = x;
			return;
		}
		case LANCE: {
			p[5] = x;
			return;
		}
		case PAWN: {
			p[6] = x;
			return;
		}
	}
}

MateVector::MateVector() {
	origin[0] = MATE_VECTOR_UNKOWN;
	origin[1] = MATE_VECTOR_UNKOWN;
	beta[0] = MATE_VECTOR_UNKOWN;
	beta[1] = MATE_VECTOR_UNKOWN;
	plus_hv[0][0].set(MATE_VECTOR_UNKOWN);
	plus_hv[0][1].set(MATE_VECTOR_UNKOWN);
	plus_hv[1][0].set(MATE_VECTOR_UNKOWN);
	plus_hv[1][1].set(MATE_VECTOR_UNKOWN);
	minus_hv[0][0].set(MATE_VECTOR_UNKOWN);
	minus_hv[0][1].set(MATE_VECTOR_UNKOWN);
	minus_hv[1][0].set(MATE_VECTOR_UNKOWN);
	minus_hv[1][1].set(MATE_VECTOR_UNKOWN);
}

MateVector::~MateVector() {
}

char MateVector::get_origin(Color c) {
	return origin[c];
}

void MateVector::set_origin(Color c, char x) {
	origin[c] = x; // まずoriginにセットする
	if (x == MATE_VECTOR_MATE) {
		beta[c] = MATE_VECTOR_MATE; // β世界線でも詰み
		plus_hv[c][c].set(MATE_VECTOR_MATE); // 自分の駒が増えても詰みは変わらない
		minus_hv[c][~c].set(MATE_VECTOR_MATE); // 相手の駒が減っても詰みは変わらない
	}
	else if (x == MATE_VECTOR_NMATE) {
		minus_hv[c][c].set(MATE_VECTOR_NMATE); // 自分の駒が減っても不詰みは変わらない
		plus_hv[c][~c].set(MATE_VECTOR_NMATE); // 相手の駒が増えても不詰みは変わらない
	}
}

void MateVector::set_beta(Color c, char x) {
	beta[c] = x;
}

void MateVector::set_plus(Color c, Color hand, Piece pi, char x) {
	plus_hv[c][hand].set(pi, x);
}

void MateVector::set_minus(Color c, Color hand, Piece pi, char x) {
	minus_hv[c][hand].set(pi, x);
}

std::string MateVector::out() const {
	std::string o;
	for (auto i = 0; i < 2; ++i) {
		o += mate_vector_string(origin[i]);
		o += mate_vector_string(beta[i]);
		o += u8"|";
		for (auto j = 0; j < 7; ++j) {
			o += mate_vector_string(plus_hv[i][0].p[j]);
		}
		o += u8"|";
		for (auto j = 0; j < 7; ++j) {
			o += mate_vector_string(minus_hv[i][0].p[j]);
		}
		o += u8"l";
		for (auto j = 0; j < 7; ++j) {
			o += mate_vector_string(plus_hv[i][1].p[j]);
		}
		o += u8"|";
		for (auto j = 0; j < 7; ++j) {
			o += mate_vector_string(minus_hv[i][1].p[j]);
		}
		o += u8"\n";
	}
	return o;
}

std::ostream& operator<<(std::ostream& os, const MateVector& mv) {
	os << mv.out();
	return os;
};
