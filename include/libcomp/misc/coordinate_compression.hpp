/**
 *  @file libcomp/misc/coordinate_compression.hpp
 */
#pragma once
#include <vector>
#include <algorithm>

namespace lc {

/**
 *  @defgroup coordinate_compression Coordinate compression
 *  @brief    座標圧縮
 *  @ingroup  misc
 *  @{
 */

/**
 *  @brief  座標圧縮
 *  @tparam T  圧縮前の座標の型
 */
template <class T>
class CoordinateCompressor {
private:
	std::vector<T> m_coords;
public:
	/**
	 *  @brief デフォルトコンストラクタ
	 */
	CoordinateCompressor() : m_coords() { }

	/**
	 *  @brief 初期化つきコンストラクタ
	 *  @param[in] first  圧縮前の座標候補列の先頭
	 *  @param[in] last   圧縮前の座標候補列の終端
	 */
	template <class Iterator>
	CoordinateCompressor(Iterator first, Iterator last)
		: m_coords(first, last)
	{
		build();
	}

	/**
	 *  @brief 圧縮前の座標候補を追加
	 *
	 *  圧縮用テーブルは build を呼ぶまで再構築されないことに注意。
	 *
	 *  @param[in] x  追加する座標
	 */
	void push(const T &x){
		m_coords.push_back(x);
	}

	/**
	 *  @brief 圧縮用テーブルの再構築
	 *    - 時間計算量: \f$ O(n \log{n}) \f$
	 *  @return 圧縮用テーブルのサイズ
	 */
	size_t build(){
		sort(m_coords.begin(), m_coords.end());
		m_coords.erase(
			unique(m_coords.begin(), m_coords.end()), m_coords.end());
		return m_coords.size();
	}

	/**
	 *  @brief 座標の圧縮
	 *    - 時間計算量: \f$ O(\log{n}) \f$
	 *  @param[in] x  圧縮前の座標
	 *  @return    圧縮された後の座標
	 */
	size_t compress(const T &x) const {
		return std::lower_bound(
			m_coords.begin(), m_coords.end(), x) - m_coords.begin();
	}

	/**
	 *  @brief 座標の伸長
	 *    - 時間計算量: \f$ O(1) \f$
	 *  @param[in] x  伸長前の座標
	 *  @return    伸長された後の座標
	 */
	const T &decompress(const size_t x) const {
		return m_coords[x];
	}
};

/**
 *  @}
 */

}

