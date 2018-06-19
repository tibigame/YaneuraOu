#include "test_board.h"
#include "test.h"
#include "../random_board.h"
#include <stdlib.h>

constexpr u64 sfen_number_per_test = 10;

void test_board_main(Position& pos_) {
	std::cout << "ランダム局面のテストを行います" << std::endl;
	std::cout << "テスト当たりの局面の個数： " << sfen_number_per_test << std::endl;
	
	test_king(pos_);
	test_rook(pos_);
	test_bishop(pos_);
	test_lance(pos_);
	test_gsn(pos_);
	test_pawn(pos_);
	/*
	test_aigoma(pos_);*/
};

// 2つのSqが互いに玉の利きにあればtrue
bool king_check(const Square king1, const Square king2) {
	int b_file = (int)file_of(king1);
	int w_file = (int)file_of(king2);
	int b_rank = (int)rank_of(king1);
	int w_rank = (int)rank_of(king2);
	return (abs(b_file - w_file) <= 1 && abs(b_rank - w_rank) <= 1);
}

// rookの1マス十字の利きにkingがあればtrue
bool rook_check(const Square king, const Square rook) {
	int k_file = file_of(king);
	int k_rank = rank_of(king);
	int r_file = file_of(rook);
	int r_rank = rank_of(rook);
	return (k_file == r_file && abs(k_rank - r_rank) == 1) || (k_rank == r_rank && abs(k_file - r_file) == 1);
}

// rookの十字の利きにkingがあればtrue
bool rook_effect(const Square king, const Square rook) {
	int k_file = file_of(king);
	int k_rank = rank_of(king);
	int r_file = file_of(rook);
	int r_rank = rank_of(rook);
	return (k_file == r_file) || (k_rank == r_rank);
}

// bishopのナナメ十字1マスの利きにkingがあればtrue
bool bishop_check(const Square king, const Square bishop) {
	int k_file = file_of(king);
	int k_rank = rank_of(king);
	int b_file = file_of(bishop);
	int b_rank = rank_of(bishop);
	return abs(k_file - b_file) == 1 && abs(k_rank - b_rank) == 1;
}

// bishopのナナメ十字の利きにkingがあればtrue
bool bishop_effect(const Square king, const Square bishop) {
	int k_file = file_of(king);
	int k_rank = rank_of(king);
	int b_file = file_of(bishop);
	int b_rank = rank_of(bishop);
	return abs(k_file - b_file) == abs(k_rank - b_rank);
}

// lanceの利きにkingがあればtrue
bool lance_effect(const Square king, const Square lance, const Color c) {
	int k_file = file_of(king);
	int k_rank = rank_of(king);
	int l_file = file_of(lance);
	int l_rank = rank_of(lance);
	if (c == BLACK) {
		return (k_file == l_file) && (k_rank < l_rank);
	}
	return (k_file == l_file) && (k_rank > l_rank);
}

// goldの利きにkingがあればtrue
bool gold_check(const Square king, const Square gold, const Color c) {
	int k_file = file_of(king);
	int k_rank = rank_of(king);
	int g_file = file_of(gold);
	int g_rank = rank_of(gold);
	if (c == BLACK) {
		return (k_file == g_file && abs(k_rank - g_rank) == 1) || (k_rank == g_rank && abs(k_file - g_file) == 1)
			|| (abs(k_file - g_file) == 1 && (k_rank - g_rank == -1));
	}
	return (k_file == g_file && abs(k_rank - g_rank) == 1) || (k_rank == g_rank && abs(k_file - g_file) == 1)
		|| (abs(k_file - g_file) == 1 && (k_rank - g_rank == 1));
}

// silverの利きにkingがあればtrue
bool silver_check(const Square king, const Square silver, const Color c) {
	int k_file = file_of(king);
	int k_rank = rank_of(king);
	int s_file = file_of(silver);
	int s_rank = rank_of(silver);
	if (c == BLACK) {
		return (abs(k_file - s_file) == 1 && abs(k_rank - s_rank) == 1)
			|| (k_file == s_file && (k_rank - s_rank == -1));
	}
	return (abs(k_file - s_file) == 1 && abs(k_rank - s_rank) == 1)
		|| (k_file == s_file && (k_rank - s_rank == 1));
}

// knightの利きにkingがあればtrue
bool knight_check(const Square king, const Square knight, const Color c) {
	int k_file = file_of(king);
	int k_rank = rank_of(king);
	int n_file = file_of(knight);
	int n_rank = rank_of(knight);
	if (c == BLACK) {
		return (abs(n_file - n_file) == 1 && (k_rank - n_rank == -2));
	}
	return (abs(n_file - n_file) == 1 && (k_rank - n_rank == 2));
}

// pawnの利きにkingがあればtrue
bool pawn_check(const Square king, const Square pawn, const Color c) {
	int k_file = file_of(king);
	int k_rank = rank_of(king);
	int p_file = file_of(pawn);
	int p_rank = rank_of(pawn);
	if (c == BLACK) {
		return (k_file == p_file) && (k_rank - p_rank == -1);
	}
	return (k_file == p_file) && (k_rank - p_rank == 1);
}

// 玉の配置のテスト
void test_king(Position& pos_) {
	std::cout << "test_king" << std::endl;
	for (auto i = 0; i < sfen_number_per_test; ++i) {
		pos_.set_blank();

		Square sq_b_king = set_b_king(pos_); // 先手玉の配置
		Square sq_w_king = set_w_king(pos_, sq_b_king); // 後手玉の配置
		pos_.update_bitboards();

		if (king_check(sq_b_king, sq_w_king)) {
			std::cout << "玉の距離が1以下" << std::endl;
			std::cout << pos_.sfen() << std::endl;
		}
	}
};

