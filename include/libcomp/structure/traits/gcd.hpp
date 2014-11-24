/**
 *  @file libcomp/structure/traits/gcd.hpp
 */
#pragma once
#include <limits>
#include <algorithm>

namespace lc {

/**
 *  @brief   区間GCDクエリを処理するためのTraits
 *  @ingroup traits
 *  @tparam  T  値型
 */
template <class T>
struct GCDTraits {
	/// 値型
	typedef T value_type;
	/// デフォルト値の取得
	value_type default_value() const { return 0; }
	/// 区間のマージ
	value_type operator()(const value_type &a, const value_type &b) const {
		return std::__gcd(a, b);
	}
};

}

