/**
 *  @file libcomp/misc/binary_search.hpp
 */
#pragma once
#include <type_traits>

namespace lc {

/**
 *  @defgroup binary_search Binary search
 *  @brief    二分探索
 *  @ingroup  misc
 *  @{
 */

/**
 *  @brief 整数二分探索
 *
 *  単調増加な二値関数 \f$ f(x) \f$ の値が1となる最小の \f$ x \in [l, r) \f$ を求める。
 *  該当する \f$ x \f$ が存在しない場合は \f$ r \f$ を返す。
 *    - 時間計算量: \f$ O(\log{(r-l)}) \f$
 *
 *  @param[in] l     探索範囲の下限
 *  @param[in] r     探索範囲の上限
 *  @param[in] func  探索対象の関数
 *  @return    見つかったxの値
 */
template <class T, class Func>
auto binary_search(T l, T r, Func f)
	-> typename std::enable_if<std::is_integral<T>::value, T>::type
{
	while(l < r){
		const T c = l + (r - l) / 2;
		if(f(c)){
			r = c;
		}else{
			l = c + 1;
		}
	}
	return l;
}

/**
 *  @brief 実数二分探索
 *
 *  単調増加な二値関数 \f$ f(x) \f$ の値が1となる最小の \f$ x \in [l, r) \f$ を求める。
 *  該当する \f$ x \f$ が存在しない場合は \f$ r \f$ を返す。
 *    - 時間計算量: \f$ O(\log{(r-l)}) \f$
 *
 *  @param[in] l         探索範囲の下限
 *  @param[in] r         探索範囲の上限
 *  @param[in] func      探索対象の関数
 *  @param[in] max_iter  最大反復回数
 *  @return    見つかったxの値
 */
template <class T, class Func>
auto binary_search(T l, T r, Func f, int max_iter = 100)
	-> typename std::enable_if<std::is_floating_point<T>::value, T>::type
{
	for(int i = 0; i < max_iter; ++i){
		const T c = l + (r - l) / 2;
		if(f(c)){
			r = c;
		}else{
			l = c;
		}
	}
	return l;
}


/**
 *  @}
 */

}

