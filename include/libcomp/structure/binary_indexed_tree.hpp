/**
 *  @file libcomp/structure/binary_indexed_tree.h
 */
#pragma once
#include <vector>

namespace lc {

/**
 *  @defgroup binary_indexed_tree Binary indexed tree
 *  @ingroup  structure
 *  @{
 */

/**
 *  @brief Binary Indexed Tree / Fenwick木
 *  @tparam T  各要素および総和の型
 */
template <typename T>
class BinaryIndexedTree {

private:
	std::vector<T> data;

public:
	/**
	 *  @brief コンストラクタ
	 *
	 *  全要素をその型のデフォルト値で初期化する。
	 *
	 *  @param[in] n  BITが格納可能な要素数
	 */
	explicit BinaryIndexedTree(int n = 0) : data(n + 1) { }

	/**
	 *  @brief 総和の計算
	 *
	 *  BITに格納されている要素のうち \f$ k \in [0, i) \f$ 番目の要素の総和を求める。
	 *    - 時間計算量: \f$ \mathcal{O}(\log{n}) \f$
	 *
	 *  @param[in] i  区間の大きさ
	 *  @return   0番目からi-1番目までの要素の総和
	 */
	T sum(int i){
		T s = T();
		for(; i > 0; i -= i & -i){ s += data[i]; }
		return s;
	}

	/**
	 *  @brief 要素の更新
	 *
	 *  i番目の要素の値にxを加算する。
	 *    - 時間計算量: \f$ \mathcal{O}(\log{n}) \f$
	 *
	 *  @param[in] i  更新したい要素のインデックス
	 *  @param[in] x  i番目の要素に加算する値
	 */
	void add(int i, const T &x){
		for(++i; i < static_cast<int>(data.size()); i += i & -i){
			data[i] += x;
		}
	}

};

/**
 *  @}
 */

}

