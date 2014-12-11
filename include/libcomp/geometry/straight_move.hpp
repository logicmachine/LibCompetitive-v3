/**
 *  @file libcomp/geometry/straight_move.hpp
 */
#pragma once
#include <limits>
#include "libcomp/geometry/point.hpp"
#include "libcomp/geometry/line.hpp"
#include "libcomp/geometry/segment.hpp"
#include "libcomp/geometry/circle.hpp"
#include "libcomp/geometry/intersect.hpp"
#include "libcomp/geometry/crossing_points.hpp"

namespace lc {

/**
 *  @defgroup straight_move Straight move
 *  @brief    障害物がある状態での直線運動
 *  @ingroup  geometry
 *  @{
 */

/**
 *  @brief  障害物（点）がある状態での円の直線運動
 */
double straight_move(const Circle &c, Point dir, const Point &obstacle){
	const double inf = std::numeric_limits<double>::infinity();
	dir = dir.unit();
	if(ccw(c.c, c.c + dir.ortho(), obstacle) != -1){ return inf; }
	const Line dir_line(c.c, c.c + dir);
	const Point proj = dir_line.projection(obstacle);
	const double a = (obstacle - proj).abs();
	if(a >= c.r){ return inf; }
	const double b = sqrt(c.r * c.r - a * a);
	return ((proj - b * dir) - c.c).abs();
}

/**
 *  @brief  障害物（線分）がある状態での円の直線運動
 */
double straight_move(const Circle &c, Point dir, const Segment &obstacle){
	dir = dir.unit();
	const Line dir_line(c.c, c.c + dir);
	const Point delta = (obstacle.b - obstacle.a).unit().ortho();
	double answer = std::min(
		straight_move(c, dir, obstacle.a), straight_move(c, dir, obstacle.b));
	for(int i = -1; i <= 1; i += 2){
		const Point d(i * c.r * delta);
		const Segment s(obstacle.a + d, obstacle.b + d);
		if(!intersect(s, dir_line)){ continue; }
		const auto cps = crossing_points(dir_line, s.to_line());
		if(cps.size() != 1){ continue; }
		if(ccw(c.c, c.c + dir.ortho(), cps[0] - d) != -1){ continue; }
		answer = std::min(answer, (cps[0] - c.c).abs());
	}
	return answer;
}

/**
 *  @}
 */

}

