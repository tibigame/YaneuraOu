#include <iomanip>
#include "../../thread.h"
#include "../../shogi.h"
#include "random_board.h"
#include "int_board.h"
#include "ex_board.h"

// 盤面を出力する。デバッグ用。
std::ostream& operator<<(std::ostream& os, const PBoard& pb) {
	os << reverse(pb.board);
	return os;
};

#ifdef AVX512
// Arrayは+1でFILEが増える方、+9でRANKが増える方
PBoard::PBoard(const IntBoard2 init_board) {
	board = reverse(init_board); // 入力が逆転列なので元に戻す
	p_sum = __accumu(this->board, this->accum); // 累計加算を計算する
}
// IntBoardが立っていない部分を0にしてaccumを計算し直す
void PBoard::and(IntBoard2& int_board) {
	__and(this->board, int_board);
};
// IntBoardが立っている部分を0にしてaccumを計算し直す
void PBoard::ninp(IntBoard2& int_board) {
	__ninp(this->board, int_board);
};
#else
// IntBoardが立っていない部分を0にしてaccumを計算し直す
void PBoard::and(IntBoard& int_board) {
	__and(this->board, int_board);
};

// IntBoardが立っている部分を0にしてaccumを計算し直す
void PBoard::ninp(IntBoard& int_board) {
	__ninp(this->board, int_board);
};
#endif

PBoard::PBoard() {
}
PBoard::PBoard(const PBoard &p_board) {
	board = p_board.board;
	accum = p_board.accum;
}

// Arrayは+1でFILEが増える方、+9でRANKが増える方
PBoard::PBoard(const IntBoard init_board) {
	board = reverse(init_board); // 入力が逆転列なので元に戻す
	p_sum = __accumu(this->board, this->accum); // 累計加算を計算する
}

void PBoard::accumu() {
	p_sum = __accumu(this->board, this->accum);
};

int PBoard::accumu_rand() {
	return __accumu_rand(this->board, this->accum);
};

int PBoard::rand() {
	return __rand(this->board, this->accum);
};

// 空の盤面で初期化します
void set_blank(Position& pos_) {
	pos_.set_blank();
};

// Square型とBitboard型との行き来をなるべく少なくする。
// IntBoard型への変換はBitboard型からしかできないので、なるべく演算結果がBitboard型になるようにする。

// -----------------------------------
//     玉の配置確率を定義する
// -----------------------------------
const IntBoard b_king_p_intboard = {
	10, 10, 8, 6, 5, 6, 8, 10, 10,
	10, 10, 8, 6, 5, 6, 8, 10, 10,
	10, 10, 8, 6, 5, 6, 8, 10, 10,
	40, 32, 28, 10, 10, 10, 28, 32, 40,
	40, 32, 28, 10, 10, 10, 28, 32, 40,
	40, 32, 28, 10, 10, 10, 28, 32, 40,
	230, 150, 120, 40, 50, 40, 100, 150, 200,
	200, 820, 120, 350, 400, 300, 100, 250, 150,
	380, 150, 380, 140, 250, 140, 250, 150, 200
};
const PBoard b_king_p(b_king_p_intboard);
// 先手玉をpdで指定された確率で配置します(返り値は配置された場所のSquare)
Square set_b_king(Position& pos_) {
	PBoard pb = b_king_p;
	Square sq = sq_table[pb.rand()];
	pos_.put_piece(sq, B_KING);
	return sq;
}; 

const IntBoard w_king_p_intboard = reverse_123(b_king_p_intboard);
const PBoard w_king_p(w_king_p_intboard);
// 後手玉をpdで指定された確率で配置します(返り値は配置された場所のSquare)
Square set_w_king(Position& pos_, const Square& b_king) {
	PBoard pb2 = w_king_p;
#ifdef AVX512
	pb2.ninp(bitboard_to_intboard2(KingEffectBB[b_king] | b_king)); // 先手玉の9近傍を除く
#else
	pb2.ninp(bitboard_to_intboard(KingEffectBB[b_king] | b_king)); // 先手玉の9近傍を除く
#endif
	Square sq = sq_table[pb2.accumu_rand()];
	pos_.put_piece(sq, W_KING);
	pos_.update_kingSquare();
	return sq;
};

