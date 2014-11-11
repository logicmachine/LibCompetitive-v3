/**
 *  @file libcomp/structure/treap_list/min.hpp
 */
#pragma once
#include <algorithm>
#include "libcomp/structure/treap_list.hpp"

namespace lc {

/**
 *  @brief   TreapListで区間最小値クエリを処理するためのTraits
 *  @ingroup treap_list
 *  @tparam  T  値型
 */
template <class T>
struct MinTreapListTraits {
	/// 値型
	typedef T value_type;
	/// 区間のマージ
	value_type operator()(const value_type &a, const value_type &b) const {
		return std::min(a, b);
	}
};

}

