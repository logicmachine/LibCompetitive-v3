/**
 *  @file libcomp/graph/mincostflow_primal_dual.hpp
 */
#pragma once
#include <vector>
#include <queue>
#include <limits>
#include <algorithm>
#include <utility>
#include "libcomp/graph/residual_network.hpp"

namespace lc {

/**
 *  @defgroup mincostflow_primal_dual Minimum cost flow (Primal-Dual)
 *  @ingroup  graph
 *  @{
 */

/**
 *  @brief Primal-Dual法による最小費用流
 *    - 時間計算量: \f$ O(F |E| \log{|V|}) \f$
 *
 *  @param[in]     source  フローの始点となる頂点
 *  @param[in]     sink    フローの終点となる頂点
 *  @param[in]     flow    sourceからsinkに流す量
 *  @param[in,out] graph   計算に使用する残余グラフ
 *  @return        sourceからsinkにflowだけフローを流した場合の最小のコスト。
 *                 フローを流しきることができない場合は-1。
 */
template <class EdgeType>
auto mincostflow_primal_dual(
	int source, int sink, decltype(EdgeType().capacity) flow,
	ResidualAdjacencyList<EdgeType> &graph)
	-> decltype(EdgeType().weight)
{
	typedef decltype(EdgeType().weight) weight_type;
	typedef decltype(EdgeType().capacity) capacity_type;
	typedef std::pair<weight_type, int> weighted_pair;
	const weight_type inf = std::numeric_limits<weight_type>::max();
	const int n = graph.size();
	weight_type result = 0;
	std::vector<weight_type> h(n);
	std::vector<int> prev_vertex(n), prev_edge(n);
	while(flow > 0){
		std::priority_queue<
			weighted_pair, std::vector<weighted_pair>,
			std::greater<weighted_pair>> pq;
		std::vector<weight_type> dist(n, inf);
		dist[source] = weight_type();
		pq.push(std::make_pair(dist[source], source));
		while(!pq.empty()){
			weighted_pair p = pq.top();
			pq.pop();
			const int u = p.second;
			if(dist[u] < p.first){ continue; }
			for(size_t i = 0; i < graph[u].size(); ++i){
				const auto &e = graph[u][i];
				if(e.capacity <= 0){ continue; }
				const int v = e.to;
				const auto new_dist = dist[u] + e.weight + h[u] - h[v];
				if(dist[v] <= new_dist){ continue; }
				dist[v] = new_dist;
				prev_vertex[v] = u;
				prev_edge[v] = i;
				pq.push(std::make_pair(new_dist, v));
			}
		}
		if(dist[sink] >= inf){ return -1; }
		for(int i = 0; i < n; ++i){ h[i] += dist[i]; }
		weight_type diff = flow;
		for(int v = sink; v != source; v = prev_vertex[v]){
			diff = std::min(
				diff, graph[prev_vertex[v]][prev_edge[v]].capacity);
		}
		flow -= diff;
		result += diff * h[sink];
		for(int v = sink; v != source; v = prev_vertex[v]){
			auto &e = graph[prev_vertex[v]][prev_edge[v]];
			e.capacity -= diff;
			graph[v][e.rev].capacity += diff;
		}
	}
	return result;
}

/**
 *  @}
 */

}

