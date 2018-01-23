#ifndef _USERENGINE_TEST_H_
#define _USERENGINE_TEST_H_

// この関数を他のファイルから呼んでください
void test_main();

// ここから先は便宜上のエントリーポイント
void assert_file_index_table(const Square &sq, const int &result);
void test_file_index_table();
void assert_rank_index_table(const Square &sq, const int &result);
void test_rank_index_table();
void assert_reverse_promotep(const PromoteP &p, const PromoteP &expect);
void test_promotep();
void test_support_is_promoted_rand(const Square &sq, const PromoteP &p);
void assert_support_is_promoted_rand(const Square &sq, const PromoteP &p, const u32 &expect);
void test__and();
void assert__and(const IntBoard &base_board, const IntBoard &and_board, const IntBoard &expect);
void test__ninp();
void assert__ninp(const IntBoard &base_board, const IntBoard &and_board, const IntBoard &expect);
void test__accumu();
void assert__accumu(const IntBoard &base_board, const IntBoard &accumu_board, const IntBoard &expect);
void test_effect();
void assert_is_effect_b_rook(const Square &w_king, const Square &b_rook, const bool &expect);
void assert_is_effect_w_rook(const Square &b_king, const Square &w_rook, const bool &expect);
void test_bitboard_to_intboard();
void assert_bitboard_to_intboard(const Bitboard &bit_board, const IntBoard &expect);

#endif _USERENGINE_TEST_H_