// -----------------------------------
//     飛車の配置確率を定義する
// -----------------------------------
// 基本の確率テーブル
const IntBoard b_rook_p_intboard = {
	40, 20, 10, 20, 80, 150, 100, 420, 380,
	10, 10, 10, 20, 50, 150, 100, 350, 250,
	10, 10, 10, 10, 40, 60, 70, 180, 80,
	5, 32, 28, 10, 10, 10, 28, 100, 5,
	5, 32, 28, 10, 10, 10, 28, 100, 5,
	10, 80, 120, 80, 160, 80, 160, 240, 10,
	1, 1, 120, 40, 50, 40, 100, 50, 2,
	2, 10, 50, 80, 110, 150, 100, 820, 250,
	5, 10, 10, 20, 20, 20, 30, 250, 100
};
// 玉が右の時の確率テーブル
const IntBoard b_rook_right_king_p_intboard = {
	380, 420, 100, 250, 150, 50, 10, 20, 40,
	250, 350, 100, 150, 100, 6, 10, 10, 10,
	80, 180, 70, 60, 40, 10, 10, 10, 10,
	10, 40, 50, 50, 50, 10, 10, 10, 5,
	10, 50, 50, 40, 50, 10, 10, 10, 5,
	10, 120, 250, 200, 200, 10, 10, 10, 5,
	5, 40, 40, 40, 50, 20, 10, 1, 2,
	100, 300, 300, 300, 350, 90, 80, 1, 5,
	10, 200, 120, 140, 250, 50, 25, 15, 20
};
// 元が相手の駒の時の確率テーブル
const IntBoard b_rook_captured_p_intboard = {
	100, 100, 100, 50, 50, 50, 100, 100, 100,
	100, 100, 100, 50, 50, 50, 100, 100, 100,
	40, 40, 40, 30, 30, 30, 40, 40, 40,
	10, 8, 8, 2, 2, 2, 8, 8, 10,
	5, 8, 8, 2, 2, 2, 8, 8, 5,
	1, 4, 4, 8, 16, 8, 4, 4, 1,
	2, 4, 4, 4, 16, 4, 4, 4, 2,
	2, 1, 5, 8, 12, 8, 5, 1, 2,
	5, 1, 1, 2, 4, 2, 1, 1, 5
};
// 成りの確率
constexpr PromoteP b_rook_promote_p = { 0.75, 0.8, 0.7, 0.2, 0.15, 0.1, 0.1, 0.1, 0.05 };
constexpr PromoteP w_rook_promote_p = reverse(b_rook_promote_p);

const PBoard b_rook_p(b_rook_p_intboard);
const PBoard b_rook_right_king_p(b_rook_right_king_p_intboard);
const PBoard b_rook_captured_p(b_rook_captured_p_intboard);
const IntBoard w_rook_p_intboard = reverse_123(b_rook_p_intboard);
const IntBoard w_rook_right_king_p_intboard = reverse_123(b_rook_right_king_p_intboard);
const IntBoard w_rook_captured_p_intboard = reverse_123(b_rook_captured_p_intboard);
const PBoard w_rook_p(w_rook_p_intboard);
const PBoard w_rook_right_king_p(w_rook_right_king_p_intboard);
const PBoard w_rook_captured_p(w_rook_captured_p_intboard);
// 飛車を配置するコア関数
// pb: 確率テーブル, e_king: 相手玉のSquare, e_king_bit: 相手玉のBitboard, occupied: 配置済みのBitboard
// set_piece: 配置する駒, set_piece_promote: 配置する成駒, promoto_p: 成り確率,
// checklist: 再チェックリスト, reason: 再チェックの理由
void set_rook_core(
	Position& pos_, PBoard &pb, const Square &e_king, const Bitboard &e_king_bit, Bitboard &occupied,
	const Piece &set_piece, const Piece &set_piece_promote, const PromoteP &promoto_p,
	CheckList &checklist, const RecheckReason &reason) {
#ifdef AVX512
	pb.ninp(bitboard_to_intboard2(cross00StepEffectBB[e_king] | occupied)); // 相手玉十字隣接と配置済みの位置を除く
#else
	pb.ninp(bitboard_to_intboard(cross00StepEffectBB[e_king] | occupied)); // 相手玉十字隣接と配置済みの位置を除く
#endif
	Square sq = sq_table[pb.accumu_rand()]; // 飛車の位置を確定させる
	occupied |= sq; // occupiedにorしていく
	pos_.put_piece(sq,
		is_promoted_rand(sq, promoto_p) // 成り判定
		&& !(cross45StepEffectBB[e_king] & sq) // 斜め隣接なら王手になるので除く
		? set_piece_promote : set_piece // 龍か飛車を配置する
	);
	if (RookStepEffectBB[sq] & e_king_bit) { // 相手玉が飛車の利きにあるか
		checklist.add(sq, reason); // 再チェックリストに入れる
	}
}

