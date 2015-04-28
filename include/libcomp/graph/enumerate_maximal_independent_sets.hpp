/**
 *  @file libcomp/graph/enumerate_maximal_independent_sets.hpp
 */
#pragma once
#include <vector>
#include <functional>
#include <cassert>
#include <cstdint>
#include "libcomp/graph/adjacency_list.hpp"

namespace lc {

/**
 *  @defgroup enumerate_maximal_independent_sets Enumerate maximal independent sets
 *  @brief    極大独立集合の列挙
 *  @ingroup  graph
 *  @{
 */

/**
 *  @brief 極大独立集合の列挙
 *    - 時間計算量: \f$ O(1.466^|V|) \f$ ?
 *
 *  @param[in] graph  極大独立集合を求める対象とするグラフ
 *  @param[in] func   列挙された極大独立集合を処理する関数オブジェクト
 */
template <typename EdgeType, typename Func>
void enumerate_maximal_independent_sets(
	const AdjacencyList<EdgeType> &graph, Func func)
{
	const int n = graph.size();
	std::vector<uint64_t> bit_graph(n), incr_bit_graph(n + 1);
	for(int i = n - 1; i >= 0; --i){
		uint64_t mask = (1ull << i);
		for(const auto &e : graph[i]){ mask |= (1ull << e.to); }
		bit_graph[i] = mask;
		incr_bit_graph[i] = mask | incr_bit_graph[i + 1];
	}
	std::function<void(int, uint64_t, uint64_t)> recur =
		[&, n](int i, uint64_t picked, uint64_t eliminated) -> void {
			if(i == n){
				func(picked);
				return;
			}
			const bool force_ignore = ((eliminated & (1ull << i)) != 0);
			int flags = 1; // 1: select v_i, 2: ignore v_i
			if(bit_graph[i] & ~(incr_bit_graph[i + 1] | eliminated)){
				flags = (force_ignore ? 2 : 1);
			}else if((incr_bit_graph[i + 1] | eliminated) & (1ull << i)){
				flags = (force_ignore ? 2 : 3);
			}
			if(flags & 1){
				recur(i + 1, picked | (1ull << i), eliminated | bit_graph[i]);
			}
			if(flags & 2){ recur(i + 1, picked, eliminated); }
		};
	recur(0, 0, 0);
}


/**
 *  @}
 */

}

