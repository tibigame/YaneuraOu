#include <math.h>
#include <stdio.h>
#include <string>
#include "graphic_primitive.h"

// OpenGLをラッパーして基礎的なグラフィック描写を行う関数群です

#ifdef GLFW3

constexpr double MATH_PI = 3.141592653589793238;

struct Float4x4 {
	float v[16];
};

// テクスチャをメモリに読み込みます
// TODO: リソース解放や読み込みエラーなどは考慮されていません
constexpr int TEXWIDTH = 960; // テクスチャの幅
constexpr int TEXHEIGHT = 960; // テクスチャの高さ
static const char *texture1 = "resource/board.raw"; /* テクスチャファイル名 */
void setupTexture(GLuint &textureID, const char *file)
{
	// 画像データのロード
	GLubyte texture[TEXHEIGHT][TEXWIDTH][3]; // テクスチャの読み込みに使う配列
	FILE *fp;
	fopen_s(&fp, texture1, "rb");
	if (fp) {
		fread(texture, sizeof texture, 1, fp); // テクスチャ画像の読み込み
		fclose(fp);
	}
	else {
		perror(texture1);
	}
	// ここまでくればとりあえず読み込みは成功しているはず

	glGenTextures(1, &textureID); // 一つのOpenGLテクスチャを作ります。

	// 新しく作ったテクスチャを"バインド"します。これ以降の関数はすべてこのテクスチャを変更します。
	glBindTexture(GL_TEXTURE_2D, textureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // テクスチャ画像はバイト単位に詰め込まれている

	/* テクスチャの割り当て */
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, TEXWIDTH, TEXHEIGHT,
		GL_RGB, GL_UNSIGNED_BYTE, texture);

	/* テクスチャを拡大・縮小する方法の指定 */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	//return textureID; // 作成したテクスチャのIDを返します。
}

// メインループに入る前に1度だけ実行する処理を書きます
void draw_init(GLuint &textureID) {
	glMatrixMode(GL_PROJECTION); // プロジェクションモードに設定
	glLoadIdentity(); // スクリーン座標を初期化
	//glEnable(GL_DEPTH_TEST);// デプスバッファの有効化
	glEnable(GL_CULL_FACE);// カリングの有効化
	glCullFace(GL_BACK); // カリングで表面のみを描写するようにする
	glEnable(GL_LIGHTING);// ライティングの有効化
	glEnable(GL_LIGHT0);// 光源0 を有効化
	glEnable(GL_BLEND); // アルファブレンドの有効化
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // アルファブレンドの設定
	glClearColor(0.9, 0.95, 1, 0);// glClear() で使用する色
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 画面の初期化
	setupTexture(textureID, texture1); // テクスチャの初期設定
}

// メインループの最初に必ず実行する処理を書きます
void draw_loop_init() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 画面をクリア
	glLoadIdentity(); // 変換行列の初期化
	glOrtho(view_left, view_right, view_bottom, view_top, -1, 1); // ビューの座標設定
}

// 線を描きます
void draw_line(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLfloat width,
	GLfloat *mat_ambient, GLfloat *mat_diffuse, GLfloat *mat_specular, GLfloat z) {
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glLineWidth(width);
	glBegin(GL_LINES);
	glVertex3f(x1, y1, z);
	glVertex3f(x2, y2, z);
	glEnd();
}