// 飛車を配置します
void set_rook(Position& pos_, const Square &b_king, const Square &w_king,
	const Bitboard &b_king_bit, const Bitboard &w_king_bit, Bitboard &occupied, CheckList &checklist) {
	PieceExistence b_rook_pos = piece_existence_rand(100, 60, 160, 110); // 先手の飛車だった駒を収束
	PieceExistence w_rook_pos = piece_existence_rand(60, 100, 110, 160); // 後手の飛車だった駒を収束
	PBoard pb; // set_rook_coreに渡すのはconstではないので作業用の変数
	// 先手の飛車だった駒を配置する
	switch (b_rook_pos) {
		// 手駒に配置する
		case PieceExistence::B_Hand: add_hand(pos_.hand[BLACK], ROOK); break;
		case PieceExistence::W_Hand: add_hand(pos_.hand[WHITE], ROOK); break;
		// 盤上の自分の駒として配置する
		case PieceExistence::B_Board: {
			pb = BitRight & b_king_bit ? b_rook_right_king_p : b_rook_p; // 先手玉が右かどうかで確率分岐
			set_rook_core(pos_, pb, w_king, w_king_bit, occupied, B_ROOK, B_DRAGON, b_rook_promote_p, checklist, RecheckReason::B_ROOK);
			break;
		}
		// 盤上の相手の駒として配置する
		case PieceExistence::W_Board: {
			pb = w_rook_captured_p;
			set_rook_core(pos_, pb, b_king, b_king_bit, occupied, W_ROOK, W_DRAGON, w_rook_promote_p, checklist, RecheckReason::W_ROOK);
			break;
		}
	}
	// 後手の飛車だった駒を配置する
	switch (w_rook_pos) {
		// 手駒に配置する
		case PieceExistence::B_Hand: add_hand(pos_.hand[BLACK], ROOK); break;
		case PieceExistence::W_Hand: add_hand(pos_.hand[WHITE], ROOK); break;
		// 盤上の自分の駒として配置する
		case PieceExistence::W_Board: {
			pb = BitLeft & w_king_bit ? w_rook_right_king_p : w_rook_p; // 後手玉が(後手から見て)右かどうかで確率分岐
			set_rook_core(pos_, pb, b_king, b_king_bit, occupied, W_ROOK, W_DRAGON, w_rook_promote_p, checklist, RecheckReason::W_ROOK);
			break;
		}
		// 盤上の相手の駒として配置する
		case PieceExistence::B_Board: {
			pb = b_rook_captured_p;
			set_rook_core(pos_, pb, w_king, w_king_bit, occupied, B_ROOK, B_DRAGON, b_rook_promote_p, checklist, RecheckReason::B_ROOK);
			break;
		}
	}
};

// -----------------------------------
//     角の配置確率を定義する
// -----------------------------------
// 基本の確率テーブル
const IntBoard b_bishop_p_intboard = {
	120, 80, 80, 80, 80, 80, 100, 120, 120,
	70, 110, 110, 120, 150, 150, 100, 250, 250,
	100, 110, 150, 100, 120, 120, 150, 110, 150,
	5, 120, 28, 220, 10, 220, 28, 120, 5,
	40, 20, 28, 10, 200, 10, 28, 20, 40,
	5, 50, 20, 180, 160, 180, 20, 50, 10,
	100, 10, 570, 40, 50, 40, 100, 10, 10,
	2, 620, 10, 80, 8, 40, 10, 80, 20,
	4, 1, 10, 2, 10, 20, 10, 5, 10
};
// 元が相手の駒の時の確率テーブル
const IntBoard b_bishop_captured_p_intboard = {
	70, 60, 100, 180, 150, 180, 100, 60, 70,
	60, 90, 100, 100, 180, 100, 100, 90, 60,
	80, 100, 100, 100, 120, 100, 100, 100, 80,
	15, 50, 80, 80, 100, 80, 80, 50, 15,
	15, 50, 80, 80, 100, 80, 80, 50, 15,
	15, 50, 80, 80, 100, 80, 80, 50, 15,
	5, 4, 4, 4, 16, 4, 4, 4, 2,
	10, 10, 20, 40, 20, 40, 20, 10, 10,
	4, 20, 20, 20, 20, 20, 20, 10, 4
};
// 成りの確率
constexpr PromoteP b_bishop_promote_p = { 0.2, 0.3, 0.3, 0.2, 0.18, 0.12, 0.12, 0.07, 0.03 };
constexpr PromoteP w_bishop_promote_p = reverse(b_bishop_promote_p);

