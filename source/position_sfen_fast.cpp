#include "position.h"

/*
Position::set_fast()はPosition::set()比で約2.6倍高速
Position::sfen_fast()はPosition::sfen()比で約20倍高速
*/

constexpr char CHAR0 = '0';

inline Piece char_to_piece(char c) {
	switch (c) {
	case 'K': return B_KING;
	case 'R': return B_ROOK;
	case 'B': return B_BISHOP;
	case 'G': return B_GOLD;
	case 'S': return B_SILVER;
	case 'N': return B_KNIGHT;
	case 'L': return B_LANCE;
	case 'P': return B_PAWN;
	case 'k': return W_KING;
	case 'r': return W_ROOK;
	case 'b': return W_BISHOP;
	case 'g': return W_GOLD;
	case 's': return W_SILVER;
	case 'n': return W_KNIGHT;
	case 'l': return W_LANCE;
	case 'p': return W_PAWN;
	}
	return NO_PIECE;
}

void Position::set_fast(std::string sfen, StateInfo* si, Thread* th) {
	std::memset(this, 0, sizeof(Position));

	// 局面をrootより遡るためには、ここまでの局面情報が必要で、それは引数のsiとして渡されているという解釈。
	// ThreadPool::start_thinking()では、
	// ここをいったんゼロクリアしたのちに、呼び出し側で、そのsiを復元することにより、局面を遡る。
	std::memset(si, 0, sizeof(StateInfo));
	st = si;

	// 変な入力をされることはあまり想定していない。
	// sfen文字列は、普通GUI側から渡ってくるのでおかしい入力であることはありえないからである。

	// --- 盤面
#if defined (USE_FV38)
	// PieceListを更新する上で、どの駒がどこにあるかを設定しなければならないが、
	// それぞれの駒をどこまで使ったかのカウンター
	PieceNumber piece_no_count[KING] = { PIECE_NUMBER_ZERO,PIECE_NUMBER_PAWN,PIECE_NUMBER_LANCE,PIECE_NUMBER_KNIGHT,
		PIECE_NUMBER_SILVER, PIECE_NUMBER_BISHOP, PIECE_NUMBER_ROOK,PIECE_NUMBER_GOLD };

	// 先手玉のいない詰将棋とか、駒落ちに対応させるために、存在しない駒はすべてBONA_PIECE_ZEROにいることにする。
	// 上のevalList.clear()で、ゼロクリアしているので、それは達成しているはず。
#elif defined(USE_FV_VAR)
	auto& dp = st->dirtyPiece;
	// FV_VARのときは直接evalListに追加せず、DirtyPieceにいったん追加して、
	// そのあと、DirtyPiece::update()でevalListに追加する。このupdate()の時に組み換えなどの操作をしたいため。
	dp.set_state_info(st);
#endif

	kingSquare[BLACK] = kingSquare[WHITE] = SQ_NB;

	constexpr Square board_index[81] = { // Squareテーブル
		SQ_91, SQ_81, SQ_71, SQ_61, SQ_51, SQ_41, SQ_31, SQ_21, SQ_11,
		SQ_92, SQ_82, SQ_72, SQ_62, SQ_52, SQ_42, SQ_32, SQ_22, SQ_12,
		SQ_93, SQ_83, SQ_73, SQ_63, SQ_53, SQ_43, SQ_33, SQ_23, SQ_13,
		SQ_94, SQ_84, SQ_74, SQ_64, SQ_54, SQ_44, SQ_34, SQ_24, SQ_14,
		SQ_95, SQ_85, SQ_75, SQ_65, SQ_55, SQ_45, SQ_35, SQ_25, SQ_15,
		SQ_96, SQ_86, SQ_76, SQ_66, SQ_56, SQ_46, SQ_36, SQ_26, SQ_16,
		SQ_97, SQ_87, SQ_77, SQ_67, SQ_57, SQ_47, SQ_37, SQ_27, SQ_17,
		SQ_98, SQ_88, SQ_78, SQ_68, SQ_58, SQ_48, SQ_38, SQ_28, SQ_18,
		SQ_99, SQ_89, SQ_79, SQ_69, SQ_59, SQ_49, SQ_39, SQ_29, SQ_19
	};
	char *token = const_cast<char *>(sfen.c_str()); // stringを1文字ずつスキャンするポインタ
	bool promote = false; // 成り駒のフラグ
	Piece pi;
	int sq_count = 0; // 読んだ升のカウント
	while (sq_count < 81) { // 正規のsfen文字列でないとパースに失敗する
		if (*token == '\0') { // せめてもの安全装置
			return; // ここに来ることはない
		}
		if (*token >= 'A') { // 駒文字を想定
			pi = char_to_piece(*token); // Pieceの導出
			if (promote) {
				pi += PIECE_PROMOTE; // 成り駒にする
				promote = false; // フラグを戻しておく
			}
			put_piece(board_index[sq_count], pi);

#if defined(USE_FV38)
			PieceNumber piece_no =
				(pi == B_KING) ? PIECE_NUMBER_BKING : // 先手玉
				(pi == W_KING) ? PIECE_NUMBER_WKING : // 後手玉
				piece_no_count[raw_type_of(pi)]++; // それ以外
			evalList.put_piece(piece_no, board_index[sq_count], pi); // sqの升にpcの駒を配置する
#elif defined(USE_FV_VAR)
			if (type_of(pi) != KING)
			{
				dp.add_piece(board_index[sq_count], pi);
				dp.do_update(evalList);
				dp.clear();
				// DirtyPieceのBonaPieceを格納するバッファ、極めて小さいのでevalListに反映させるごとにクリアしておく。

				//Eval::print_eval_list(*this);
			}
#endif
			sq_count++; // 駒文字を読んだので升をカウントする
		}
		else if (*token >= '0') { // 48-57の数字を想定
			sq_count += (*token - CHAR0); // 升目の数だけカウントする
		}
		else if (*token == '+') { // 43
			promote = true; // '+'は次の駒が成駒であることを意味する
		}
		// 他の文字「/」などは意味をなさないのでスキップして次の文字を読む
		++token;
	}

	// put_piece()を使ったので更新しておく。
	// set_state()で駒種別のbitboardを参照するのでそれまでにこの関数を呼び出す必要がある。
	update_bitboards();

	// kingSquare[]の更新
	update_kingSquare();

	// --- 手番

	// 正規のsfen文字列ならこの直後に「スペース, 手番, スペース」が続くはずである
	++token;
	sideToMove = (*token == 'w' ? WHITE : BLACK);
	token += 2;

	// --- 手駒

	hand[BLACK] = hand[WHITE] = (Hand)0;

	// 手駒なし
	if (*token == '-') {
		token += 2; // "- "を飛ばす
	}
	else {
		int ct = 0; // 駒数のカウント
		bool under_ten = true;
		// 歩が20枚以上などは考慮されていない
		while (*token != ' ') { // スペースなら手駒終端に達したということ
			if (*token == '1' && under_ten) { // 1が現れるのは歩が10枚以上のときのみ (ここは十の位で通過する)
				under_ten = false;
			} else if (*token <= '9') { // ここは一の位で通過する
				ct = *token - '0';
				if (!under_ten) { // 十の位のフローを通過していれば
					ct += 10;
					under_ten = true; // フラグを元に戻す
				}
			}
			else { // 駒文字の想定
				if (ct == 0) { // 1は省略されていると考える
					ct = 1;
				}
				pi = char_to_piece(*token);
				add_hand(hand[color_of(pi)], type_of(pi), ct); // 手駒を加える

				// FV38などではこの個数分だけpieceListに突っ込まないといけない。
				for (int i = 0; i < ct; ++i)
				{
					Piece rpc = raw_type_of(pi);
#if defined (USE_FV38)
					PieceNumber piece_no = piece_no_count[rpc]++;
					ASSERT_LV1(is_ok(piece_no));
					evalList.put_piece(piece_no, color_of(pi), rpc, i);
#elif defined(USE_FV_VAR)
					dp.add_piece(color_of(pi), rpc, i);
					dp.do_update(evalList);
					dp.clear();
#endif
				}
				ct = 0; // 駒数カウントを元に戻す
			}
			++token;
		}
		++token;
	}

	// --- 手数(平手の初期局面からの手数)

	// gamePlyとして将棋所では(検討モードなどにおいて)ここで常に1が渡されている。
	// 検討モードにおいても棋譜上の手数を渡して欲しい気がするし、棋譜上の手数がないなら0を渡して欲しい気はする。
	// ここで渡されてきた局面をもとに探索してその指し手を定跡DBに登録しようとするときに、ここの手数が不正確であるのは困る。
	gamePly = 0;
	while (*token >= '0' && *token <= '9') { // 数字以外が出るまで読む
		if (gamePly > 1000000) { // 手数が発散してきたら適当な所でループを抜けておく
			break;
		}
		gamePly *= 10;
		gamePly += (*token - '0');
		++token;
	}

	// --- StateInfoの更新

	set_state(st);

	// --- evaluate

	st->materialValue = Eval::material(*this);
	Eval::compute_eval(*this);

	// --- effect

#if defined (LONG_EFFECT_LIBRARY)
	// 利きの全計算による更新
	LongEffect::calc_effect(*this);
#endif

	// --- validation

#if ASSERT_LV >= 3
	// これassertにしてしまうと、先手玉のいない局面や駒落ちの局面で落ちて困る。
	if (!is_ok(*this))
		std::cout << "info string Illigal Position?" << endl;
#endif

	thisThread = th;
}

