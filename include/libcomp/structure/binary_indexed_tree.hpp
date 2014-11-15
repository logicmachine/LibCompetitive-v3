/**
 *  @file libcomp/structure/binary_indexed_tree.h
 */
#pragma once
#include <vector>
#include <functional>

namespace lc {

/**
 *  @defgroup binary_indexed_tree Binary indexed tree
 *  @brief    Binary Indexed Tree / Fenwick木
 *  @ingroup  structure
 *  @{
 */

/**
 *  @brief Binary Indexed Tree / Fenwick木
 *  @tparam T     各要素および総和の型
 *  @tparam Func  要素間の演算内容を示す関数
 */
template <typename T, typename Func = std::plus<T>>
class BinaryIndexedTree {

private:
	std::vector<T> m_data;
	Func m_func;

public:
	/**
	 *  @brief コンストラクタ
	 *
	 *  全要素をその型のデフォルト値で初期化する。
	 *
	 *  @param[in] n  BITが格納可能な要素数
	 *  @param[in] f  要素間の演算内容を示す関数
	 */
	explicit BinaryIndexedTree(int n = 0, const Func &f = Func())
		: m_data(n + 1)
		, m_func(f)
	{ }

	/**
	 *  @brief 総和の計算
	 *
	 *  BITに格納されている要素のうち \f$ k \in [0, i) \f$ 番目の要素の総和を求める。
	 *    - 時間計算量: \f$ O(\log{n}) \f$
	 *
	 *  @param[in] i  区間の大きさ
	 *  @return   0番目からi-1番目までの要素の総和
	 */
	T query(int i){
		T s = T();
		for(; i > 0; i -= i & -i){ s = m_func(s, m_data[i]); }
		return s;
	}

	/**
	 *  @brief 要素の更新
	 *
	 *  i番目の要素の値にxを加算する。
	 *    - 時間計算量: \f$ O(\log{n}) \f$
	 *
	 *  @param[in] i  更新したい要素のインデックス
	 *  @param[in] x  i番目の要素に加算する値
	 */
	void modify(int i, const T &x){
		const int n = m_data.size();
		for(++i; i < n; i += i & -i){
			m_data[i] = m_func(m_data[i], x);
		}
	}

};

/**
 *  @}
 */

}

