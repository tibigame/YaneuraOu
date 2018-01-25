#ifndef _USERENGINE_TEST_H_
#define _USERENGINE_TEST_H_

// この関数を他のファイルから呼んでください
void test_main();

extern u64 error_count;

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
void assert__ninp(const IntBoard &base_board, const IntBoard &ninp_board, const IntBoard &expect);
void test__accumu();
void assert__accumu(const IntBoard &base_board, const IntBoard &accumu_board, const IntBoard &expect);
void test_effect();
void test_bitboard_to_intboard();
void assert_bitboard_to_intboard(const Bitboard &bit_board, const IntBoard &expect);

#ifdef AVX512
void test__and2();
void assert__and2(const IntBoard2 &base_board, const IntBoard2 &and_board, const IntBoard2 &expect);
void test__ninp2();
void assert__ninp2(const IntBoard2 &base_board, const IntBoard2 &ninp_board, const IntBoard2 &expect);
void test__accumu2();
void assert__accumu2(const IntBoard2 &base_board, const IntBoard2 &accumu_board, const IntBoard2 &expect);
void test_bitboard_to_intboard2();
void assert_bitboard_to_intboard2(const Bitboard &bit_board, const IntBoard2 &expect);
#endif

#endif _USERENGINE_TEST_H_
