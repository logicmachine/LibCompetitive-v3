/**
 *  @file libcomp/graph/residual_network.hpp
 */
#pragma once
#include <functional>
#include <type_traits>
#include "libcomp/graph/adjacency_list.hpp"
#include "libcomp/graph/edge_traits.hpp"

namespace lc {

/**
 *  @defgroup residual_network Residual network
 *  @ingroup  graph
 *  @{
 */

/**
 *  @brief 残余ネットワークの辺データ型
 *  @tparam EdgeType  元のグラフの辺データ型
 */
template <class EdgeType>
struct ResidualEdge : public EdgeType {
	typedef ResidualEdge<EdgeType> self_type;

	int rev;

	ResidualEdge() : EdgeType(), rev(0) { }
	template <class... Args>
	ResidualEdge(int rev, Args&&... args)
		: EdgeType(args...)
		, rev(rev)
	{ }
	explicit ResidualEdge(const EdgeType &e)
		: EdgeType(e)
		, rev(0)
	{ }
};

/**
 *  @brief 残余ネットワークの隣接リスト表現
 *  @tparam EdgeType  元のグラフの辺データ型
 */
template <class EdgeType>
class ResidualAdjacencyList
	: public AdjacencyList<ResidualEdge<EdgeType>>
{
public:
	explicit ResidualAdjacencyList(int n = 0)
		: AdjacencyList<ResidualEdge<EdgeType>>(n)
	{ }
};

/**
 *  @brief 重みなし残余ネットワークの生成
 *  @tparam EdgeType  元のグラフの辺データ型
 */
template <class EdgeType>
auto make_residual(const AdjacencyList<EdgeType> &graph)
	-> typename std::enable_if<
		!HasWeight<EdgeType>::value, ResidualAdjacencyList<EdgeType>>::type
{
	HasWeight<EdgeType>(); // [[MINIFIER_REMOVE]]
	typedef decltype(EdgeType().capacity) capacity_type;
	const int n = graph.size();
	ResidualAdjacencyList<EdgeType> result(n);
	for(int u = 0; u < n; ++u){
		for(const auto &e : graph[u]){
			const int v = e.to;
			const int rev_u = result[v].size();
			const int rev_v = result[u].size();
			result[u].emplace_back(rev_u, e);
			result[v].emplace_back(rev_v, e);
			result[v].back().to = u;
			result[v].back().capacity = capacity_type();
		}
	}
	return result;
}

/**
 *  @brief 重みつき残余ネットワークの生成
 *  @tparam EdgeType  元のグラフの辺データ型
 */
template <class EdgeType>
auto make_residual(const AdjacencyList<EdgeType> &graph)
	-> typename std::enable_if<
		HasWeight<EdgeType>::value, ResidualAdjacencyList<EdgeType>>::type
{
	HasWeight<EdgeType>(); // [[MINIFIER_REMOVE]]
	typedef decltype(EdgeType().capacity) capacity_type;
	const int n = graph.size();
	ResidualAdjacencyList<EdgeType> result(n);
	for(int u = 0; u < n; ++u){
		for(const auto &e : graph[u]){
			const int v = e.to;
			const int rev_u = result[v].size();
			const int rev_v = result[u].size();
			result[u].emplace_back(rev_u, e);
			result[v].emplace_back(rev_v, e);
			result[v].back().to = u;
			result[v].back().capacity = capacity_type();
			result[v].back().weight = -e.weight;
		}
	}
	return result;
}

/**
 *  @}
 */

}

