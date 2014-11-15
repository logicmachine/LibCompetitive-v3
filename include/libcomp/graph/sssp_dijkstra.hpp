/**
 *  @file libcomp/graph/sssp_dijkstra.hpp
 */
#pragma once
#include <vector>
#include <queue>
#include <limits>
#include <utility>
#include "libcomp/graph/adjacency_list.hpp"

namespace lc {

/**
 *  @defgroup sssp_dijkstra SSSP (Dijkstra)
 *  @brief    Dijkstra 法による単一始点最短路
 *  @ingroup  graph
 *  @{
 */

/**
 *  @brief ダイクストラ法による単一始点最短路
 *
 *  負の閉路を含まないグラフにおける単一始点最短路。
 *    - 時間計算量: \f$ O(|E| \log |V|) \f$
 *
 *  @tparam    EdgeType  辺データ型
 *  @param[in] source    始点となる頂点
 *  @param[in] graph     グラフデータ
 *  @return    sourceから各頂点への最短路の距離
 */
template <typename EdgeType>
auto sssp_dijkstra(int source, const AdjacencyList<EdgeType> &graph)
	-> std::vector<decltype(EdgeType().weight)>
{
	typedef decltype(EdgeType().weight) weight_type;
	typedef std::pair<weight_type, int> weighted_pair;
	const auto inf = std::numeric_limits<weight_type>::max();
	const int n = graph.size();
	std::vector<weight_type> result(n, inf);
	std::priority_queue<weighted_pair, std::vector<weighted_pair>, std::greater<weighted_pair>> pq;
	result[source] = weight_type();
	pq.push(std::make_pair(result[source], source));
	while(!pq.empty()){
		const auto d = pq.top().first;
		const auto u = pq.top().second;
		pq.pop();
		if(result[u] < d){ continue; }
		for(const auto &e : graph[u]){
			const auto v = e.to;
			const auto w = e.weight;
			if(d + w < result[v]){
				result[v] = d + w;
				pq.push(std::make_pair(result[v], v));
			}
		}
	}
	return result;
}

/**
 *  @}
 */

}

