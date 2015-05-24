#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <iterator>
#include <utility>
#include "libcomp/geometry/convex.hpp"
#include "libcomp/geometry/distance.hpp"
#include "libcomp/misc/ternary_search.hpp"

using namespace std;
static const double MAX_TIME = 1e8;

inline pair<lc::Polygon, lc::Point> read_convex(){
	int n;
	cin >> n;
	lc::Polygon p(n);
	for(int i = 0; i < n; ++i){ cin >> p[i].x >> p[i].y; }
	if(lc::ccw(p[0], p[1], p[2]) != 1){
		for(int i = 0; i + i < n; ++i){ swap(p[i], p[n - 1 - i]); }
	}
	lc::Point delta;
	cin >> delta.x >> delta.y;
	return make_pair(p, delta);
}

inline vector<double> compute_tset(
	const pair<lc::Polygon, lc::Point> &p,
	const pair<lc::Polygon, lc::Point> &q)
{
	const int n = p.first.size(), m = q.first.size();
	vector<double> tset;
	for(int i = 0; i < n; ++i){
		for(int j = 0; j < m; ++j){
			const auto dist_func = [&](double x) -> double {
				const auto t = p.first[i] + p.second * x;
				const lc::Segment s(
					q.first[j] + q.second * x,
					q.first[(j + 1) % m] + q.second * x);
				return lc::distance(s, t);
			};
			const double t = lc::ternary_search(0.0, MAX_TIME, dist_func);
			const double d = dist_func(t);
			if(t > 0.0 && d < lc::EPS){ tset.push_back(t); }
		}
	}
	return tset;
}

int main(){
	ios_base::sync_with_stdio(false);
	cout << setiosflags(ios::fixed) << setprecision(10);
	const auto p = read_convex();
	const auto q = read_convex();
	const int n = p.first.size(), m = q.first.size();
	vector<double> tset;
	const auto tset_a = compute_tset(p, q);
	const auto tset_b = compute_tset(q, p);
	tset.push_back(0.0);
	tset.push_back(MAX_TIME);
	copy(tset_a.begin(), tset_a.end(), back_inserter(tset));
	copy(tset_b.begin(), tset_b.end(), back_inserter(tset));
	sort(tset.begin(), tset.end());
	const auto func = [&](double t) -> double {
		lc::Polygon sp = p.first, sq = q.first;
		for(int i = 0; i < n; ++i){ sp[i] += p.second * t; }
		for(int i = 0; i < m; ++i){ sq[i] += q.second * t; }
		const double a = lc::convex_and(sp, sq).area();
		if(a > 0.0){ return -a; }
		for(int i = 0; i < n; ++i){
			for(int j = 0; j < m; ++j){
				if(intersect(sp[i], sq.side(j))){ return 0.0; }
				if(intersect(sp.side(i), sq[j])){ return 0.0; }
			}
		}
		return 1.0;
	};
	double max_area = -1.0, answer = 0.0;
	for(size_t i = 0; i + 1 < tset.size(); ++i){
		const double t = lc::ternary_search(tset[i], tset[i + 1], func);
		const double area = -func(t);
		if(area > max_area + lc::EPS){
			max_area = area;
			answer = t;
		}
	}
	if(max_area < 0.0){
		cout << "never" << endl;
	}else{
		cout << answer << endl;
	}
	return 0;
}

