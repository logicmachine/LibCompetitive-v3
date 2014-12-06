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
 *  @infgourp string
 *  @{
 */

template <int MAX_CODE = 128>
class AhoCorasick {

private:
	struct State {
		int next[MAX_CODE];
		int failure;
		std::vector<int> accept;

		State() : failure(0), accept() {
			std::fill(next, next + MAX_CODE, -1);
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
			for(int i = 0; i < MAX_CODE; ++i){
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
	AhoCorasick()
		: m_states(1)
	{ }

	template <class Iterator>
	AhoCorasick(Iterator first, Iterator last)
		: m_states()
	{
		construct_trie(first, last);
		write_failure_links();
	}

	std::pair<int, const std::vector<int> &> iterate(int c, int s) const {
		while(s != 0 && m_states[s].next[c] < 0){ s = m_states[s].failure; }
		s = std::max(0, m_states[s].next[c]);
		return std::pair<int, const std::vector<int> &>(s, m_states[s].accept);
	}

	size_t size() const {
		return m_states.size();
	}

};

/**
 *  @}
 */

}