// 長方形を描きます
// display_xは画面右方向、display_yは画面下方向への平行移動、degreeは長方形の中心での回転で360.0で一周する
// draw_rect_exの方を使うこと
void draw_rect(GLfloat left, GLfloat bottom, GLfloat right, GLfloat top,
	GLfloat *mat_ambient, GLfloat *mat_diffuse, GLfloat *mat_specular, GLfloat z,
	GLfloat display_x, GLfloat display_y, double degree, bool is_texture, GLuint textureID) {
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	bool is_move = display_x != 0.0 || display_y != 0.0 || degree != 0.0; // 何らかの移動が発生する
	if (is_move) {
		glPushMatrix();

		GLfloat center_x = (left + right) * 0.5f;
		GLfloat center_y = (top + bottom) * 0.5f;
		// 原点に平行移動
		glTranslatef(-center_x, -center_y, 0.f);

		// 回転する
		glRotatef(static_cast<float>(degree), 0.f, 0.f, 1.f);

		// 平行移動
		glTranslatef(center_x + display_x, center_y - display_y, 0.f);

	}
	if (is_texture) { // テクスチャマッピング
		glBindTexture(GL_TEXTURE_2D, textureID);
		glEnable(GL_TEXTURE_2D);
		glBegin(GL_TRIANGLE_FAN);
		glTexCoord2f(0.0f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glVertex3f(left, top, z);
		glTexCoord2f(0.0f, 1.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glVertex3f(left, bottom, z);
		glTexCoord2f(1.0f, 1.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glVertex3f(right, bottom, z);
		glTexCoord2f(1.0f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glVertex3f(right, top, z);
		glEnd();
		glDisable(GL_TEXTURE_2D);
	}
	else {
		glBegin(GL_TRIANGLE_FAN);
		glVertex3f(left, top, z);
		glVertex3f(left, bottom, z);
		glVertex3f(right, bottom, z);
		glVertex3f(right, top, z);
		glEnd();

	}
	if (is_move) {
		glFlush();
		glPopMatrix();
	}

}

constexpr double step_num = 17.0; // 円を分割して描写する (少なくても円に見えるように、素数にしておく)
constexpr double step_arg = 2.0 * MATH_PI / step_num;
// 円を描きます
void draw_circle(GLfloat x, GLfloat y, GLfloat r,
	GLfloat *mat_ambient, GLfloat *mat_diffuse, GLfloat *mat_specular, GLfloat z) {
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(x, y); // 中心
	for (double arg = 0.f; arg < 2.0 * MATH_PI; arg += step_arg)
	{
		glVertex3f(static_cast<float>(x + r * cos(arg)), static_cast<float>(y + r * sin(arg)), z);
	}
	glEnd();
}

void draw_rect_ex(GLfloat left, GLfloat bottom, GLfloat right, GLfloat top,
	GLfloat *mat_ambient, GLfloat *mat_diffuse, GLfloat *mat_specular, GLfloat z,
	GLfloat display_x, GLfloat display_y, double degree, GLfloat scale, GLfloat string_offset,
	bool is_texture, GLuint textureID) {
	glPushMatrix();

	// OpenGLは行列の適応順が逆順
	// 文字がセンタリングされないことの調整
	if (degree > 90) {
		glTranslated(-string_offset, string_offset, 0.f); //平行移動
	}
	else {
		glTranslated(string_offset, -string_offset, 0.f); //平行移動
	}
	glTranslated(display_x, display_y, 0.f); //平行移動
	glRotated(degree, 0.0, 0.0, 1.0); //Z軸回転
	glScalef(1.1f, 1.1f, 1.f); // 拡大

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);

	if (is_texture) { // テクスチャマッピング
		glBindTexture(GL_TEXTURE_2D, textureID);
		glEnable(GL_TEXTURE_2D);
		glBegin(GL_TRIANGLE_FAN);
		glTexCoord2f(0.0f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glVertex3f(left, top, z);
		glTexCoord2f(0.0f, 1.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glVertex3f(left, bottom, z);
		glTexCoord2f(1.0f, 1.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glVertex3f(right, bottom, z);
		glTexCoord2f(1.0f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glVertex3f(right, top, z);
		glEnd();
		glDisable(GL_TEXTURE_2D);
	}
	else {
		glBegin(GL_TRIANGLE_FAN);
		glVertex3f(left, top, z);
		glVertex3f(left, bottom, z);
		glVertex3f(right, bottom, z);
		glVertex3f(right, top, z);
		glEnd();
	}
	glPopMatrix();
}

// 駒形の五角形を描きます
void draw_pentagon_ex(GLfloat left, GLfloat bottom, GLfloat right, GLfloat top,
	GLfloat *mat_ambient, GLfloat *mat_diffuse, GLfloat *mat_specular, GLfloat z,
	GLfloat display_x, GLfloat display_y, double degree, GLfloat scale, GLfloat string_offset,
	bool is_texture, GLuint textureID) {
	glPushMatrix();

	// OpenGLは行列の適応順が逆順
	// 文字がセンタリングされないことの調整
	if (degree > 90) {
		glTranslated(-string_offset, string_offset, 0.f); //平行移動
	}
	else {
		glTranslated(string_offset, -string_offset, 0.f); //平行移動
	}
	glTranslated(display_x, display_y, 0.f); //平行移動
	glRotated(degree, 0.0, 0.0, 1.0); //Z軸回転
	glScalef(1.1f, 1.1f, 1.f); // 拡大

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);

	GLfloat x[5], y[5]; // 上部から左回りに頂点を指定する
	x[0] = (left + right) * 0.5f;
	y[0] = top;
	x[1] = left * 0.8f + right * 0.2f;
	y[1] = top * 0.8f + bottom * 0.2f;
	x[2] = left * 0.9f + right * 0.1f;
	y[2] = bottom;
	x[3] = left * 0.1f + right * 0.9f;
	y[3] = bottom;
	x[4] = left * 0.2f + right * 0.8f;
	y[4] = top * 0.8f + bottom * 0.2f;

	if (is_texture) { // テクスチャマッピング
		// TODO: glTexCoord2fを適切に設定すること
		glBindTexture(GL_TEXTURE_2D, textureID);
		glEnable(GL_TEXTURE_2D);
		glBegin(GL_TRIANGLE_FAN);
		glTexCoord2f(0.5f, 0.f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glVertex3f(x[0], y[0], z);
		glTexCoord2f(0.8f, 0.2f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glVertex3f(x[1], y[1], z);
		glTexCoord2f(1.0f, 1.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glVertex3f(x[2], y[2], z);
		glTexCoord2f(0.f, 1.f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glVertex3f(x[3], y[3], z);
		glTexCoord2f(0.2f, 0.2f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glVertex3f(x[4], y[4], z);
		glEnd();
		glDisable(GL_TEXTURE_2D);
	}
	else {
		glBegin(GL_TRIANGLE_FAN);
		glVertex3f(x[0], y[0], z);
		glVertex3f(x[1], y[1], z);
		glVertex3f(x[2], y[2], z);
		glVertex3f(x[3], y[3], z);
		glVertex3f(x[4], y[4], z);
		glEnd();
	}
	glPopMatrix();
}

#endif
