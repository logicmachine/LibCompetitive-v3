/**
 *  @file libcomp/structure/segment_tree.h
 */
#pragma once
#include <vector>
#include <algorithm>
#include <iterator>

namespace lc {

/**
 *  @defgroup segment_tree Segment tree
 *  @brief    セグメント木
 *  @ingroup  structure
 *  @{
 */

/**
 *  @brief  セグメント木
 *  @sa MinSegmentTreeTraits
 *  @tparam Traits  セグメント木の動作を示す型
 */
template <typename Traits>
class SegmentTree {

public:
	/// 値型
	typedef typename Traits::value_type value_type;

private:
	Traits m_traits;
	std::vector<value_type> m_data;
	int m_size;

	void initialize(){
		for(int i = m_size - 2; i >= 0; --i){
			m_data[i] = m_traits(m_data[i * 2 + 1], m_data[i * 2 + 2]);
		}
	}

	value_type query(int a, int b, int k, int l, int r) const {
		if(r <= a || b <= l){ return m_traits.default_value(); }
		if(a <= l && r <= b){ return m_data[k]; }
		const value_type vl = query(a, b, k * 2 + 1, l, (l + r) / 2);
		const value_type vr = query(a, b, k * 2 + 2, (l + r) / 2, r);
		return m_traits(vl, vr);
	}

public:
	/**
	 *  @brief コンストラクタ (既定値で初期化)
	 *
	 *  葉をすべて既定値で初期化した状態のセグメント木を構築する。
	 *    - 時間計算量: \f$ O(n) \f$
	 *
	 *  @param[in] size    最低限必要な葉の数
	 *  @param[in] traits  処理内容を示す関数オブジェクト
	 */
	explicit SegmentTree(int size = 0, const Traits &traits = Traits()) :
		m_size(1), m_traits(traits)
	{
		while(m_size < size){ m_size *= 2; }
		m_data.assign(m_size * 2 - 1, m_traits.default_value());
		initialize();
	}

	/**
	 *  @brief コンストラクタ (要素列による初期化)
	 *
	 *  葉をすべて [first, last) で初期化した状態のセグメント木を構築する。
	 *    - 時間計算量: \f$ O(n) \f$。
	 *
	 *  @param[in] first   要素列の先頭を指すイテレータ
	 *  @param[in] last    要素列の終端を指すイテレータ
	 *  @param[in] traits  処理内容を示す関数オブジェクト
	 */
	template <typename Iterator>
	SegmentTree(
		Iterator first, Iterator last, const Traits &traits = Traits()) :
		m_size(1), m_traits(traits)
	{
		const int n = std::distance(first, last);
		while(m_size < n){ m_size *= 2; }
		m_data.resize(m_size * 2 - 1, m_traits.default_value());
		std::copy(first, last, m_data.begin() + m_size - 1);
		initialize();
	}

	/**
	 *  @brief 葉の更新
	 *
	 *  i番目の葉の値をvalで更新する。また、必要に応じて節点の値も更新する。
	 *    - 時間計算量: \f$ O(\log{n}) \f$
	 *
	 *  @param[in] i    更新したい葉のインデックス
	 *  @param[in] val  更新後の値
	 */
	void update(int i, const value_type &val){
		i += m_size - 1;
		m_data[i] = val;
		while(i > 0){
			i = (i - 1) / 2;
			m_data[i] = m_traits(m_data[i * 2 + 1], m_data[i * 2 + 2]);
		}
	}

	/**
	 *  @brief 区間についての問い合わせ
	 *
	 *  インデックスが区間 [a, b) に含まれる要素すべてを結合した結果を求める。
	 *    - 時間計算量: \f$ O(\log{n}) \f$
	 *
	 *  @param[in] a  区間の始端
	 *  @param[in] b  区間の終端
	 *  @return    計算された結果
	 */
	value_type query(int a, int b) const {
		return query(a, b, 0, 0, m_size);
	}

	/**
	 *  @brief 葉の取得
	 *
	 *  i番目の葉の値を取得する。
	 *    - 時間計算量: \f$ O(1) \f$
	 *
	 *  @param[in] i  取得する葉のインデックス
	 *  @return    取得された値
	 */
	value_type operator[](int i) const {
		return m_data[m_size - 1 + i];
	}

};

/**
 *  @}
 */

}

