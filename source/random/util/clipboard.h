#ifndef _CLIPBOARD_H_
#define _CLIPBOARD_H_

// Windowsでクリップボードを扱う関数
#define WINDOWS

#ifdef WINDOWS
#include "windows.h"
#endif

// クリップボードにテキストデータをコピーする関数
bool SetClipboardText(const char *char_str); // WINDOWSが未定義なら常にfalseが返る

#endif _CLIPBOARD_H_
