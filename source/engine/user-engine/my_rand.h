#ifndef _MYRAND_H_
#define _MYRAND_H_

#include <random>
#include "./SFMT/SFMT.h"

//SIMD利用の方が大概速いので
//#define CUDA

#ifdef CUDA
#include <curand.h>
#endif

// CACHE_RAND_NUMBERの数だけu32乱数を生成しておく
// 100万個程度ならリリース環境で10ミリ秒とかからない
constexpr uint32_t CACHE_RAND_NUMBER = 16384000; // 何でもいいが、キリの良い数(1024の倍数など)にしておく
constexpr uint32_t CACHE_RAND_NUMBER_SFMT = CACHE_RAND_NUMBER / 2; // 内部的にu64乱数を発生させるので数は半分になる

class MyRand
{
private:
	int counter = CACHE_RAND_NUMBER; // 次の乱数発生で使われる乱数のインデックス(減算式)
	void gen(); // CACHE_RAND_NUMBER個のu32乱数を生成する
	sfmt_t sfmt; // 乱数生成器
	uint32_t *ptr; // 乱数を格納する配列を指し示すポインタ(参照用に32bit)
	uint64_t *sfmt_ptr; // 乱数を格納する配列を指し示すポインタ(乱数生成器用に64bit)
#ifdef CUDA
	curandGenerator_t g;
	uint32_t* gpu_ptr;
#endif
public:
	MyRand();
	~MyRand();
	uint32_t rand(); // キャッシュした乱数を1つ取り出す
};

#endif _MYRAND_H_
