/**
 *  @file libcomp/misc/containing_tree.hpp
 */
#pragma once
#include <vector>
#include <set>
#include <queue>
#include <tuple>
#include <algorithm>
#include <utility>
#include "libcomp/misc/rectangle.hpp"
#include "libcomp/misc/coordinate_compression.hpp"

namespace lc {

/**
 *  @defgroup containing_tree Containing tree
 *  @brief    包含関係グラフ構築
 *  @ingroup  misc
 *  @{
 */

/**
 *  @brief 矩形の包含関係を求める
 *
 *  矩形の包含関係表を構築する。
 *    - 時間計算量: \f$ O(n \log^2 {n}) \f$
 *
 *  @note 定数倍改善が足りていない気がする
 *
 *  @param[in] rects  矩形の集合
 *  @return    矩形の親にあたる矩形のインデックスを記録したテーブル
 */
template <class T>
std::vector<int> containing_tree(const std::vector<Rectangle<T>> &rects){
	typedef std::pair<int, int> pii;
	typedef std::tuple<int, int, int, int> tiiii;
	const int n = rects.size();
	CoordinateCompressor<T> xcomp, ycomp;
	for(int i = 0; i < n; ++i){
		xcomp.push(rects[i].x1);
		ycomp.push(rects[i].y1);
		xcomp.push(rects[i].x2);
		ycomp.push(rects[i].y2);
	}
	xcomp.build();
	ycomp.build();
	std::vector<std::pair<Rectangle<int>, int>> rs(n);
	for(int i = 0; i < n; ++i){
		const auto &r = rects[i];
		rs[i] = std::make_pair(Rectangle<int>(
			xcomp.compress(r.x1), ycomp.compress(r.y1),
			xcomp.compress(r.x2), ycomp.compress(r.y2)), i);
	}
	std::sort(rs.begin(), rs.end());
	const int m = xcomp.size();
	std::vector<std::set<pii>> raw_bit(m + 1);
	std::priority_queue<tiiii, std::vector<tiiii>, std::greater<tiiii>> pq;
	std::vector<int> parents(n, -1);
	for(int i = 0; i < n; ++i){
		const auto &r = rs[i].first;
		while(!pq.empty() && std::get<0>(pq.top()) <= r.y1){
			for(int j = std::get<1>(pq.top()) + 1; j <= m; j += j & -j){
				raw_bit[j].erase(std::make_pair(
					std::get<2>(pq.top()), std::get<3>(pq.top())));
			}
			pq.pop();
		}
		pii answer(xcomp.size(), n);
		for(int j = r.x1; j > 0; j -= j & -j){
			const auto it = raw_bit[j].lower_bound(std::make_pair(r.x2, -1));
			if(it != raw_bit[j].end()){
				answer = std::min(answer, *it);
			}
		}
		const int k = rs[i].second;
		for(int j = r.x1 + 1; j <= m; j += j & -j){
			raw_bit[j].insert(std::make_pair(r.x2, k));
		}
		parents[k] = (answer.second >= n ? -1 : answer.second);
		pq.push(std::make_tuple(r.y2, r.x1, r.x2, k));
	}
	return parents;
}

/**
 *  @}
 */

}

