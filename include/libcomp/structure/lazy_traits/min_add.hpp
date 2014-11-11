/**
 *  @file libcomp/structure/lazy_traits/min_add.hpp
 */
#pragma once
#include <limits>
#include <algorithm>
#include <utility>

namespace lc {

/**
 *  @brief   区間最小値・区間加算用Traits
 *  @ingroup lazy_traits
 *  @tparam  T  値の型
 */
template <typename T>
struct LazyMinRangeAddTraits {
	/// 値型
	typedef T value_type;
	/// 更新クエリ型
	typedef T modifier_type;

	/// デフォルト値の取得
	value_type default_value() const { return std::numeric_limits<T>::max(); }
	/// 値を変更しない更新クエリの取得
	modifier_type default_modifier() const { return 0; }

	/// 更新クエリの分割
	std::pair<modifier_type, modifier_type>
	split_modifier(const modifier_type &m, size_t k) const {
		return std::make_pair(m, m);
	}
	/// 更新クエリの結合
	modifier_type merge_modifier(
		const modifier_type &a, const modifier_type &b) const
	{
		return a + b;
	}

	/// 値の結合
	value_type operator()(const value_type &a, const value_type &b) const {
		return std::min(a, b);
	}
	/// 更新クエリの解決
	value_type resolve(
		size_t n, const value_type &v, const modifier_type &m) const
	{
		return v + m;
	}
};

}

