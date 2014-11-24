/**
 *  @file libcomp/geometry/crossing_points.hpp
 */
#pragma once
#include <vector>
#include "libcomp/geometry/line.hpp"
#include "libcomp/geometry/segment.hpp"
#include "libcomp/geometry/circle.hpp"
#include "libcomp/geometry/intersect.hpp"

namespace lc {

/**
 *  @brief 直線と円の交点
 *  @param[in] l  直線
 *  @param[in] c  円
 *  @return    lとcの交点の座標
 */
inline std::vector<Point> crossing_points(const Line &l, const Circle &c){
	const Point &p = l.projection(c.c);
	const double t = (p - c.c).abs();
	if(t > c.r + EPS){ return std::vector<Point>(); }
	if(t > c.r - EPS){ return std::vector<Point>(1, p); }
	const Point v = l.direction();
	const double d = sqrt(c.r * c.r - t * t);
	return std::vector<Point>({ p + d * v, p - d * v });
}

/**
 *  @brief 線分と円の交点
 *  @param[in] s  線分
 *  @param[in] c  円
 *  @return    sとcの交点の座標
 */
inline std::vector<Point> crossing_points(const Segment &s, const Circle &c){
	const std::vector<Point> cps(crossing_points(s.to_line(), c));
	std::vector<Point> result;
	for(const auto &p : cps){
		if(intersect(s, p)){ result.push_back(p); }
	}
	return result;
}

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

