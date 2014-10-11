/**
 *  @file libcomp/graph/edge_traits.hpp
 */
#pragma once

namespace lc {

/**
 *  @brief 辺が重みをもつかどうかを判定するメタ関数
 *  @tparam EdgeType  辺データ型
 */
template <class EdgeType>
struct HasWeight {
private:
	template <typename U>
	static auto check(const U &x) -> decltype(x.weight, std::true_type());
	static std::false_type check(...);
public:
	static const bool value = decltype(check(EdgeType()))::value;
};

/**
 *  @brief 辺が容量をもつかどうかを判定するメタ関数
 *  @tparam EdgeType  辺データ型
 */
template <class EdgeType>
struct HasCapacity {
private:
	template <typename U>
	static auto check(const U &x) -> decltype(x.capacity, std::true_type());
	static std::false_type check(...);
public:
	static const bool value = decltype(check(EdgeType()))::value;
};

}

