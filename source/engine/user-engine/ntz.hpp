#ifndef _NTZ_H_
#define _NTZ_H_

#include <cstdint>

//8bit版
static constexpr auto magic8 = 0x1DU;
static constexpr int ntz_table8[15] = { 8,  0, -1,  1,  6, -1, -1,  2,  7, -1,  5, -1, -1,  4,  3 };
static constexpr int nlz_table8[15] = { 8,  7, -1,  6,  1, -1, -1,  5,  0, -1,  2, -1, -1,  3,  4 };
//16bit版
static constexpr auto magic16 = 0x0F2DU;
static constexpr int ntz_table16[31] = {
	16,  0, -1,  1, -1,  8, -1,  2, 14, -1, -1,  9, -1, 11, -1,  3,
	15, -1,  7, -1, 13, -1, 10, -1, -1,  6, 12, -1,  5, -1,  4,
};
static constexpr int nlz_table16[31] = {
	16, 15, -1, 14, -1,  7, -1, 13,  1, -1, -1,  6, -1,  4, -1, 12,
	0, -1,  8, -1,  2, -1,  5, -1, -1,  9,  3, -1, 10, -1, 11,
};
// 32bit版
static constexpr auto magic32 = 0x07C56E99U;
static constexpr int ntz_table32[63] = {
	32,  0, -1,  1, -1, 10, -1,  2, 29, -1, 11, -1, 25, -1, -1,  3,
	30, -1, -1, 23, -1, 12, 14, -1, -1, 26, -1, 16, -1, 19, -1,  4,
	31, -1,  9, -1, 28, -1, 24, -1, -1, 22, -1, 13, -1, 15, 18, -1,
	-1,  8, 27, -1, 21, -1, -1, 17,  7, -1, 20, -1,  6, -1,  5
};
static constexpr int nlz_table32[63] = {
	32, 31, -1, 30, -1, 21, -1, 29,  2, -1, 20, -1,  6, -1, -1, 28,
	1, -1, -1,  8, -1, 19, 17, -1, -1,  5, -1, 15, -1, 12, -1, 27,
	0, -1, 22, -1,  3, -1,  7, -1, -1,  9, -1, 18, -1, 16, 13, -1,
	-1, 23,  4, -1, 10, -1, -1, 14, 24, -1, 11, -1, 25, -1, 26
};
// 64bit版
static constexpr auto magic64 = 0x03F0A933ADCBD8D1ULL;
static constexpr int ntz_table64[127] = {
	64,  0, -1,  1, -1, 12, -1,  2, 60, -1, 13, -1, -1, 53, -1,  3,
	61, -1, -1, 21, -1, 14, -1, 42, -1, 24, 54, -1, -1, 28, -1,  4,
	62, -1, 58, -1, 19, -1, 22, -1, -1, 17, 15, -1, -1, 33, -1, 43,
	-1, 50, -1, 25, 55, -1, -1, 35, -1, 38, 29, -1, -1, 45, -1,  5,
	63, -1, 11, -1, 59, -1, 52, -1, -1, 20, -1, 41, 23, -1, 27, -1,
	-1, 57, 18, -1, 16, -1, 32, -1, 49, -1, -1, 34, 37, -1, 44, -1,
	-1, 10, -1, 51, -1, 40, -1, 26, 56, -1, -1, 31, 48, -1, 36, -1,
	9, -1, 39, -1, -1, 30, 47, -1,  8, -1, -1, 46,  7, -1,  6,
};
static constexpr int nlz_table64[127] = {
	64, 63, -1, 62, -1, 51, -1, 61,  3, -1, 50, -1, -1, 10, -1, 60,
	2, -1, -1, 42, -1, 49, -1, 21, -1, 39,  9, -1, -1, 35, -1, 59,
	1, -1,  5, -1, 44, -1, 41, -1, -1, 46, 48, -1, -1, 30, -1, 20,
	-1, 13, -1, 38,  8, -1, -1, 28, -1, 25, 34, -1, -1, 18, -1, 58,
	0, -1, 52, -1,  4, -1, 11, -1, -1, 43, -1, 22, 40, -1, 36, -1,
	-1,  6, 45, -1, 47, -1, 31, -1, 14, -1, -1, 29, 26, -1, 19, -1,
	-1, 53, -1, 12, -1, 23, -1, 37,  7, -1, -1, 32, 15, -1, 27, -1,
	54, -1, 24, -1, -1, 33, 16, -1, 55, -1, -1, 17, 56, -1, 57,
};


template<std::size_t size>
struct ntz_traits;

template<>
struct ntz_traits<1>
{
	using type = std::uint8_t;
	static constexpr int shift = 4;
	static constexpr auto magic = magic8;
	static constexpr auto ntz_table = ntz_table8;
	static constexpr auto nlz_table = nlz_table8;
};

template<>
struct ntz_traits<2>
{
	using type = std::uint16_t;
	static constexpr int shift = 11;
	static constexpr auto magic = magic16;
	static constexpr auto ntz_table = ntz_table16;
	static constexpr auto nlz_table = nlz_table16;
};

template<>
struct ntz_traits<4>
{
	using type = std::uint32_t;
	static constexpr int shift = 26;
	static constexpr auto magic = magic32;
	static constexpr auto ntz_table = ntz_table32;
	static constexpr auto nlz_table = nlz_table32;
};

template<>
struct ntz_traits<8>
{
	using type = std::uint64_t;
	static constexpr int shift = 57;
	static constexpr auto magic = magic64;
	static constexpr auto ntz_table = ntz_table64;
	static constexpr auto nlz_table = nlz_table64;
};

// SFINAEに必要なもろもろ
extern void* enabler;
template<bool condition, typename T = void>
using enable_if_type = typename std::enable_if<condition, T>::type;
template<typename T>
using make_unsigned_type = typename std::make_unsigned<T>::type;

// unsigned型のNTZ。例の黒魔術
template<typename T, enable_if_type<std::is_unsigned<T>{}>*& = enabler>
inline constexpr int ntz(T val) noexcept {
	using tr = ntz_traits<sizeof(T)>;
	using type = typename tr::type;
	return tr::ntz_table[static_cast<type>(tr::magic*static_cast<type>(val&-val)) >> tr::shift];
}

// signdな型はunsignedな型にキャストする
template<typename T, enable_if_type<std::is_signed<T>{}>*& = enabler>
inline constexpr int ntz(T val) noexcept {
	return ntz(static_cast<make_unsigned_type<T>>(val));
}

// enum型は同じサイズの整数型にキャストする
template<typename T, enable_if_type<std::is_enum<T>{}>*& = enabler>
inline constexpr int ntz(T val) noexcept {
	return ntz(static_cast<typename ntz_traits<sizeof(T)>::type>(val));
}

// ポインタ型はuintptr_tにキャストして整数型として扱う
template<typename T>
inline int ntz(T* val) noexcept { return ntz(reinterpret_cast<std::uintptr_t>(val)); }

// bool型は1bitとみなす
inline constexpr int ntz(bool val) noexcept { return val ? 0 : 1; }

// nullptrは0bitとみなす。多分使わないけど
inline constexpr int ntz(std::nullptr_t) noexcept { return 0; }

#endif _NTZ_H_
