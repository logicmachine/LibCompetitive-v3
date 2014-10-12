/**
 *  @file libcomp/string/kmp.hpp
 */
#pragma once
#include <string>
#include <vector>
#include <utility>

namespace lc {

/**
 *  @defgroup kmp Knuth-Morris-Pratt algorithm
 *  @ingroup  string
 *  @{
 */

/**
 *  @brief KMP法による部分文字列検索
 */
class KMPMatcher {

private:
	std::string m_pattern;
	std::vector<int> m_table;

public:
	/**
	 *  @brief テーブルの構築
	 *  @param[in] pattern  検索するパターン
	 */
	explicit KMPMatcher(const std::string &pattern = std::string())
		: m_pattern(pattern)
		, m_table(pattern.size())
	{
		const int n = pattern.size();
		if(n == 0){ return; }
		m_table[0] = -1;
		for(int i = 2, j = 0; i < n; ++i){
			if(pattern[i - 1] == pattern[j]){
				m_table[i] = ++j;
			}else if(j > 0){
				j = m_table[j];
				--i;
			}else{
				m_table[i] = 0;
			}
		}
	}

	/**
	 *  @brief 文字を1文字追加して状態を進める
	 *  @param[in] c      入力する文字
	 *  @param[in] state  更新する前の状態
	 *  @return    更新後の状態とマッチの有無
	 */
	std::pair<int, bool> iterate(char c, int state) const {
		const int n = m_pattern.size();
		while(state >= 0){
			if(m_pattern[state] == c){ break; }
			state = m_table[state];
		}
		if(++state == n){ return std::make_pair(0, true); }
		return std::make_pair(state, false);
	}

	/**
	 *  @brief 文字列中のパターン出現回数の数え上げ
	 *  @param[in] s  検索対象のテキスト
	 *  @return    s中に含まれるパターンの数
	 */
	int match(const std::string &s) const {
		const int n = s.size();
		int result = 0, state = 0;
		for(int i = 0; i < n; ++i){
			const auto p = iterate(s[i], state);
			if(p.second){ ++result; }
			state = p.first;
		}
		return result;
	}

};

/**
 *  @}
 */

}

