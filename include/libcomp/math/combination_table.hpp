/**
 *  @file libcomp/math/combination_table.hpp
 */
#pragma once
#include <vector>
#include <limits>

namespace lc {

/**
 *  @defgroup combination_table Combination table
 *  @ingroup  math
 *  @{
 */

/**
 *  @brief 組み合わせテーブル
 *  @tparam T    テーブルの値を保持する型
 *  @tparam MOD  法とする値。明示的に剰余を取らない場合は0。
 */
template <class T, T MOD = 0>
class CombinationTable {
private:
	std::vector<std::vector<T>> m_table;
public:
	/**
	 *  @brief テーブル初期化
	 *  @param[in] n  テーブルのサイズ
	 */
	explicit CombinationTable(int n)
		: m_table(n)
	{
		for(int i = 0; i < n; ++i){
			m_table[i].resize(i + 1);
			m_table[i][0] = m_table[i][i] = 1;
			for(int j = 1; j < i; ++j){
				m_table[i][j] = m_table[i - 1][j - 1] + m_table[i - 1][j];
			}
			if(MOD > 0){
				for(int j = 0; j <= i; ++j){
					if(m_table[i][j] >= MOD){ m_table[i][j] -= MOD; }
				}
			}
		}
	}
	/**
	 *  @brief テーブルの参照
	 *  @param[in] n  \f$ _n C _m \f$ の \f$ n \f$
	 *  @param[in] m  \f$ _n C _m \f$ の \f$ m \f$
	 *  @return    テーブルから取得した \f$ _n C _m \f$ の値
	 */
	T operator()(int n, int m) const {
		if(m < 0 || n < m){ return 0; }
		return m_table[n][m];
	}
};

/**
 *  @}
 */

}

