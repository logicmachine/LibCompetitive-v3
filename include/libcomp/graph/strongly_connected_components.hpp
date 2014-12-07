/**
 *  @file libcomp/graph/strongly_connected_components.hpp
 */
#pragma once
#include <vector>
#include <stack>
#include "libcomp/graph/edges.hpp"
#include "libcomp/graph/adjacency_list.hpp"

namespace lc {

/**
 *  @defgroup strongly_connected_components Strongly connected components
 *  @brief    強連結成分分解
 *  @ingroup  graph
 *  @{
 */

/**
 *  @brief 強連結成分分解
 *    - 時間計算量: \f$ O(|V|+|E|) \f$
 *  @param[in] graph  グラフデータ
 *  @return    各頂点がどの強連結成分分解に属するかのテーブル。
 *             各強連結成分の番号はその成分中のいずれかの頂点の番号となる。
 */
template <class EdgeType>
std::vector<int> strongly_connected_components(
	const AdjacencyList<EdgeType> &graph)
{
	const int n = graph.size();
	AdjacencyList<Edge> inv_graph(n);
	for(int u = 0; u < n; ++u){
		for(const auto &e : graph[u]){ inv_graph.add_edge(e.to, u); }
	}
	std::vector<bool> used(n);
	std::vector<int> order;
	for(int i = 0; i < n; ++i){
		if(used[i]){ continue; }
		std::stack<std::pair<int, int>> stack;
		stack.push(std::make_pair(i, 0));
		while(!stack.empty()){
			const int v = stack.top().first;
			const size_t j = stack.top().second;
			stack.pop();
			if(j == 0){ used[v] = true; }
			if(j == graph[v].size()){
				order.push_back(v);
			}else{
				stack.push(std::make_pair(v, j + 1));
				if(!used[graph[v][j].to]){
					stack.push(std::make_pair(graph[v][j].to, 0));
				}
			}
		}
	}
	std::vector<int> scc(n, -1);
	for(int i = n - 1; i >= 0; --i){
		if(scc[order[i]] >= 0){ continue; }
		std::stack<std::pair<int, int>> stack;
		stack.push(std::make_pair(order[i], 0));
		while(!stack.empty()){
			const int v = stack.top().first;
			const size_t j = stack.top().second;
			stack.pop();
			if(j == 0){ scc[v] = order[i]; }
			if(j == inv_graph[v].size()){ continue; }
			stack.push(std::make_pair(v, j + 1));
			if(scc[inv_graph[v][j].to] < 0){
				stack.push(std::make_pair(inv_graph[v][j].to, 0));
			}
		}
	}
	return scc;
}

/**
 *  @brief 強連結成分ごとにグラフを縮約
 *    - 時間計算量: \f$ O(|V|+|E|) \f$
 *  @param[in] graph  元となるグラフデータ
 *  @param[in] scc    graphに対して求められた強連結成分テーブル
 *  @return    縮約されたグラフ
 */
template <class EdgeType>
AdjacencyList<EdgeType> scc_contract(
	const AdjacencyList<EdgeType> &graph,
	const std::vector<int> &scc)
{
	const int n = graph.size();
	AdjacencyList<EdgeType> result(n);
	for(int u = 0; u < n; ++u){
		const int su = scc[u];
		for(const auto &e : graph[u]){
			const int v = e.to, sv = scc[v];
			if(sv == su){ continue; }
			EdgeType f(e);
			f.to = sv;
			result.add_edge(su, f);
		}
	}
	return result;
}

/**
 *  @}
 */

}