const PBoard b_bishop_p(b_bishop_p_intboard);
const PBoard b_bishop_captured_p(b_bishop_captured_p_intboard);
const IntBoard w_bishop_p_intboard = reverse_123(b_bishop_p_intboard);
const IntBoard w_bishop_captured_p_intboard = reverse_123(b_bishop_captured_p_intboard);
const PBoard w_bishop_p(w_bishop_p_intboard);
const PBoard w_bishop_captured_p(w_bishop_captured_p_intboard);
// 角を配置するコア関数
// pb: 確率テーブル, e_king: 相手玉のSquare, e_king_bit: 相手玉のBitboard, occupied: 配置済みのBitboard
// set_piece: 配置する駒, set_piece_promote: 配置する成駒, promoto_p: 成り確率,
// checklist: 再チェックリスト, reason: 再チェックの理由
void set_bishop_core(
	Position& pos_, PBoard &pb, const Square &e_king, const Bitboard &e_king_bit, Bitboard &occupied,
	const Piece &set_piece, const Piece &set_piece_promote, const PromoteP &promoto_p,
	CheckList &checklist, const RecheckReason &reason) {
#ifdef AVX512
	pb.ninp(bitboard_to_intboard2(cross45StepEffectBB[e_king] | occupied)); // 相手玉斜め隣接と配置済みの位置を除く
#else
	pb.ninp(bitboard_to_intboard(cross45StepEffectBB[e_king] | occupied)); // 相手玉斜め隣接と配置済みの位置を除く
#endif
	Square sq = sq_table[pb.accumu_rand()]; // 角の位置を確定させる
	occupied |= sq; // occupiedにorしていく
	pos_.put_piece(sq,
		is_promoted_rand(sq, promoto_p) // 成り判定
		&& !(cross00StepEffectBB[e_king] & sq) // 十字隣接なら王手になるので除く
		? set_piece_promote : set_piece // 馬か角を配置する
	);
	if (BishopStepEffectBB[sq] & e_king_bit) { // 相手玉が角の利きにあるか
		checklist.add(sq, reason); // 再チェックリストに入れる
	}
}

// 角を配置します
void set_bishop(Position& pos_, const Square &b_king, const Square &w_king,
	const Bitboard &b_king_bit, const Bitboard &w_king_bit, Bitboard &occupied, CheckList &checklist) {
	PieceExistence b_bishop_pos = piece_existence_rand(60, 120, 100, 200); // 先手の角だった駒を収束
	PieceExistence w_bishop_pos = piece_existence_rand(120, 60, 200, 100); // 後手の角だった駒を収束
	PBoard pb; // set_bishop_coreに渡すのはconstではないので作業用の変数
	// 先手の角だった駒を配置する
	switch (b_bishop_pos) {
		// 手駒に配置する
		case PieceExistence::B_Hand: add_hand(pos_.hand[BLACK], BISHOP); break;
		case PieceExistence::W_Hand: add_hand(pos_.hand[WHITE], BISHOP); break;
		// 盤上の自分の駒として配置する
		case PieceExistence::B_Board: {
			pb = b_bishop_p;
			set_bishop_core(pos_, pb, w_king, w_king_bit, occupied, B_BISHOP, B_HORSE, b_bishop_promote_p, checklist, RecheckReason::B_BISHOP);
			break;
	}
		// 盤上の相手の駒として配置する
		case PieceExistence::W_Board: {
			pb = w_bishop_captured_p;
			set_bishop_core(pos_, pb, b_king, b_king_bit, occupied, W_BISHOP, W_HORSE, w_bishop_promote_p, checklist, RecheckReason::W_BISHOP);
			break;
		}
	}
	// 後手の角だった駒を配置する
	switch (w_bishop_pos) {
		// 手駒に配置する
		case PieceExistence::B_Hand: add_hand(pos_.hand[BLACK], BISHOP); break;
		case PieceExistence::W_Hand: add_hand(pos_.hand[WHITE], BISHOP); break;
		// 盤上の自分の駒として配置する
		case PieceExistence::W_Board: {
			pb = w_bishop_p;
			set_bishop_core(pos_, pb, b_king, b_king_bit, occupied, W_BISHOP, W_HORSE, w_bishop_promote_p, checklist, RecheckReason::W_BISHOP);
			break;
		}
		// 盤上の相手の駒として配置する
		case PieceExistence::B_Board: {
			pb = b_bishop_captured_p;
			set_bishop_core(pos_, pb, w_king, w_king_bit, occupied, B_BISHOP, B_HORSE, b_bishop_promote_p, checklist, RecheckReason::B_BISHOP);
			break;
		}
	}
};

