/**
 *  @file libcomp/misc/top_k.hpp
 */
#pragma once
#include <functional>
#include <utility>

namespace lc {

/**
 *  @defgroup top_k Top-K list
 *  @brief    ソートされた状態で先頭K個の要素のみを保持する固定長リスト
 *  @ingroup  misc
 *  @{
 */

/**
 *  @brief ソートされた状態で先頭K個の要素のみを保持する固定長リスト
 *  @tparam T           値の型
 *  @tparam K           保持する最大要素数
 *  @tparam Comparator  順序付けに使用する比較器
 */
template <class T, int K, class Comparator = std::less<T>>
class TopK {

private:
	Comparator m_comparator;
	int m_count;
	T m_buffer[K];

public:
	/**
	 *  @brief コンストラクタ
	 *  @param[in] cmp  このインスタンスで使用する比較器
	 */
	explicit TopK(const Comparator &cmp = Comparator())
		: m_comparator(cmp)
		, m_count(0)
		, m_buffer()
	{ }

	/**
	 *  @brief     要素の挿入
	 *  @param[in] x  挿入する要素
	 */
	void insert(const T &x){
		if(m_count == K && m_comparator(m_buffer[K - 1], x)){ return; }
		if(m_count < K){ ++m_count; }
		m_buffer[m_count - 1] = x;
		for(int i = m_count - 2; i >= 0; --i){
			if(m_comparator(m_buffer[i + 1], m_buffer[i])){
				std::swap(m_buffer[i + 1], m_buffer[i]);
			}
		}
	}

	/**
	 *  @brief 要素の取得
	 *  @param[in] i  取得する要素のインデックス
	 *  @return    i番目の要素
	 */
	const T &operator[](size_t i) const { return m_buffer[i]; }

	/**
	 *  @brief 挿入されている要素数の取得
	 *  @return 挿入されている要素数
	 */
	size_t size() const { return m_count; }

};

/**
 *  @}
 */

}

