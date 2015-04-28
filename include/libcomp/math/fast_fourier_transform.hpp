/**
 *  @file math/fast_fourier_transform.hpp
 */
#pragma once
#include <vector>
#include <complex>
#include <cmath>
#include <cassert>

namespace lc {

/**
 *  @defgroup fast_fourier_transform 高速フーリエ変換
 *  @ingroup  math
 *  @{
 */

/**
 *  @brief 高速フーリエ変換
 *    - 時間計算量: \f$ O(n \log{n}) \f$
 *
 *  @param[in] f        入力データ (要素数が2冪であること)
 *  @param[in] inverse  逆変換フラグ
 *  @return    f を離散フーリエ (逆) 変換した結果
 */
template <typename T>
std::vector<std::complex<T>> fast_fourier_transform(
	const std::vector<std::complex<T>> &f, bool inverse)
{
	const int n = f.size(), mask = n - 1;
	assert((n & (n - 1)) == 0);
	const T pi = atan(1.0) * 4.0;
	std::vector<T> sin_table(n);
	const int cos_offset = (inverse ? 3 : 1) * (n >> 2);
	for(int i = 0; i < n; ++i){
		const int k = (inverse ? ((n - i) & mask) : i);
		sin_table[k] = sin((2.0 * pi / n) * i);
	}
	std::vector<std::complex<T>> result(f);
	for(int i = 0, j = 1; i < n - 1; ++j){
		for(int k = n >> 1; k > (i ^= k); k >>= 1);
		if(j < i){ swap(result[i], result[j]); }
	}
	for(int mh = 1; mh + mh <= n; mh += mh){
		const int m = (mh << 1);
		int irev = 0;
		for(int i = 0; i < n; i += m){
			const std::complex<T> w(
				sin_table[(irev + cos_offset) & mask], sin_table[irev]);
			for(int k = n >> 2; k > (irev ^= k); k >>= 1);
			for(int j = i; j < mh + i; ++j){
				const int k = j + mh;
				const std::complex<T> x = result[j] - result[k];
				result[j] += result[k];
				result[k] = w * x;
			}
		}
	}
	if(inverse){
		const T r = 1.0 / n;
		for(int i = 0; i < n; ++i){ result[i] *= r; }
	}
	return result;
}

/**
 *  @}
 */

}