constexpr int MAX_SFEN_SIZE = 128; // 128バイト (おそらくsfenはここに収まる)
constexpr char *sfen_ = "sfen ";
constexpr int PIECE_ENUM_NUM = 34; // 多分これぐらい
size_t piece_enum_str_size[PIECE_ENUM_NUM]; // 文字サイズを表す
char *piece_enum_str[PIECE_ENUM_NUM * 2]; // PIECE1個あたり2文字分を確保する

// Position::sfen_fast()関連の各種テーブルの初期化。
void position_sfen_init_helper(Piece pi, char* str, size_t length) {
	int x = static_cast<int>(pi);
	piece_enum_str_size[x] = length; // 長さ(1か2)を保存しておく
	memcpy(&piece_enum_str[2 * x], str, 2); // 常に2文字分をコピーする
}

void position_sfen_init() {
	memset(piece_enum_str, '\0', PIECE_ENUM_NUM * 2);
	position_sfen_init_helper(NO_PIECE, "-\0", 0);
	position_sfen_init_helper(B_PAWN, "P\0", 1);
	position_sfen_init_helper(B_PRO_PAWN, "+P", 2);
	position_sfen_init_helper(W_PAWN, "p\0", 1);
	position_sfen_init_helper(W_PRO_PAWN, "+p", 2);
	position_sfen_init_helper(B_LANCE, "L\0", 1);
	position_sfen_init_helper(B_PRO_LANCE, "+L", 2);
	position_sfen_init_helper(W_LANCE, "l\0", 1);
	position_sfen_init_helper(W_PRO_LANCE, "+l", 2);
	position_sfen_init_helper(B_KNIGHT, "N\0", 1);
	position_sfen_init_helper(B_PRO_KNIGHT, "+N", 2);
	position_sfen_init_helper(W_KNIGHT, "n\0", 1);
	position_sfen_init_helper(W_PRO_KNIGHT, "+n", 2);
	position_sfen_init_helper(B_SILVER, "S\0", 1);
	position_sfen_init_helper(B_PRO_SILVER, "+S", 2);
	position_sfen_init_helper(W_SILVER, "s\0", 1);
	position_sfen_init_helper(W_PRO_SILVER, "+s", 2);
	position_sfen_init_helper(B_GOLD, "G\0", 1);
	position_sfen_init_helper(W_GOLD, "g\0", 1);
	position_sfen_init_helper(B_BISHOP, "B\0", 1);
	position_sfen_init_helper(B_HORSE, "+B", 2);
	position_sfen_init_helper(W_BISHOP, "b\0", 1);
	position_sfen_init_helper(W_HORSE, "+b", 2);
	position_sfen_init_helper(B_ROOK, "R\0", 1);
	position_sfen_init_helper(B_DRAGON, "+R", 2);
	position_sfen_init_helper(W_ROOK, "r\0", 1);
	position_sfen_init_helper(W_DRAGON, "+r", 2);
	position_sfen_init_helper(B_KING, "K\0", 1);
	position_sfen_init_helper(W_KING, "k\0", 1);
	position_sfen_init_helper(PIECE_NB, "/\0", 1); // 「PIECE_NB」をRANKの境界文字として使う
}

