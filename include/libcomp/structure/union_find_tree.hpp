/**
 *  @file libcomp/structure/union_find_tree.hpp
 */
#pragma once
#include <vector>

namespace lc {

/**
 *  @defgroup union_find_tree Union-find tree (disjoint set)
 *  @brief    Union-Find 木 / Disjoint set
 *  @ingroup  structure
 *  @{
 */

/**
 *  @brief Union-Find木
 */
class UnionFindTree {

private:
	std::vector<int> m_parent;
	std::vector<int> m_rank;

public:
	/**
	 *  @brief 全要素が独立な状態でUnion-Find木を初期化するコンストラクタ
	 *    - 時間計算量は \f$ O(n) \f$
	 *  @param[in] n  Union-Find木の要素数
	 */
	explicit UnionFindTree(int n = 0)
		: m_parent(n)
		, m_rank(n)
	{
		for(int i = 0; i < n; ++i){ m_parent[i] = i; }
	}

	/**
	 *  @brief 要素の属する集合を取得
	 *    - 時間計算量は \f$ O(\alpha(n)) \f$ (amortized)
	 *  @param[in] x  対象とする要素のインデックス
	 *  @return    要素xが属している集合を示す値
	 */
	int find(int x){
		if(m_parent[x] == x){ return x; }
		m_parent[x] = find(m_parent[x]);
		return m_parent[x];
	}

	/**
	 *  @brief 2つの集合の併合
	 *    - 時間計算量は \f$ O(\alpha(n)) \f$ (amortized)
	 *  @param[in] x  片方の集合に含まれる要素
	 *  @param[in] y  他方の集合に含まれる要素
	 *  @return    併合後の集合を示す値
	 */
	int unite(int x, int y){
		x = find(x);
		y = find(y);
		if(x == y){ return x; }
		if(m_rank[x] < m_rank[y]){
			m_parent[x] = y;
			return y;
		}else if(m_rank[x] > m_rank[y]){
			m_parent[y] = x;
			return x;
		}else{
			m_parent[y] = x;
			++m_rank[x];
			return x;
		}
	}

	/**
	 *  @brief 2つの要素が同じ集合に属しているかの判定
	 *    - 時間計算量は \f$ O(\alpha(n)) \f$ (amortized)
	 *  @param[in] x      片方の要素
	 *  @param[in] y      他方の要素
	 *  @retval    true   xとyが同じ集合に属している
	 *  @retval    false  xとyが同じ集合に属していない
	 */
	bool same(int x, int y){
		return find(x) == find(y);
	}

};

/**
 *  @}
 */

}

