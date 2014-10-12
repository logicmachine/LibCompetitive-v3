/**
 *  @file libcomp/geometry/crossing_points.hpp
 */
#pragma once
#include <vector>
#include "libcomp/geometry/circle.hpp"
#include "libcomp/geometry/intersect.hpp"

namespace lc {

/**
 *  @brief 円と円の交点
 *  @param[in] a  片方の円
 *  @param[in] b  片方の円
 *  @return    aとbの交点の座標
 *  @note aとbが等しい場合は代表として3点を返す
 */
inline std::vector<Point> crossing_points(const Circle &a, const Circle &b){
	std::vector<Point> ret;
	if(!intersect(a, b)){ return ret; }
	if(tolerant_eq(a, b)){
		ret.push_back(a.c + Point(a.r, 0.0));
		ret.push_back(a.c + Point(-a.r, 0.0));
		ret.push_back(a.c + Point(0.0, a.r));
	}else{
		const double d = (a.c - b.c).abs();
		const double rc = (d * d + a.r * a.r - b.r * b.r) / (2.0 * d);
		const double rs = sqrt(a.r * a.r - rc * rc);
		const Point diff = (b.c - a.c) / d;
		const Point x = a.c + diff * Point(rc, rs);
		const Point y = a.c + diff * Point(rc, -rs);
		ret.push_back(x);
		if(!tolerant_eq(x, y)){ ret.push_back(y); }
	}
	return ret;
}

}