constexpr char *hand_rb_b = "2RB2R2B"; // 手駒の飛角の文字列 (先手)
constexpr char *hand_rb_w = "2rb2r2b"; // 手駒の飛角の文字列 (後手)
constexpr uint32_t hand_rb_bitmask = 0x03300000; // 飛角のビットだけを残す
// Destにhandの飛角の文字列をrb_strからコピーする
// 返り値はコピーした文字数
inline int hand_rb_write(char* Dest, const Hand hand, const char *rb_str) {
	uint32_t rb = hand & hand_rb_bitmask;
	switch (rb) {
	case 0x00000000:
		return 0;
	case 0x00100000: // 角
		memcpy(Dest, &rb_str[2], 1);
		return 1;
	case 0x00200000: // 角2
		memcpy(Dest, &rb_str[5], 2);
		return 2;
	case 0x01000000: // 飛
		memcpy(Dest, &rb_str[1], 1);
		return 1;
	case 0x02000000: // 飛2
		memcpy(Dest, &rb_str[0], 2);
		return 2;
	case 0x01100000: // 飛角
		memcpy(Dest, &rb_str[1], 2);
		return 2;
	case 0x02100000: // 飛2角
		memcpy(Dest, &rb_str[0], 3);
		return 3;
	case 0x01200000: // 飛角2
		memcpy(Dest, &rb_str[4], 3);
		return 3;
	case 0x02200000: // 飛2角2
		memcpy(Dest, &rb_str[3], 4);
		return 4;
	}
	return 0; // ここに来ることはない
}
// Destにhandにmaskとshiftをして駒数を求めてから駒文字pieceを書き込む
// 返り値は書き込んだ文字数
inline int hand_gsnlp_write(char* Dest, const Hand hand, const uint32_t mask, const uint32_t shift, const char piece) {
	auto n = (hand & mask) >> shift;
	if (n == 0) { // その駒を持っていないので何もしない
		return 0;
	}
	if (n == 1) { // 駒文字を書き込む
		*Dest = piece;
		return 1;
	}
	if (n < 10) { // 駒数と駒文字を書き込む
		*Dest = CHAR0 + n;
		*(Dest + 1) = piece;
		return 2;
	}
	// 10-18枚以上の場合
	*Dest = CHAR0 + 1;
	*(Dest + 1) = CHAR0 + n - 10;
	*(Dest + 2) = piece;
	return 3;
}

