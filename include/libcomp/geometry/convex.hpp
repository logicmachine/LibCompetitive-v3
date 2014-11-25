/**
 *  @file libcomp/geometry/convex.hpp
 */
#pragma once
#include <algorithm>
#include "libcomp/geometry/polygon.hpp"

namespace lc {

/**
 *  @defgroup convex Convex
 *  @ingroup  geometry
 *  @{
 */

/**
 *  @brief 凸多角形かの判定
 *
 *  計算量は \f$ \mathcal{O}(|P|) \f$。
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
 *
 *  Andrew's Monotone Chain による凸包の計算。
 *  計算量は \f$ \mathcal{O}(|P| \log{|P|}) \f$。
 *
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
 *  @}
 */

}

