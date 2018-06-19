#ifndef _PBOARD_H_
#define _PBOARD_H_

#include <array>
#include "../position.h"
#include "int_board.h"

// 盤面の位置を確率的に選択するためのクラス
class PBoard
{
private:
#ifdef AVX512
	IntBoard2 board;
	IntBoard2 accum;
#else
	IntBoard board;
	IntBoard accum;
#endif
	int p_sum;
public:
	PBoard();
	PBoard(const PBoard &p_board);

	PBoard(const IntBoard init_board);
#ifdef AVX512
	PBoard(const IntBoard2 init_board);
	// IntBoard2とのビット演算を行う。累計和の更新は行われないので、accumu()を呼ぶこと。
	void and(IntBoard2& int_board); // IntBoardが立っていない部分を0にする。
	void ninp(IntBoard2& int_board); // IntBoardが立っている部分を0にする。
#else
	// IntBoardとのビット演算を行う。累計和の更新は行われないので、accumu()を呼ぶこと。
	void and(IntBoard& int_board); // IntBoardが立っていない部分を0にする。
	void ninp(IntBoard& int_board); // IntBoardが立っている部分を0にする。
#endif
	void and(Bitboard& bitboard); // Bitboardが立っていない部分を0にする。
	void ninp(Bitboard& bitboard); // Bitboardが立っている部分を0にする。

	// 累計和の計算と確率的選択
	void accumu(); // 累計加算のBoardを計算する
	int rand(); // 2分探索で累計加算のBoardから選択する
	int accumu_rand(); // accumulateとrandを同時に実行する

	bool is_zero(); // すべて0ならtrueを返す。

	// 盤面を出力する。デバッグ用。
	friend std::ostream& operator<<(std::ostream& os, const PBoard& pos);
};

#endif _PBOARD_H_