// is_prefixがtrueのときは先頭に「sfen 」の接頭辞を付けます
const std::string Position::sfen_fast(bool is_prefix) const
{
	char *str = static_cast<char*>(_aligned_malloc(MAX_SFEN_SIZE, 64));
	char *p = str; // 文字列操作用のポインタ
	if (is_prefix) {
		memcpy(str, sfen_, 5); // 接頭辞
		p += 5;
	}
	// ここから盤面の文字列
	char empty_cnt = 0;
	int enum_int;
	size_t copy_size;
	const Piece piece[89] = { // sfen変換対象として処理していく駒の配列
		board[72], board[63], board[54], board[45], board[36], board[27], board[18], board[9], board[0], PIECE_NB,
		board[73], board[64], board[55], board[46], board[37], board[28], board[19], board[10], board[1], PIECE_NB,
		board[74], board[65], board[56], board[47], board[38], board[29], board[20], board[11], board[2], PIECE_NB,
		board[75], board[66], board[57], board[48], board[39], board[30], board[21], board[12], board[3], PIECE_NB,
		board[76], board[67], board[58], board[49], board[40], board[31], board[22], board[13], board[4], PIECE_NB,
		board[77], board[68], board[59], board[50], board[41], board[32], board[23], board[14], board[5], PIECE_NB,
		board[78], board[69], board[60], board[51], board[42], board[33], board[24], board[15], board[6], PIECE_NB,
		board[79], board[70], board[61], board[52], board[43], board[34], board[25], board[16], board[7], PIECE_NB,
		board[80], board[71], board[62], board[53], board[44], board[35], board[26], board[17], board[8]
	};
	for (auto i = 0; i < 89; ++i) {
		enum_int = static_cast<int>(piece[i]);
		copy_size = piece_enum_str_size[enum_int];
		if (copy_size) { // 何らかの駒が存在した
			if (empty_cnt) { // 0でないということは空升がいくつかあったということ
				*p = CHAR0 + empty_cnt; // 空升の数をコピーする
				++p;
				empty_cnt = 0;
			}
			memcpy(p, &piece_enum_str[enum_int * 2], copy_size);
			p += copy_size;
		}
		else { // 文字サイズが0ということは空升だったということ
			++empty_cnt;
		}
	}
	if (empty_cnt) { // 空升が残っていればフラッシュする
		*p = CHAR0 + empty_cnt;
		++p;
	}
	
	// ここから手番の文字列
	memcpy(p, sideToMove == WHITE ? " w " : " b ", 3);
	p += 3;
	
	// ここから手駒の文字列
	if (!(hand[BLACK] || hand[WHITE])) { // 手駒なしのとき
		memcpy(p, "- ", 2);
		p += 2;
	}
	else {
		p += hand_rb_write(p, hand[BLACK], hand_rb_b);
		p += hand_gsnlp_write(p, hand[BLACK], PIECE_BIT_MASK2[GOLD], PIECE_BITS[GOLD], 'G');
		p += hand_gsnlp_write(p, hand[BLACK], PIECE_BIT_MASK2[SILVER], PIECE_BITS[SILVER], 'S');
		p += hand_gsnlp_write(p, hand[BLACK], PIECE_BIT_MASK2[KNIGHT], PIECE_BITS[KNIGHT], 'N');
		p += hand_gsnlp_write(p, hand[BLACK], PIECE_BIT_MASK2[LANCE], PIECE_BITS[LANCE], 'L');
		p += hand_gsnlp_write(p, hand[BLACK], PIECE_BIT_MASK2[PAWN], PIECE_BITS[PAWN], 'P');
		p += hand_rb_write(p, hand[WHITE], hand_rb_w);
		p += hand_gsnlp_write(p, hand[WHITE], PIECE_BIT_MASK2[GOLD], PIECE_BITS[GOLD], 'g');
		p += hand_gsnlp_write(p, hand[WHITE], PIECE_BIT_MASK2[SILVER], PIECE_BITS[SILVER], 's');
		p += hand_gsnlp_write(p, hand[WHITE], PIECE_BIT_MASK2[KNIGHT], PIECE_BITS[KNIGHT], 'n');
		p += hand_gsnlp_write(p, hand[WHITE], PIECE_BIT_MASK2[LANCE], PIECE_BITS[LANCE], 'l');
		p += hand_gsnlp_write(p, hand[WHITE], PIECE_BIT_MASK2[PAWN], PIECE_BITS[PAWN], 'p');
		*p = ' ';
		++p;
	}

	// ここから手数の文字列
	int gp = gamePly;
	if (gp < 10) { // 10未満
		*p = CHAR0 + gp;
		++p;
		*p = '\0';
		return std::string(str);
	}
	div_t r = div(gp, 10);
	if (r.quot < 10) { // 100未満
		*p = CHAR0 + r.quot;
		++p;
		*p = CHAR0 + r.rem;
		++p;
		*p = '\0';
		return std::string(str);
	}
	if (r.quot < 100) { // 1000未満
		*(p + 2) = CHAR0 + r.rem;
		r = div(r.quot, 10);
		*p = CHAR0 + r.quot;
		++p;
		*p = CHAR0 + r.rem;
		p+=2;
		*p = '\0';
		return std::string(str);
	}
	// 手数が10000以上になることはないだろう
	*(p + 3) = CHAR0 + r.rem;
	r = div(r.quot, 10);
	*(p + 2) = CHAR0 + r.rem;
	r = div(r.quot, 10);
	*p = CHAR0 + r.quot;
	++p;
	*p = CHAR0 + r.rem;
	p += 3;
	*p = '\0';
	return std::string(str);
}