// -----------------------------------
//     香の配置確率を定義する
// -----------------------------------
// 基本の確率テーブル
const IntBoard b_lance_l_p_intboard = {
	80,  90, 30, 10, 8, 7, 7, 7, 7,
	110, 80, 10, 8, 7, 7, 7, 7, 7,
	190, 70, 8, 7, 7, 7, 7, 7, 7,
	320, 8, 7, 6, 6, 6, 6, 6, 6,
	520, 7, 6, 6, 6, 6, 6, 6, 6,
	600, 6, 6, 6, 6, 6, 6, 6, 6,
	1200, 7, 7, 7, 7, 7, 7, 7, 7,
	4800, 10, 10, 10, 10, 10, 10, 10, 10,
	9600, 20, 20, 20, 20, 20, 20, 20, 1
};
const IntBoard b_lance_r_p_intboard = reverse(b_lance_l_p_intboard);
// 元が相手の駒の時の確率テーブル
const IntBoard b_lance_captured_p_intboard = {
	30, 30, 30, 30, 30, 30, 30, 30, 30,
	30, 30, 30, 30, 30, 30, 30, 30, 30,
	30, 30, 30, 30, 30, 30, 30, 30, 30,
	10, 10, 10, 10, 10, 10, 10, 10, 10,
	10, 10, 10, 10, 10, 10, 10, 10, 10,
	30, 30, 30, 30, 30, 30, 30, 30, 30,
	80, 80, 80, 80, 80, 80, 80, 80, 80,
	100, 100, 100, 100, 100, 100, 100, 100, 100,
	200, 200, 200, 200, 200, 200, 200, 200, 200
};
// 成りの確率
constexpr PromoteP b_lance_promote_p = { 1.0, 0.92, 0.8, 0.3, 0.05, 0.001, 0.0001, 0.00001, 0.000005 };
constexpr PromoteP w_lance_promote_p = reverse(b_lance_promote_p);

