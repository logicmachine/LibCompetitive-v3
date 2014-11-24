/**
 *  @file libcomp/misc/ternary_search.hpp
 */
#pragma once
#include <type_traits>

namespace lc {

/**
 *  @defgroup ternary_search Ternary search
 *  @brief    三分探索
 *  @ingroup  misc
 *  @{
 */

/**
 *  @brief 整数三分探索
 *
 *  下に凸な関数 \f$ f(x) \f$ の値を最小化する \f$ x \in [l, r) \f$ を求める。
 *  該当する \f$ x \f$ が複数ある場合は最小のものを返す。
 *    - 時間計算量: \f$ O(\log{(r-l)}) \f$
 *
 *  @param[in] l     探索範囲の下限
 *  @param[in] r     探索範囲の上限
 *  @param[in] func  探索対象の関数
 *  @return    見つかったxの値
 */
template <class T, class Func>
auto ternary_search(T l, T r, Func f)
	-> typename std::enable_if<std::is_integral<T>::value, T>::type
{
	while(l < r){
		const T delta = (r - l);
		const T cl = l + delta / 3, cr = l + 2 * delta / 3;
		if(f(cl) <= f(cr)){
			r = cr;
		}else{
			l = cl + 1;
		}
	}
	return l;
}

/**
 *  @brief 実数三分探索
 *
 *  下に凸な関数 \f$ f(x) \f$ の値を最小化する \f$ x \in [l, r) \f$ を求める。
 *  該当する \f$ x \f$ が複数ある場合は最小のものを返す。
 *    - 時間計算量: \f$ O(\log{(r-l)}) \f$
 *
 *  @param[in] l         探索範囲の下限
 *  @param[in] r         探索範囲の上限
 *  @param[in] func      探索対象の関数
 *  @param[in] max_iter  最大反復回数
 *  @return    見つかったxの値
 */
template <class T, class Func>
auto ternary_search(T l, T r, Func f, int max_iter = 100)
	-> typename std::enable_if<std::is_floating_point<T>::value, T>::type
{
	for(int i = 0; i < max_iter; ++i){
		const T delta = (r - l);
		const T cl = l + delta / 3, cr = l + 2 * delta / 3;
		if(f(cl) <= f(cr)){
			r = cr;
		}else{
			l = cl;
		}
	}
	return l;
}


/**
 *  @}
 */

}

