/**
 *  @file libcomp/geometry/convex.hpp
 */
#pragma once
#include <algorithm>
#include <cassert>
#include "libcomp/geometry/polygon.hpp"
#include "libcomp/geometry/intersect.hpp"
#include "libcomp/geometry/crossing_points.hpp"

namespace lc {

/**
 *  @defgroup convex Convex
 *  @ingroup  geometry
 *  @{
 */

/**
 *  @brief 凸多角形かの判定
 *    - 時間計算量: \f$ O(|P|) \f$
 *
 *  @param[in] p  判定する多角形
 *  @retval    true   pが凸多角形だった場合
 *  @retval    false  pが凸多角形ではなかった場合
 *  @note      自己交差を含む多角形には使用不可
 */
inline bool is_convex(const Polygon &p){
	const size_t n = p.size();
	if(n <= 2){ return true; }
	int first_ccw = 0;
	for(size_t i = 0; i < n; ++i){
		const size_t j = (i + 1 >= n) ? 0 : (i + 1);
		const size_t k = (j + 1 >= n) ? 0 : (j + 1);
		const int cur_ccw = ccw(p[i], p[j], p[k]);
		if(i == 0){
			first_ccw = cur_ccw;
			if(cur_ccw != 1 && cur_ccw != -1){ return false; }
		}else if(cur_ccw != first_ccw){
			return false;
		}
	}
	return true;
}

/**
 *  @brief 点群に対する凸包の計算
 *    - 時間計算量: \f$ O(|P| \log{|P|}) \f$
 *
 *  Andrew's Monotone Chain による凸包の計算。
 *  参考: http://www.prefield.com/algorithm/geometry/convex_hull.html
 *
 *  @param[in] points  点群
 *  @return    pointsに対する凸包となる多角形
 */
inline Polygon convex_hull(const std::vector<Point> &points){
	const int n = points.size();
	std::vector<Point> ps(points);
	std::sort(ps.begin(), ps.end());
	std::vector<Point> ch(2 * n);
	int k = 0;
	for(int i = 0; i < n; ch[k++] = ps[i++]){
		while(k >= 2 && ccw(ch[k - 2], ch[k - 1], ps[i]) <= 0){ --k; }
	}
	for(int i = n - 2, t = k + 1; i >= 0; ch[k++] = ps[i--]){
		while(k >= t && ccw(ch[k - 2], ch[k - 1], ps[i]) <= 0){ --k; }
	}
	ch.resize(k - 1);
	return Polygon(ch.begin(), ch.end());
}

/**
 *  @brief 凸多角形の直線による切断
 *    - 時間計算量: \f$ O(|P|) \f$
 *
 *  凸多角形を直線lで切断し、その左側にあった領域を求める。
 *
 *  @param[in] p  切断する凸多角形
 *  @param[in] l  切断に使用する直線
 *  @return    pからlの左側にある部分のみを抽出した多角形
 */
inline Polygon convex_cut(const Polygon &p, const Line &l){
	const int n = p.size();
	std::vector<Point> ps;
	for(int i = 0; i < n; ++i){
		if(ccw(l.a, l.b, p[i]) != -1){ ps.push_back(p[i]); }
		if(ccw(l.a, l.b, p[i]) * ccw(l.a, l.b, p[(i + 1) % n]) < 0){
			ps.push_back(crossing_points(p.side(i), l)[0]);
		}
	}
	return Polygon(ps.begin(), ps.end());
}

/**
 *  @brief 凸多角形の共通領域
 *    - 時間計算量: \f$ O(|P| |Q|) \f$
 *
 *  @todo  \f$ O(n + m) \f$ のアルゴリズムを実装する
 *  @param[in] p  凸多角形1
 *  @param[in] q  凸多角形2
 *  @return    pとqの共通領域を表す多角形
 */
inline Polygon convex_and(const Polygon &p, const Polygon &q){
	const int m = q.size();
	Polygon ret = p;
	for(int i = 0; i < m; ++i){ ret = convex_cut(ret, q.side(i).to_line()); }
	return ret;
}

/**
 *  @}
 */

}

