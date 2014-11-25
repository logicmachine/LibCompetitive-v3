#include <iostream>
#include <iomanip>
#include <vector>
#include "libcomp/geometry/segment.hpp"
#include "libcomp/geometry/circle.hpp"
#include "libcomp/geometry/line.hpp"
#include "libcomp/geometry/polygon.hpp"
#include "libcomp/geometry/convex.hpp"
#include "libcomp/geometry/debug.hpp"

using namespace std;
static const double EPS = lc::EPS;
static const lc::Point DIR_TABLE[4] = {
	lc::Point(1.0, 0.0),
	lc::Point(0.0, 1.0),
	lc::Point(-1.0, 0.0),
	lc::Point(0.0, -1.0)
};

int main(){
	ios_base::sync_with_stdio(false);
	cout << setiosflags(ios::fixed) << setprecision(10);
	lc::Visualizer vis(1.0, 0.0, 0.0);
	while(true){
		int m, n;
		cin >> m >> n;
		if(m == 0 && n == 0){ break; }
		vector<lc::Point> segments(m), polls(n * 4);
		for(int i = 0; i < m; ++i){ cin >> segments[i].x >> segments[i].y; }
		for(int i = 0; i < n; ++i){
			double x, y;
			cin >> x >> y;
			polls[i * 4 + 0] = lc::Point(x, y) + DIR_TABLE[0] * 10.0 * EPS;
			polls[i * 4 + 1] = lc::Point(x, y) + DIR_TABLE[1] * 10.0 * EPS;
			polls[i * 4 + 2] = lc::Point(x, y) + DIR_TABLE[2] * 10.0 * EPS;
			polls[i * 4 + 3] = lc::Point(x, y) + DIR_TABLE[3] * 10.0 * EPS;
		}
		while(true){
			bool modified = false;
			segments.erase(unique(
				segments.begin(), segments.end()), segments.end());
			vector<lc::Point> next;
			next.clear();
			for(size_t i = 0; i < segments.size(); ++i){
				next.push_back(segments[i]);
				if(i + 2 >= segments.size()){ continue; }
				if((segments[i + 2] - segments[i]).abs() < 2.0 * EPS){ continue; }
				lc::Polygon tri({
					segments[i], segments[i + 1], segments[i + 2]
				});
				for(size_t j = 0; j < polls.size(); ++j){
					const auto &p = polls[j];
					if(tri[1] == p){ tri[1] += DIR_TABLE[j % 4] * EPS; }
				}
				vector<lc::Point> pts({ segments[i], segments[i + 2] });
				for(size_t j = 0; j < polls.size(); ++j){
					const auto &p = polls[j];
					const int contains = tri.contains(p);
					if(contains > 0){
						pts.push_back(p);
					}else if(contains == 0){
						const auto &dir = DIR_TABLE[j % 4];
						const auto proj = lc::Line(
							segments[i], segments[i + 2]).projection(p);
						if(lc::dot(proj - p, dir) < 0.0){ pts.push_back(p); }
					}
				}
				const lc::Polygon conv = convex_hull(pts);
				int locs[3] = { -1, -1, -1 };
				for(int j = 0; j < conv.size(); ++j){
					for(int k = 0; k < 3; ++k){
						if(segments[i + k] == conv[j]){ locs[k] = j; }
					}
				}
				if(locs[1] < 0){
					const int t = conv.size();
					const int d = ((locs[0] + 1) % t == locs[2]) ? -1 : 1;
					for(int j = locs[0]; j != locs[2]; j = (j + d + t) % t){
						next.push_back(conv[j]);
					}
					modified = true;
					for(size_t j = i + 2; j < segments.size(); ++j){
						next.push_back(segments[j]);
					}
					break;
				}else{
					next.push_back(segments[i]);
				}
			}
			if(!modified){ break; }
			segments.swap(next);
		}
		double answer = 0.0;
		for(size_t i = 0; i + 1 < segments.size(); ++i){
			answer += (segments[i + 1] - segments[i]).abs();
		}
		cout << answer << endl;
	}
	return 0;
}

