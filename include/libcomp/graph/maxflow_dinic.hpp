/**
 *  @file libcomp/graph/maxflow_dinic.hpp
 */
#pragma once
#include <vector>
#include <queue>
#include <limits>
#include "libcomp/graph/residual_network.hpp"

namespace lc {

/**
 *  @defgroup maxflow_dinic Maximum flow (Dinic)
 *  @ingroup  graph
 *  @{
 */

/**
 *  @brief Dinic法による最大流
 *    - 時間計算量: \f$ O(|E| |V|^2) \f$
 *      - 二部グラフの最大マッチングの場合: \f$ O(\sqrt{|V|} |E|) \f$
 *      - 辺の容量がすべて等しい場合: \f$ O(\min\{ \sqrt{|E|}, |V|^{1.5} \} |E|) \f$
 *
 *  @param[in]     source  フローの始点となる頂点
 *  @param[in]     sink    フローの終点となる頂点
 *  @param[in,out] graph   計算に使用する残余グラフ
 *  @return        sourceからsinkに流すことのできる最大の流量
 */
template <class EdgeType>
auto maxflow_dinic(
	int source, int sink, ResidualAdjacencyList<EdgeType> &graph)
	-> decltype(EdgeType().capacity)
{
	typedef decltype(EdgeType().capacity) capacity_type;
	const capacity_type inf = std::numeric_limits<capacity_type>::max();
	const int n = graph.size();
	capacity_type flow = 0;
	while(true){
		std::vector<int> level(n, -1);
		std::queue<int> q;
		level[source] = 0;
		q.push(source);
		while(!q.empty()){
			const int u = q.front();
			q.pop();
			for(const auto &e : graph[u]){
				const int v = e.to;
				if(e.capacity <= 0 || level[v] >= 0){ continue; }
				level[v] = level[u] + 1;
				q.push(v);
			}
		}
		if(level[sink] < 0){ break; }
		std::vector<size_t> iteration(n, 0);
		while(true){
			std::function<capacity_type(int, capacity_type)> dfs =
				[&](int u, capacity_type limit) -> capacity_type {
					if(u == sink){ return limit; }
					for(; iteration[u] < graph[u].size(); ++iteration[u]){
						auto &e = graph[u][iteration[u]];
						const int v = e.to;
						if(e.capacity <= 0 || level[u] >= level[v]){ continue; }
						const capacity_type diff =
							dfs(v, std::min(e.capacity, limit));
						if(diff > 0){
							e.capacity -= diff;
							graph[v][e.rev].capacity += diff;
							return diff;
						}
					}
					return 0;
				};
			const auto f = dfs(source, inf);
			if(f <= 0){ break; }
			flow += f;
		}
	}
	return flow;
}

/**
 *  @}
 */

}

