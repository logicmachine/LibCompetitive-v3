/**
 *  @file libcomp/graph/topological_sort.hpp
 */
#pragma once
#include <vector>
#include <queue>
#include "libcomp/graph/adjacency_list.hpp"

namespace lc {

/**
 *  @defgroup topological_sort Topological sort
 *  @brief    トポロジカルソート
 *  @ingroup  graph
 *  @{
 */

/**
 *  @brief トポロジカルソート
 *    - 時間計算量: \f$ O(|E| + |V|) \f$
 *
 *  @tparam    EdgeType  辺データ型
 *  @param[in] graph     グラフデータ
 *  @return    グラフに含まれる頂点のトポロジカル順序。
 *             グラフが閉路を含む場合は空のベクタを返す。
 */
template <typename EdgeType>
std::vector<int> topological_sort(const AdjacencyList<EdgeType> &graph){
	const int n = graph.size();
	std::vector<int> in_degrees(n);
	for(int u = 0; u < n; ++u){
		for(const auto &e : graph[u]){ ++in_degrees[e.to]; }
	}
	std::queue<int> q;
	for(int u = 0; u < n; ++u){
		if(in_degrees[u] == 0){ q.push(u); }
	}
	std::vector<int> result;
	while(!q.empty()){
		const int u = q.front();
		q.pop();
		result.push_back(u);
		for(const auto &e : graph[u]){
			if(--in_degrees[e.to] == 0){ q.push(e.to); }
		}
	}
	if(static_cast<int>(result.size()) != n){ return std::vector<int>(); }
	return result;
}

/**
 *  @}
 */

}

