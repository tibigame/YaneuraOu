#include "io.h"

constexpr char* filename = "result.txt"; // 出力ファイル名
constexpr size_t buffer = 10000; // キューに何個以上溜まっていたら書き込み処理を行うか

// ファイル入出力へのグローバルなアクセス
IO io;

IO::IO() {
}

IO::~IO() {
}

void IO::init() {
	write_que_num = 0; // 最初のキュー番号は0
	file_close_flag = false;
}

void IO::create_thread() {
	io_thread = std::thread([&] { main(); });
}

void IO::detach_thread() {
	io_thread.detach();
}

void IO::main() {
	while (!file_close_flag) {
		if (string_que[write_que_num].size() >= buffer) {
			file_write();
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 適度にスリープ
	}
	of.close();
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

void IO::file_open() {
	init();
	create_thread();
	of.open(filename, std::ios::out);
}
void IO::file_close() {
	file_close_flag = true;
	io_thread.join();
}
void IO::file_write() {
	io_mtx.lock(); // ioのミューテックスを取得する
	size_t old_que_number = write_que_num; // 現在のキュー番号を保存しておく
	write_que_num = write_que_num == 0 ? 1 : 0; // キュー番号を入れ替える
	io_mtx.unlock(); // もう他のスレッドからは前のキュー番号は干渉しないのでミューテックスを解放する
	std::string temp; // キューから取り出した文字列を組み立てるための一時オブジェクト
	while (!string_que[old_que_number].empty()) {
		temp += string_que[old_que_number].front() + "\n"; // 先頭要素を改行を加えて追加していく
		string_que[old_que_number].pop(); // 先頭要素を削除
	}
	of << temp.c_str();
}

void IO::file_flash() {
	std::lock_guard<std::mutex> lock(io_mtx); // ioのミューテックスを取得する
	std::string temp; // キューから取り出した文字列を組み立てるための一時オブジェクト
	while (!string_que[0].empty()) {
		temp += string_que[0].front() + "\n";
		string_que[0].pop();
	}
	while (!string_que[1].empty()) {
		temp += string_que[1].front() + "\n";
		string_que[1].pop();
	}
	of << temp.c_str();
}

void IO::add_que(std::string &&str) {
	std::lock_guard<std::mutex> lock(io_mtx); // ioのミューテックスを取得する
	string_que[write_que_num].push(std::move(str)); // 現在のキュー番号のところに追加する
}
