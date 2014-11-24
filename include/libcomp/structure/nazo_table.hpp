/**
 *  @file libcomp/structure/nazo_table.hpp
 */
#pragma once
#include <vector>
#include <algorithm>

namespace lc {

/**
 *  @defgroup nazo_table Nazo table
 *  @brief    謎テーブル
 *  @ingroup  structure
 *  @{
 */

/**
 *  @brief 謎テーブル
 *
 *  \f$ O(n \log{n}) \f$ の空間計算量となるテーブルを用いて区間クエリを
 *  \f$ O(1) \f$ で処理するデータ構造。
 *  Sparse table の互換品と思ってれば問題なさそう?
 *
 *  https://twitter.com/zerokugi/status/483271924663844864
 *
 *  @tparam Traits  謎テーブルの動作を示す型
 */
template <class Traits>
class NazoTable {

public:
	/// 値型
	typedef typename Traits::value_type value_type;

private:
	Traits m_traits;
	std::vector<std::vector<value_type>> m_table;

	template <typename Iterator>
	void forward_fill(Iterator it, int row, int first, int last){
		value_type acc = m_traits.default_value();
		m_table[row][first] = acc;
		for(int i = first + 1; i < last; ++i, ++it){
			acc = m_traits(acc, *it);
			m_table[row][i] = acc;
		}
	}
	template <typename Iterator>
	void backward_fill(Iterator it, int row, int first, int last){
		value_type acc = m_traits.default_value();
		--it;
		for(int i = first - 1; i >= last; --i, --it){
			acc = m_traits(*it, acc);
			m_table[row][i] = acc;
		}
	}

public:
	/**
	 *  @brief コンストラクタ (要素列による初期化)
	 *
	 *  テーブルを [first, last) で初期化する。
	 *    - 時間計算量: \f$ O(n \log{n}) \f$
	 *
	 *  @param[in] first   要素列の先頭を指すイテレータ
	 *  @param[in] last    要素列の終端を指すイテレータ
	 *  @param[in] traits  処理内容を示すオブジェクト
	 */
	template <class Iterator>
	NazoTable(Iterator first, Iterator last, const Traits &traits = Traits())
		: m_traits(traits)
		, m_table()
	{
		const int n = std::distance(first, last);
		const int b = 32 - __builtin_clz(n + 1);
		m_table.assign(b, std::vector<value_type>(n + 1));
		Iterator it = first;
		++it;
		for(int i = 1; i <= n; ++i, ++it){
			const int ctz = __builtin_ctz(i);
			backward_fill(it, ctz, i, i - (1 << ctz));
			forward_fill(it, ctz, i, std::min(n + 1, i + (1 << ctz)));
		}
	}

	/**
	 *  @brief 区間についての問い合わせ
	 *
	 *  インデックスが区間 [a, b) に含まれる要素すべてを結合した結果を求める。
	 *    - 時間計算量: \f$ O(1) \f$
	 *
	 *  @param[in] l  区間の始端
	 *  @param[in] r  区間の終端
	 *  @return    計算された結果
	 */
	value_type query(size_t l, size_t r) const {
		const int s = 31 - __builtin_clz(l ^ r);
		return m_traits(m_table[s][l], m_table[s][r]);
	}

};

/**
 *  @}
 */

}

