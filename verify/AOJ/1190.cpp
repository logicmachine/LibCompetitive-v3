#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <iterator>
#include "libcomp/geometry/point.hpp"
#include "libcomp/geometry/circle.hpp"
#include "libcomp/geometry/crossing_points.hpp"

using namespace std;

inline double pow2(double x){ return x * x; }

int main(){
	ios_base::sync_with_stdio(false);
	cout << setiosflags(ios::fixed) << setprecision(10);
	while(true){
		int n;
		cin >> n;
		if(n == 0){ break; }
		vector<lc::Point> pegs(n);
		vector<double> ropes(n);
		for(int i = 0; i < n; ++i){
			cin >> pegs[i].x >> pegs[i].y >> ropes[i];
		}
		double left = 0.0, right = *min_element(ropes.begin(), ropes.end());
		for(int iter = 0; iter < 100; ++iter){
			const double center = (left + right) * 0.5;
			vector<lc::Circle> areas(n);
			for(int i = 0; i < n; ++i){
				areas[i] = lc::Circle(
					pegs[i], sqrt(max(0.0, pow2(ropes[i]) - pow2(center))));
			}
			vector<lc::Point> candidates(pegs);
			for(int i = 0; i < n; ++i){
				for(int j = i + 1; j < n; ++j){
					const auto cps = lc::crossing_points(areas[i], areas[j]);
					copy(cps.begin(), cps.end(), back_inserter(candidates));
				}
			}
			bool accept = false;
			for(const auto &p : candidates){
				bool flag = true;
				for(int i = 0; flag && i < n; ++i){
					lc::Circle c(areas[i].c, areas[i].r + lc::EPS);
					if(!c.contains(p)){ flag = false; }
				}
				if(flag){
					accept = true;
					break;
				}
			}
			if(accept){
				left = center;
			}else{
				right = center;
			}
		}
		cout << left << endl;
	}
	return 0;
}

