/**
 *  @file libcomp/geometry/distance.hpp
 */
#pragma once
#include <cmath>
#include "libcomp/geometry/point.hpp"
#include "libcomp/geometry/line.hpp"
#include "libcomp/geometry/segment.hpp"
#include "libcomp/geometry/circle.hpp"
#include "libcomp/geometry/intersect.hpp"

namespace lc {

/**
 *  @defgroup distance Distance
 *  @brief    幾何要素同士の距離
 *  @ingroup  geometry
 *  @{
 */

/**
 *  @brief 点と点の距離
 *  @param[in] a  判定する点
 *  @param[in] b  判定する点
 *  @return    点aと点bの距離
 */
inline bool distance(const Point &a, const Point &b){
	return (a - b).abs();
}

/**
 *  @brief 点と直線の距離
 *  @param[in] a  判定する点
 *  @param[in] b  判定する直線
 *  @return    点aと直線bの距離
 */
inline double distance(const Point &a, const Line &b){
	const Point p = b.projection(a);
	return (a - p).abs();
}

/**
 *  @brief 直線と点の距離
 *  @param[in] a  判定する直線
 *  @param[in] b  判定する点
 *  @return    直線aと点bの距離
 */
inline double distance(const Line &a, const Point &b){
	return distance(b, a);
}

/**
 *  @brief 点と線分の距離
 *  @param[in] a  判定する点
 *  @param[in] b  判定する線分
 *  @return    点aと線分bの距離
 */
inline double distance(const Point &a, const Segment &b){
	const Point p = b.to_line().projection(a);
	if(intersect(b, p)){ return (a - p).abs(); }
	return std::min((b.a - a).abs(), (b.b - a).abs());
}

/**
 *  @brief 線分と点の距離
 *  @param[in] a  判定する線分
 *  @param[in] b  判定する点
 *  @return    線分aと点bの距離
 */
inline double distance(const Segment &a, const Point &b){
	return distance(b, a);
}

/**
 *  @brief 点と円の距離
 *  @param[in] a  判定する点
 *  @param[in] b  判定する円
 *  @return    点aと円bの距離
 */
inline double distance(const Point &a, const Circle &c){
	return std::abs((a - c.c).abs() - c.r);
}

/**
 *  @brief 円と点の距離
 *  @param[in] a  判定する円
 *  @param[in] b  判定する点
 *  @return    円aと点bの距離
 */
inline double distance(const Circle &a, const Point &b){
	return distance(b, a);
}

/**
 *  @brief 直線と直線の距離
 *  @param[in] a  判定する直線
 *  @param[in] b  判定する直線
 *  @return    直線aと直線bの距離
 */
inline double distance(const Line &a, const Line &b){
	if(!is_parallel(a, b)){ return 0.0; }
	return distance(a.a, b);
}

/**
 *  @brief 直線と線分の距離
 *  @param[in] a  判定する直線
 *  @param[in] b  判定する線分
 *  @return    直線aと線分bの距離
 */
inline double distance(const Line &a, const Segment &b){
	if(intersect(a, b)){ return 0.0; }
	return std::min(distance(b.a, a), distance(b.b, a));
}

/**
 *  @brief 線分と直線の距離
 *  @param[in] a  判定する線分
 *  @param[in] b  判定する直線
 *  @return    線分aと直線bの距離
 */
inline double distance(const Segment &a, const Line &b){
	return distance(b, a);
}

/**
 *  @brief 直線と円の距離
 *  @param[in] a  判定する直線
 *  @param[in] b  判定する円
 *  @return    直線aと円bの距離
 */
inline double distance(const Line &a, const Circle &b){
	return std::max(0.0, distance(b.c, a) - b.r);
}

/**
 *  @brief 円と直線の距離
 *  @param[in] a  判定する円
 *  @param[in] b  判定する直線
 *  @return    円aと直線bの距離
 */
inline double distance(const Circle &a, const Line &b){
	return distance(b, a);
}

/**
 *  @brief 線分と線分の距離
 *  @param[in] a  判定する線分
 *  @param[in] b  判定する線分
 *  @return    線分aと線分bの距離
 */
inline double distance(const Segment &a, const Segment &b){
	if(intersect(a, b)){ return 0.0; }
	const double x = std::min(distance(a.a, b), distance(a.b, b));
	const double y = std::min(distance(b.a, a), distance(b.b, a));
	return std::min(x, y);
}

/**
 *  @brief 線分と円の距離
 *  @param[in] a  判定する線分
 *  @param[in] b  判定する円
 *  @return    線分aと円bの距離
 */
inline double distance(const Segment &a, const Circle &b){
	if(intersect(a, b)){ return 0.0; }
	const double d = std::abs(distance(a, b.c) - b.r);
	return std::min({ distance(a.a, b), distance(a.b, b), d });
}

/**
 *  @brief 円と線分の距離
 *  @param[in] a  判定する円
 *  @param[in] b  判定する線分
 *  @return    円aと線分bの距離
 */
inline double distance(const Circle &a, const Segment &b){
	return distance(b, a);
}

/**
 *  @brief 円と円の距離
 *  @param[in] a  判定する円
 *  @param[in] b  判定する円
 *  @return    円aと円bの距離
 */
inline double distance(const Circle &a, const Circle &b){
	const double d = distance(a.c, b.c);
	const double ra = std::max(a.r, b.r), rb = std::min(a.r, b.r);
	if(d >= ra + rb){ return d - ra - rb; }
	return ra - d - rb;
}

/**
 *  @}
 */

}

