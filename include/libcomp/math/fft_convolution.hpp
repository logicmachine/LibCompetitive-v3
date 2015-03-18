/**
 *  @file math/fft_convolution.hpp
 */
#pragma once
#include "libcomp/math/fast_fourier_transform.hpp"

namespace lc {

/**
 *  @defgroup fft_convolution FFTを用いた畳み込み
 *  @ingroup  math
 *  @{
 */

/**
 *  @brief FFTを用いた畳み込み乗算
 *    - 時間計算量: \f$ O(n \log{n}) \f$
 *  @param[in] f  入力データ1
 *  @param[in] g  入力データ2
 *  @return    fとgを畳み込んだ結果
 */
template <typename T>
std::vector<std::complex<double>> fft_convolution(
	const std::vector<T> &f, const std::vector<T> &g)
{
	const int n = f.size(), m = g.size();
	int length = 1;
	while(length < (n + m - 1)){ length += length; }
	std::vector<std::complex<double>> complex_f(length);
	for(int i = 0; i < n; ++i){ complex_f[i] = std::complex<double>(f[i]); }
	std::vector<std::complex<double>> complex_g(length);
	for(int i = 0; i < m; ++i){
		complex_g[(length - i) & (length - 1)] = std::complex<double>(g[i]);
	}
	complex_f = fast_fourier_transform(complex_f, false);
	complex_g = fast_fourier_transform(complex_g, false);
	for(int i = 0; i < length; ++i){ complex_f[i] *= complex_g[i]; }
	complex_f = fast_fourier_transform(complex_f, true);
	complex_f.resize(n);
	return complex_f;
}

/**
 *  @}
 */

}

