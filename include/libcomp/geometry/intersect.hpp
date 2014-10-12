/**
 *  @file libcomp/geometry/intersect.h
 */
#pragma once
#include "libcomp/geometry/point.hpp"
#include "libcomp/geometry/line.hpp"
#include "libcomp/geometry/segment.hpp"
#include "libcomp/geometry/circle.hpp"

namespace lc {

/**
 *  @defgroup intersect Intersect
 *  @ingroup  geometry
 *  @{
 */

/**
 *  @brief 点と点の交差判定
 *  @param[in] a      判定する点
 *  @param[in] b      判定する点
 *  @retval    true   aとbが交差している
 *  @retval    false  aとbが交差していない
 */
inline bool intersect(const Point &a, const Point &b){
	return tolerant_eq(a, b);
}

/**
 *  @brief 点と直線の交差判定
 *  @param[in] a      判定する点
 *  @param[in] b      判定する直線
 *  @retval    true   aとbが交差している
 *  @retval    false  aとbが交差していない
 */
inline bool intersect(const Point &a, const Line &b){
	return abs(cross(b.b - a, b.a - a)) < EPS;
}

/**
 *  @brief 直線と点の交差判定
 *  @param[in] a      判定する直線
 *  @param[in] b      判定する点
 *  @retval    true   aとbが交差している
 *  @retval    false  aとbが交差していない
 */
inline bool intersect(const Line &a, const Point &b){
	return intersect(b, a);
}

/**
 *  @brief 点と線分の交差判定
 *  @param[in] a      判定する点
 *  @param[in] b      判定する線分
 *  @retval    true   aとbが交差している
 *  @retval    false  aとbが交差していない
 */
inline bool intersect(const Point &a, const Segment &b){
	return (b.a - a).abs() + (b.b - a).abs() - (b.b - b.a).abs() < EPS;
}

/**
 *  @brief 線分と点の交差判定
 *  @param[in] a      判定する線分
 *  @param[in] b      判定する点
 *  @retval    true   aとbが交差している
 *  @retval    false  aとbが交差していない
 */
inline bool intersect(const Segment &a, const Point &b){
	return intersect(b, a);
}

/**
 *  @brief 点と円の交差判定
 *  @param[in] a      判定する点
 *  @param[in] b      判定する円
 *  @retval    true   aとbが交差している
 *  @retval    false  aとbが交差していない
 */
inline bool intersect(const Point &a, const Circle &b){
	return tolerant_eq((a - b.c).abs(), b.r);
}

/**
 *  @brief 円と点の交差判定
 *  @param[in] a      判定する円
 *  @param[in] b      判定する点
 *  @retval    true   aとbが交差している
 *  @retval    false  aとbが交差していない
 */
inline bool intersect(const Circle &a, const Point &b){
	return intersect(b, a);
}

/**
 *  @brief 直線と直線の交差判定
 *  @param[in] a      判定する直線
 *  @param[in] b      判定する直線
 *  @retval    true   aとbが交差している
 *  @retval    false  aとbが交差していない
 */
inline bool intersect(const Line &a, const Line &b){
	if(abs(cross(a.b - a.a, b.b - b.a)) > EPS){ return true; }
	return tolerant_eq(a, b);
}

/**
 *  @brief 直線と線分の交差判定
 *  @param[in] a      判定する直線
 *  @param[in] b      判定する線分
 *  @retval    true   aとbが交差している
 *  @retval    false  aとbが交差していない
 */
inline bool intersect(const Line &a, const Segment &b){
	return cross(a.b - a.a, b.a - a.a) * cross(a.b - a.a, b.b - a.a) < EPS;
}

/**
 *  @brief 線分と直線の交差判定
 *  @param[in] a      判定する線分
 *  @param[in] b      判定する直線
 *  @retval    true   aとbが交差している
 *  @retval    false  aとbが交差していない
 */
inline bool intersect(const Segment &a, const Line &b){
	return intersect(b, a);
}

/**
 *  @brief 直線と円の交差判定
 *  @param[in] a      判定する直線
 *  @param[in] b      判定する円
 *  @retval    true   aとbが交差している
 *  @retval    false  aとbが交差していない
 */
inline bool intersect(const Line &a, const Circle &b){
	return (b.c - a.projection(b.c)).abs() <= b.r;
}
/**
 *  @brief 円と直線の交差判定
 *  @param[in] a      判定する円
 *  @param[in] b      判定する直線
 *  @retval    true   aとbが交差している
 *  @retval    false  aとbが交差していない
 */
inline bool intersect(const Circle &a, const Line &b){
	return intersect(b, a);
}

/**
 *  @brief 線分と線分の交差判定
 *  @param[in] a      判定する線分
 *  @param[in] b      判定する線分
 *  @retval    true   aとbが交差している
 *  @retval    false  aとbが交差していない
 */
inline bool intersect(const Segment &a, const Segment &b){
	if(ccw(a.a, a.b, b.a) * ccw(a.a, a.b, b.b) > 0){ return false; }
	if(ccw(b.a, b.b, a.a) * ccw(b.a, b.b, a.b) > 0){ return false; }
	return true;
}

/**
 *  @brief 線分と円の交差判定
 *  @param[in] a      判定する線分
 *  @param[in] b      判定する円
 *  @retval    true   aとbが交差している
 *  @retval    false  aとbが交差していない
 */
inline bool intersect(const Segment &a, const Circle &b){
	const Point r = a.to_line().projection(b.c);
	const double d = intersect(a, r) ?
		(r - b.c).abs() : std::min((a.a - b.c).abs(), (a.b - b.c).abs());
	if(d > b.r){ return false; }
	return (a.a - b.c).abs() >= b.r || (a.b - b.c).abs() >= b.r;
}

/**
 *  @brief 円と線分の交差判定
 *  @param[in] a      判定する円
 *  @param[in] b      判定する線分
 *  @retval    true   aとbが交差している
 *  @retval    false  aとbが交差していない
 */
inline bool intersect(const Circle &a, const Segment &b){
	return intersect(b, a);
}

/**
 *  @brief 円と円の交差判定
 *  @param[in] a      判定する円
 *  @param[in] b      判定する円
 *  @retval    true   aとbが交差している
 *  @retval    false  aとbが交差していない
 */
inline bool intersect(const Circle &a, const Circle &b){
	const double d = (a.c - b.c).abs();
	return d <= a.r + b.r && a.r + d >= b.r && b.r + d >= a.r;
}

/**
 *  @}
 */

}

