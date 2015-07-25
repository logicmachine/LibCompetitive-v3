#pragma once
#include <vector>
#include <cassert>
#include "libcomp/math/extended_gcd.hpp"

namespace lc {

/**
 *  @defgroup modulus_utility Utilities for modulus calculation
 *  @brief    法演算のためのユーティリティ
 *  @ingroup  math
 *  @{
 */

/**
 *  @brief MODを法とする時の逆数の計算
 *    - 時間計算量 \f$ O(\log{x}) \f$
 *
 *  @param[in] x  入力
 *  @return    \f$ x \times y \equiv 1 \pmod{\mathit{MOD}} \f$ となる \f$ y \f$
 */
template <int MOD>
inline int modulus_inverse(int x){
	return (extended_gcd(x, MOD).first + MOD) % MOD;
}

/**
 *  @brief MODを法とする時の階乗の計算
 *    - 時間計算量 \f$ O(n) \f$
 *      - 複数回使用する場合は \f$ O(\max{n}) \f$
 *
 *  @param[in] n  入力
 *  @return    \f$ n! \pmod{\mathit{MOD}} \f$
 */
template <int MOD>
inline int modulus_factorial(int n){
	static std::vector<int> table(1, 1);
	assert(n >= 0);
	while(static_cast<int>(table.size()) <= n){
		const long long x = table.size();
		table.push_back(x * table.back() % MOD);
	}
	return table[n];
}

/**
 *  @brief MODを法とする時の階乗の逆数の計算
 *    - 時間計算量 \f$ O(n \log{n}) \f$
 *      - 複数回使用する場合は \f$ O(\max{n} \log{\max{n}}) \f$
 *
 *  @param[in] n  入力
 *  @return    \f$ 1/n! \pmod{\mathit{MOD}} \f$
 */
template <int MOD>
inline int modulus_inv_factorial(int n){
	static std::vector<int> table(1, 1);
	assert(n >= 0);
	while(static_cast<int>(table.size()) <= n){
		const long long x = modulus_inverse<MOD>(table.size());
		table.push_back(x * table.back() % MOD);
	}
	return table[n];
}

/**
 *  @brief MODを法とする時の組み合わせの総数の計算
 *    - 時間計算量 \f$ O(n \log{n}) \f$
 *      - 複数回使用する場合は \f$ O(\max{n} \log{\max{n}}) \f$
 *
 *  @param[in] n  入力1
 *  @param[in] k  入力2
 *  @return    \f$ _n C _k \pmod{\mathit{MOD}} \f$
 */
template <int MOD>
inline int modulus_combination(int n, int k){
	if(k < 0 || n < k){ return 0; }
	const long long a = modulus_factorial<MOD>(n);
	const long long b = modulus_inv_factorial<MOD>(n - k);
	const long long c = modulus_inv_factorial<MOD>(k);
	return a * b % MOD * c % MOD;
}

/**
 *  @}
 */

}

