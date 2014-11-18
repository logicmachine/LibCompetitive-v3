/**
 *  @file libcomp/string/longest_common_prefix.hpp
 */
#pragma once
#include "libcomp/string/suffix_array.hpp"

namespace lc {

/**
 *  @defgroup longest_common_prefix Longest common prefix
 *  @brief    最長共通接頭辞
 *  @ingroup  string
 *  @{
 */

/**
 *  @brief Suffix array 上での最長共通接頭辞の計算
 *
 *  Suffix array 上で隣接した2接尾辞間の共通接頭辞の長さを求める。
 *    - 時間計算量: \f$ O(|s|) \f$
 *
 *  @param[in] s   sa の元となった文字列
 *  @param[in] sa  s から計算された接尾辞配列
 *  @return    v[i] = sa[i] と sa[i+1] の共通接頭辞の長さとなるベクタ
 */
template <class T>
std::vector<int> longest_common_prefix(const T &s, const SuffixArray &sa){
	const int n = sa.size();
	std::vector<int> vs(n), isa(n), lcp(n - 1);
	for(int i = 0; i + 1 < n; ++i){ vs[i] = s[i]; }
	for(int i = 0; i < n; ++i){ isa[sa[i]] = i; }
	int h = 0;
	for(int i = 0; i < n; ++i){
		const int j = isa[i];
		if(j > 0){
			const int k = j - 1;
			while(vs[sa[j] + h] == vs[sa[k] + h]){ ++h; }
			lcp[k] = h;
			if(h > 0){ --h; }
		}
	}
	return lcp;
}

/**
 *  @}
 */

}