const PBoard b_lance_l_p(b_lance_l_p_intboard);
const PBoard b_lance_r_p(b_lance_r_p_intboard);
const PBoard b_lance_captured_p(b_lance_captured_p_intboard);
const IntBoard w_lance_l_p_intboard = reverse_123(b_lance_l_p_intboard);
const IntBoard w_lance_r_p_intboard = reverse(w_lance_l_p_intboard);
const IntBoard w_lance_captured_p_intboard = reverse_123(b_lance_captured_p_intboard);
const PBoard w_lance_l_p(w_lance_l_p_intboard);
const PBoard w_lance_r_p(w_lance_r_p_intboard);
const PBoard w_lance_captured_p(w_lance_captured_p_intboard);
// 香を配置するコア関数
// pb: 確率テーブル, e_king: 相手玉のSquare, e_king_bit: 相手玉のBitboard, occupied: 配置済みのBitboard
// set_piece: 配置する駒, set_piece_promote: 配置する成駒, promoto_p: 成り確率,
// checklist: 再チェックリスト, reason: 再チェックの理由,
// my_c: 自玉の手番, e_c: 相手玉の手番, confirm_promote: 確定成りになるBitboard (1行目か9行目)
void set_lance_core(
	Position& pos_, PBoard &pb, const Square &e_king, const Bitboard &e_king_bit, Bitboard &occupied,
	const Piece &set_piece, const Piece &set_piece_promote, const PromoteP &promoto_p,
	CheckList &checklist, const RecheckReason &reason,
	const Color my_c, const Color e_c, const Bitboard confirm_promote) {
#ifdef AVX512
	// 相手玉前と配置済みの位置と非合法な位置を除く
	pb.ninp(bitboard_to_intboard2(PawnEffectBB[e_king][e_c] | occupied | (cross00StepEffectBB[e_king] & confirm_promote)));
#else
	// 相手玉前と配置済みの位置と非合法な位置を除く
	pb.ninp(bitboard_to_intboard(PawnEffectBB[e_king][e_c] | occupied | (cross00StepEffectBB[e_king] & confirm_promote)));
#endif
	Square sq = sq_table[pb.accumu_rand()]; // 香の位置を確定させる
	occupied |= sq; // occupiedにorしていく
	const bool is_promote = is_promoted_rand(sq, promoto_p) // 成り判定
		&& !(e_king_bit & GoldEffectBB[sq][my_c]); // 金の利きに入るなら王手になるので除く
	pos_.put_piece(sq, is_promote ? set_piece_promote : set_piece // 成香か香を配置する
	);
	if (!is_promote && (LanceStepEffectBB[sq][my_c] & e_king_bit)) { // 相手玉が香の利きにあるか
		checklist.add(sq, reason); // 再チェックリストに入れる
	}
}

// 香を配置します
void set_lance(Position& pos_, const Square &b_king, const Square &w_king,
	const Bitboard &b_king_bit, const Bitboard &w_king_bit, Bitboard &occupied, CheckList &checklist) {
	PieceExistence b_lance_l_pos = piece_existence_rand(400, 30, 100, 100); // 先手の左香だった駒を収束
	PieceExistence b_lance_r_pos = piece_existence_rand(400, 30, 100, 100); // 先手の右香だった駒を収束
	PieceExistence w_lance_l_pos = piece_existence_rand(400, 30, 100, 100); // 後手の左香だった駒を収束
	PieceExistence w_lance_r_pos = piece_existence_rand(400, 30, 100, 100); // 後手の右香だった駒を収束
	PBoard pb; // set_bishop_coreに渡すのはconstではないので作業用の変数
	// 先手の左香だった駒を配置する
	switch (b_lance_l_pos) {
		// 手駒に配置する
		case PieceExistence::B_Hand: add_hand(pos_.hand[BLACK], LANCE); break;
		case PieceExistence::W_Hand: add_hand(pos_.hand[WHITE], LANCE); break;
		// 盤上の自分の駒として配置する
		case PieceExistence::B_Board: {
			pb = b_lance_l_p;
			set_lance_core(pos_, pb, w_king, w_king_bit, occupied, B_LANCE, B_PRO_LANCE, b_lance_promote_p, checklist, RecheckReason::B_LANCE,
				BLACK, WHITE, BitLancePromoteBlack);
			break;
		}
		// 盤上の相手の駒として配置する
		case PieceExistence::W_Board: {
			pb = w_lance_captured_p;
			set_lance_core(pos_, pb, b_king, b_king_bit, occupied, W_LANCE, W_PRO_LANCE, w_lance_promote_p, checklist, RecheckReason::W_LANCE,
				WHITE, BLACK, BitLancePromoteWhite);
			break;
		}
	}
	// 先手の右香だった駒を配置する
	switch (b_lance_r_pos) {
		// 手駒に配置する
		case PieceExistence::B_Hand: add_hand(pos_.hand[BLACK], LANCE); break;
		case PieceExistence::W_Hand: add_hand(pos_.hand[WHITE], LANCE); break;
		// 盤上の自分の駒として配置する
		case PieceExistence::B_Board: {
			pb = b_lance_r_p;
			set_lance_core(pos_, pb, w_king, w_king_bit, occupied, B_LANCE, B_PRO_LANCE, b_lance_promote_p, checklist, RecheckReason::B_LANCE,
				BLACK, WHITE, BitLancePromoteBlack);
			break;
		}
		// 盤上の相手の駒として配置する
		case PieceExistence::W_Board: {
			pb = w_lance_captured_p;
			set_lance_core(pos_, pb, b_king, b_king_bit, occupied, W_LANCE, W_PRO_LANCE, w_lance_promote_p, checklist, RecheckReason::W_LANCE,
				WHITE, BLACK, BitLancePromoteWhite);
			break;
		}
	}
	// 後手の左香だった駒を配置する
	switch (b_lance_l_pos) {
		// 手駒に配置する
		case PieceExistence::B_Hand: add_hand(pos_.hand[BLACK], LANCE); break;
		case PieceExistence::W_Hand: add_hand(pos_.hand[WHITE], LANCE); break;
		// 盤上の自分の駒として配置する
		case PieceExistence::B_Board: {
			pb = w_lance_l_p;
			set_lance_core(pos_, pb, b_king, b_king_bit, occupied, W_LANCE, W_PRO_LANCE, w_lance_promote_p, checklist, RecheckReason::W_LANCE,
				WHITE, BLACK, BitLancePromoteWhite);
			break;
		}
		// 盤上の相手の駒として配置する
		case PieceExistence::W_Board: {
			pb = b_lance_captured_p;
			set_lance_core(pos_, pb, w_king, w_king_bit, occupied, B_LANCE, B_PRO_LANCE, b_lance_promote_p, checklist, RecheckReason::B_LANCE,
				BLACK, WHITE, BitLancePromoteBlack);
			break;
		}
	}
	// 後手の右香だった駒を配置する
	switch (w_lance_r_pos) {
		// 手駒に配置する
		case PieceExistence::B_Hand: add_hand(pos_.hand[BLACK], LANCE); break;
		case PieceExistence::W_Hand: add_hand(pos_.hand[WHITE], LANCE); break;
		// 盤上の自分の駒として配置する
		case PieceExistence::B_Board: {
			pb = w_lance_r_p;
			set_lance_core(pos_, pb, b_king, b_king_bit, occupied, W_LANCE, W_PRO_LANCE, w_lance_promote_p, checklist, RecheckReason::W_LANCE,
				WHITE, BLACK, BitLancePromoteWhite);
			break;
		}
		// 盤上の相手の駒として配置する
		case PieceExistence::W_Board: {
			pb = b_lance_captured_p;
			set_lance_core(pos_, pb, w_king, w_king_bit, occupied, B_LANCE, B_PRO_LANCE, b_lance_promote_p, checklist, RecheckReason::B_LANCE,
				BLACK, WHITE, BitLancePromoteBlack);
			break;
		}
	}
};

