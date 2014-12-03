/**
 *  @file libcomp/graph/lowest_common_ancestor.hpp
 */
#pragma once
#include <vector>
#include "libcomp/graph/adjacency_list.hpp"

namespace lc {

/**
 *  @defgroup lowest_common_ancestor Lowest common ancestor
 *  @brief    最近共通祖先クエリ処理
 *  @ingroup  graph
 *  @{
 */

/**
 *  @brief    最近共通祖先クエリ処理
 */
class LowestCommonAncestor {

private:
	std::vector<int> m_depth_table;
	std::vector<std::vector<int>> m_skip_table;

	template <class EdgeType>
	void build_tables(
		int u, std::vector<int> &s, const AdjacencyList<EdgeType> &graph)
	{
		if(m_depth_table[u] >= 0){ return; }
		const int d = s.size(), m = m_skip_table.size();
		for(int i = 0, j = 1; i < m && j <= d; ++i, j += j){
			m_skip_table[u][i] = s[d - j];
		}
		m_depth_table[u] = d;
		s.push_back(u);
		for(size_t i = 0; i < graph[u].size(); ++i){
			build_tables(graph[u][i].to, s, graph);
		}
		s.pop_back();
	}

public:
	/**
	 *  @ brief デフォルトコンストラクタ
	 */
	LowestCommonAncestor() : m_depth_table(), m_skip_table() { }

	/**
	 *  @brief コンストラクタ
	 *
	 *  クエリ処理に必要なテーブルを求める。
	 *    - 時間計算量: \f$ O(|V| \log{|V|} \f$
	 *
	 *  @tparam    EdgeType  辺データ型
	 *  @param[in] root      木の根となる頂点の番号
	 *  @param[in] graph     グラフデータ
	 */
	template <class EdgeType>
	LowestCommonAncestor(
		int root, const AdjacencyList<EdgeType> &graph)
		: m_depth_table(graph.size(), -1)
		, m_skip_table(
			graph.size(),
			std::vector<int>(32 - __builtin_clz(graph.size()), -1))
	{
		std::vector<int> s;
		build_tables(root, s, graph);
	}

	/**
	 *  @brief 最近共通祖先を求める
	 *
	 *  頂点 a, b の最近共通祖先を求める。
	 *    - 時間計算量: \f$ O(\log{|V|}) \f$
	 *
	 *  @param[in] a  頂点番号
	 *  @param[in] b  頂点番号
	 *  @return    a, b の最近共通祖先の頂点番号
	 */
	int query(int a, int b) const {
		const int m = m_skip_table[0].size();
		if(m_depth_table[a] < m_depth_table[b]){ std::swap(a, b); }
		for(int i = m - 1; i >= 0; --i){
			if(m_depth_table[a] - (1 << i) >= m_depth_table[b]){
				a = m_skip_table[a][i];
			}
		}
		if(a == b){ return a; }
		for(int i = m - 1; i >= 0; --i){
			if(m_skip_table[a][i] != m_skip_table[b][i]){
				a = m_skip_table[a][i];
				b = m_skip_table[b][i];
			}
		}
		return m_skip_table[a][0];
	}

	/**
	 *  @brief 頂点の根からの距離を求める
	 *
	 *  頂点 v の根からの距離を求める。
	 *
	 *  @param[in] v  頂点番号
	 *  @return    v の根からの距離
	 */
	int get_depth(int v) const {
		return m_depth_table[v];
	}

	/**
	 *  @brief 頂点の親を取得する
	 *
	 *  頂点 v の親を取得する。
	 *
	 *  @param[in] v  頂点番号
	 *  @return    v の親の頂点番号。v が根の場合は-1。
	 */
	int get_parent(int v) const {
		return m_skip_table[v][0];
	}

};

/**
 *  @}
 */

}

