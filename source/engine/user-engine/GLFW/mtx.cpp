#include "mtx.h"

std::mutex cmd_mtx; // cmdとの通信のためのミューテックス
std::mutex store_mtx; // Storeのキューにアクセスするためのミューテックス
