/**
 *  @file libcomp/string/suffix_array.hpp
 */
#pragma once
#include <string>
#include <vector>
#include <algorithm>

namespace lc {

/**
 *  @defgroup suffix_array Suffix array
 *  @brief    接尾辞配列
 *  @ingroup  string
 *  @{
 */

/**
 *  @brief 接尾辞配列
 *
 *  SA-IS による接尾辞配列の構築を行う。
 *  Two Efficient Algorithms for Linear Time Suffix Array Construction
 *  に書かれているCのコードを少しC++っぽくしただけ。
 */
class SuffixArray {

private:
	std::vector<int> m_suffix_array;

	template <bool End, class T>
	std::vector<int> compute_buckets(const T *s, int n, int k) const {
		std::vector<int> buckets(k);
		for(int i = 0; i < n; ++i){ ++buckets[s[i]]; }
		for(int i = 0, sum = 0; i < k; ++i){
			sum += buckets[i];
			buckets[i] = (End ? sum : (sum - buckets[i]));
		}
		return buckets;
	}

	template <class T>
	void induce_sa_l(
		int *sa, const T *s, int n, int k,
		const std::vector<bool> &types) const
	{
		std::vector<int> buckets(compute_buckets<false>(s, n, k));
		for(int i = 0; i < n; ++i){
			const int j = sa[i] - 1;
			if(j >= 0 && !types[j]){ sa[buckets[s[j]]++] = j; }
		}
	}
	template <class T>
	void induce_sa_s(
		int *sa, const T *s, int n, int k,
		const std::vector<bool> &types) const
	{
		std::vector<int> buckets(compute_buckets<true>(s, n, k));
		for(int i = n - 1; i >= 0; --i){
			const int j = sa[i] - 1;
			if(j >= 0 && types[j]){ sa[--buckets[s[j]]] = j; }
		}
	}

	bool is_lms(const std::vector<bool> &types, int i) const {
		return i > 0 && !types[i - 1] && types[i];
	}

	template <class T>
	void sa_is(int *sa, const T *s, int n) const {
		std::vector<bool> types(n);
		types[n - 1] = true;
		for(int i = n - 2; i >= 0; --i){
			types[i] = (s[i] != s[i + 1] ? s[i] < s[i + 1] : types[i + 1]);
		}
		const int k = static_cast<int>(*std::max_element(s, s + n)) + 1;
		std::vector<int> buckets(compute_buckets<true>(s, n, k));
		for(int i = 0; i < n; ++i){ sa[i] = -1; }
		for(int i = 1; i < n; ++i){
			if(is_lms(types, i)){ sa[--buckets[s[i]]] = i; }
		}
		induce_sa_l(sa, s, n, k, types);
		induce_sa_s(sa, s, n, k, types);
		int m = 0;
		for(int i = 0; i < n; ++i){
			if(is_lms(types, sa[i])){ sa[m++] = sa[i]; }
		}
		for(int i = m; i < n; ++i){ sa[i] = -1; }
		int num_names = 0, prev = -1;
		for(int i = 0; i < m; ++i){
			int p = sa[i];
			bool diff = false;
			for(int d = 0; d < n; ++d){
				const int l = p + d, r = prev + d;
				if(prev < 0 || s[l] != s[r] || types[l] != types[r]){
					diff = true;
					break;
				}else if(d > 0 && (is_lms(types, l) || is_lms(types, r))){
					break;
				}
			}
			if(diff){
				++num_names;
				prev = p;
			}
			p = (p - (p & 1)) / 2;
			sa[m + p] = num_names - 1;
		}
		for(int i = n - 1, j = n - 1; i >= m; --i){
			if(sa[i] >= 0){ sa[j--] = sa[i]; }
		}
		int *reduced_s = sa + n - m;
		if(num_names < m){
			sa_is(sa, reduced_s, m);
		}else{
			for(int i = 0; i < m; ++i){ sa[reduced_s[i]] = i; }
		}
		buckets = compute_buckets<true>(s, n, k);
		for(int i = 1, j = 0; i < n; ++i){
			if(is_lms(types, i)){ reduced_s[j++] = i; }
		}
		for(int i = 0; i < m; ++i){ sa[i] = reduced_s[sa[i]]; }
		for(int i = m; i < n; ++i){ sa[i] = -1; }
		for(int i = m - 1; i >= 0; --i){
			const int j = sa[i];
			sa[i] = -1;
			sa[--buckets[s[j]]] = j;
		}
		induce_sa_l(sa, s, n, k, types);
		induce_sa_s(sa, s, n, k, types);
	}

	template <class T>
	std::vector<int> sa_is(const T &s) const {
		if(s.size() == 0){ return std::vector<int>(1); }
		const int n = s.size() + 1;
		std::vector<int> vs(n);
		for(int i = 0; i + 1 < n; ++i){ vs[i] = s[i]; }
		std::vector<int> sa(n);
		sa_is(sa.data(), vs.data(), n);
		return sa;
	}

public:
	/**
	 *  @brief デフォルトコンストラクタ
	 */
	SuffixArray() : m_suffix_array(1) { }
	/**
	 *  @brief 接尾辞配列の構築
	 *    - 時間計算量: \f$ O(n) \f$
	 *  @param[in] s  接尾辞配列の元とする文字列
	 */
	template <class T>
	explicit SuffixArray(const T &s)
		: m_suffix_array(sa_is(s))
	{ }

	/**
	 *  @brief  接尾辞配列のサイズ種痘
	 *  @return 接尾辞配列のサイズ（元の文字列の長さ+1）
	 */
	size_t size() const { return m_suffix_array.size(); }

	/**
	 *  @brief 接尾辞配列の取得
	 *  @param[in] i  取得したい要素のインデックス
	 *  @return    辞書順でi番目の接尾辞の先頭文字のインデックス
	 */
	int operator[](int i) const { return m_suffix_array[i]; }

};

/**
 *  @}
 */

}

