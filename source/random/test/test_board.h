#ifndef _USERENGINE_TESTBOARD_H
#define _USERENGINE_TESTBOARD_H

#include "../../position.h"
#include "../my_rand.h"
#include "../int_board.h"
#include "../../shogi.h"

// この関数を他のファイルから呼んでください
void test_board_main(Position& pos_);

// ここから先は便宜上のエントリーポイント
void test_king(Position& pos_);
void test_rook(Position& pos_);
void test_bishop(Position& pos_);
void test_lance(Position& pos_);
void test_gsn(Position& pos_);
void test_pawn(Position& pos_);
void test_aigoma(Position& pos_);


#endif _USERENGINE_TESTBOARD_H
