/**
 *  @file libcomp/math/extended_gcd.hpp
 */
#pragma once
#include <utility>

namespace lc {

/**
 *  @defgroup extended_gcd Extended Euclid's algorithm
 *  @brief    拡張ユークリッド互除法
 *  @ingroup  math
 *  @{
 */

/**
 *  @brief  拡張ユークリッド互除法
 *    - 時間計算量: \f$ O(\log{(a + b)}) \f$
 *
 *  @tparam    T  使用する整数の型 
 *  @param[in] a  入力 \f$ a \f$
 *  @param[in] b  入力 \f$ b \f$
 *  @return    \f$ ax + by = \gcd(a, b) \f$ を満たす \f$ (x, y) \f$
 */
template <typename T>
static std::pair<T, T> extended_gcd(T a, T b){
	if(b == 0){ return std::pair<T, T>(1, 0); }
	const auto p = extended_gcd(b, a % b);
	return std::pair<T, T>(p.second, p.first - a / b * p.second);
}

/**
 *  @}
 */

}

