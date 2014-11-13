/**
 *  @file libcomp/graph/adjacency_list.hpp
 */
#pragma once
#include <vector>

namespace lc {

/**
 *  @defgroup adjacency_list Adjacency list
 *  @brief    グラフの隣接リスト表現
 *  @ingroup  graph
 *  @{
 */

/**
 *  @brief  グラフの隣接リスト表現
 *  @tparam EdgeType  辺のデータ型
 */
template <typename EdgeType>
class AdjacencyList {
public:
	typedef std::vector<EdgeType> ListType;
private:
	std::vector<ListType> m_lists;
public:
	/**
	 *  @brief  コンストラクタ
	 *  @param[in] n  グラフに含まれる頂点の数
	 */
	explicit AdjacencyList(int n = 0)
		: m_lists(n)
	{ }
	/**
	 *  @brief  頂点数の取得
	 *  @return グラフに含まれる頂点の数
	 */
	int size() const { return m_lists.size(); }
	/**
	 *  @brief  辺の追加
	 *  @param[in] u     辺の始点
	 *  @param[in] args  辺の情報（始点を除く）
	 */
	template <typename... Args>
	void add_edge(int u, Args&&... args){
		m_lists[u].emplace_back(args...);
	}
	/**
	 *  @brief  ある頂点を始点とする辺リストの取得
	 *  @param[in] u  辺の始点
	 *  @return    uを始点とする辺集合を示す区間
	 */
	const ListType &operator[](int u) const { return m_lists[u]; }
	/**
	 *  @brief  ある頂点を始点とする辺リストの取得
	 *  @param[in] u  辺の始点
	 *  @return    uを始点とする辺集合を示す区間
	 */
	ListType &operator[](int u){ return m_lists[u]; }
};

/**
 *  @}
 */

}

