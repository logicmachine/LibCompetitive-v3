/**
 *  @file libcomp/structure/traits/sum.hpp
 */
#pragma once
#include <limits>
#include <algorithm>

namespace lc {

/**
 *  @brief   区間和クエリを処理するためのTraits
 *  @ingroup traits
 *  @tparam  T  値型
 */
template <class T>
struct SumTraits {
	/// 値型
	typedef T value_type;
	/// デフォルト値の取得
	value_type default_value() const { return 0; }
	/// 区間のマージ
	value_type operator()(const value_type &a, const value_type &b) const {
		return a + b;
	}
};

}