// 飛の配置のテスト
void test_rook(Position& pos_) {
	std::cout << "test_rook" << std::endl;
	for (auto i = 0; i < sfen_number_per_test; ++i) {
		list_bi.clear();
		pos_.set_blank();
		Square sq_b_king = set_b_king(pos_); // 先手玉の配置
		Square sq_w_king = set_w_king(pos_, sq_b_king); // 後手玉の配置
		Bitboard bit_b_king(sq_b_king); // 玉のBitboardはよく使うのでここで変換したものを以降使う
		Bitboard bit_w_king(sq_w_king); // 玉のBitboardはよく使うのでここで変換したものを以降使う
		Bitboard occupied = bit_b_king | bit_w_king; // 駒を配置するたびに記録するBitboard
		CheckList checklist; // 盤面再チェック用のリスト
		set_rook(pos_, sq_b_king, sq_w_king, bit_b_king, bit_w_king, occupied, checklist); // 飛車の配置

		pos_.update_bitboards();

		// 盤面に配置した駒リストを全チェックする
		for (BoardInfo x : list_bi) {
			// まずは近接してないかのチェック
			if (x.pi == B_DRAGON) {
				if (king_check(sq_w_king, x.sq)) {
					std::cout << "先手の龍と後手玉が隣接" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
			}
			else if (x.pi == W_DRAGON) {
				if (king_check(sq_b_king, x.sq)) {
					std::cout << "後手の龍と先手玉が隣接" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
			}
			else if (x.pi == B_ROOK) {
				if (rook_check(sq_w_king, x.sq)) {
					std::cout << "先手の飛と後手玉が隣接" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
			}
			else if (x.pi == W_ROOK) {
				if (rook_check(sq_b_king, x.sq)) {
					std::cout << "後手の飛と先手玉が隣接" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
			}
			else {
				continue;
			}
			// 遠方の利きに関するチェック
			if ((x.pi == B_ROOK || x.pi == B_DRAGON) && rook_effect(sq_w_king, x.sq)) {
				// 後手玉が先手の飛の利きにいるのでチェックリストのバリデーションを行う
				if (checklist.check_item_rook[0].color == WHITE && checklist.check_item_rook[0].commit != ZERO_BB) {
				}
				else if (checklist.check_item_rook[1].color == WHITE && checklist.check_item_rook[1].commit != ZERO_BB) {
				}
				else {
					std::cout << "後手玉が先手の飛の利きにいるのにチェックリストに入ってない" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
			}
			else if ((x.pi == W_ROOK || x.pi == W_DRAGON) && rook_effect(sq_b_king, x.sq)) {
				// 先手玉が後手の飛の利きにいるのでチェックリストのバリデーションを行う
				if (checklist.check_item_rook[0].color == BLACK && checklist.check_item_rook[0].commit != ZERO_BB) {
				}
				else if (checklist.check_item_rook[1].color == BLACK && checklist.check_item_rook[1].commit != ZERO_BB) {
				}
				else {
					std::cout << "先手玉が後手の飛の利きにいるのにチェックリストに入ってない" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
			}
		}
	}
};

// 角の配置のテスト
void test_bishop(Position& pos_) {
	std::cout << "test_bishop" << std::endl;
	for (auto i = 0; i < sfen_number_per_test; ++i) {
		list_bi.clear();
		pos_.set_blank();
		Square sq_b_king = set_b_king(pos_); // 先手玉の配置
		Square sq_w_king = set_w_king(pos_, sq_b_king); // 後手玉の配置
		Bitboard bit_b_king(sq_b_king); // 玉のBitboardはよく使うのでここで変換したものを以降使う
		Bitboard bit_w_king(sq_w_king); // 玉のBitboardはよく使うのでここで変換したものを以降使う
		Bitboard occupied = bit_b_king | bit_w_king; // 駒を配置するたびに記録するBitboard
		CheckList checklist; // 盤面再チェック用のリスト
		set_rook(pos_, sq_b_king, sq_w_king, bit_b_king, bit_w_king, occupied, checklist); // 飛車の配置
		set_bishop(pos_, sq_b_king, sq_w_king, bit_b_king, bit_w_king, occupied, checklist); // 角の配置

		pos_.update_bitboards();
		// 盤面に配置した駒リストを全チェックする
		for (BoardInfo x : list_bi) {
			// まずは近接してないかのチェック
			if (x.pi == B_HORSE) {
				if (king_check(sq_w_king, x.sq)) {
					std::cout << "先手の馬と後手玉が隣接" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
			}
			else if (x.pi == W_HORSE) {
				if (king_check(sq_b_king, x.sq)) {
					std::cout << "後手の馬と先手玉が隣接" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
			}
			else if (x.pi == B_BISHOP) {
				if (bishop_check(sq_w_king, x.sq)) {
					std::cout << "先手の角と後手玉が隣接" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
			}
			else if (x.pi == W_BISHOP) {
				if (bishop_check(sq_b_king, x.sq)) {
					std::cout << "後手の角と先手玉が隣接" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
			}
			else {
				continue;
			}
			// 遠方の利きに関するチェック
			if ((x.pi == B_BISHOP || x.pi == B_HORSE) && bishop_effect(sq_w_king, x.sq)) {
				// 後手玉が先手の角の利きにいるのでチェックリストのバリデーションを行う
				if (checklist.check_item_bishop[0].color == WHITE && checklist.check_item_bishop[0].commit != ZERO_BB) {
				}
				else if (checklist.check_item_bishop[1].color == WHITE && checklist.check_item_bishop[1].commit != ZERO_BB) {
				}
				else {
					std::cout << "後手玉が先手の角の利きにいるのにチェックリストに入ってない" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
			}
			else if ((x.pi == W_BISHOP || x.pi == W_HORSE) && bishop_effect(sq_b_king, x.sq)) {
				// 先手玉が後手の角の利きにいるのでチェックリストのバリデーションを行う
				if (checklist.check_item_bishop[0].color == BLACK && checklist.check_item_bishop[0].commit != ZERO_BB) {
				}
				else if (checklist.check_item_bishop[1].color == BLACK && checklist.check_item_bishop[1].commit != ZERO_BB) {
				}
				else {
					std::cout << "先手玉が後手の角の利きにいるのにチェックリストに入ってない" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
			}
		}
	}
}

// 香の配置のテスト
void test_lance(Position& pos_){
	std::cout << "test_lance" << std::endl;
	for (auto i = 0; i < sfen_number_per_test; ++i) {
		list_bi.clear();
		pos_.set_blank();
		Square sq_b_king = set_b_king(pos_); // 先手玉の配置
		Square sq_w_king = set_w_king(pos_, sq_b_king); // 後手玉の配置
		Bitboard bit_b_king(sq_b_king); // 玉のBitboardはよく使うのでここで変換したものを以降使う
		Bitboard bit_w_king(sq_w_king); // 玉のBitboardはよく使うのでここで変換したものを以降使う
		Bitboard occupied = bit_b_king | bit_w_king; // 駒を配置するたびに記録するBitboard
		CheckList checklist; // 盤面再チェック用のリスト
		set_rook(pos_, sq_b_king, sq_w_king, bit_b_king, bit_w_king, occupied, checklist); // 飛車の配置
		set_bishop(pos_, sq_b_king, sq_w_king, bit_b_king, bit_w_king, occupied, checklist); // 角の配置
		set_lance(pos_, sq_b_king, sq_w_king, bit_b_king, bit_w_king, occupied, checklist); // 香の配置

		pos_.update_bitboards();
		// 盤面に配置した駒リストを全チェックする
		for (BoardInfo x : list_bi) {
			// まずは近接してないかのチェック
			if (x.pi == B_LANCE) {
				if (pawn_check(sq_w_king, x.sq, BLACK)) {
					std::cout << "先手の香と後手玉が隣接" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
			}
			else if (x.pi == W_LANCE) {
				if (pawn_check(sq_b_king, x.sq, WHITE)) {
					std::cout << "後手の香と先手玉が隣接" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
			}
			else if (x.pi == B_PRO_LANCE) {
				if (gold_check(sq_w_king, x.sq, BLACK)) {
					std::cout << "先手の成香と後手玉が隣接" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
			}
			else if (x.pi == W_PRO_LANCE) {
				if (gold_check(sq_b_king, x.sq, WHITE)) {
					std::cout << "後手の成香と先手玉が隣接" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
			}
			else {
				continue;
			}
			// 遠方の利きに関するチェック
			if ((x.pi == B_LANCE) && lance_effect(sq_w_king, x.sq, BLACK)) {
				// 後手玉が先手の香の利きにいるのでチェックリストのバリデーションを行う
				if (checklist.check_item_lance[0].color == WHITE && checklist.check_item_lance[0].commit != ZERO_BB) {
				}
				else {
					std::cout << "後手玉が先手の香の利きにいるのにチェックリストに入ってない" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
			}
			else if ((x.pi == W_LANCE) && lance_effect(sq_b_king, x.sq, WHITE)) {
				// 先手玉が後手の香の利きにいるのでチェックリストのバリデーションを行う
				if (checklist.check_item_lance[1].color == BLACK && checklist.check_item_lance[1].commit != ZERO_BB) {
				}
				else {
					std::cout << "先手玉が後手の香の利きにいるのにチェックリストに入ってない" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
			}
		}
	}
}

// 金銀桂の配置のテスト
void test_gsn(Position& pos_) {
	std::cout << "test_gold_silver_knight" << std::endl;
	for (auto i = 0; i < sfen_number_per_test; ++i) {
		list_bi.clear();
		pos_.set_blank();
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

		pos_.update_bitboards();
		// 盤面に配置した駒リストを全チェックする
		for (BoardInfo x : list_bi) {
			// 近接してないかのチェック
			if (x.pi == B_KNIGHT) {
				if (knight_check(sq_w_king, x.sq, BLACK)) {
					std::cout << "先手の桂と後手玉が隣接" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
			}
			else if (x.pi == W_KNIGHT) {
				if (knight_check(sq_b_king, x.sq, WHITE)) {
					std::cout << "後手の桂と先手玉が隣接" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
			}
			else if (x.pi == B_PRO_KNIGHT) {
				if (gold_check(sq_w_king, x.sq, BLACK)) {
					std::cout << "先手の成桂と後手玉が隣接" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
			}
			else if (x.pi == W_PRO_KNIGHT) {
				if (gold_check(sq_b_king, x.sq, WHITE)) {
					std::cout << "後手の成桂と先手玉が隣接" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
			}
			else if (x.pi == B_SILVER) {
				if (silver_check(sq_w_king, x.sq, BLACK)) {
					std::cout << "先手の銀と後手玉が隣接" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
			}
			else if (x.pi == W_SILVER) {
				if (silver_check(sq_b_king, x.sq, WHITE)) {
					std::cout << "後手の銀と先手玉が隣接" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
			}
			else if (x.pi == B_PRO_SILVER) {
				if (gold_check(sq_w_king, x.sq, BLACK)) {
					std::cout << "先手の成銀と後手玉が隣接" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
			}
			else if (x.pi == W_PRO_SILVER) {
				if (gold_check(sq_b_king, x.sq, WHITE)) {
					std::cout << "後手の成銀と先手玉が隣接" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
			}
			else if (x.pi == B_GOLD) {
				if (gold_check(sq_w_king, x.sq, BLACK)) {
					std::cout << "先手の金と後手玉が隣接" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
			}
			else if (x.pi == W_GOLD) {
				if (gold_check(sq_b_king, x.sq, WHITE)) {
					std::cout << "後手の金と先手玉が隣接" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
			}
			else {
				continue;
			}
		}
	}
}

// 歩の配置のテスト
void test_pawn(Position& pos_) {
	std::cout << "test_pawn" << std::endl;
	for (auto i = 0; i < sfen_number_per_test; ++i) {
	START_CREATE_BOARD_TEST:
		list_bi.clear();
		pos_.set_blank();
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
			goto START_CREATE_BOARD_TEST; // 王手回避ができなかったので局面生成をやり直す
		};

		pos_.update_bitboards();

		std::vector<bool> b_pawn_flag = { false, false, false, false, false, false, false, false, false };
		std::vector<bool> w_pawn_flag = { false, false, false, false, false, false, false, false, false };

		// 盤面に配置した駒リストを全チェックする
		for (BoardInfo x : list_bi) {
			// 近接してないかのチェック
			if (x.pi == B_PAWN) {
				if (pawn_check(sq_w_king, x.sq, BLACK)) {
					std::cout << "先手の歩と後手玉が隣接" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
				if (b_pawn_flag[(int)file_of(x.sq)]) {
					std::cout << "先手が二歩" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
				else {
					b_pawn_flag[(int)file_of(x.sq)] = true;
				}
			}
			else if (x.pi == W_PAWN) {
				if (pawn_check(sq_b_king, x.sq, WHITE)) {
					std::cout << "後手の歩と先手玉が隣接" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
				if (w_pawn_flag[(int)file_of(x.sq)]) {
					std::cout << "後手が二歩" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
				else {
					w_pawn_flag[(int)file_of(x.sq)] = true;
				}
			}
			else if (x.pi == B_PRO_PAWN) {
				if (gold_check(sq_w_king, x.sq, BLACK)) {
					std::cout << "先手のと金と後手玉が隣接" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
			}
			else if (x.pi == W_PRO_PAWN) {
				if (gold_check(sq_b_king, x.sq, WHITE)) {
					std::cout << "後手のと金と先手玉が隣接" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
			}
			else {
				continue;
			}
		}
	}
}


// 全配置後のテスト
void test_aigoma(Position& pos_) {
	std::cout << "test_aigoma" << std::endl;
	for (auto i = 0; i < sfen_number_per_test; ++i) {
		if (i % 5000000 == 0) {
			std::cout << i << std::endl;
		}
	START_CREATE_BOARD_TEST:
		list_bi.clear();
		pos_.set_blank();
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
			goto START_CREATE_BOARD_TEST; // 王手回避ができなかったので局面生成をやり直す
		};

		pos_.update_bitboards();

		// 二歩のチェックフラグ
		std::vector<bool> b_pawn_flag = { false, false, false, false, false, false, false, false, false };
		std::vector<bool> w_pawn_flag = { false, false, false, false, false, false, false, false, false };

		// 遠距離の利きのチェックフラグ
		bool b_rook_flag1;
		bool b_rook_flag2;
		bool w_rook_flag1;
		bool w_rook_flag2;
		bool b_bishop_flag1;
		bool b_bishop_flag2;
		bool w_bishop_flag1;
		bool w_bishop_flag2;
		bool b_lance_flag;
		bool w_lance_flag;
		Square b_rook1, b_rook2, w_rook1, w_rook2;
		Square b_bishop1, b_bishop2, w_bishop1, w_bishop2;
		Square b_lance, w_lance;

		b_rook_flag1 = false;
		b_rook_flag2 = false;
		w_rook_flag1 = false;
		w_rook_flag2 = false;
		b_bishop_flag1 = false;
		b_bishop_flag2 = false;
		w_bishop_flag1 = false;
		w_bishop_flag2 = false;
		b_lance_flag = false;
		w_lance_flag = false;

		// 枚数チェック用
		int pawn = 0;
		int lance = 0;
		int knight = 0;
		int silver = 0;
		int gold = 0;
		int bishop = 0;
		int rook = 0;

		// 盤面に配置した駒リストを全チェックする
		for (BoardInfo x : list_bi) {
			// 遠距離の利きのチェック
			if ((x.pi == B_ROOK || x.pi == B_DRAGON) && rook_effect(sq_w_king, x.sq)) {
				if (b_rook_flag1) {
					b_rook_flag2 = true;
					b_rook2 = x.sq;
				}
				else {
					b_rook_flag1 = true;
					b_rook1 = x.sq;
				}
			}
			else if ((x.pi == W_ROOK || x.pi == W_DRAGON) && rook_effect(sq_b_king, x.sq)) {
				if (w_rook_flag1) {
					w_rook_flag2 = true;
					w_rook2 = x.sq;
				}
				else {
					w_rook_flag1 = true;
					w_rook1 = x.sq;
				}
			}
			else if ((x.pi == B_BISHOP || x.pi == B_HORSE) && bishop_effect(sq_w_king, x.sq)) {
				if (b_bishop_flag1) {
					b_bishop_flag2 = true;
					b_bishop2 = x.sq;
				}
				else {
					b_bishop_flag1 = true;
					b_bishop1 = x.sq;
				}
			}
			else if ((x.pi == W_BISHOP || x.pi == W_HORSE) && bishop_effect(sq_b_king, x.sq)) {
				if (w_bishop_flag1) {
					w_bishop_flag2 = true;
					w_bishop2 = x.sq;
				}
				else {
					w_bishop_flag1 = true;
					w_bishop1 = x.sq;
				}
			}
			else if ((x.pi == B_LANCE) && lance_effect(sq_w_king, x.sq, BLACK)) {
				if (b_lance_flag) {
					if (rank_of(x.sq) < rank_of(b_lance)) {
						b_lance = x.sq;
					}
				}
				else {
					b_lance_flag = true;
					b_lance = x.sq;
				}
			}
			else if ((x.pi == W_LANCE) && lance_effect(sq_b_king, x.sq, WHITE)) {
				if (w_lance_flag) {
					if (rank_of(x.sq) > rank_of(w_lance)) {
						w_lance = x.sq;
					}
				}
				else {
					w_lance_flag = true;
					w_lance = x.sq;
				}
			}

			// 近接してないかのチェック
			if (x.pi == B_DRAGON) {
				if (king_check(sq_w_king, x.sq)) {
					std::cout << "先手の龍と後手玉が隣接" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
				++rook;
			}
			else if (x.pi == W_DRAGON) {
				if (king_check(sq_b_king, x.sq)) {
					std::cout << "後手の龍と先手玉が隣接" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
				++rook;
			}
			else if (x.pi == B_ROOK) {
				if (rook_check(sq_w_king, x.sq)) {
					std::cout << "先手の飛と後手玉が隣接" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
				++rook;
			}
			else if (x.pi == W_ROOK) {
				if (rook_check(sq_b_king, x.sq)) {
					std::cout << "後手の飛と先手玉が隣接" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
				++rook;
			}
			else if (x.pi == B_HORSE) {
				if (king_check(sq_w_king, x.sq)) {
					std::cout << "先手の馬と後手玉が隣接" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
				++bishop;
			}
			else if (x.pi == W_HORSE) {
				if (king_check(sq_b_king, x.sq)) {
					std::cout << "後手の馬と先手玉が隣接" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
				++bishop;
			}
			else if (x.pi == B_BISHOP) {
				if (bishop_check(sq_w_king, x.sq)) {
					std::cout << "先手の角と後手玉が隣接" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
				++bishop;
			}
			else if (x.pi == W_BISHOP) {
				if (bishop_check(sq_b_king, x.sq)) {
					std::cout << "後手の角と先手玉が隣接" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
				++bishop;
			}
			else if (x.pi == B_LANCE) {
				if (rank_of(x.sq) == 0) {
					std::cout << "先手の香が行き所のない駒" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
				if (pawn_check(sq_w_king, x.sq, BLACK)) {
					std::cout << "先手の香と後手玉が隣接" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
				++lance;
			}
			else if (x.pi == W_LANCE) {
				if (rank_of(x.sq) == 8) {
					std::cout << "後手の香が行き所のない駒" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
				if (pawn_check(sq_b_king, x.sq, WHITE)) {
					std::cout << "後手の香と先手玉が隣接" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
				++lance;
			}
			else if (x.pi == B_PRO_LANCE) {
				if (gold_check(sq_w_king, x.sq, BLACK)) {
					std::cout << "先手の成香と後手玉が隣接" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
				++lance;
			}
			else if (x.pi == W_PRO_LANCE) {
				if (gold_check(sq_b_king, x.sq, WHITE)) {
					std::cout << "後手の成香と先手玉が隣接" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
				++lance;
			}
			else if (x.pi == B_KNIGHT) {
				if (rank_of(x.sq) <= 1) {
					std::cout << "先手の桂が行き所のない駒" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
				if (knight_check(sq_w_king, x.sq, BLACK)) {
					std::cout << "先手の桂と後手玉が隣接" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
				++knight;
			}
			else if (x.pi == W_KNIGHT) {
				if (rank_of(x.sq) >= 7) {
					std::cout << "後手の桂が行き所のない駒" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
				if (knight_check(sq_b_king, x.sq, WHITE)) {
					std::cout << "後手の桂と先手玉が隣接" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
				++knight;
			}
			else if (x.pi == B_PRO_KNIGHT) {
				if (gold_check(sq_w_king, x.sq, BLACK)) {
					std::cout << "先手の成桂と後手玉が隣接" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
				++knight;
			}
			else if (x.pi == W_PRO_KNIGHT) {
				if (gold_check(sq_b_king, x.sq, WHITE)) {
					std::cout << "後手の成桂と先手玉が隣接" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
				++knight;
			}
			else if (x.pi == B_SILVER) {
				if (silver_check(sq_w_king, x.sq, BLACK)) {
					std::cout << "先手の銀と後手玉が隣接" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
				++silver;
			}
			else if (x.pi == W_SILVER) {
				if (silver_check(sq_b_king, x.sq, WHITE)) {
					std::cout << "後手の銀と先手玉が隣接" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
				++silver;
			}
			else if (x.pi == B_PRO_SILVER) {
				if (gold_check(sq_w_king, x.sq, BLACK)) {
					std::cout << "先手の成銀と後手玉が隣接" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
				++silver;
			}
			else if (x.pi == W_PRO_SILVER) {
				if (gold_check(sq_b_king, x.sq, WHITE)) {
					std::cout << "後手の成銀と先手玉が隣接" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
				++silver;
			}
			else if (x.pi == B_GOLD) {
				if (gold_check(sq_w_king, x.sq, BLACK)) {
					std::cout << "先手の金と後手玉が隣接" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
				++gold;
			}
			else if (x.pi == W_GOLD) {
				if (gold_check(sq_b_king, x.sq, WHITE)) {
					std::cout << "後手の金と先手玉が隣接" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
				++gold;
			}
			else if (x.pi == B_PAWN) {
				if (rank_of(x.sq) == 0) {
					std::cout << "先手の歩が行き所のない駒" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
				if (pawn_check(sq_w_king, x.sq, BLACK)) {
					std::cout << "先手の歩と後手玉が隣接" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
				if (b_pawn_flag[(int)file_of(x.sq)]) {
					std::cout << "先手が二歩" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
				else {
					b_pawn_flag[(int)file_of(x.sq)] = true;
				}
				++pawn;
			}
			else if (x.pi == W_PAWN) {
				if (rank_of(x.sq) == 8) {
					std::cout << "後手の歩が行き所のない駒" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
				if (pawn_check(sq_b_king, x.sq, WHITE)) {
					std::cout << "後手の歩と先手玉が隣接" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
				if (w_pawn_flag[(int)file_of(x.sq)]) {
					std::cout << "後手が二歩" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
				else {
					w_pawn_flag[(int)file_of(x.sq)] = true;
				}
				++pawn;
			}
			else if (x.pi == B_PRO_PAWN) {
				if (gold_check(sq_w_king, x.sq, BLACK)) {
					std::cout << "先手のと金と後手玉が隣接" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
				++pawn;
			}
			else if (x.pi == W_PRO_PAWN) {
				if (gold_check(sq_b_king, x.sq, WHITE)) {
					std::cout << "後手のと金と先手玉が隣接" << std::endl;
					std::cout << pos_.sfen() << std::endl;
				}
				++pawn;
			}
			else {
				continue;
			}
		}

		pawn += hand_count(pos_.hand[0], PAWN);
		pawn += hand_count(pos_.hand[1], PAWN);
		lance += hand_count(pos_.hand[0], LANCE);
		lance += hand_count(pos_.hand[1], LANCE);
		knight += hand_count(pos_.hand[0], KNIGHT);
		knight += hand_count(pos_.hand[1], KNIGHT);
		silver += hand_count(pos_.hand[0], SILVER);
		silver += hand_count(pos_.hand[1], SILVER);
		gold += hand_count(pos_.hand[0], GOLD);
		gold += hand_count(pos_.hand[1], GOLD);
		bishop += hand_count(pos_.hand[0], BISHOP);
		bishop += hand_count(pos_.hand[1], BISHOP);
		rook += hand_count(pos_.hand[0], ROOK);
		rook += hand_count(pos_.hand[1], ROOK);

		if (pawn != 18) {
			std::cout << "歩が18枚でない" << std::endl;
			std::cout << pos_.sfen() << std::endl;
		}
		if (lance != 4) {
			std::cout << "香が4枚でない" << std::endl;
			std::cout << pos_.sfen() << std::endl;
		}
		if (knight != 4) {
			std::cout << "桂が4枚でない" << std::endl;
			std::cout << pos_.sfen() << std::endl;
		}
		if (silver != 4) {
			std::cout << "銀が4枚でない" << std::endl;
			std::cout << pos_.sfen() << std::endl;
		}
		if (gold != 4) {
			std::cout << "金が4枚でない" << std::endl;
			std::cout << pos_.sfen() << std::endl;
		}
		if (bishop != 2) {
			std::cout << "角が2枚でない" << std::endl;
			std::cout << pos_.sfen() << std::endl;
		}
		if (rook != 2) {
			std::cout << "飛が2枚でない" << std::endl;
			std::cout << pos_.sfen() << std::endl;
		}


		// チェックリストの検証
		bool check_temp = false;
		if (b_lance_flag) {
			if (file_of(sq_w_king) != file_of(b_lance) || rank_of(sq_w_king) >= rank_of(b_lance)) {
				std::cout << "Assert(後手玉が先手の香の利きの先にない)" << std::endl;
			}
			for (BoardInfo x : list_bi) {
				if (file_of(x.sq) == file_of(b_lance) && rank_of(x.sq) < rank_of(b_lance) && rank_of(x.sq) > rank_of(sq_w_king)) {
					check_temp = true;
					break;
				}
			}
			if (!check_temp) {
				std::cout << "後手玉が先手の香で王手になっている" << std::endl;
				std::cout << pos_.sfen() << std::endl;
			}
			check_temp = false;
		}

		if (w_lance_flag) {
			if (file_of(sq_b_king) != file_of(w_lance) || rank_of(sq_b_king) <= rank_of(w_lance)) {
				std::cout << "Assert(先手玉が後手の香の利きの先にない)" << std::endl;
			}
			for (BoardInfo x : list_bi) {
				if (file_of(x.sq) == file_of(w_lance) && rank_of(x.sq) > rank_of(w_lance) && rank_of(x.sq) < rank_of(sq_b_king)) {
					check_temp = true;
					break;
				}
			}
			if (!check_temp) {
				std::cout << "先手玉が後手の香で王手になっている" << std::endl;
				std::cout << pos_.sfen() << std::endl;
			}
			check_temp = false;
		}

		if (b_rook_flag1) {
			if (file_of(sq_w_king) == file_of(b_rook1)) {
				for (BoardInfo x : list_bi) {
					if (
						file_of(x.sq) == file_of(b_rook1) && (
							(rank_of(b_rook1) < rank_of(x.sq) && rank_of(x.sq) < rank_of(sq_w_king)) ||
							(rank_of(sq_w_king) < rank_of(x.sq) && rank_of(x.sq) < rank_of(b_rook1))
							)
						) {
						check_temp = true;
						break;
					}
				}
			}
			else if (rank_of(sq_w_king) == rank_of(b_rook1)) {
				for (BoardInfo x : list_bi) {
					if (
						rank_of(x.sq) == rank_of(b_rook1) && (
							(file_of(b_rook1) < file_of(x.sq) && file_of(x.sq) < file_of(sq_w_king)) ||
							(file_of(sq_w_king) < file_of(x.sq) && file_of(x.sq) < file_of(b_rook1))
							)
						) {
						check_temp = true;
						break;
					}
				}
			}
			else {
				std::cout << "Assert(後手玉が先手の飛の利きの先にない)" << std::endl;
			}
			if (!check_temp) {
				std::cout << "後手玉が先手の飛で王手になっている" << std::endl;
				std::cout << pos_.sfen() << std::endl;
			}
			check_temp = false;
		}

		if (b_rook_flag2) {
			if (file_of(sq_w_king) == file_of(b_rook2)) {
				for (BoardInfo x : list_bi) {
					if (file_of(x.sq) == file_of(b_rook2) &&
						(
							rank_of(b_rook2) < rank_of(x.sq) && rank_of(x.sq) < rank_of(sq_w_king) ||
							rank_of(sq_w_king) < rank_of(x.sq) && rank_of(x.sq) < rank_of(b_rook2)
							)
						) {
						check_temp = true;
						break;
					}
				}
			}
			else if (rank_of(sq_w_king) == rank_of(b_rook2)) {
				for (BoardInfo x : list_bi) {
					if (rank_of(x.sq) == rank_of(b_rook2) &&
						(
							file_of(b_rook2) < file_of(x.sq) && file_of(x.sq) < file_of(sq_w_king) ||
							file_of(sq_w_king) < file_of(x.sq) && file_of(x.sq) < file_of(b_rook2)
							)
						) {
						check_temp = true;
						break;
					}
				}
			}
			else {
				std::cout << "Assert(後手玉が先手の飛の利きの先にない)" << std::endl;
			}
			if (!check_temp) {
				std::cout << "後手玉が先手の飛で王手になっている" << std::endl;
				std::cout << pos_.sfen() << std::endl;
			}
			check_temp = false;
		}

		if (w_rook_flag1) {
			if (file_of(sq_b_king) == file_of(w_rook1)) {
				for (BoardInfo x : list_bi) {
					if (file_of(x.sq) == file_of(w_rook1) && (
							rank_of(w_rook1) < rank_of(x.sq) && rank_of(x.sq) < rank_of(sq_b_king) ||
							rank_of(sq_b_king) < rank_of(x.sq) && rank_of(x.sq) < rank_of(w_rook1)
							)
						) {
						check_temp = true;
						break;
					}
				}
			}
			else if (rank_of(sq_b_king) == rank_of(w_rook1)) {
				for (BoardInfo x : list_bi) {
					if (rank_of(x.sq) == rank_of(w_rook1) && (
							file_of(w_rook1) < file_of(x.sq) && file_of(x.sq) < file_of(sq_b_king) ||
							file_of(sq_b_king) < file_of(x.sq) && file_of(x.sq) < file_of(w_rook1)
							)
						) {
						check_temp = true;
						break;
					}
				}
			}
			else {
				std::cout << "Assert(先手玉が後手の飛の利きの先にない)" << std::endl;
			}
			if (!check_temp) {
				std::cout << "先手玉が後手の飛で王手になっている" << std::endl;
				std::cout << pos_.sfen() << std::endl;
			}
			check_temp = false;
		}

		if (w_rook_flag2) {
			if (file_of(sq_b_king) == file_of(w_rook2)) {
				for (BoardInfo x : list_bi) {
					if (file_of(x.sq) == file_of(w_rook2) && (
							rank_of(w_rook2) < rank_of(x.sq) && rank_of(x.sq) < rank_of(sq_b_king) ||
							rank_of(sq_b_king) < rank_of(x.sq) && rank_of(x.sq) < rank_of(w_rook2)
							)
						) {
						check_temp = true;
						break;
					}
				}
			}
			else if (rank_of(sq_b_king) == rank_of(w_rook2)) {
				for (BoardInfo x : list_bi) {
					if (rank_of(x.sq) == rank_of(w_rook2) && (
							file_of(w_rook2) < file_of(x.sq) && file_of(x.sq) < file_of(sq_b_king) ||
							file_of(sq_b_king) < file_of(x.sq) && file_of(x.sq) < file_of(w_rook2)
							)
						) {
						check_temp = true;
						break;
					}
				}
			}
			else {
				std::cout << "Assert(先手玉が後手の飛の利きの先にない)" << std::endl;
			}
			if (!check_temp) {
				std::cout << "先手玉が後手の飛で王手になっている" << std::endl;
				std::cout << pos_.sfen() << std::endl;
			}
			check_temp = false;
		}

		if (b_bishop_flag1) {
			bool rank_parity = rank_of(sq_w_king) > rank_of(b_bishop1);
			bool file_parity = file_of(sq_w_king) > file_of(b_bishop1);

			for (BoardInfo x : list_bi) {
				if (
					(rank_parity == rank_of(sq_w_king) > rank_of(x.sq)) &&
					(file_parity == file_of(sq_w_king) > file_of(x.sq)) && 
					(abs(rank_of(sq_w_king) - rank_of(b_bishop1)) > abs(rank_of(sq_w_king) - rank_of(x.sq))) &&
					(abs(file_of(sq_w_king) - file_of(b_bishop1)) > abs(file_of(sq_w_king) - file_of(x.sq))) &&
					(abs(rank_of(sq_w_king) - rank_of(x.sq)) == abs(file_of(sq_w_king) - file_of(x.sq)))
					) {
					check_temp = true;
					break;
				}
			}
			if (abs(rank_of(sq_w_king) - rank_of(b_bishop1)) != abs(file_of(sq_w_king) - file_of(b_bishop1))) {
				std::cout << "Assert(後手玉が先手の角の利きの先にない)" << std::endl;
			}
			if (!check_temp) {
				std::cout << "後手玉が先手の角で王手になっている" << std::endl;
				std::cout << pos_.sfen() << std::endl;
			}
			check_temp = false;
		}

		if (b_bishop_flag2) {
			bool rank_parity = rank_of(sq_w_king) > rank_of(b_bishop2);
			bool file_parity = file_of(sq_w_king) > file_of(b_bishop2);

			for (BoardInfo x : list_bi) {
				if (
					(rank_parity == rank_of(sq_w_king) > rank_of(x.sq)) &&
					(file_parity == file_of(sq_w_king) > file_of(x.sq)) &&
					(abs(rank_of(sq_w_king) - rank_of(b_bishop2)) > abs(rank_of(sq_w_king) - rank_of(x.sq))) &&
					(abs(file_of(sq_w_king) - file_of(b_bishop2)) > abs(file_of(sq_w_king) - file_of(x.sq))) &&
					(abs(rank_of(sq_w_king) - rank_of(x.sq)) == abs(file_of(sq_w_king) - file_of(x.sq)))
					) {
					check_temp = true;
					break;
				}
			}
			if (abs(rank_of(sq_w_king) - rank_of(b_bishop2)) != abs(file_of(sq_w_king) - file_of(b_bishop2))) {
				std::cout << "Assert(後手玉が先手の角の利きの先にない)" << std::endl;
			}
			if (!check_temp) {
				std::cout << "後手玉が先手の角で王手になっている" << std::endl;
				std::cout << pos_.sfen() << std::endl;
			}
			check_temp = false;
		}

		if (w_bishop_flag1) {
			bool rank_parity = rank_of(sq_b_king) > rank_of(w_bishop1);
			bool file_parity = file_of(sq_b_king) > file_of(w_bishop1);

			for (BoardInfo x : list_bi) {
				if (
					(rank_parity == rank_of(sq_b_king) > rank_of(x.sq)) &&
					(file_parity == file_of(sq_b_king) > file_of(x.sq)) &&
					(abs(rank_of(sq_b_king) - rank_of(w_bishop1)) > abs(rank_of(sq_b_king) - rank_of(x.sq))) &&
					(abs(file_of(sq_b_king) - file_of(w_bishop1)) > abs(file_of(sq_b_king) - file_of(x.sq))) &&
					(abs(rank_of(sq_b_king) - rank_of(x.sq)) == abs(file_of(sq_b_king) - file_of(x.sq)))
					) {
					check_temp = true;
					break;
				}
			}
			if (abs(rank_of(sq_b_king) - rank_of(w_bishop1)) != abs(file_of(sq_b_king) - file_of(w_bishop1))) {
				std::cout << "Assert(先手玉が後手の角の利きの先にない)" << std::endl;
			}
			if (!check_temp) {
				std::cout << "先手玉が後手の角で王手になっている" << std::endl;
				std::cout << pos_.sfen() << std::endl;
			}
			check_temp = false;
		}

		if (w_bishop_flag2) {
			bool rank_parity = rank_of(sq_b_king) > rank_of(w_bishop2);
			bool file_parity = file_of(sq_b_king) > file_of(w_bishop2);

			for (BoardInfo x : list_bi) {
				if (
					(rank_parity == rank_of(sq_b_king) > rank_of(x.sq)) &&
					(file_parity == file_of(sq_b_king) > file_of(x.sq)) &&
					(abs(rank_of(sq_b_king) - rank_of(w_bishop2)) > abs(rank_of(sq_b_king) - rank_of(x.sq))) &&
					(abs(file_of(sq_b_king) - file_of(w_bishop2)) > abs(file_of(sq_b_king) - file_of(x.sq))) &&
					(abs(rank_of(sq_b_king) - rank_of(x.sq)) == abs(file_of(sq_b_king) - file_of(x.sq)))
					) {
					check_temp = true;
					break;
				}
			}
			if (abs(rank_of(sq_b_king) - rank_of(w_bishop2)) != abs(file_of(sq_b_king) - file_of(w_bishop2))) {
				std::cout << "Assert(先手玉が後手の角の利きの先にない)" << std::endl;
			}
			if (!check_temp) {
				std::cout << "先手玉が後手の角で王手になっている" << std::endl;
				std::cout << pos_.sfen() << std::endl;
			}
			check_temp = false;
		}
	}
}
