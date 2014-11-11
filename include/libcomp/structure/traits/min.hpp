/**
 *  @file libcomp/structure/traits/min.hpp
 */
#pragma once
#include <limits>
#include <algorithm>

namespace lc {

/**
 *  @brief   区間最小値クエリを処理するためのTraits
 *  @ingroup traits
 *  @tparam  T  値型
 */
template <class T>
struct MinTraits {
	/// 値型
	typedef T value_type;
	/// デフォルト値の取得
	value_type default_value() const {
		return std::numeric_limits<value_type>::max();
	}
	/// 区間のマージ
	value_type operator()(const value_type &a, const value_type &b) const {
		return std::min(a, b);
	}
};

}

