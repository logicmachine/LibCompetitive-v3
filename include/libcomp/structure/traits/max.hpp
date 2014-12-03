/**
 *  @file libcomp/structure/traits/max.hpp
 */
#pragma once
#include <limits>
#include <algorithm>

namespace lc {

/**
 *  @brief   区間最大値クエリを処理するためのTraits
 *  @ingroup traits
 *  @tparam  T  値型
 */
template <class T>
struct MaxTraits {
	/// 値型
	typedef T value_type;
	/// デフォルト値の取得
	value_type default_value() const {
		return std::numeric_limits<value_type>::min();
	}
	/// 区間のマージ
	value_type operator()(const value_type &a, const value_type &b) const {
		return std::max(a, b);
	}
};

}