// -----------------------------------
//     桂の配置確率を定義する
// -----------------------------------

// 桂を配置します
void set_knight(Position& pos_, const Square &b_king, const Square &w_king,
	const Bitboard &b_king_bit, const Bitboard &w_king_bit, Bitboard &occupied, CheckList &checklist) {
};

void end_game_mate(Position& pos_) {
	pos_.set_blank(); // 空の盤面で初期化する
	Square sq_b_king = set_b_king(pos_); // 先手玉の配置
	Square sq_w_king = set_w_king(pos_, sq_b_king); // 後手玉の配置
	Bitboard bit_b_king(sq_b_king); // 玉のBitboardはよく使うのでここで変換したものを以降使う
	Bitboard bit_w_king(sq_w_king); // 玉のBitboardはよく使うのでここで変換したものを以降使う
	Bitboard occupied = bit_b_king | bit_w_king; // 駒を配置するたびに記録するBitboard
	CheckList checklist; // 盤面再チェック用のリスト
	set_rook(pos_, sq_b_king, sq_w_king, bit_b_king, bit_w_king, occupied, checklist); // 飛車の配置
	set_bishop(pos_, sq_b_king, sq_w_king, bit_b_king, bit_w_king, occupied, checklist); // 角の配置
	set_lance(pos_, sq_b_king, sq_w_king, bit_b_king, bit_w_king, occupied, checklist); // 香の配置
	set_knight(pos_, sq_b_king, sq_w_king, bit_b_king, bit_w_king, occupied, checklist); // 桂の配置
	pos_.update_bitboards();
};

// -----------------------------------------------------------------
// ここからテスト用の関数
// -----------------------------------------------------------------

// sq列をBitBoardに変換してIntBoardに戻すテスト
void sq_test() {
	for (auto i = 0; i < 81; ++i) {
		std::cout << "index: " << i << std::endl;
		Square sq = sq_table[i];
		Bitboard bb(sq);
		std::cout << bitboard_to_intboard(bb) << std::endl;
	}
};
