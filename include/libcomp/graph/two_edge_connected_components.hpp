/**
 *  @file libcomp/graph/two_edge_connected_components.hpp
 */
#pragma once
#include <vector>
#include <stack>
#include <limits>
#include <algorithm>
#include <tuple>
#include "libcomp/structure/union_find_tree.hpp"
#include "libcomp/graph/adjacency_list.hpp"

namespace lc {

/**
 *  @defgroup two_edge_connected_components 2-edge connected components
 *  @brief    二重辺連結成分分解
 *  @ingroup  graph
 *  @{
 */

/**
 *  @brief 二重辺連結成分分解
 *    - 時間計算量: \f$ O(|V| \alpha(|V|) + |E|)) \f$
 *  @param[in] graph  グラフデータ
 *  @return    各頂点がどの二重辺連結成分に属するかのテーブル。
 *             各二重辺連結成分の番号はその成分中のいずれかの頂点の番号となる。
 */
template <class EdgeType>
std::vector<int> two_edge_connected_components(
	const AdjacencyList<EdgeType> &graph)
{
	const int inf = std::numeric_limits<int>::max();
	const int n = graph.size();
	UnionFindTree uft(n);
	for(int i = 0; i < n; ++i){
		std::vector<int> v;
		for(const auto &e : graph[i]){ v.push_back(e.to); }
		std::sort(v.begin(), v.end());
		for(int j = 1; j < static_cast<int>(v.size()); ++j){
			if(v[j - 1] == v[j]){ uft.unite(i, v[j]); }
		}
	}
	std::vector<int> depth(n, -1);
	std::stack<std::tuple<int, int, int, int>> stack;
	int last_returned = -1;
	for(int i = 0; i < n; ++i){
		if(depth[i] >= 0){ continue; }
		stack.emplace(i, -1, 0, inf);
		while(!stack.empty()){
			const int u = std::get<0>(stack.top());
			const int p = std::get<1>(stack.top());
			const size_t j = std::get<2>(stack.top());
			int minval = std::get<3>(stack.top());
			stack.pop();
			if(j == 0 && depth[u] >= 0){
				last_returned = depth[u];
				continue;
			}else if(j == 0){
				depth[u] = static_cast<int>(stack.size());
			}else{
				if(last_returned <= static_cast<int>(stack.size())){
					uft.unite(u, graph[u][j - 1].to);
				}
				minval = std::min(minval, last_returned);
			}
			if(j == graph[u].size()){
				last_returned = minval;
			}else{
				const int v = graph[u][j].to;
				stack.emplace(u, p, j + 1, minval);
				if(v != p){
					stack.emplace(v, u, 0, inf);
				}else{
					last_returned = inf;
				}
			}
		}
	}
	std::vector<int> tcc(n, -1);
	for(int i = 0; i < n; ++i){ tcc[i] = uft.find(i); }
	return tcc;
}

/**
 *  @brief 二重辺連結成分ごとにグラフを縮約
 *    - 時間計算量: \f$ O(|V|+|E|) \f$
 *  @param[in] graph  元となるグラフデータ
 *  @param[in] tcc    graphに対して求められた二重辺連結成分テーブル
 *  @return    縮約されたグラフ
 */
template <class EdgeType>
AdjacencyList<EdgeType> tcc_contract(
	const AdjacencyList<EdgeType> &graph,
	const std::vector<int> &tcc)
{
	const int n = graph.size();
	AdjacencyList<EdgeType> result(n);
	for(int u = 0; u < n; ++u){
		const int bu = tcc[u];
		for(const auto &e : graph[u]){
			const int v = e.to, bv = tcc[v];
			if(bv == bu){ continue; }
			EdgeType f(e);
			f.to = bv;
			result.add_edge(bu, f);
		}
	}
	return result;
}

/**
 *  @}
 */

}

