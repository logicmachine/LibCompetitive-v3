#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <cmath>
#include "libcomp/geometry/point.hpp"
#include "libcomp/geometry/line.hpp"
#include "libcomp/geometry/circle.hpp"
#include "libcomp/geometry/polygon.hpp"
#include "libcomp/geometry/distance.hpp"
#include "libcomp/geometry/crossing_points.hpp"
#include "libcomp/geometry/straight_move.hpp"
#include "libcomp/geometry/debug.hpp"

using namespace std;
static const double INF = 1e10;

double rotate_move(
	const lc::Point &p, double start, double r, const lc::Point &obstacle)
{
	const lc::Point cc = p + r * lc::Point(cos(start), sin(start));
	const lc::Point dv = obstacle - p;
	const double d = dv.abs();
	if(d >= 2.0 * r){ return start - M_PI; }
	if(lc::tolerant_eq((cc - obstacle).abs(), r)){
		if(lc::ccw(p, cc, obstacle) == -1){
			return start;
		}else{
			return start - M_PI;
		}
	}
	const lc::Point c =
		p + dv * 0.5 + dv.unit().ortho() * sqrt(r * r - d * d * 0.25);
	double answer = (c - p).arg() + 2.0 * M_PI;
	while(answer >= start - lc::EPS){ answer -= 2.0 * M_PI; }
	return max(answer, start - M_PI);
}
double rotate_move(
	const lc::Point &p, double start, double r, const lc::Segment &obstacle)
{
	if(lc::tolerant_eq(p, obstacle.b)){ return start - M_PI; }
	double answer = -2.0 * M_PI;
	if(lc::tolerant_eq(p, obstacle.a)){
		answer = (obstacle.b - obstacle.a).unit().ortho().arg();
		while(answer > start + lc::EPS){ answer -= 2.0 * M_PI; }
		return answer;
	}else{
		answer = max(
			rotate_move(p, start, r, obstacle.a),
			rotate_move(p, start, r, obstacle.b));
	}
	const lc::Point shift = (obstacle.b - obstacle.a).unit().ortho();
	const lc::Segment shifted_obstacle(
		obstacle.a + shift * r, obstacle.b + shift * r);
	const lc::Circle cpath(p, r);
	const auto cps = lc::crossing_points(shifted_obstacle, cpath);
	if(cps.empty()){
		return answer;
	}
	const lc::Point cc = p + r * lc::Point(cos(start), sin(start));
	if(lc::tolerant_eq(lc::distance(cc, obstacle), r)){
		const auto touch = lc::crossing_points(obstacle, lc::Circle(cc, r));
		if(!lc::tolerant_eq(p, touch[0]) && lc::ccw(p, cc, touch[0]) == -1){
			return start;
		}
	}
	double srot = (cps[0] - p).arg() + 2.0 * M_PI;
	while(srot >= start - lc::EPS){ srot -= 2.0 * M_PI; }
	answer = max(answer, srot);
	return max(answer, start - M_PI);
}

