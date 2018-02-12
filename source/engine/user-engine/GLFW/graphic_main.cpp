#include "graphic_main.h"
#include "graphic_primitive.h"
#include "graphic_object.h"
#include "graphic_string.h"

constexpr char *window_title(u8"将棋のランダム局面生成 with やねうら王");

// グラフィックへのグローバルなアクセス
Gui gui;

// コンストラクタとデストラクタ
Gui::Gui() {
#ifdef GLFW3
	gl_string = new GlString;
#endif
}

Gui::~Gui() {
#ifdef GLFW3
	delete gl_string;
	glfwDestroyWindow(window);
	glfwTerminate();
#endif
}

// OpenGLの処理は呼んだスレッドから限定なので描写用のスレッドを新規作成する
void Gui::create_thread() {
#ifdef GLFW3
	gui_thread = std::thread([&] { main(); });
#endif
}

// コールバック系の処理
#ifdef GLFW3
void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

// 初期化処理
// 主にウィンドウの属性指定とコールバックの設定
void Gui::init() {
	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
		exit(EXIT_FAILURE);
	window = glfwCreateWindow(window_width, window_height, window_title, NULL, NULL); // サイズを指定して生成
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwSetWindowPos(window, 1280, 280); // 位置を指定
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	gl_string->font_init(); // フォントの読み込みと駒文字などのフォントキャッシュの生成
}

// OpenGLのメインループ
void Gui::main() {
	//P.set("knsg1g1nl/l3r1s2/pppp1pbpp/6p2/4p2P1/P1P2SP2/1PSPPP2P/1BK4R1/LN1G1G1NL w - 24");
	init();
	draw_init(textureID_shogiboard);
	while (!glfwWindowShouldClose(window)) {
		draw_loop_init();
		draw_shogiboard(textureID_shogiboard);
		draw_shogiboard_rank_file_number(gl_string);
		draw_info(info, gl_string);

		if (is_render_pos) { // pos_が送られてくると盤面が表示される
			draw_board(pos_, gl_string);
			draw_hand(pos_, gl_string);
			draw_teban(pos_, gl_string);
			
		}
		else { // デバッグ用に適当に駒を出力しておく
			board_piece_string(B_KNIGHT, SQ_89, gl_string, BLACK);
			board_piece_string(B_KNIGHT, SQ_29, gl_string, BLACK);
			board_piece_string(B_LANCE, SQ_19, gl_string, BLACK);
			board_piece_string(W_KNIGHT, SQ_81, gl_string, WHITE);
			board_piece_string(W_KNIGHT, SQ_21, gl_string, WHITE);
			board_piece_string(B_GOLD, SQ_49, gl_string, BLACK);
			board_piece_string(B_SILVER, SQ_38, gl_string, BLACK);
			board_piece_string(B_ROOK, SQ_55, gl_string, BLACK);
			board_piece_string(W_HORSE, SQ_73, gl_string, WHITE);
			board_piece_string(W_DRAGON, SQ_88, gl_string, WHITE);
			board_piece_string(W_PRO_PAWN, SQ_99, gl_string, WHITE);
			board_piece_string(B_PRO_KNIGHT, SQ_12, gl_string, BLACK);
			board_piece_string(B_PRO_LANCE, SQ_42, gl_string, BLACK);
			board_piece_string(W_PRO_SILVER, SQ_27, gl_string, WHITE);
			board_piece_string(W_PRO_PAWN, SQ_26, gl_string, WHITE);
			board_piece_string(B_PAWN, SQ_54, gl_string, BLACK);
		}

		

		/*
		draw_pentagon_ex(0.f, 0.f, 0.5f, 0.5f,
		conv_GL_color(0, 0, 0, 255), conv_GL_color(255, 255, 255, 255), conv_GL_color(0, 0, 0, 255), 0.6f,
		0.f, 0.f, 0.0, 1.f, 0.12f);
		GLfloat f_size = 0.6f;
		int num = -1;
		GLfloat display_x = board_border + hand_inner_margin - 0.13f;
		GLfloat display_y = -0.229f;

		draw_pentagon_ex(0.f, 0.f, f_size, f_size,
			conv_GL_color(0, 0, 0, 255), conv_GL_color(0, 0, 0, 255), conv_GL_color(0, 0, 0, 255), 0.6f,
			display_x, display_y, 0.0, 1.f, 0.12f);
		*/
		

		glFlush();
		glfwSwapBuffers(window);
		// いらないかもだけど、リアルタイム更新も不要なので適当にスリーブしとく
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		glfwWaitEvents();
	}
}

// 新しいPositionをコピーしてセットする
void Gui::set_pos(const Position &p) {
	// コピーコンストラクタがなかったので必要な分だけ無理矢理コピー
	// GUI側ではせいぜいsfen変換ぐらいしかしないので問題ないだろう
	for (auto i = 0; i < 82; ++i) {
		pos_.board[i] = p.board[i];
	}
	pos_.hand[BLACK] = p.hand[BLACK];
	pos_.hand[WHITE] = p.hand[WHITE];
	pos_.sideToMove = p.sideToMove;
	pos_.gamePly = p.gamePly;
	pos_.kingSquare[BLACK] = p.kingSquare[BLACK];
	pos_.kingSquare[WHITE] = p.kingSquare[WHITE];
	is_render_pos = true;
}

// 新しいstringをコピーしてセットする
void Gui::set_info(const std::string &str) {
	info = str;
}

#endif
