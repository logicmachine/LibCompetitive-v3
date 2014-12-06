/**
 *  @file libcomp/string/aho_corasick.hpp
 */
#pragma once
#include <string>
#include <vector>
#include <queue>
#include <algorithm>
#include <limits>
#include <iterator>

namespace lc {

/**
 *  @defgroup aho_corasick Aho-Corasick
 *  @brief    Aho-Corasick法による複数パターンマッチング
 *  @ingourp string
 *  @{
 */

/**
 *  @brief Aho-Corasick法による複数パターンマッチング
 *  @tparam CHARACTER_KINDS  文字集合の大きさ
 */
template <int CHARACTER_KINDS = 128>
class AhoCorasick {

private:
	struct State {
		int next[CHARACTER_KINDS];
		int failure;
		std::vector<int> accept;

		State() : failure(0), accept() {
			std::fill(next, next + CHARACTER_KINDS, -1);
		}
	};

	std::vector<State> m_states;

	template <class Iterator>
	void construct_trie(Iterator first, Iterator last){
		m_states.emplace_back();
		int i = 0;
		for(Iterator it = first; it != last; ++it, ++i){
			int cur = 0;
			for(const int c : *it){
				if(m_states[cur].next[c] < 0){
					m_states[cur].next[c] = m_states.size();
					m_states.emplace_back();
				}
				cur = m_states[cur].next[c];
			}
			m_states[cur].accept.push_back(i);
		}
	}

	void write_failure_links(){
		std::queue<int> q;
		q.push(0);
		while(!q.empty()){
			const int index = q.front();
			q.pop();
			const State &s = m_states[index];
			for(int i = 0; i < CHARACTER_KINDS; ++i){
				const int next = s.next[i];
				if(next < 0){ continue; }
				q.push(next);
				if(index == 0){ continue; }
				int f = s.failure;
				while(f != 0 && m_states[f].next[i] < 0){
					f = m_states[f].failure;
				}
				const int failure = std::max(0, m_states[f].next[i]);
				m_states[next].failure = failure;
				const auto &accept = m_states[failure].accept;
				std::copy(
					accept.begin(), accept.end(),
					std::back_inserter(m_states[next].accept));
			}
		}
	}

public:
	/**
	 *  @brief デフォルトコンストラクタ
	 */
	AhoCorasick()
		: m_states(1)
	{ }

	/**
	 *  @brief パターンマッチオートマトンの生成
	 *    - 時間計算量: \f$ O(\sum |S|) \f$
	 *  @param[in] first  パターン集合の先頭を指すイテレータ
	 *  @param[in] last   パターン集合の終端を指すイテレータ
	 */
	template <class Iterator>
	AhoCorasick(Iterator first, Iterator last)
		: m_states()
	{
		construct_trie(first, last);
		write_failure_links();
	}

	/**
	 *  @brief 文字を1文字追加して状態を進める
	 *    - 時間計算量: \f$ O(1) \f$ (文字集合サイズに比例)
	 *  @param[in] c  入力する文字
	 *  @param[in] s  更新する前の状態
	 *  @return    更新後の状態とマッチしたパターンの集合
	 */
	std::pair<int, const std::vector<int> &> iterate(int c, int s) const {
		while(s != 0 && m_states[s].next[c] < 0){ s = m_states[s].failure; }
		s = std::max(0, m_states[s].next[c]);
		return std::pair<int, const std::vector<int> &>(s, m_states[s].accept);
	}

	/**
	 *  @brief  パターンマッチオートマトンのノード数の取得
	 *  @return パターンマッチオートマトンのノード数
	 */
	size_t size() const {
		return m_states.size();
	}

};

/**
 *  @}
 */

}

