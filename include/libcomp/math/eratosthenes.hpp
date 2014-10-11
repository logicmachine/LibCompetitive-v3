/**
 *  @file libcomp/math/eratosthenes.h
 */
#pragma once
#include <vector>

namespace lc {

/**
 *  @defgroup eratosthenes Eratosthenes
 *  @ingroup  math
 *  @{
 */

/**
 *  @brief エラトステネスの篩による素数列挙
 *
 *  [0, n) の範囲に含まれる素数集合を可変長配列・ビット集合の形で返す。
 *    - 時間計算量: \f$ \mathcal{O}(n \log{\log{n}}) \f$
 *
 *  @tparam        T      篩のデータ型
 *  @param[in,out] sieve  素数集合のビット集合による表現
 *  @return        素数集合の可変長配列による表現
 */
template <typename T>
std::vector<int> eratosthenes(T &sieve){
	const int n = sieve.size();
	for(int i = 0; i < n; ++i){ sieve[i] = 1; }
	sieve[0] = sieve[1] = 0;
	std::vector<int> primes;
	for(int i = 2; i < n; ++i){
		if(!sieve[i]){ continue; }
		primes.push_back(i);
		for(int j = i + i; j < n; j += i){ sieve[j] = 0; }
	}
	return primes;
}

/**
 *  @brief エラトステネスの篩による素数列挙
 *
 *  [0, n) の範囲に含まれる素数集合を可変長配列の形で返す。
 *    - 時間計算量: \f$ \mathcal{O}(n \log{\log{n}}) \f$
 *
 *  @param[in] n  素数集合の範囲
 *  @return    素数集合の可変長配列による表現
 */
std::vector<int> eratosthenes(int n){
	std::vector<bool> sieve(n);
	return eratosthenes(sieve);
}

/**
 *  @}
 */

}

