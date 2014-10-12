#include <iostream>
#include <iomanip>
#include <utility>
#include <algorithm>
#include "libcomp/geometry/crossing_points.hpp"
#include "libcomp/geometry/debug.hpp"

using namespace std;
typedef pair<lc::Point, int> ppi;

inline bool contains_all(const lc::Circle &c, const vector<lc::Circle> &cs){
	for(const auto &t : cs){
		const double d = (c.c - t.c).abs();
		if(d + t.r > c.r + lc::EPS){ return false; }
	}
	return true;
}

int main(){
	ios_base::sync_with_stdio(false);
	cout << setiosflags(ios::fixed) << setprecision(10);
	while(true){
		int n;
		double r;
		cin >> n >> r;
		if(n == 0){ break; }
		std::vector<lc::Circle> circles(n);
		for(int i = 0; i < n; ++i){
			cin >> circles[i].c.x >> circles[i].c.y >> circles[i].r;
		}
		vector<lc::Point> corners;
		vector<vector<double>> args(n);
		for(int i = 0; i < n; ++i){
			for(int j = i + 1; j < n; ++j){
				const lc::Circle a(circles[i].c, r - circles[i].r);
				const lc::Circle b(circles[j].c, r - circles[j].r);
				const auto cps = lc::crossing_points(a, b);
				for(const auto &p : cps){
					if(contains_all(lc::Circle(p, r), circles)){
						corners.push_back(p);
						args[i].push_back((p - a.c).arg());
						args[j].push_back((p - b.c).arg());
					}
				}
			}
		}
		int m = 0;
		for(const auto &p : corners){
			bool accept = true;
			for(int i = 0; accept && i < m; ++i){
				if(!lc::tolerant_eq(p, corners[i])){ continue; }
				accept = false;
			}
			if(accept){ corners[m++] = p; }
		}
		corners.resize(m);
		double answer = 0.0;
		if(m == 0){
			for(const auto &c : circles){
				if(!contains_all(c, circles)){ continue; }
				const double s = r * 2.0 - c.r;
				answer = s * 2.0 * M_PI;
				break;
			}
		}else if(m == 1){
			const lc::Circle c(corners[0], r);
			if(contains_all(c, circles)){
				answer = r * 2.0 * M_PI;
			}
		}else{
			double remains = 2.0 * M_PI;
			for(int i = 0; i < n; ++i){
				sort(args[i].begin(), args[i].end());
				int k = 0;
				for(size_t j = 0; j < args[i].size(); ++j){
					if(k == 0 || !lc::tolerant_eq(args[i][j], args[i][k - 1])){
						args[i][k++] = args[i][j];
					}
				}
				args[i].resize(k);
				double sum = 0.0;
				for(size_t j = 0; j + 1 < args[i].size(); j += 2){
					sum += args[i][j + 1] - args[i][j];
				}
				sum = min(sum, 2.0 * M_PI - sum);
				const double s = r * 2.0 - circles[i].r;
				answer += s * sum;
				remains -= sum;
			}
			answer += r * remains;
		}
		cout << answer << endl;
	}
	return 0;
}

