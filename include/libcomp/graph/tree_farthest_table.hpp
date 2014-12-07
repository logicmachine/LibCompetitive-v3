/**
 *  @file libcomp/graph/tree_farthest_table.hpp
 */
#pragma once
#include <vector>
#include <stack>
#include <queue>
#include <type_traits>
#include "libcomp/graph/edge_traits.hpp"
#include "libcomp/graph/adjacency_list.hpp"
#include "libcomp/misc/top_k.hpp"

namespace lc {

/**
 *  @defgroup tree_farthest_table Farthest table of tree
 *  @brief    木上の最遠点テーブル
 *  @ingroup  graph
 *  @{
 */

/**
 *  @brief  木上の最遠点テーブル
 *  @tparam EdgeType  辺データ型
 */
template <class EdgeType>
class TreeFarthestTable {

public:
	typedef decltype(EdgeType().weight) weight_type;

private:
	std::vector<int> m_farthest_vertices;
	std::vector<weight_type> m_farthest_weights;

public:
	/**
	 *  @brief 最遠点テーブルの構築
	 *    - 時間計算量: \f$ O(|V|) \f$
	 *  @param[in] graph  グラフデータ
	 */
	TreeFarthestTable(
		const AdjacencyList<EdgeType> &graph = AdjacencyList<EdgeType>())
		: m_farthest_vertices(graph.size())
		, m_farthest_weights(graph.size())
	{
		typedef std::pair<weight_type, int> weighted_pair;
		typedef TopK<weighted_pair, 2, std::greater<weighted_pair>> topk_pair;
		const int n = graph.size();
		std::vector<int> parent(n, -1), parent_weight(n);
		std::vector<topk_pair> result(n);
		std::stack<std::pair<int, int>> stack;
		stack.push(std::make_pair(0, 0));
		parent[0] = 0;
		while(!stack.empty()){
			auto &frame = stack.top();
			const int u = frame.first;
			if(frame.second == 0){
				for(const auto &e : graph[u]){
					if(parent[e.to] >= 0){ continue; }
					parent[e.to] = u;
					parent_weight[e.to] = e.weight;
					stack.push(std::make_pair(e.to, 0));
				}
				frame.second = 1;
			}else{
				result[u].insert(weighted_pair(weight_type(), u));
				for(const auto &e : graph[u]){
					if(e.to == parent[u]){ continue; }
					const auto w = result[e.to][0];
					result[u].insert(
						weighted_pair(w.first + e.weight, w.second));
				}
				stack.pop();
			}
		}
		std::queue<int> queue;
		queue.push(0);
		while(!queue.empty()){
			const int u = queue.front(), p = parent[u];
			queue.pop();
			for(const auto &e : graph[u]){
				if(e.to != p){ queue.push(e.to); }
			}
			if(u == 0){ continue; }
			const weight_type &pw = parent_weight[u];
			const int k =
				(result[u][0].first + pw == result[p][0].first ? 1 : 0);
			const weighted_pair &w = result[p][k];
			result[u].insert(weighted_pair(w.first + pw, w.second));
		}
		for(int i = 0; i < n; ++i){
			m_farthest_weights[i] = result[i][0].first;
			m_farthest_vertices[i] = result[i][0].second;
		}
	}

	/**
	 *  @brief  最遠点テーブルの取得
	 *  @return 最遠点テーブルへの参照
	 */
	const std::vector<int> &vertices() const {
		return m_farthest_vertices;
	}

	/**
	 *  @brief 最遠点テーブルの参照
	 *  @param[in] v  頂点番号
	 *  @return    頂点vから最も遠い頂点の頂点番号
	 */
	int vertice(int v) const {
		return m_farthest_vertices[v];
	}

	/**
	 *  @brief  最遠点への距離のテーブルの取得
	 *  @return 最遠点への距離のテーブルへの参照
	 */
	const std::vector<weight_type> &weights() const {
		return m_farthest_weights;
	}

	/**
	 *  @brief 最遠点への距離テーブルの参照
	 *  @param[in] v  頂点番号
	 *  @return    頂点vから最も遠い頂点への頂点vからの距離
	 */
	weight_type weight(int v) const {
		return m_farthest_weights[v];
	}

};

/**
 *  @}
 */

}

