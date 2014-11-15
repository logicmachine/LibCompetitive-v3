/**
 *  @file libcomp/graph/sssp_bellman_ford.hpp
 */
#pragma once
#include <vector>
#include <queue>
#include <limits>
#include <utility>
#include "libcomp/graph/adjacency_list.hpp"

namespace lc {

/**
 *  @defgroup sssp_bellman_ford SSSP (Bellman-Ford)
 *  @brief    Bellman-Ford 法による単一始点最短路
 *  @ingroup  graph
 *  @{
 */

/**
 *  @brief ベルマン・フォード法による単一始点最短路
 *
 *  負の閉路を含みうるグラフにおける単一始点最短路。
 *    - 時間計算量: \f$ O(|V| |E|) \f$
 *
 *  @tparam    EdgeType  辺データ型
 *  @param[in] source    始点となる頂点
 *  @param[in] graph     グラフデータ
 *  @return    sourceから各頂点への最短路の距離
 *             負閉路を含む場合は空のベクタを返す
 */
template <typename EdgeType>
auto sssp_bellman_ford(int source, const AdjacencyList<EdgeType> &graph)
	-> std::vector<decltype(EdgeType().weight)>
{
	typedef decltype(EdgeType().weight) weight_type;
	const auto inf = std::numeric_limits<weight_type>::max();
	const int n = graph.size();
	std::vector<weight_type> result(n, inf);
	result[source] = weight_type();
	for(int level = 0; level < n; ++level){
		bool updated = false;
		for(int u = 0; u < n; ++u){
			if(result[u] >= inf){ continue; }
			for(const auto &e : graph[u]){
				const auto v = e.to;
				const auto t = result[u] + e.weight;
				if(t < result[v]){
					result[v] = t;
					updated = true;
				}
			}
		}
		if(!updated){ return result; }
	}
	return std::vector<weight_type>();
}

/**
 *  @}
 */

}

