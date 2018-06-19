#include "./clipboard.h"

// Windowsでクリップボードを扱う関数

bool SetClipboardText(const char *char_str)
{
#ifdef WINDOWS
	size_t buf_size;
	char *buf;
	HANDLE hMem;

	buf_size = strlen(char_str) + 1; // 確保するメモリのサイズを計算する
	hMem = GlobalAlloc(GMEM_SHARE | GMEM_MOVEABLE, buf_size); // 移動可能な共有メモリを確保する
	if (!hMem) return false;

	buf = (char *)GlobalLock(hMem); // 確保したメモリをロックし，アクセス可能にする
	if (buf)
	{
		strcpy_s(buf, buf_size, char_str); // 文字列を複写する
		GlobalUnlock(hMem); // メモリのロックを解除する
		if (OpenClipboard(NULL))
		{
			EmptyClipboard(); // クリップボード内の古いデータを解放する
			SetClipboardData(CF_TEXT, hMem); // クリップボードに新しいデータを入力する
			CloseClipboard();
			return true;
		}
	}
#endif
	return false;
}
