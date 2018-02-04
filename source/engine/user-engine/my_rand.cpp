#include "my_rand.h"
#include <iostream>

MyRand myrand;

MyRand::MyRand() {
	std::random_device rd; // ハードウェア乱数で初期化
	std::mt19937_64 mt(rd()); // 最初の種としてシステムのメルセンヌ・ツイスタで生成する
	sfmt_init_gen_rand(&sfmt, static_cast<uint32_t>(mt())); // ライブラリのメルセンヌ・ツイスタクラスの初期化
	
#ifdef CUDA
	cudaMalloc((void**)&gpu_ptr, CACHE_RAND_NUMBER * sizeof(uint32_t)); // GPUのメモリ確保は非常に重い
	curandCreateGenerator(&g, CURAND_RNG_PSEUDO_MTGP32); // 乱数タイプ設定
#endif
	sfmt_ptr = static_cast<uint64_t*>(_aligned_malloc(4 * CACHE_RAND_NUMBER, 64)); // キャッシュの数だけメモリを確保する
	ptr = (uint32_t *)&sfmt_ptr[0]; // 参照用のポインタにも確保したアドレスを渡す
	gen(); // 初回の乱数生成
};

MyRand::~MyRand() {
	_aligned_free(sfmt_ptr); // 確保したメモリを開放する
#ifdef CUDA
	curandDestroyGenerator(g);
	cudaFree(gpu_ptr); // GPU用のメモリ解放
#endif
}

// 乱数を生成する
void MyRand::gen() {
#ifdef CUDA
	curandGenerate(g, gpu_ptr, CACHE_RAND_NUMBER); // 乱数生成
	cudaMemcpy(&ptr[0], gpu_ptr, CACHE_RAND_NUMBER * sizeof(uint32_t), cudaMemcpyDeviceToHost); // CPUにメモリコピー
#else
	sfmt_fill_array64(&sfmt, sfmt_ptr, CACHE_RAND_NUMBER_SFMT);
#endif
	++gen_count;
};

// u32乱数を返します (キャッシュされている場合はテーブルから返すだけなので高速です)
uint32_t MyRand::rand() {
	if (!counter) { // カウンター=0なら乱数を使い切った
		gen(); // 乱数を再生成する
		counter = CACHE_RAND_NUMBER; // カウンターを元に戻す
	}
	return ptr[--counter]; // カウンターを減らしつつインデックスを参照する
};

// キャッシュした乱数を1つ取り出し、0～m-1までにして返す
uint32_t MyRand::rand_m(const uint32_t mod) {
	// 剰余は遅いので、rand() % modの代替として乗算 + シフトを使う
	return (uint64_t)rand() * (uint64_t)mod >> 32;
};

constexpr uint32_t rand_b_constexpr(double p) {
	return static_cast<uint32_t>(p * UINT_MAX);
};
// キャッシュした乱数を1つ取り出し、確率pでtrueを返す
bool MyRand::rand_b(double p) {
	return rand() <= rand_b_constexpr(p);
};

// 使用した乱数の数を返します
uint64_t MyRand::get_rand_count() {
	return gen_count * CACHE_RAND_NUMBER - counter;
};

// 使用した乱数の個数の情報を出力します
void MyRand::cout() {
	uint64_t n = get_rand_count();
	if (n < 10000) {
		std::cout << "乱数を" << n << "個使用しました" << std::endl;
		return;
	}
	double d = n / 10000.0;
	if (d < 10000) {
		std::cout << "乱数を" << d << "万個使用しました" << std::endl;
		return;
	}
	d /= 10000.0;
	if (d < 10000) {
		std::cout << "乱数を" << d << "億個使用しました" << std::endl;
		return;
	}
	d /= 10000.0;
	if (d < 10000) {
		std::cout << "乱数を" << d << "兆個使用しました" << std::endl;
		return;
	}
	d /= 10000.0;
	std::cout << "乱数を" << d << "京個使用しました" << std::endl;
};
