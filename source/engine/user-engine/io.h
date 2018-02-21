#ifndef _USERIO_H_
#define _USERIO_H_

#include <fstream> 
#include <queue>
#include <mutex>
#include <thread>

class IO {
private:
	std::thread io_thread;
	std::ofstream of;
	bool file_close_flag;

	void init();
	void main();
	void file_write();
	void create_thread();
	void detach_thread();

public:
	std::mutex io_mtx; // ioのためのミューテックス
	std::queue<std::string> string_que[2]; // Stringを格納するキュー (複数個用意して書き換える)
	size_t write_que_num; // 書き込むキューの番号

	IO();
	~IO();

	void file_open();
	void file_close();
	void file_flash();

	void add_que(std::string str);
};

extern IO io;

#endif _USERIO_H_
