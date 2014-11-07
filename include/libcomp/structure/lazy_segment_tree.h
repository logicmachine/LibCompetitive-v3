/**
 *  @file libcomp/structure/lazy_segment_tree.hpp
 */
#pragma once
#include <vector>
#include <algorithm>
#include <iterator>

namespace lc {

/**
 *  @defgroup lazy_segment_tree Segment tree with lazy propagation
 *  @ingroup  structure
 *  @{
 */

/**
 *  @brief 遅延更新セグメント木
 *  @sa SumLazySegmentTreeTraits
 *  @tparam Traits  セグメント木の動作を示す型
 */
template <typename Traits>
class LazySegmentTree {

public:
	/// 値データ型
	typedef typename Traits::value_type value_type;
	/// 更新クエリ型
	typedef typename Traits::modifier_type modifier_type;

private:
	Traits m_traits;
	vector<value_type> m_values;
	vector<modifier_type> m_modifiers;
	size_t m_size;

	void initialize(){
		for(int i = static_cast<int>(m_size) - 2; i >= 0; --i){
			m_values[i] = m_traits.merge_value(
				m_values[i * 2 + 1], m_values[i * 2 + 2]);
		}
	}

	value_type modify(
		int a, int b, int k, int l, int r, const modifier_type &modifier)
	{
		if(r <= a || b <= l){
			return m_traits.resolve(l, r, m_values[k], m_modifiers[k]);
		}
		if(a <= l && r <= b){
			m_modifiers[k] =
				m_traits.merge_modifier(m_modifiers[k], modifier);
			return m_traits.resolve(l, r, m_values[k], m_modifiers[k]);
		}
		const modifier_type prev = m_modifiers[k];
		m_modifiers[k] = m_traits.default_modifier();
		const int c = (l + r) / 2, lk = k * 2 + 1, rk = k * 2 + 2;
		m_modifiers[lk] = m_traits.merge_modifier(prev, m_modifiers[lk]);
		m_modifiers[rk] = m_traits.merge_modifier(prev, m_modifiers[rk]);
		const value_type vl = modify(a, b, lk, l, c, modifier);
		const value_type vr = modify(a, b, rk, c, r, modifier);
		m_values[k] = m_traits.merge_value(vl, vr);
		return m_values[k];
	}

	value_type query(int a, int b, int k, int l, int r){
		if(r <= a || b <= l){ return m_traits.default_value(); }
		if(a <= l && r <= b){
			return m_traits.resolve(l, r, m_values[k], m_modifiers[k]);
		}
		const modifier_type modifier = m_modifiers[k];
		m_values[k] = m_traits.resolve(l, r, m_values[k], modifier);
		m_modifiers[k] = m_traits.default_modifier();
		const int c = (l + r) / 2, lk = k * 2 + 1, rk = k * 2 + 2;
		m_modifiers[lk] = m_traits.merge_modifier(m_modifiers[lk], modifier);
		m_modifiers[rk] = m_traits.merge_modifier(m_modifiers[rk], modifier);
		const value_type vl = query(a, b, lk, l, c);
		const value_type vr = query(a, b, rk, c, r);
		return m_traits.merge_value(vl, vr);
	}

public:
	/**
	 *  @brief コンストラクタ (既定値で初期化)
	 *
	 *  葉をすべて既定値で初期化した状態のセグメント木を構築する。
	 *    - 時間計算量: \f$ O(n) \f$
	 *
	 *  @param[in] size    最低限必要な葉の数
	 *  @param[in] traits  処理内容を示すオブジェクト
	 */
	explicit LazySegmentTree(
		size_t size = 0, const Traits &traits = Traits()) :
		m_size(1), m_traits(traits)
	{
		while(m_size < size){ m_size *= 2; }
		m_values.resize(m_size * 2 - 1, m_traits.default_value());
		m_modifiers.resize(m_size * 2 - 1, m_traits.default_modifier());
		initialize();
	}

	/**
	 *  @brief コンストラクタ (要素列による初期化)
	 *
	 *  葉をすべて [first, last) で初期化した状態のセグメント木を構築する。
	 *    - 時間計算量: \f$ O(n) \f$ (n = distance(first, last))
	 *
	 *  @param[in] first   要素列の先頭を指すイテレータ
	 *  @param[in] last    要素列の終端を指すイテレータ
	 *  @param[in] traits  処理内容を示すオブジェクト
	 */
	template <typename Iterator>
	LazySegmentTree(
		Iterator first, Iterator last, const Traits &traits = Traits()) :
		m_size(1), m_traits(traits)
	{
		const size_t n = distance(first, last);
		while(m_size < n){ m_size *= 2; }
		m_values.resize(m_size * 2 - 1, m_traits.default_value());
		m_modifiers.resize(m_size * 2 - 1, m_traits.default_modifier());
		copy(first, last, m_values.begin() + m_size - 1);
		initialize();
	}

	/**
	 *  @brief 要素の更新
	 *
	 *  [a, b) の区間中の要素全てを更新する。
	 *    - 時間計算量: \f$ O(\log{n}) \f$
	 *
	 *  @param[in] a         更新したい区間の先頭インデックス
	 *  @param[in] b         更新したい区間の終端インデックス
	 *  @param[in] modifier  更新内容を示す値
	 */
	void modify(int a, int b, const modifier_type &modifier){
		modify(a, b, 0, 0, m_size, modifier);
	}

	/**
	 *  @brief 区間についての問い合わせ
	 *
	 *  インデックスが区間 [a, b) に含まれる要素全てを統合した結果を求める。
	 *    - 時間計算量: \f$ O(\log{n}) \f$
	 *
	 *  @param[in] a  区間の始端
	 *  @param[in] b  区間の終端
	 *  @return    計算された結果
	 */
	value_type query(int a, int b){
		return query(a, b, 0, 0, m_size);
	}

};

/**
 *  @}
 */

}

