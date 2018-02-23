#include "../../shogi.h"
#ifdef MATE_ENGINE

#include <atomic>
#include "../../position.h"

std::string mate_search_func(const std::string &sfen);
std::string mate_search_func2(Position &pos_);

#endif
