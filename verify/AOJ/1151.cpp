#include <iostream>
#include <iomanip>
#include <vector>
#include <utility>
#include "libcomp/geometry/point.hpp"
#include "libcomp/geometry/segment.hpp"
#include "libcomp/geometry/circle.hpp"
#include "libcomp/geometry/crossing_points.hpp"
#include "libcomp/geometry/debug.hpp"

using namespace std;
static const double EPS = lc::EPS;

pair<double, double> solve(
	const lc::Segment &bar, const vector<lc::Point> &poly)
{
	if(bar.length() < EPS){ return make_pair(2.0 * M_PI, 0.0); }
	const int n = poly.size();
	const lc::Circle c(bar.a, bar.length());
	const double base_arg = bar.direction().arg();
	double min_rot = 2.0 * M_PI, max_dist = 0.0;
	for(int i = 0; i < n; ++i){
		const lc::Segment edge(poly[i], poly[(i + 1) % n]);
		auto cps = crossing_points(edge, c);
		if(c.contains(edge.a)){ cps.push_back(edge.a); }
		if(c.contains(edge.b)){ cps.push_back(edge.b); }
		for(const auto &p : cps){
			if((p - bar.a).abs() < EPS){ continue; }
			double arg = base_arg - (p - bar.a).arg();
			while(arg <= -EPS){ arg += 2.0 * M_PI; }
			if(abs(arg) < EPS){
				const lc::Line ortho(
					bar.b + bar.direction().ortho(), bar.b);
				if(lc::tolerant_eq(bar.to_line(), edge.to_line())){
					arg = 2.0 * M_PI;
				}else if(
					!lc::intersect(bar.to_line(), edge.a) &&
					lc::ccw(bar.to_line(), edge.a) == -1)
				{
					if(lc::ccw(ortho, edge.a) != -1){ arg = 2.0 * M_PI; }
				}else if(
					!lc::intersect(bar.to_line(), edge.b) &&
					lc::ccw(bar.to_line(), edge.b) == -1)
				{
					if(lc::ccw(ortho, edge.b) != -1){ arg = 2.0 * M_PI; }
				}else{
					arg = 2.0 * M_PI;
				}
			}
			if(arg < min_rot){
				min_rot = arg;
				max_dist = (p - bar.a).abs();
			}else if(arg == min_rot){
				max_dist = max(max_dist, (p - bar.a).abs());
			}
		}
	}
	return make_pair(min_rot, max_dist);
}

int main(){
	ios_base::sync_with_stdio(false);
	cout << setiosflags(ios::fixed) << setprecision(10);
	while(true){
		double l, r;
		int n;
		cin >> l >> r >> n;
		if(l == 0.0 && r == 0.0 && n == 0){ break; }
		vector<lc::Point> outline(n);
		for(int i = 0; i < n; ++i){ cin >> outline[i].x >> outline[i].y; }
		lc::Segment bar[2] = {
			lc::Segment(lc::Point(0, 0), lc::Point(0, l)),
			lc::Segment(lc::Point(0, 0), lc::Point(0, 0))
		};
		int flip = 0;
		r *= 2.0 * M_PI;
		while(r > EPS){
			const auto head_ret = solve(bar[0], outline);
			const auto tail_ret = solve(bar[1], outline);
			const double rot = min(r, min(head_ret.first, tail_ret.first));
			if(rot < EPS){ break; }
			const lc::Point trans_rot(cos(rot), -sin(rot));
			for(int i = 0; i < 2; ++i){
				if(bar[i].length() < EPS){ continue; }
				bar[i].b = bar[i].a + (bar[i].b - bar[i].a) * trans_rot;
			}
			const double shift =
				head_ret.first <= tail_ret.first ?
					head_ret.second : -tail_ret.second;
			const lc::Point trans_shift = (bar[0].b - bar[1].b).unit() * shift;
			bar[0].a += trans_shift;
			bar[1].a += trans_shift;
			if(bar[0].length() < EPS){
				swap(bar[0], bar[1]);
				flip = 1 - flip;
			}
			r -= rot;
		}
		const lc::Point answer = bar[1 - flip].b;
		cout << answer.x << " " << answer.y << endl;
	}
	return 0;
}

