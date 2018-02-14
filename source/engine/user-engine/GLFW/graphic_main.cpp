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

void mouse_callback(GLFWwindow* window, int button, int action, int mods) {
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	gui.mouse(xpos, ypos);
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
	glfwSetMouseButtonCallback(window, mouse_callback);
	gl_string->font_init(); // フォントの読み込みと駒文字などのフォントキャッシュの生成
}

// OpenGLのメインループ
void Gui::main() {
	init();
	store.init(gl_string);
	
	// デバッグ用の局面を出力しておく
	store.state.pos_.set_fast_sfenonly("l4+R1n1/1pgk5/5+P3/4Sp1+P1/1N7/1Kp1P4/4SP1+p1/9/4+p1+sL1 w BGSNPrb2gn2l8p 1240");
	store.state.is_render_pos = true;

	while (!glfwWindowShouldClose(window)) {
		draw_loop_init();
		store.exe_action_que();
		render(store.provider());

		glFlush();
		glfwSwapBuffers(window);
		// いらないかもだけど、リアルタイム更新も不要なので適当にスリーブしとく
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		glfwWaitEvents();
	}
}

// マウスコールバックの処理
void Gui::mouse(const double xpos, const double ypos) {
	if (xpos < 100) {
		gui.set_info(u8"xpos<100");
	}
	else {
		gui.set_info(u8"xpos>=100");
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
