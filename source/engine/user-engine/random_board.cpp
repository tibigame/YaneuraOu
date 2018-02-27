#include <iomanip>
#include "../../thread.h"
#include "../../shogi.h"
#include "random_board.h"
#include "int_board.h"
#include "pboard.h"
#include "ex_board.h"

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
	pb2.ninp(KingEffectBB[b_king] | b_king); // 先手玉の9近傍を除く
	Square sq = sq_table[pb2.accumu_rand()];
	pos_.put_piece(sq, W_KING);
	pos_.update_kingSquare();
	return sq;
};

bool null_bool; // temp用の場所 (ここに書いた情報は捨てられる)
// 駒を配置するコア関数 (配置に失敗するとfalseを返す)
// sq: 配置する駒の位置, occupied: 配置済みのBitboard
// set_piece: 配置する駒, set_piece_promote: 配置する成駒, promoto_p: 成り確率,
// not_check: 不成で王手にならない, not_check_promote: 成りで王手にならない, add_judge: 追加の判定 (二歩や2段目の桂など)
inline bool set_piece_core(
	Position& pos_, const Square sq, Bitboard &occupied,
	const Piece &set_piece, const Piece &set_piece_promote, const PromoteP &promoto_p,
	const bool not_check, const bool not_check_promote, const bool add_judge, bool &not_promote = null_bool) {
	if (is_promoted_rand(sq, promoto_p)) { // 成り→不成の順で判定
		if (not_check_promote) { // 成りで王手にならない
			pos_.put_piece(sq, set_piece_promote); // 成りを配置する
			occupied |= sq; // occupiedにorしていく
			not_promote = false;
			return true;
		}
		if (not_check && add_judge) { // 不成で王手にならない & 追加の判定 (二歩や2段目の桂など)
			pos_.put_piece(sq, set_piece); // 不成を配置する
			occupied |= sq; // occupiedにorしていく
			not_promote = true;
			return true;
		}
	}
	else { // 不成→成りの順で判定
		if (not_check && add_judge) { // 不成で王手にならない & 追加の判定 (二歩や2段目の桂など)
			pos_.put_piece(sq, set_piece); // 不成を配置する
			occupied |= sq; // occupiedにorしていく
			not_promote = true;
			return true;
		}
		if (not_check_promote) { // 成りで王手にならない
			pos_.put_piece(sq, set_piece_promote); // 成りを配置する
			occupied |= sq; // occupiedにorしていく
			not_promote = false;
			return true;
		}
	}
	return false; // 合法な位置に配置できなかった
}

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
// pb: 確率テーブル, e_king: 相手玉のSquare, e_king_bit: 相手玉のBitboard, e_c: 相手の手番, my_c: 自分の手番, occupied: 配置済みのBitboard
// set_piece: 配置する駒, set_piece_promote: 配置する成駒, promoto_p: 成り確率,
// checklist: 再チェックリスト
void set_rook_core(
	Position& pos_, PBoard &pb, const Square &e_king, const Bitboard &e_king_bit, Color e_c, Color my_c, Bitboard &occupied,
	const Piece &set_piece, const Piece &set_piece_promote, const PromoteP &promoto_p, CheckList &checklist) {
	pb.ninp(cross00StepEffectBB[e_king] | occupied); // 相手玉十字隣接と配置済みの位置を除く
	Square sq = sq_table[pb.accumu_rand()]; // 飛車の位置を確定させる
	set_piece_core(pos_, sq, occupied, set_piece, set_piece_promote, promoto_p, true, !(cross45StepEffectBB[e_king] & sq), true);
	/*
	occupied |= sq; // occupiedにorしていく
	pos_.put_piece(sq,
		is_promoted_rand(sq, promoto_p) // 成り判定
		&& !(cross45StepEffectBB[e_king] & sq) // 斜め隣接なら王手になるので除く
		? set_piece_promote : set_piece // 龍か飛車を配置する
	);
	*/
	if (RookStepEffectBB[sq] & e_king_bit) { // 相手玉が飛車の利きにあるか
		Bitboard lance_bitboard = LanceStepEffectBB[e_king][e_c] & LanceStepEffectBB[sq][my_c]; // 香車の位置
		// 再チェックリストに入れる
		if (lance_bitboard) {
			checklist.add(e_c, RecheckReason::LANCE, lance_bitboard); // 香と同一視する
		}
		else {
			checklist.add(e_c, RecheckReason::ROOK, between_bb(e_king, sq));
		}
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
			set_rook_core(pos_, pb, w_king, w_king_bit, WHITE, BLACK, occupied, B_ROOK, B_DRAGON, b_rook_promote_p, checklist);
			break;
		}
		// 盤上の相手の駒として配置する
		case PieceExistence::W_Board: {
			pb = w_rook_captured_p;
			set_rook_core(pos_, pb, b_king, b_king_bit, BLACK, WHITE, occupied, W_ROOK, W_DRAGON, w_rook_promote_p, checklist);
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
			set_rook_core(pos_, pb, b_king, b_king_bit, BLACK, WHITE, occupied, W_ROOK, W_DRAGON, w_rook_promote_p, checklist);
			break;
		}
		// 盤上の相手の駒として配置する
		case PieceExistence::B_Board: {
			pb = b_rook_captured_p;
			set_rook_core(pos_, pb, w_king, w_king_bit, WHITE, BLACK, occupied, B_ROOK, B_DRAGON, b_rook_promote_p, checklist);
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
// pb: 確率テーブル, e_king: 相手玉のSquare, e_king_bit: 相手玉のBitboard, e_c: 相手の手番, my_c: 自分の手番, occupied: 配置済みのBitboard
// set_piece: 配置する駒, set_piece_promote: 配置する成駒, promoto_p: 成り確率, checklist: 再チェックリスト
void set_bishop_core(
	Position& pos_, PBoard &pb, const Square &e_king, const Bitboard &e_king_bit, Color e_c, Color my_c, Bitboard &occupied,
	const Piece &set_piece, const Piece &set_piece_promote, const PromoteP &promoto_p, CheckList &checklist) {
	pb.ninp(cross45StepEffectBB[e_king] | occupied); // 相手玉斜め隣接と配置済みの位置を除く
	Square sq = sq_table[pb.accumu_rand()]; // 角の位置を確定させる
	set_piece_core(pos_, sq, occupied, set_piece, set_piece_promote, promoto_p, true, !(cross00StepEffectBB[e_king] & sq), true);
	/*
	occupied |= sq; // occupiedにorしていく
	pos_.put_piece(sq,
	is_promoted_rand(sq, promoto_p) // 成り判定
	&& !(cross00StepEffectBB[e_king] & sq) // 十字隣接なら王手になるので除く
	? set_piece_promote : set_piece // 馬か角を配置する
	);
	*/
	if (BishopStepEffectBB[sq] & e_king_bit) { // 相手玉が角の利きにあるか
		checklist.add(e_c, RecheckReason::BISHOP, between_bb(e_king, sq)); // 再チェックリストに入れる
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
			set_bishop_core(pos_, pb, w_king, w_king_bit, WHITE, BLACK, occupied, B_BISHOP, B_HORSE, b_bishop_promote_p, checklist);
			break;
	}
		// 盤上の相手の駒として配置する
		case PieceExistence::W_Board: {
			pb = w_bishop_captured_p;
			set_bishop_core(pos_, pb, b_king, b_king_bit, BLACK, WHITE, occupied, W_BISHOP, W_HORSE, w_bishop_promote_p, checklist);
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
			set_bishop_core(pos_, pb, b_king, b_king_bit, BLACK, WHITE, occupied, W_BISHOP, W_HORSE, w_bishop_promote_p, checklist);
			break;
		}
		// 盤上の相手の駒として配置する
		case PieceExistence::B_Board: {
			pb = b_bishop_captured_p;
			set_bishop_core(pos_, pb, w_king, w_king_bit, WHITE, BLACK, occupied, B_BISHOP, B_HORSE, b_bishop_promote_p, checklist);
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
	2000, 10, 10, 10, 10, 10, 10, 10, 10,
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
// checklist: 再チェックリスト, my_c: 自玉の手番, e_c: 相手玉の手番,
// confirm_promote: 確定成りになるBitboard (1行目か9行目)
void set_lance_core(
	Position& pos_, PBoard &pb, const Square &e_king, const Bitboard &e_king_bit, Bitboard &occupied,
	const Piece &set_piece, const Piece &set_piece_promote, const PromoteP &promoto_p,
	CheckList &checklist, const Color my_c, const Color e_c, const Bitboard confirm_promote) {
	// 相手玉前と配置済みの位置と非合法な位置を除く
	pb.ninp(PawnEffectBB[e_king][e_c] | occupied | (cross00StepEffectBB[e_king] & confirm_promote));
	Square sq = sq_table[pb.accumu_rand()]; // 香の位置を確定させる
	const bool not_promote = set_piece_core(pos_, sq, occupied, set_piece, set_piece_promote, promoto_p, true, !(e_king_bit & GoldEffectBB[sq][my_c]), true);
	/*
	occupied |= sq; // occupiedにorしていく
	const bool is_promote = is_promoted_rand(sq, promoto_p) // 成り判定
		&& !(e_king_bit & GoldEffectBB[sq][my_c]); // 金の利きに入るなら王手になるので除く
	pos_.put_piece(sq, is_promote ? set_piece_promote : set_piece // 成香か香を配置する
	);
	*/
	if (not_promote && (LanceStepEffectBB[sq][my_c] & e_king_bit)) { // 相手玉が香の利きにあるか
		checklist.add(my_c, RecheckReason::LANCE, LanceStepEffectBB[e_king][e_c] & LanceStepEffectBB[sq][my_c]);
	}
}

// 香を配置しますBitboard lance_bitboard = LanceStepEffectBB[e_king][e_c] & LanceStepEffectBB[sq][my_c]; // 香車の位置
void set_lance(Position& pos_, const Square &b_king, const Square &w_king,
	const Bitboard &b_king_bit, const Bitboard &w_king_bit, Bitboard &occupied, CheckList &checklist) {
	PieceExistence b_lance_l_pos = piece_existence_rand(400, 30, 100, 100); // 先手の左香だった駒を収束
	PieceExistence b_lance_r_pos = piece_existence_rand(400, 30, 100, 100); // 先手の右香だった駒を収束
	PieceExistence w_lance_l_pos = piece_existence_rand(400, 30, 100, 100); // 後手の左香だった駒を収束
	PieceExistence w_lance_r_pos = piece_existence_rand(400, 30, 100, 100); // 後手の右香だった駒を収束
	PBoard pb; // set_lance_coreに渡すのはconstではないので作業用の変数
	// 先手の左香だった駒を配置する
	switch (b_lance_l_pos) {
		// 手駒に配置する
		case PieceExistence::B_Hand: add_hand(pos_.hand[BLACK], LANCE); break;
		case PieceExistence::W_Hand: add_hand(pos_.hand[WHITE], LANCE); break;
		// 盤上の自分の駒として配置する
		case PieceExistence::B_Board: {
			pb = b_lance_l_p;
			set_lance_core(pos_, pb, w_king, w_king_bit, occupied, B_LANCE, B_PRO_LANCE, b_lance_promote_p, checklist,
				BLACK, WHITE, BitLancePromoteBlack);
			break;
		}
		// 盤上の相手の駒として配置する
		case PieceExistence::W_Board: {
			pb = w_lance_captured_p;
			set_lance_core(pos_, pb, b_king, b_king_bit, occupied, W_LANCE, W_PRO_LANCE, w_lance_promote_p, checklist,
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
			set_lance_core(pos_, pb, w_king, w_king_bit, occupied, B_LANCE, B_PRO_LANCE, b_lance_promote_p, checklist,
				BLACK, WHITE, BitLancePromoteBlack);
			break;
		}
		// 盤上の相手の駒として配置する
		case PieceExistence::W_Board: {
			pb = w_lance_captured_p;
			set_lance_core(pos_, pb, b_king, b_king_bit, occupied, W_LANCE, W_PRO_LANCE, w_lance_promote_p, checklist,
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
			set_lance_core(pos_, pb, b_king, b_king_bit, occupied, W_LANCE, W_PRO_LANCE, w_lance_promote_p, checklist,
				WHITE, BLACK, BitLancePromoteWhite);
			break;
		}
		// 盤上の相手の駒として配置する
		case PieceExistence::W_Board: {
			pb = b_lance_captured_p;
			set_lance_core(pos_, pb, w_king, w_king_bit, occupied, B_LANCE, B_PRO_LANCE, b_lance_promote_p, checklist,
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
			set_lance_core(pos_, pb, b_king, b_king_bit, occupied, W_LANCE, W_PRO_LANCE, w_lance_promote_p, checklist,
				WHITE, BLACK, BitLancePromoteWhite);
			break;
		}
		// 盤上の相手の駒として配置する
		case PieceExistence::W_Board: {
			pb = b_lance_captured_p;
			set_lance_core(pos_, pb, w_king, w_king_bit, occupied, B_LANCE, B_PRO_LANCE, b_lance_promote_p, checklist,
				BLACK, WHITE, BitLancePromoteBlack);
			break;
		}
	}
};

// -----------------------------------
//     桂の配置確率を定義する
// -----------------------------------
// 基本の確率テーブル
const IntBoard b_knight_l_p_intboard = {
	20,  40, 40, 40, 40, 20, 20, 10, 5,
	60, 80, 40, 40, 40, 20, 20, 10, 5,
	220, 80, 380, 120, 380, 120, 90, 70, 20,
	10, 60, 70, 60, 60, 60, 70, 60, 10,
	6, 800, 40, 600, 40, 6, 6, 6, 6,
	6, 6, 6, 6, 6, 6, 6, 6, 6,
	1200, 7, 4800, 7, 7, 7, 7, 7, 7,
	2, 6, 6, 10, 10, 10, 10, 2, 2,
	2, 9600, 15, 15, 15, 15, 15, 2, 2
};
const IntBoard b_knight_r_p_intboard = reverse(b_knight_l_p_intboard);
// 元が相手の駒の時の確率テーブル
const IntBoard b_knight_captured_p_intboard = {
	10, 25, 40, 40, 40, 40, 40, 25, 10,
	50, 80, 100, 120, 120, 120, 100, 80, 50,
	80, 150, 150, 250, 320, 250, 150, 150, 80,
	120, 220, 340, 380, 400, 380, 340, 220, 120,
	100, 250, 300, 300, 400, 300, 300, 250, 100,
	20, 80, 80, 80, 80, 80, 80, 80, 20,
	10, 10, 10, 10, 10, 10, 10, 10, 10,
	5, 10, 30, 30, 30, 30, 30, 10, 5,
	5, 40, 30, 30, 30, 30, 30, 40, 5
};
// 成りの確率
constexpr PromoteP b_knight_promote_p = { 1.0, 1.0, 0.8, 0.3, 0.05, 0.001, 0.0001, 0.00001, 0.000005 };
constexpr PromoteP w_knight_promote_p = reverse(b_knight_promote_p);

const PBoard b_knight_l_p(b_knight_l_p_intboard);
const PBoard b_knight_r_p(b_knight_r_p_intboard);
const PBoard b_knight_captured_p(b_knight_captured_p_intboard);
const IntBoard w_knight_l_p_intboard = reverse_123(b_knight_l_p_intboard);
const IntBoard w_knight_r_p_intboard = reverse(w_knight_l_p_intboard);
const IntBoard w_knight_captured_p_intboard = reverse_123(b_knight_captured_p_intboard);
const PBoard w_knight_l_p(w_knight_l_p_intboard);
const PBoard w_knight_r_p(w_knight_r_p_intboard);
const PBoard w_knight_captured_p(w_knight_captured_p_intboard);
// 桂を配置するコア関数
// pb: 確率テーブル, e_king: 相手玉のSquare, e_king_bit: 相手玉のBitboard, occupied: 配置済みのBitboard
// set_piece: 配置する駒, set_piece_promote: 配置する成駒, promoto_p: 成り確率,
// my_c: 自玉の手番, e_c: 相手玉の手番, confirm_promote: 確定成りになるBitboard (1行目か9行目)
void set_knight_core(
	Position& pos_, PBoard &pb, const Square &e_king, const Bitboard &e_king_bit, Bitboard &occupied,
	const Piece &set_piece, const Piece &set_piece_promote, const PromoteP &promoto_p,
	const Color my_c, const Color e_c, const Bitboard confirm_promote) {
	Bitboard exclude_knight = ( // ケイマの位置を除外するBitboard (後段で非合法を除くときに不自然にならないように)
		confirm_promote & e_king // 2段目までに相手玉がいるか
		) ? ZERO_BB : KnightEffectBB[e_king][e_c]; // 自分の桂が5段目以下で王手になる位置で成桂の確率は低いので……
	// 特定条件でのケイマの位置と配置済みの位置と非合法な位置を除く
	pb.ninp(exclude_knight | occupied | (GoldEffectBB[e_king][e_c] & confirm_promote));
	Square sq = sq_table[pb.accumu_rand()]; // 桂の位置を確定させる
	set_piece_core(pos_, sq, occupied, set_piece, set_piece_promote, promoto_p,
		!(KnightEffectBB[e_king][e_c] & sq), !(e_king_bit & GoldEffectBB[sq][my_c]), true);
	/*
	occupied |= sq; // occupiedにorしていく
	bool is_promote = is_promoted_rand(sq, promoto_p) // 成り判定
	&& !(e_king_bit & GoldEffectBB[sq][my_c]); // 金の利きに入るなら王手になるので除く
	if (e_king_bit & KnightEffectBB[sq][e_c]) { is_promote = true; } // 桂の利きに入るなら成桂にする
	pos_.put_piece(sq, is_promote ? set_piece_promote : set_piece); // 成桂か桂を配置する
	*/
}

// 桂を配置します
void set_knight(Position& pos_, const Square &b_king, const Square &w_king,
	const Bitboard &b_king_bit, const Bitboard &w_king_bit, Bitboard &occupied) {
	PieceExistence b_knight_l_pos = BitRight & b_king_bit ? 
		piece_existence_rand(50, 200, 200, 450) : piece_existence_rand(450, 50, 100, 100); // 先手の左桂だった駒を収束
	PieceExistence b_knight_r_pos = BitRight & b_king_bit ?
		piece_existence_rand(450, 50, 100, 100) : piece_existence_rand(50, 200, 200, 450); // 先手の右桂だった駒を収束
	PieceExistence w_knight_l_pos = BitLeft & w_king_bit ?
		piece_existence_rand(200, 50, 450, 200) : piece_existence_rand(50, 450, 100, 100); // 後手の左桂だった駒を収束
	PieceExistence w_knight_r_pos = BitLeft & w_king_bit ?
		piece_existence_rand(50, 450, 100, 100) : piece_existence_rand(200, 50, 450, 200); // 後手の右桂だった駒を収束
	PBoard pb; // set_knight_coreに渡すのはconstではないので作業用の変数
	// 先手の左桂だった駒を配置する
	switch (b_knight_l_pos) {
		// 手駒に配置する
		case PieceExistence::B_Hand: add_hand(pos_.hand[BLACK], KNIGHT); break;
		case PieceExistence::W_Hand: add_hand(pos_.hand[WHITE], KNIGHT); break;
		// 盤上の自分の駒として配置する
		case PieceExistence::B_Board: {
			pb = b_knight_l_p;
			set_knight_core(pos_, pb, w_king, w_king_bit, occupied, B_KNIGHT, B_PRO_KNIGHT, b_knight_promote_p,
				BLACK, WHITE, BitKnightPromoteBlack);
			break;
		}
		// 盤上の相手の駒として配置する
		case PieceExistence::W_Board: {
			pb = w_knight_captured_p;
			set_knight_core(pos_, pb, b_king, b_king_bit, occupied, W_KNIGHT, W_PRO_KNIGHT, w_knight_promote_p,
				WHITE, BLACK, BitKnightPromoteWhite);
			break;
		}
	}
	// 先手の右桂だった駒を配置する
	switch (b_knight_r_pos) {
		// 手駒に配置する
		case PieceExistence::B_Hand: add_hand(pos_.hand[BLACK], KNIGHT); break;
		case PieceExistence::W_Hand: add_hand(pos_.hand[WHITE], KNIGHT); break;
		// 盤上の自分の駒として配置する
		case PieceExistence::B_Board: {
			pb = b_knight_r_p;
			set_knight_core(pos_, pb, w_king, w_king_bit, occupied, B_KNIGHT, B_PRO_KNIGHT, b_knight_promote_p,
				BLACK, WHITE, BitKnightPromoteBlack);
			break;
		}
		// 盤上の相手の駒として配置する
		case PieceExistence::W_Board: {
			pb = w_knight_captured_p;
			set_knight_core(pos_, pb, b_king, b_king_bit, occupied, W_KNIGHT, W_PRO_KNIGHT, w_knight_promote_p,
				WHITE, BLACK, BitKnightPromoteWhite);
			break;
		}
	}
	// 後手の左桂だった駒を配置する
	switch (b_knight_l_pos) {
		// 手駒に配置する
		case PieceExistence::B_Hand: add_hand(pos_.hand[BLACK], KNIGHT); break;
		case PieceExistence::W_Hand: add_hand(pos_.hand[WHITE], KNIGHT); break;
		// 盤上の自分の駒として配置する
		case PieceExistence::B_Board: {
			pb = w_knight_l_p;
			set_knight_core(pos_, pb, b_king, b_king_bit, occupied, W_KNIGHT, W_PRO_KNIGHT, w_knight_promote_p,
				WHITE, BLACK, BitKnightPromoteWhite);
			break;
		}
		// 盤上の相手の駒として配置する
		case PieceExistence::W_Board: {
			pb = b_knight_captured_p;
			set_knight_core(pos_, pb, w_king, w_king_bit, occupied, B_KNIGHT, B_PRO_KNIGHT, b_knight_promote_p,
				BLACK, WHITE, BitKnightPromoteBlack);
			break;
		}
	}
	// 後手の右桂だった駒を配置する
	switch (w_knight_r_pos) {
		// 手駒に配置する
		case PieceExistence::B_Hand: add_hand(pos_.hand[BLACK], KNIGHT); break;
		case PieceExistence::W_Hand: add_hand(pos_.hand[WHITE], KNIGHT); break;
		// 盤上の自分の駒として配置する
		case PieceExistence::B_Board: {
			pb = w_knight_r_p;
			set_knight_core(pos_, pb, b_king, b_king_bit, occupied, W_KNIGHT, W_PRO_KNIGHT, w_knight_promote_p,
				WHITE, BLACK, BitKnightPromoteWhite);
			break;
		}
		// 盤上の相手の駒として配置する
		case PieceExistence::W_Board: {
			pb = b_knight_captured_p;
			set_knight_core(pos_, pb, w_king, w_king_bit, occupied, B_KNIGHT, B_PRO_KNIGHT, b_knight_promote_p,
				BLACK, WHITE, BitKnightPromoteBlack);
			break;
		}
	}
};

// -----------------------------------
//     銀の配置確率を定義する
// -----------------------------------
// 基本の確率テーブル
const IntBoard b_silver_l_p_intboard = {
	20, 50, 100, 100, 100, 100, 100, 50, 20,
	10, 50, 100, 100, 100, 100, 100, 50, 10,
	10, 50, 100, 100, 100, 100, 100, 50, 10,
	20, 50, 100, 100, 100, 100, 100, 50, 20,
	50, 100, 100, 150, 200, 150, 120, 50, 50,
	100, 320, 300, 280, 200, 100, 50, 40, 20,
	150, 400, 480, 380, 200, 35, 60, 15, 7,
	30, 450, 280, 200, 200, 20, 10, 10, 5,
	20, 120, 450, 120, 60, 6, 4, 4, 3
};
const IntBoard b_silver_r_p_intboard = {
	20, 50, 100, 100, 100, 100, 100, 50, 10,
	10, 50, 100, 100, 100, 100, 100, 50, 20,
	10, 50, 100, 100, 100, 100, 100, 50, 30,
	10, 40, 100, 100, 100, 100, 100, 50, 20,
	10, 20, 100, 150, 200, 150, 120, 70, 50,
	5, 10, 100, 180, 380, 150, 150, 80, 8,
	3, 80, 100, 50, 250, 280, 200, 150, 7,
	2, 50, 80, 150, 10, 350, 400, 80, 5,
	1, 5, 120, 50, 100, 1, 450, 1, 4
};
const IntBoard b_silver_l_king_r_p_intboard = {
	10, 20, 70, 100, 100, 100, 80, 50, 20,
	10, 20, 70, 100, 100, 100, 80, 50, 10,
	10, 40, 70, 100, 100, 100, 80, 50, 10,
	5, 50, 100, 100, 100, 100, 100, 50, 20,
	10, 100, 150, 150, 200, 140, 80, 30, 10,
	10, 150, 200, 280, 280, 100, 50, 20, 10,
	30, 100, 250, 380, 200, 120, 80, 60, 7,
	5, 80, 280, 200, 100, 120, 100, 80, 1,
	5, 5, 450, 10, 60, 10, 120, 1, 1
};
const IntBoard b_silver_r_king_r_p_intboard = {
	10, 20, 70, 100, 100, 100, 100, 80, 20,
	10, 20, 70, 100, 100, 100, 100, 80, 10,
	10, 40, 70, 100, 100, 100, 100, 80, 10,
	5, 50, 100, 100, 100, 100, 100, 100, 20,
	10, 100, 100, 120, 200, 140, 100, 100, 10,
	10, 40, 70, 100, 100, 120, 120, 160, 30,
	2, 10, 70, 100, 100, 100, 220, 320, 40,
	1, 5, 10, 80, 100, 230, 450, 450, 10,
	1, 2, 5, 10, 50, 10, 450, 30, 10
};
// 元が相手の駒の時の確率テーブル
const IntBoard b_silver_captured_p_intboard = {
	40, 100, 150, 320, 320, 320, 150, 100, 40,
	40, 100, 150, 320, 320, 320, 150, 100, 40,
	40, 100, 150, 320, 320, 320, 150, 100, 40,
	20, 80, 80, 80, 80, 80, 80, 80, 20,
	20, 80, 80, 80, 80, 80, 80, 80, 20,
	20, 80, 80, 80, 80, 80, 80, 80, 20,
	30, 50, 100, 80, 80, 80, 100, 50, 30,
	30, 50, 100, 100, 100, 100, 100, 50, 30,
	30, 50, 100, 100, 100, 100, 100, 50, 30,
};
// 成りの確率
constexpr PromoteP b_silver_promote_p = { 0.3, 0.5, 0.7, 0.3, 0.05, 0.002, 0.0002, 0.00001, 0.000005 };
constexpr PromoteP w_silver_promote_p = reverse(b_silver_promote_p);

const PBoard b_silver_l_p(b_silver_l_p_intboard);
const PBoard b_silver_r_p(b_silver_r_p_intboard);
const PBoard b_silver_l_king_r_p(b_silver_l_king_r_p_intboard);
const PBoard b_silver_r_king_r_p(b_silver_r_king_r_p_intboard);
const PBoard b_silver_captured_p(b_silver_captured_p_intboard);
const IntBoard w_silver_l_p_intboard = reverse_123(b_silver_l_p_intboard);
const IntBoard w_silver_r_p_intboard = reverse_123(b_silver_r_p_intboard);
const IntBoard w_silver_l_king_r_p_intboard = reverse_123(b_silver_r_king_r_p_intboard);
const IntBoard w_silver_r_king_r_p_intboard = reverse_123(b_silver_r_king_r_p_intboard);
const IntBoard w_silver_captured_p_intboard = reverse_123(b_silver_captured_p_intboard);
const PBoard w_silver_l_p(w_silver_l_p_intboard);
const PBoard w_silver_r_p(w_silver_r_p_intboard);
const PBoard w_silver_l_king_r_p(w_silver_l_king_r_p_intboard);
const PBoard w_silver_r_king_r_p(w_silver_r_king_r_p_intboard);
const PBoard w_silver_captured_p(w_silver_captured_p_intboard);
// 銀を配置するコア関数
// pb: 確率テーブル, e_king: 相手玉のSquare, e_king_bit: 相手玉のBitboard, occupied: 配置済みのBitboard
// set_piece: 配置する駒, set_piece_promote: 配置する成駒, promoto_p: 成り確率,
// my_c: 自玉の手番, e_c: 相手玉の手番
void set_silver_core(
	Position& pos_, PBoard &pb, const Square &e_king, const Bitboard &e_king_bit, Bitboard &occupied,
	const Piece &set_piece, const Piece &set_piece_promote, const PromoteP &promoto_p,
	const Color my_c, const Color e_c) {
	// 配置済みの位置と非合法な位置を除く
	pb.ninp(occupied | (GoldEffectBB[e_king][e_c] & SilverEffectBB[e_king][e_c]));
	Square sq = sq_table[pb.accumu_rand()]; // 銀の位置を確定させる
	set_piece_core(pos_, sq, occupied, set_piece, set_piece_promote, promoto_p,
		!(e_king_bit & SilverEffectBB[sq][my_c]), !(e_king_bit & GoldEffectBB[sq][my_c]), true);
	/*
	occupied |= sq; // occupiedにorしていく
	bool is_promote = is_promoted_rand(sq, promoto_p) // 成り判定
		&& !(e_king_bit & GoldEffectBB[sq][my_c]); // 金の利きに入るなら王手になるので除く
	if (e_king_bit & SilverEffectBB[sq][my_c]) { is_promote = true; } // 銀の利きに入るなら成銀にする
	pos_.put_piece(sq, is_promote ? set_piece_promote : set_piece); // 成銀か銀を配置する
	*/
}

// 銀を配置します
void set_silver(Position& pos_, const Square &b_king, const Square &w_king,
	const Bitboard &b_king_bit, const Bitboard &w_king_bit, Bitboard &occupied) {
	PieceExistence b_silver_l_pos = BitRight & b_king_bit ?
		piece_existence_rand(140, 200, 200, 250) : piece_existence_rand(450, 50, 100, 100); // 先手の左銀だった駒を収束
	PieceExistence b_silver_r_pos = BitRight & b_king_bit ?
		piece_existence_rand(450, 50, 100, 100) : piece_existence_rand(100, 100, 200, 250); // 先手の右銀だった駒を収束
	PieceExistence w_silver_l_pos = BitLeft & w_king_bit ?
		piece_existence_rand(200, 140, 250, 200) : piece_existence_rand(50, 450, 100, 100); // 後手の左銀だった駒を収束
	PieceExistence w_silver_r_pos = BitLeft & w_king_bit ?
		piece_existence_rand(50, 450, 100, 100) : piece_existence_rand(100, 100, 250, 100); // 後手の右銀だった駒を収束
	PBoard pb; // set_silver_coreに渡すのはconstではないので作業用の変数
	// 先手の左銀だった駒を配置する
	switch (b_silver_l_pos) {
		// 手駒に配置する
		case PieceExistence::B_Hand: add_hand(pos_.hand[BLACK], SILVER); break;
		case PieceExistence::W_Hand: add_hand(pos_.hand[WHITE], SILVER); break;
		// 盤上の自分の駒として配置する
		case PieceExistence::B_Board: {
			pb = BitRight & b_king_bit ? b_silver_l_king_r_p : b_silver_l_p;
			set_silver_core(pos_, pb, w_king, w_king_bit, occupied, B_SILVER, B_PRO_SILVER, b_silver_promote_p,
				BLACK, WHITE);
			break;
		}
		// 盤上の相手の駒として配置する
		case PieceExistence::W_Board: {
			pb = w_silver_captured_p;
			set_silver_core(pos_, pb, b_king, b_king_bit, occupied, W_SILVER, W_PRO_SILVER, w_silver_promote_p,
				WHITE, BLACK);
			break;
		}
	}
	// 先手の右銀だった駒を配置する
	switch (b_silver_r_pos) {
		// 手駒に配置する
		case PieceExistence::B_Hand: add_hand(pos_.hand[BLACK], SILVER); break;
		case PieceExistence::W_Hand: add_hand(pos_.hand[WHITE], SILVER); break;
		// 盤上の自分の駒として配置する
		case PieceExistence::B_Board: {
			pb = BitRight & b_king_bit ? b_silver_r_king_r_p : b_silver_r_p;
			set_silver_core(pos_, pb, w_king, w_king_bit, occupied, B_SILVER, B_PRO_SILVER, b_silver_promote_p,
				BLACK, WHITE);
			break;
		}
		// 盤上の相手の駒として配置する
		case PieceExistence::W_Board: {
			pb = w_silver_captured_p;
			set_silver_core(pos_, pb, b_king, b_king_bit, occupied, W_SILVER, W_PRO_SILVER, w_silver_promote_p,
				WHITE, BLACK);
			break;
		}
	}
	// 後手の左銀だった駒を配置する
	switch (b_silver_l_pos) {
		// 手駒に配置する
		case PieceExistence::B_Hand: add_hand(pos_.hand[BLACK], SILVER); break;
		case PieceExistence::W_Hand: add_hand(pos_.hand[WHITE], SILVER); break;
		// 盤上の自分の駒として配置する
		case PieceExistence::B_Board: {
			pb = BitLeft & w_king_bit ? w_silver_l_king_r_p : w_silver_l_p;
			set_silver_core(pos_, pb, b_king, b_king_bit, occupied, W_SILVER, W_PRO_SILVER, w_silver_promote_p,
				WHITE, BLACK);
			break;
		}
		// 盤上の相手の駒として配置する
		case PieceExistence::W_Board: {
			pb = b_silver_captured_p;
			set_silver_core(pos_, pb, w_king, w_king_bit, occupied, B_SILVER, B_PRO_SILVER, b_silver_promote_p,
				BLACK, WHITE);
			break;
		}
	}
	// 後手の右銀だった駒を配置する
	switch (w_silver_r_pos) {
		// 手駒に配置する
		case PieceExistence::B_Hand: add_hand(pos_.hand[BLACK], SILVER); break;
		case PieceExistence::W_Hand: add_hand(pos_.hand[WHITE], SILVER); break;
		// 盤上の自分の駒として配置する
		case PieceExistence::B_Board: {
			pb = BitLeft & w_king_bit ? w_silver_r_king_r_p : w_silver_r_p;
			set_silver_core(pos_, pb, b_king, b_king_bit, occupied, W_SILVER, W_PRO_SILVER, w_silver_promote_p,
				WHITE, BLACK);
			break;
		}
		// 盤上の相手の駒として配置する
		case PieceExistence::W_Board: {
			pb = b_silver_captured_p;
			set_silver_core(pos_, pb, w_king, w_king_bit, occupied, B_SILVER, B_PRO_SILVER, b_silver_promote_p,
				BLACK, WHITE);
			break;
		}
	}
}

// -----------------------------------
//    金の配置確率を定義する
// -----------------------------------
// 基本の確率テーブル
const IntBoard b_gold_l_p_intboard = {
	5, 20, 70, 80, 80, 80, 70, 50, 5,
	10, 50, 150, 150, 150, 150, 150, 50, 10,
	10, 50, 150, 150, 150, 150, 150, 50, 10,
	20, 50, 150, 150, 150, 150, 150, 50, 10,
	50, 100, 100, 150, 200, 150, 120, 50, 50,
	80, 150, 300, 280, 200, 100, 50, 40, 20,
	80, 180, 320, 400, 200, 35, 60, 15, 7,
	30, 300, 450, 200, 200, 20, 10, 10, 5,
	20, 100, 300, 450, 100, 6, 4, 4, 3
};
const IntBoard b_gold_r_p_intboard = {
	5, 20, 70, 80, 80, 80, 70, 50, 5,
	10, 50, 150, 150, 150, 150, 150, 50, 10,
	10, 50, 150, 150, 150, 150, 150, 50, 10,
	10, 50, 150, 150, 150, 150, 150, 50, 20,
	10, 20, 100, 150, 200, 150, 120, 70, 50,
	5, 10, 100, 180, 380, 150, 150, 80, 8,
	3, 80, 100, 280, 250, 280, 200, 80, 7,
	2, 50, 80, 200, 450, 350, 400, 80, 5,
	1, 5, 120, 100, 100, 450, 80, 20, 4
};
const IntBoard b_gold_l_king_r_p_intboard = {
	5, 10, 50, 70, 80, 80, 70, 10, 5,
	10, 20, 150, 150, 150, 150, 150, 50, 10,
	10, 40, 150, 150, 150, 150, 150, 50, 10,
	5, 50, 150, 150, 150, 150, 150, 50, 20,
	10, 50, 150, 150, 200, 140, 80, 30, 10,
	10, 50, 150, 280, 280, 100, 50, 20, 10,
	30, 40, 180, 380, 200, 120, 80, 60, 7,
	5, 60, 320, 200, 300, 180, 120, 80, 1,
	1, 5, 100, 450, 200, 180, 120, 5, 1
};
const IntBoard b_gold_r_king_r_p_intboard = {
	5, 10, 50, 70, 80, 80, 70, 10, 5,
	10, 20, 70, 150, 150, 150, 80, 50, 10,
	10, 40, 70, 150, 150, 150, 80, 50, 10,
	5, 50, 100, 100, 100, 100, 100, 100, 20,
	10, 100, 100, 120, 200, 140, 100, 100, 10,
	10, 40, 70, 100, 100, 120, 120, 160, 30,
	2, 10, 70, 100, 100, 100, 220, 320, 40,
	1, 5, 10, 80, 100, 230, 450, 450, 10,
	1, 2, 5, 10, 100, 450, 300, 30, 10
};
// 元が相手の駒の時の確率テーブル
const IntBoard b_gold_captured_p_intboard = {
	5, 30, 50, 60, 70, 60, 50, 30, 5,
	10, 80, 150, 150, 150, 150, 150, 80, 10,
	10, 100, 150, 200, 200, 200, 150, 100, 10,
	10, 100, 150, 200, 200, 200, 150, 100, 10,
	10,100, 150, 200, 200, 200, 150, 100, 10,
	10, 80, 80, 80, 80, 80, 80, 80, 10,
	10, 50, 100, 80, 80, 80, 100, 50, 10,
	10, 50, 100, 100, 100, 100, 100, 50, 10,
	5, 20, 100, 100, 100, 100, 100, 20, 5,
};
constexpr PromoteP b_gold_promote_p = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
constexpr PromoteP w_gold_promote_p = reverse(b_gold_promote_p);

const PBoard b_gold_l_p(b_gold_l_p_intboard);
const PBoard b_gold_r_p(b_gold_r_p_intboard);
const PBoard b_gold_l_king_r_p(b_gold_l_king_r_p_intboard);
const PBoard b_gold_r_king_r_p(b_gold_r_king_r_p_intboard);
const PBoard b_gold_captured_p(b_gold_captured_p_intboard);
const IntBoard w_gold_l_p_intboard = reverse_123(b_gold_l_p_intboard);
const IntBoard w_gold_r_p_intboard = reverse_123(b_gold_r_p_intboard);
const IntBoard w_gold_l_king_r_p_intboard = reverse_123(b_gold_r_king_r_p_intboard);
const IntBoard w_gold_r_king_r_p_intboard = reverse_123(b_gold_r_king_r_p_intboard);
const IntBoard w_gold_captured_p_intboard = reverse_123(b_gold_captured_p_intboard);
const PBoard w_gold_l_p(w_gold_l_p_intboard);
const PBoard w_gold_r_p(w_gold_r_p_intboard);
const PBoard w_gold_l_king_r_p(w_gold_l_king_r_p_intboard);
const PBoard w_gold_r_king_r_p(w_gold_r_king_r_p_intboard);
const PBoard w_gold_captured_p(w_gold_captured_p_intboard);
// 金を配置するコア関数
// pb: 確率テーブル, e_king: 相手玉のSquare, e_king_bit: 相手玉のBitboard, occupied: 配置済みのBitboard
// set_piece: 配置する駒, my_c: 自玉の手番, e_c: 相手玉の手番
void set_gold_core(
	Position& pos_, PBoard &pb, const Square &e_king, const Bitboard &e_king_bit, Bitboard &occupied,
	const Piece &set_piece, const Color my_c, const Color e_c) {
	pb.ninp(occupied | GoldEffectBB[e_king][e_c]); // 配置済みの位置と非合法な位置を除く
	Square sq = sq_table[pb.accumu_rand()]; // 金の位置を確定させる
	occupied |= sq; // occupiedにorしていく
	pos_.put_piece(sq, set_piece);
}

// 金を配置します
void set_gold(Position& pos_, const Square &b_king, const Square &w_king,
	const Bitboard &b_king_bit, const Bitboard &w_king_bit, Bitboard &occupied) {
	PieceExistence b_gold_l_pos = BitRight & b_king_bit ?
		piece_existence_rand(40, 100, 200, 450) : piece_existence_rand(450, 50, 80, 80); // 先手の左金だった駒を収束
	PieceExistence b_gold_r_pos = BitRight & b_king_bit ?
		piece_existence_rand(450, 50, 100, 100) : piece_existence_rand(100, 60, 100, 150); // 先手の右金だった駒を収束
	PieceExistence w_gold_l_pos = BitLeft & w_king_bit ?
		piece_existence_rand(100, 40, 450, 200) : piece_existence_rand(50, 450, 80, 80); // 後手の左金だった駒を収束
	PieceExistence w_gold_r_pos = BitLeft & w_king_bit ?
		piece_existence_rand(50, 450, 100, 100) : piece_existence_rand(60, 100, 150, 100); // 後手の右金だった駒を収束
	PBoard pb; // set_gold_coreに渡すのはconstではないので作業用の変数
	// 先手の左金だった駒を配置する
	switch (b_gold_l_pos) {
		// 手駒に配置する
		case PieceExistence::B_Hand: add_hand(pos_.hand[BLACK], GOLD); break;
		case PieceExistence::W_Hand: add_hand(pos_.hand[WHITE], GOLD); break;
		// 盤上の自分の駒として配置する
		case PieceExistence::B_Board: {
			pb = BitRight & b_king_bit ? b_gold_l_king_r_p : b_gold_l_p;
			set_gold_core(pos_, pb, w_king, w_king_bit, occupied, B_GOLD, BLACK, WHITE);
			break;
		}
		// 盤上の相手の駒として配置する
		case PieceExistence::W_Board: {
			pb = w_gold_captured_p;
			set_gold_core(pos_, pb, b_king, b_king_bit, occupied, W_GOLD, WHITE, BLACK);
			break;
		}
	}
	// 先手の右金だった駒を配置する
	switch (b_gold_r_pos) {
		// 手駒に配置する
		case PieceExistence::B_Hand: add_hand(pos_.hand[BLACK], GOLD); break;
		case PieceExistence::W_Hand: add_hand(pos_.hand[WHITE], GOLD); break;
		// 盤上の自分の駒として配置する
		case PieceExistence::B_Board: {
			pb = BitRight & b_king_bit ? b_gold_r_king_r_p : b_gold_r_p;
			set_gold_core(pos_, pb, w_king, w_king_bit, occupied, B_GOLD, BLACK, WHITE);
			break;
		}
		// 盤上の相手の駒として配置する
		case PieceExistence::W_Board: {
			pb = w_gold_captured_p;
			set_gold_core(pos_, pb, b_king, b_king_bit, occupied, W_GOLD, WHITE, BLACK);
			break;
		}
	}
	// 後手の左金だった駒を配置する
	switch (b_gold_l_pos) {
		// 手駒に配置する
		case PieceExistence::B_Hand: add_hand(pos_.hand[BLACK], GOLD); break;
		case PieceExistence::W_Hand: add_hand(pos_.hand[WHITE], GOLD); break;
		// 盤上の自分の駒として配置する
		case PieceExistence::B_Board: {
			pb = BitLeft & w_king_bit ? w_gold_l_king_r_p : w_gold_l_p;
			set_gold_core(pos_, pb, b_king, b_king_bit, occupied, W_GOLD, WHITE, BLACK);
			break;
		}
		// 盤上の相手の駒として配置する
		case PieceExistence::W_Board: {
			pb = b_gold_captured_p;
			set_gold_core(pos_, pb, w_king, w_king_bit, occupied, B_GOLD, BLACK, WHITE);
			break;
		}
	}
	// 後手の右金だった駒を配置する
	switch (w_gold_r_pos) {
		// 手駒に配置する
		case PieceExistence::B_Hand: add_hand(pos_.hand[BLACK], GOLD); break;
		case PieceExistence::W_Hand: add_hand(pos_.hand[WHITE], GOLD); break;
		// 盤上の自分の駒として配置する
		case PieceExistence::B_Board: {
			pb = BitLeft & w_king_bit ? w_gold_r_king_r_p : w_gold_r_p;
			set_gold_core(pos_, pb, b_king, b_king_bit, occupied, W_GOLD, WHITE, BLACK);
			break;
		}
		// 盤上の相手の駒として配置する
		case PieceExistence::W_Board: {
			pb = b_gold_captured_p;
			set_gold_core(pos_, pb, w_king, w_king_bit, occupied, B_GOLD, BLACK, WHITE);
			break;
		}
	}
}

// -----------------------------------
//     歩の配置確率を定義する
// -----------------------------------
// 基本の確率テーブル
const IntBoard b_pawn_p_intboard = {
	200, 200, 200, 200, 200, 200, 200, 200, 200,
	300, 300, 200, 200, 200, 200, 200, 300, 300,
	600, 500, 400, 400, 400, 400, 400, 500, 600,
	800, 600, 600, 600, 600, 600, 600, 600, 800,
	800, 800, 800, 800, 800, 800, 800, 800, 800,
	800, 800, 1000, 800, 800, 800, 800, 1000, 800,
	800, 1000, 400, 1000, 1000, 1000, 1000, 500, 800,
	30, 30, 30, 30, 30, 30, 30, 30, 30,
	30, 30, 30, 30, 30, 30, 30, 30, 30
};
// 必ずと金が配置される時の確率テーブル
const IntBoard b_pawn_pro_p_intboard = {
	3600, 3600, 3600, 3600, 3600, 3600, 3600, 3600, 3600,
	2800, 2800, 2800, 2800, 2800, 2800, 2800, 2800, 2800,
	2200, 2200, 2200, 2200, 2200, 2200, 2200, 2200, 2200,
	1600, 1600, 1600, 1600, 1600, 1600, 1600, 1600, 1600,
	400, 400, 400, 400, 400, 400, 400, 400, 400,
	100, 100, 100, 100, 100, 100, 100, 100, 100,
	25, 25, 25, 25, 25, 25, 25, 25, 25,
	10, 10, 10, 10, 10, 10, 10, 10, 10,
	1, 1, 1, 1, 1, 1, 1, 1, 1
};
const IntBoard w_pawn_p_intboard = reverse(b_pawn_p_intboard);
const IntBoard w_pawn_pro_p_intboard = reverse(b_pawn_pro_p_intboard);
const PBoard b_pawn_p(b_pawn_p_intboard);
const PBoard w_pawn_p(w_pawn_p_intboard);
const PBoard b_pawn_pro_p(b_pawn_pro_p_intboard);
const PBoard w_pawn_pro_p(w_pawn_pro_p_intboard);

// 成りの確率
constexpr PromoteP b_pawn_promote_p = { 1.0, 0.9, 0.7, 0.3, 0.02, 0.001, 0.0003, 0.0001, 0.00003 };
constexpr PromoteP w_pawn_promote_p = reverse(b_pawn_promote_p);
u64 cnt = 0;

// コクのある歩の乱数
void pawn_distribution(int &b_pawn, int &w_pawn, int &b_board, int &w_board) {
	int tolal_hands_pawns = 0;
	for (auto i = 0; i < 18; ++i) { // 持ち歩の合計を二項分布で求める
		if (myrand.rand_b(0.38)) { ++tolal_hands_pawns; }
	}
	if (myrand.rand_b(0.5)) { // 先後どちらが持ち歩が多いかの判定
		for (auto i = 0; i < tolal_hands_pawns; ++i) { // 多い方が確率多めの二項分布
			if (myrand.rand_b(0.67)) { ++b_pawn; }
		}
		w_pawn = tolal_hands_pawns - b_pawn;
	}
	else {
		for (auto i = 0; i < tolal_hands_pawns; ++i) {
			if (myrand.rand_b(0.67)) { ++w_pawn; }
		}
		b_pawn = tolal_hands_pawns - w_pawn;
	}
	int tolal_board_pawns = 18 - tolal_hands_pawns;
	for (auto i = 0; i < tolal_board_pawns; ++i) { // 盤上の歩の二項分布
		if (myrand.rand_b(0.5)) { ++b_board; }
	}
	w_board = tolal_board_pawns - b_board;
	// std::cout << "b_board: " << b_board << ", w_board: " << w_board << ", b_pawn: " << b_pawn << ", w_pawn:" << w_pawn << std::endl;
};

// 適当に合い駒を発生させて王手を回避する (先手が王手をかけられている)
// pos_: 盤面, sq: 駒を打つ場所,
// b_king_bit: 先手玉の位置, w_king_bit: 後手玉の位置, occupied: 既に置かれているマス
// b_enable_set_pawn: 先手の歩の置けるFILE, w_enable_set_pawn: 後手の歩の置けるFILE
bool _aigoma_b(Position& pos_, const Square sq,
	const Bitboard &b_king_bit, const Bitboard &w_king_bit, Bitboard &occupied, int &b_enable_set_pawn, int &w_enable_set_pawn) {
	bool not_double_pawn_b = (b_enable_set_pawn & (1 << file_index_table[sq])) && hand_exists(pos_.hand[0], PAWN); // 二歩でない+歩を持っている
	bool not_double_pawn_w = (w_enable_set_pawn & (1 << file_index_table[sq])) && hand_exists(pos_.hand[1], PAWN); // 二歩でない+歩を持っている
	bool not_promote;
	// 一定確率で歩の合い駒を発生させる
	if (not_double_pawn_b && myrand.rand_b(0.8)) { // 二歩判定を上段で入れる
		if (set_piece_core(pos_, sq, occupied, B_PAWN, B_PRO_PAWN, b_pawn_promote_p,
			!(w_king_bit & PawnEffectBB[sq][BLACK]), !(w_king_bit & GoldEffectBB[sq][BLACK]), (~BitLancePromoteBlack) & sq, not_promote)){
			if (not_promote) {
				b_enable_set_pawn &= ~(1 << file_index_table[sq]); // 歩の配置フラグを立てる
			}
			sub_hand(pos_.hand[0], PAWN);
			return true;
		}
	};
	if (not_double_pawn_w && myrand.rand_b(0.4)) { // 二歩判定を上段で入れる
		if (set_piece_core(pos_, sq, occupied, W_PAWN, W_PRO_PAWN, w_pawn_promote_p,
			!(b_king_bit & PawnEffectBB[sq][WHITE]), !(b_king_bit & GoldEffectBB[sq][WHITE]), (~BitLancePromoteWhite) & sq, not_promote)) {
			if (not_promote) {
				w_enable_set_pawn &= ~(1 << file_index_table[sq]); // 歩の配置フラグを立てる
			}
			sub_hand(pos_.hand[1], PAWN);
			return true;
		}
	};
	double select_p = not_double_pawn_b ? 0.4 : 0.8; // 先手が歩を打てない条件なら配置確率を上げる
	// 香、桂、銀、金の順で合い駒を試みる
	if (hand_exists(pos_.hand[0], LANCE) && myrand.rand_b(select_p)) {
		if (set_piece_core(pos_, sq, occupied, B_LANCE, B_PRO_LANCE, b_lance_promote_p,
			!(lanceEffect(BLACK, sq, occupied) & w_king_bit), !(w_king_bit & GoldEffectBB[sq][BLACK]), (~BitLancePromoteBlack) & sq)) {
			sub_hand(pos_.hand[0], LANCE);
			return true;
		}
	};
	if (hand_exists(pos_.hand[1], LANCE) && myrand.rand_b(select_p)) {
		if (set_piece_core(pos_, sq, occupied, W_LANCE, W_PRO_LANCE, w_lance_promote_p,
			!(lanceEffect(WHITE, sq, occupied) & b_king_bit), !(b_king_bit & GoldEffectBB[sq][WHITE]), (~BitLancePromoteWhite) & sq)) {
			sub_hand(pos_.hand[1], LANCE);
			return true;
		}
	};
	if (hand_exists(pos_.hand[0], KNIGHT) && myrand.rand_b(select_p)) {
		if (set_piece_core(pos_, sq, occupied, B_KNIGHT, B_PRO_KNIGHT, b_knight_promote_p,
			!(KnightEffectBB[w_king_bit][WHITE] & sq), !(w_king_bit & GoldEffectBB[sq][BLACK]), (~BitKnightPromoteBlack) & sq)) {
			sub_hand(pos_.hand[0], KNIGHT);
			return true;
		}
	};
	if (hand_exists(pos_.hand[1], KNIGHT) && myrand.rand_b(select_p)) {
		if (set_piece_core(pos_, sq, occupied, W_KNIGHT, W_PRO_KNIGHT, w_knight_promote_p,
			!(KnightEffectBB[b_king_bit][BLACK] & sq), !(b_king_bit & GoldEffectBB[sq][WHITE]), (~BitKnightPromoteWhite) & sq)) {
			sub_hand(pos_.hand[1], KNIGHT);
			return true;
		}
	};
	if (hand_exists(pos_.hand[0], SILVER) && myrand.rand_b(select_p)) {
		if (set_piece_core(pos_, sq, occupied, B_SILVER, B_PRO_SILVER, b_silver_promote_p,
			!(w_king_bit & SilverEffectBB[sq][BLACK]), !(w_king_bit & GoldEffectBB[sq][BLACK]), true)) {
			sub_hand(pos_.hand[0], SILVER);
			return true;
		}
	};
	if (hand_exists(pos_.hand[1], SILVER) && myrand.rand_b(select_p)) {
		if (set_piece_core(pos_, sq, occupied, W_SILVER, W_PRO_SILVER, w_silver_promote_p,
			!(b_king_bit & SilverEffectBB[sq][WHITE]), !(b_king_bit & GoldEffectBB[sq][WHITE]), true)) {
			sub_hand(pos_.hand[1], SILVER);
			return true;
		}
	};
	if (hand_exists(pos_.hand[0], GOLD) && myrand.rand_b(select_p)) {
		if (set_piece_core(pos_, sq, occupied, B_GOLD, B_GOLD, b_gold_promote_p,
			!(w_king_bit & GoldEffectBB[sq][BLACK]), true, true)) {
			sub_hand(pos_.hand[0], GOLD);
			return true;
		}
	};
	if (hand_exists(pos_.hand[1], GOLD) && myrand.rand_b(select_p)) {
		if (set_piece_core(pos_, sq, occupied, W_GOLD, W_GOLD, w_gold_promote_p,
			!(b_king_bit & GoldEffectBB[sq][WHITE]), true, true)) {
			sub_hand(pos_.hand[1], GOLD);
			return true;
		}
	};
	// 最後に先手の歩を強制的に採用する
	if (not_double_pawn_b && myrand.rand_b(1.0)) {
		if (set_piece_core(pos_, sq, occupied, B_PAWN, B_PRO_PAWN, b_pawn_promote_p,
			!(w_king_bit & PawnEffectBB[sq][BLACK]), !(w_king_bit & GoldEffectBB[sq][BLACK]), (~BitLancePromoteBlack) & sq, not_promote)) {
			if (not_promote) {
				b_enable_set_pawn &= ~(1 << file_index_table[sq]); // 歩の配置フラグを立てる
			}
			sub_hand(pos_.hand[0], PAWN);
			return true;
		}
	};
	return false;
};

// 適当に合い駒を発生させて王手を回避する (後手が王手をかけられている)
// pos_: 盤面, sq: 駒を打つ場所,
// b_king_bit: 先手玉の位置, w_king_bit: 後手玉の位置, occupied: 既に置かれているマス
// b_enable_set_pawn: 先手の歩の置けるFILE, w_enable_set_pawn: 後手の歩の置けるFILE
bool _aigoma_w(Position& pos_, const Square sq,
	const Bitboard &b_king_bit, const Bitboard &w_king_bit, Bitboard &occupied, int &b_enable_set_pawn, int &w_enable_set_pawn) {
	bool not_double_pawn_b = (b_enable_set_pawn & (1 << file_index_table[sq])) && hand_exists(pos_.hand[0], PAWN); // 二歩でない+歩を持っている
	bool not_double_pawn_w = (w_enable_set_pawn & (1 << file_index_table[sq])) && hand_exists(pos_.hand[1], PAWN); // 二歩でない+歩を持っている

	bool not_promote;
	// 一定確率で歩の合い駒を発生させる
	if (not_double_pawn_w && myrand.rand_b(0.8)) { // 二歩判定を上段で入れる
		if (set_piece_core(pos_, sq, occupied, W_PAWN, W_PRO_PAWN, w_pawn_promote_p,
			!(b_king_bit & PawnEffectBB[sq][WHITE]), !(b_king_bit & GoldEffectBB[sq][WHITE]), (~BitLancePromoteWhite) & sq, not_promote)) {
			if (not_promote) {
				w_enable_set_pawn &= ~(1 << file_index_table[sq]); // 歩の配置フラグを立てる
			}
			sub_hand(pos_.hand[1], PAWN);
			return true;
		}
	};
	if (not_double_pawn_b && myrand.rand_b(0.4)) { // 二歩判定を上段で入れる
		if (set_piece_core(pos_, sq, occupied, B_PAWN, B_PRO_PAWN, b_pawn_promote_p,
			!(w_king_bit & PawnEffectBB[sq][BLACK]), !(w_king_bit & GoldEffectBB[sq][BLACK]), (~BitLancePromoteBlack) & sq, not_promote)) {
			if (not_promote) {
				b_enable_set_pawn &= ~(1 << file_index_table[sq]); // 歩の配置フラグを立てる
			}
			sub_hand(pos_.hand[0], PAWN);
			return true;
		}
	};
	double select_p = not_double_pawn_w ? 0.4 : 0.8; // 後手が歩を打てない条件なら配置確率を上げる
	// 香、桂、銀、金の順で合い駒を試みる
	if (hand_exists(pos_.hand[1], LANCE) && myrand.rand_b(select_p)) {
		if (set_piece_core(pos_, sq, occupied, W_LANCE, W_PRO_LANCE, w_lance_promote_p,
			!(lanceEffect(WHITE, sq, occupied) & b_king_bit), !(b_king_bit & GoldEffectBB[sq][WHITE]), (~BitLancePromoteWhite) & sq)) {
			sub_hand(pos_.hand[1], LANCE);
			return true;
		}
	};
	if (hand_exists(pos_.hand[0], LANCE) && myrand.rand_b(select_p)) {
		if (set_piece_core(pos_, sq, occupied, B_LANCE, B_PRO_LANCE, b_lance_promote_p,
			!(lanceEffect(BLACK, sq, occupied) & w_king_bit), !(w_king_bit & GoldEffectBB[sq][BLACK]), (~BitLancePromoteBlack) & sq)) {
			sub_hand(pos_.hand[0], LANCE);
			return true;
		}
	};
	if (hand_exists(pos_.hand[1], KNIGHT) && myrand.rand_b(select_p)) {
		if (set_piece_core(pos_, sq, occupied, W_KNIGHT, W_PRO_KNIGHT, w_knight_promote_p,
			!(KnightEffectBB[b_king_bit][BLACK] & sq), !(b_king_bit & GoldEffectBB[sq][WHITE]), (~BitKnightPromoteWhite) & sq)) {
			sub_hand(pos_.hand[1], KNIGHT);
			return true;
		}
	};
	if (hand_exists(pos_.hand[0], KNIGHT) && myrand.rand_b(select_p)) {
		if (set_piece_core(pos_, sq, occupied, B_KNIGHT, B_PRO_KNIGHT, b_knight_promote_p,
			!(KnightEffectBB[w_king_bit][WHITE] & sq), !(w_king_bit & GoldEffectBB[sq][BLACK]), (~BitKnightPromoteBlack) & sq)) {
			sub_hand(pos_.hand[0], KNIGHT);
			return true;
		}
	};
	if (hand_exists(pos_.hand[1], SILVER) && myrand.rand_b(select_p)) {
		if (set_piece_core(pos_, sq, occupied, W_SILVER, W_PRO_SILVER, w_silver_promote_p,
			!(b_king_bit & SilverEffectBB[sq][WHITE]), !(b_king_bit & GoldEffectBB[sq][WHITE]), true)) {
			sub_hand(pos_.hand[1], SILVER);
			return true;
		}
	};
	if (hand_exists(pos_.hand[0], SILVER) && myrand.rand_b(select_p)) {
		if (set_piece_core(pos_, sq, occupied, B_SILVER, B_PRO_SILVER, b_silver_promote_p,
			!(w_king_bit & SilverEffectBB[sq][BLACK]), !(w_king_bit & GoldEffectBB[sq][BLACK]), true)) {
			sub_hand(pos_.hand[0], SILVER);
			return true;
		}
	};
	if (hand_exists(pos_.hand[1], GOLD) && myrand.rand_b(select_p)) {
		if (set_piece_core(pos_, sq, occupied, W_GOLD, W_GOLD, w_gold_promote_p,
			!(b_king_bit & GoldEffectBB[sq][WHITE]), true, true)) {
			sub_hand(pos_.hand[1], GOLD);
			return true;
		}
	};
	if (hand_exists(pos_.hand[0], GOLD) && myrand.rand_b(select_p)) {
		if (set_piece_core(pos_, sq, occupied, B_GOLD, B_GOLD, b_gold_promote_p,
			!(w_king_bit & GoldEffectBB[sq][BLACK]), true, true)) {
			sub_hand(pos_.hand[0], GOLD);
			return true;
		}
	};
	// 最後に後手の歩を強制的に採用する
	if (not_double_pawn_w && myrand.rand_b(1.0)) {
		if (set_piece_core(pos_, sq, occupied, W_PAWN, W_PRO_PAWN, w_pawn_promote_p,
			!(b_king_bit & PawnEffectBB[sq][WHITE]), !(b_king_bit & GoldEffectBB[sq][WHITE]), (~BitLancePromoteWhite) & sq, not_promote)) {
			if (not_promote) {
				w_enable_set_pawn &= ~(1 << file_index_table[sq]); // 歩の配置フラグを立てる
			}
			sub_hand(pos_.hand[1], PAWN);
			return true;
		}
	};
	return false;
};

// 飛び道具の利きによる王手の再チェックを行います
bool recheck(Position& pos_, CheckList &checklist, const Bitboard &b_king_bit, const Bitboard &w_king_bit, Bitboard &occupied,
	const PBoard &b_pawn_p, const PBoard &w_pawn_p, int &b_enable_set_pawn, int &w_enable_set_pawn, int &b_board, int &w_board) {
	if (checklist.check_item_lance[0].commit) { // ここは先手の香の利き固定
		Bitboard must_occupy = checklist.check_item_lance[0].commit;
		if (must_occupy & occupied) { // クリア
		}
		else {
			PBoard temp_p = b_pawn_p;
			temp_p.and(must_occupy);
			Square sq = sq_table[temp_p.accumu_rand()]; // 合い駒の位置を確定させる
			if (!_aigoma_w(pos_, sq, b_king_bit, w_king_bit, occupied, b_enable_set_pawn, w_enable_set_pawn)) {
				// エラー
				++cnt;
				return false;
			}
		}
	}
	if (checklist.check_item_lance[1].commit) { // ここは後手の香の利き固定
		Bitboard must_occupy = checklist.check_item_lance[1].commit;
		if (must_occupy & occupied) { // クリア
		}
		else {
			PBoard temp_p = w_pawn_p;
			temp_p. and (must_occupy);
			Square sq = sq_table[temp_p.accumu_rand()]; // 合い駒の位置を確定させる
			if (!_aigoma_b(pos_, sq, b_king_bit, w_king_bit, occupied, b_enable_set_pawn, w_enable_set_pawn)) {
				// エラー
				++cnt;
				return false;
			}
		}
	}
	if (checklist.check_item_rook[0].commit) {
		Bitboard must_occupy = checklist.check_item_rook[0].commit;
		if (checklist.check_item_rook[0].commit & occupied) { // クリア
		}
		else {
			if (checklist.check_item_rook[0].color == BLACK) {
				PBoard temp_p = b_pawn_p;
				temp_p.and(must_occupy);
				Square sq = sq_table[temp_p.accumu_rand()]; // 合い駒の位置を確定させる
				if (!_aigoma_b(pos_, sq, b_king_bit, w_king_bit, occupied, b_enable_set_pawn, w_enable_set_pawn)) {
					// エラー
					++cnt;
					return false;
				}
			}
			else {
				PBoard temp_p = w_pawn_p;
				temp_p.and(must_occupy);
				Square sq = sq_table[temp_p.accumu_rand()]; // 合い駒の位置を確定させる
				if (!_aigoma_w(pos_, sq, b_king_bit, w_king_bit, occupied, b_enable_set_pawn, w_enable_set_pawn)) {
					// エラー
					++cnt;
					return false;
				}
			}
		}
		if (checklist.check_item_rook[1].commit) {
			Bitboard must_occupy = checklist.check_item_rook[1].commit;
			if (checklist.check_item_rook[1].commit & occupied) { // クリア
			}
			else {
				if (checklist.check_item_rook[0].color == BLACK) {
					PBoard temp_p = b_pawn_p;
					temp_p.and(must_occupy);
					Square sq = sq_table[temp_p.accumu_rand()]; // 合い駒の位置を確定させる
					if (!_aigoma_b(pos_, sq, b_king_bit, w_king_bit, occupied, b_enable_set_pawn, w_enable_set_pawn)) {
						// エラー
						++cnt;
						return false;
					}
				}
				else {
					PBoard temp_p = w_pawn_p;
					temp_p.and(must_occupy);
					Square sq = sq_table[temp_p.accumu_rand()]; // 合い駒の位置を確定させる
					if (!_aigoma_w(pos_, sq, b_king_bit, w_king_bit, occupied, b_enable_set_pawn, w_enable_set_pawn)) {
						// エラー
						++cnt;
						return false;
					}
				}
			}
		}
	}
	if (checklist.check_item_bishop[0].commit) {
		Bitboard must_occupy = checklist.check_item_bishop[0].commit;
		if (checklist.check_item_bishop[0].commit & occupied) { // クリア
		}
		else {
			if (checklist.check_item_bishop[0].color == BLACK) {
				PBoard temp_p = b_pawn_p;
				temp_p.and(must_occupy);
				Square sq = sq_table[temp_p.accumu_rand()]; // 合い駒の位置を確定させる
				if (!_aigoma_b(pos_, sq, b_king_bit, w_king_bit, occupied, b_enable_set_pawn, w_enable_set_pawn)) {
					// エラー
					++cnt;
					return false;
				}
			}
			else {
				PBoard temp_p = w_pawn_p;
				temp_p.and(must_occupy);
				Square sq = sq_table[temp_p.accumu_rand()]; // 合い駒の位置を確定させる
				if (!_aigoma_w(pos_, sq, b_king_bit, w_king_bit, occupied, b_enable_set_pawn, w_enable_set_pawn)) {
					// エラー
					++cnt;
					return false;
				}
			}
		}
		if (checklist.check_item_bishop[1].commit) {
			Bitboard must_occupy = checklist.check_item_bishop[1].commit;
			if (checklist.check_item_bishop[1].commit & occupied) { // クリア
			}
			else {
				if (checklist.check_item_bishop[1].color == BLACK) {
					PBoard temp_p = b_pawn_p;
					temp_p.and(must_occupy);
					Square sq = sq_table[temp_p.accumu_rand()]; // 合い駒の位置を確定させる
					if (!_aigoma_b(pos_, sq, b_king_bit, w_king_bit, occupied, b_enable_set_pawn, w_enable_set_pawn)) {
						// エラー
						++cnt;
						return false;
					}
				}
				else {
					PBoard temp_p = w_pawn_p;
					temp_p.and(must_occupy);
					Square sq = sq_table[temp_p.accumu_rand()]; // 合い駒の位置を確定させる
					if (!_aigoma_w(pos_, sq, b_king_bit, w_king_bit, occupied, b_enable_set_pawn, w_enable_set_pawn)) {
						// エラー
						++cnt;
						return false;
					}
				}
			}
		}
	}
	return true;
};

void view() {
	std::cout << "再チェック: " << cnt << std::endl;
};

constexpr int cyclic[6] = { 1, 2, 4, 5, 7, 8 }; // 位数9で巡回的になる元

// 歩を配置します
bool set_pawn(Position& pos_, const Square &b_king, const Square &w_king,
	const Bitboard &b_king_bit, const Bitboard &w_king_bit, Bitboard &occupied, CheckList &checklist) {
	int b_enable_set_pawn = 0b111111111;
	int w_enable_set_pawn = 0b111111111;
	int b_pawn = 0;
	int w_pawn = 0;
	int b_board = 0;
	int w_board = 0;
	pawn_distribution(b_pawn, w_pawn, b_board, w_board);
	add_hand(pos_.hand[BLACK], PAWN, b_pawn);
	add_hand(pos_.hand[WHITE], PAWN, w_pawn);
	// 再チェックを合い駒によって帳尻を合わせます
	if (!recheck(pos_, checklist, b_king_bit, w_king_bit, occupied,
		b_pawn_p, w_pawn_p, b_enable_set_pawn, w_enable_set_pawn, b_board, w_board)) {
		return false; // 0.07%で失敗する
	};
	PBoard pb;
	Square sq;
	bool not_promote;
	bool not_double_pawn;
	int file;
	int cycle; // 
	// TODO: 駒柱ができたときに配置できるか
	// 先手の歩を配置します
	file = myrand.rand_m(9); // 開始筋を確定させる
	cycle = cyclic[myrand.rand_m(6)]; // 巡回パターンを定める
	for (auto i = 0; i < b_board; ++i) {
		not_double_pawn = b_enable_set_pawn & (1 << file);
		if (!not_double_pawn && myrand.rand_b(0.6)) { // 二歩のときは一定確率で再生成させる
			goto NEXT_SET_BLACK_PAWN;
		}
		if (not_double_pawn) { // 二歩でない
			pb = b_pawn_p;
			pb.ninp(PawnEffectBB[w_king][WHITE] | occupied); // 相手玉前と配置済みの位置を除く
			pb.and(FILE_BB[file]); // その筋だけのpboard
			sq = sq_table[pb.accumu_rand()];
			if (set_piece_core(pos_, sq, occupied, B_PAWN, B_PRO_PAWN, b_pawn_promote_p,
				true, !(w_king_bit & GoldEffectBB[sq][BLACK]), (Bitboard(sq) & (~BitLancePromoteBlack)), not_promote)) {
				if (not_promote) {
					b_enable_set_pawn &= ~(1 << file_index_table[sq]); // 歩の配置フラグを立てる
				}
			}
			else {
				add_hand(pos_.hand[BLACK], PAWN); // 配置に失敗したら手駒に加えておく
			}
		}
		else { // 二歩になる
			if (myrand.rand_b(0.6)) {
				add_hand(pos_.hand[BLACK], PAWN); // 一定確率で手駒に落とす
			}
			else { // 必ずと金で配置される
				pb = b_pawn_pro_p;
				pb.ninp(PawnEffectBB[w_king][WHITE] | occupied); // 相手玉前と配置済みの位置を除く
				pb.and(FILE_BB[file]); // その筋だけのpboard
				sq = sq_table[pb.accumu_rand()];
				if (set_piece_core(pos_, sq, occupied, B_PRO_PAWN, B_PRO_PAWN, b_pawn_promote_p,
					false, !(w_king_bit & GoldEffectBB[sq][BLACK]), false)) {
				}
				else {
					add_hand(pos_.hand[BLACK], PAWN); // 配置に失敗したら手駒に加えておく
				}
			}
		}
		// 次の筋に移る
	NEXT_SET_BLACK_PAWN:
		file += cycle;
		if (file >= 9) { file -= 9; }
	}
	// 後手の歩を配置します
	file = myrand.rand_m(9); // 開始筋を確定させる
	cycle = cyclic[myrand.rand_m(6)]; // 巡回パターンを定める
	for (auto i = 0; i < w_board; ++i) {
		not_double_pawn = w_enable_set_pawn & (1 << file);
		if (!not_double_pawn && myrand.rand_b(0.6)) { // 二歩のときは一定確率で再生成させる
			goto NEXT_SET_WHITE_PAWN;
		}
		if (not_double_pawn) { // 二歩でない
			pb = w_pawn_p;
			pb.ninp(PawnEffectBB[b_king][BLACK] | occupied); // 相手玉前と配置済みの位置を除く
			pb.and(FILE_BB[file]); // その筋だけのpboard
			sq = sq_table[pb.accumu_rand()];
			if (set_piece_core(pos_, sq, occupied, W_PAWN, W_PRO_PAWN, w_pawn_promote_p,
				true, !(b_king_bit & GoldEffectBB[sq][WHITE]), (Bitboard(sq) & (~BitLancePromoteWhite)), not_promote)) {
				if (not_promote) {
					w_enable_set_pawn &= ~(1 << file_index_table[sq]); // 歩の配置フラグを立てる
				}
			}
			else {
				add_hand(pos_.hand[WHITE], PAWN); // 配置に失敗したら手駒に加えておく
			}
		}
		else { // 二歩になる
			if (myrand.rand_b(0.6)) {
				add_hand(pos_.hand[WHITE], PAWN); // 一定確率で手駒に落とす
			}
			else { // 必ずと金で配置される
				pb = w_pawn_pro_p;
				pb.ninp(PawnEffectBB[b_king][BLACK] | occupied); // 相手玉前と配置済みの位置を除く
				pb.and(FILE_BB[file]); // その筋だけのpboard
				sq = sq_table[pb.accumu_rand()];
				if (set_piece_core(pos_, sq, occupied, W_PRO_PAWN, W_PRO_PAWN, w_pawn_promote_p,
					false, !(b_king_bit & GoldEffectBB[sq][WHITE]), false)) {
				}
				else {
					add_hand(pos_.hand[WHITE], PAWN); // 配置に失敗したら手駒に加えておく
				}
			}
		}
		// 次の筋に移る
	NEXT_SET_WHITE_PAWN:
		file += cycle;
		if (file >= 9) { file -= 9; }
	}
	return true;
}

std::string end_game_mate() {
START_CREATE_BOARD:
	Position pos_;
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
	set_knight(pos_, sq_b_king, sq_w_king, bit_b_king, bit_w_king, occupied); // 桂の配置
	set_silver(pos_, sq_b_king, sq_w_king, bit_b_king, bit_w_king, occupied); // 銀の配置
	set_gold(pos_, sq_b_king, sq_w_king, bit_b_king, bit_w_king, occupied); // 金の配置
	if (!set_pawn(pos_, sq_b_king, sq_w_king, bit_b_king, bit_w_king, occupied, checklist)) { // 歩の配置
		goto START_CREATE_BOARD; // 王手回避ができなかったので局面生成をやり直す
	};
	pos_.update_bitboards();
	return pos_.sfen_fast();

	// pos_.set_hirate(pos_.state(), pos_.this_thread());

	//std::cout << pos_ << std::endl;
	//std::string result = pos_.sfen();
	//std::cout << result << std::endl;
	
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
