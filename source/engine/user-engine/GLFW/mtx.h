#ifndef _JSON_SCHEMA_H_
#ifndef _MTX_H_
#define _MTX_H_

#include <mutex>

extern std::mutex cmd_mtx; // cmdとの通信のためのミューテックス
extern std::mutex store_mtx; // Storeのキューにアクセスするためのミューテックス

#endif

#endif _MTX_H_