int main(){
	ios_base::sync_with_stdio(false);
	cout << setiosflags(ios::fixed) << setprecision(10);
// lc::Visualizer vis(3.0, 400.0, 400.0);
//lc::Visualizer vis(30.0, 400.0, 400.0);
//lc::Visualizer vis(10.0, 800.0, -100.0);
	while(true){
		int n;
		double sx, sy, r;
		cin >> n >> sx >> sy >> r;
		if(n == 0 && sx == 0.0 && sy == 0.0 && r == 0){ break; }
		lc::Polygon poly(n);
		for(int i = 0; i < n; ++i){
			double x, y;
			cin >> x >> y;
			poly[i] = lc::Point(x - sx, y - sy);
		}
		lc::Circle cur(lc::Point(0.0, 0.0), r);
		double init_x = INF;
		for(int i = 0; i < n; ++i){
			const auto s = poly.side(i);
			init_x = min(
				init_x, lc::straight_move(cur, lc::Point(1.0, 0.0), s));
//vis(s);
		}
		cur.c.x = init_x;
		// initialize (rotate)
		lc::Point prev;
		for(int i = 0; i < n; ++i){
			if(!lc::tolerant_eq((cur.c - poly[i]).abs(), cur.r)){ continue; }
			const double start = (cur.c - poly[i]).arg();
			double goal = start - M_PI;
			for(int j = 0; j < n; ++j){
				goal = max(goal, rotate_move(poly[i], start, cur.r, poly.side(j)));
			}
			cur.c = poly[i] + cur.r * lc::Point(cos(goal), sin(goal));
			prev = poly[i];
			break;
		}
//vis(lc::Circle(cur.c, cur.r + 1.0));
		// initialize (straight move)
		int start_point = -1;
		for(int i = 0; i < n; ++i){
			const lc::Segment &side = poly.side(i);
			const lc::Line &line = side.to_line();
			if(!lc::tolerant_eq(lc::distance(cur.c, side), cur.r)){ continue; }
			if(!lc::tolerant_eq(lc::distance(cur.c, line), cur.r)){ continue; }
			const lc::Point dir = (side.b - side.a).unit();
			double move = (side.b - line.projection(cur.c)).abs();
			for(int j = 0; j < n; ++j){
				move = min(move, lc::straight_move(
					lc::Circle(cur.c, cur.r - lc::EPS),
					dir, poly.side(j)) - lc::EPS);
			}
			cur.c += dir * move;
			prev = line.projection(cur.c);
			start_point = (i + 1) % n;
			break;
		}
//vis(lc::Circle(cur.c, cur.r + 1.0));
//vis(lc::Circle(cur.c, cur.r));
		// initialize (starting point)
		for(int i = 0; start_point < 0 && i < n; ++i){
			if(!lc::tolerant_eq((cur.c - poly[i]).abs(), cur.r)){ continue; }
			prev = poly[i];
			start_point = i;
		}
//vis(lc::Circle(poly[start_point], 0.3));
		// simulate
		vector<lc::Point> trace;
		for(int i = 0, end = 3 * n; i < end; ++i){
			const int j = (i + start_point) % n;
			const lc::Segment &side = poly.side(j);
			const lc::Line &line = side.to_line();
			bool move_flag = false;
			if(lc::tolerant_eq(lc::distance(cur.c, line), cur.r) &&
			   lc::tolerant_eq(lc::distance(cur.c, side), cur.r))
			{
//vis(lc::Circle(prev, 0.1));
//vis(lc::Circle(line.projection(cur.c), 0.2));
				trace.push_back(cur.c);
				trace.push_back(line.projection(cur.c));
				// straight move
				const lc::Point dir = (side.b - side.a).unit();
				double move = (side.b - line.projection(cur.c)).abs();
				for(int k = 0; k < n; ++k){
					move = min(move, lc::straight_move(
						lc::Circle(cur.c, cur.r - lc::EPS),
						dir, poly.side(k)) - lc::EPS);
				}
				cur.c += dir * move;
				prev = line.projection(cur.c);
				trace.push_back(prev);
//vis(cur);
				move_flag = true;
			}
			if((cur.c - side.b).abs() < cur.r + lc::EPS){
				trace.push_back(cur.c);
				trace.push_back(side.b);
				// rotate
				const double start = (cur.c - side.b).arg();
				double goal = start - M_PI;
				for(int k = 0; k < n; ++k){
					goal = max(goal, rotate_move(
						side.b, start, cur.r, poly.side(k)));
				}
				if(lc::ccw(side.a, side.b, poly[(j + 2) % n]) == -2){
					goal = start;
				}
				cur.c = side.b + cur.r * lc::Point(cos(goal), sin(goal));
				trace.push_back(cur.c);
				prev = side.b;
//vis(cur);
				move_flag = true;
			}
			if(end >= 3 * n && move_flag){
				const auto t = trace.back();
				trace.assign(1, t);
				end = i + n + 1;
			}
		}
		std::vector<lc::Point> uniq_trace;
		for(const auto &p : trace){
			if(uniq_trace.empty() || !lc::tolerant_eq(uniq_trace.back(), p)){
				uniq_trace.push_back(p);
			}
		}
		while(lc::tolerant_eq(uniq_trace[0], uniq_trace.back())){
			uniq_trace.pop_back();
		}
		const lc::Polygon trace_poly(uniq_trace.begin(), uniq_trace.end());
//for(int i = 0; i < trace_poly.size(); ++i){
//	vis(trace_poly.side(i));
//}
		double answer = trace_poly.area();
		const int m = trace_poly.size();
		for(int i = 0; i < m; ++i){
			const lc::Point a(trace_poly[i]);
			const lc::Point b(trace_poly[(i + 1) % m]);
			const lc::Point c(trace_poly[(i + 2) % m]);
			if(lc::ccw(a, b, c) == -1){
				double ta = (a - b).arg(), tc = (c - b).arg();
				while(tc < ta){ tc += 2.0 * M_PI; }
				answer += r * r * (tc - ta) * 0.5;
			}
		}
		cout << answer << endl;
	}
	return 0;
}

