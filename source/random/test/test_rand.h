﻿#ifndef _USERENGINE_TESTRAND_H_
#define _USERENGINE_TESTRAND_H_

#include "../my_rand.h"
#include "../int_board.h"
#include "../../shogi.h"

// この関数を他のファイルから呼んでください
void test_rand_main();

// ここから先は便宜上のエントリーポイント
void test_uniform();
void test_piece_existence_rand();
void test_accumu_rand();
void test_accumu_rand_avx512();
void test_is_promoted_rand();


#endif _USERENGINE_TESTRAND_H_
