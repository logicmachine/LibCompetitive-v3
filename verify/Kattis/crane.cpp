#include <iostream>
#include <cmath>
#include "libcomp/geometry/polygon.hpp"
#include "libcomp/misc/binary_search.hpp"

using namespace std;
typedef long long ll;
static const double INF = 1e11;

inline void display_upper(double x){
	if(x >= INF){
		cout << "inf" << endl;
	}else{
		cout << static_cast<ll>(ceil(x - lc::EPS)) << endl;
	}
}

int main(){
	ios_base::sync_with_stdio(false);
	int n;
	cin >> n;
	lc::Polygon poly(n);
	for(int i = 0; i < n; ++i){
		cin >> poly[i].x >> poly[i].y;
	}
	if(poly.area() < 0.0){
		for(int i = 0; i < n; ++i){ poly[i].x *= -1.0; }
	}
	double left = INF, right = -INF;
	for(int i = 0; i < n; ++i){
		if(poly[i].y < lc::EPS){
			left = min(left, poly[i].x);
			right = max(right, poly[i].x);
		}
	}
	const auto triangles = poly.triangulate();
	const double area_sum = poly.area();
	lc::Point center;
	for(const auto &tri : triangles){
		const auto s = (tri[0] + tri[1] + tri[2]) / 3;
		center += s * tri.area();
	}
	center /= area_sum;
	if(poly[0].x <= left){
		const double tl = lc::binary_search(
			0.0, INF, [&](double x) -> bool {
				const auto p =
					(center * area_sum + poly[0] * x) / (area_sum + x);
				return p.x < left;
			});
		const double tr = lc::binary_search(
			0.0, INF, [&](double x) -> bool {
				const auto p =
					(center * area_sum + poly[0] * x) / (area_sum + x);
				return p.x <= right;
			});
		if(center.x < left - lc::EPS){
			cout << "unstable" << endl;
		}else if(center.x <= right + lc::EPS){
			cout << "0 .. ";
			display_upper(tl);
		}else{
			cout << static_cast<ll>(floor(tr + lc::EPS)) << " .. ";
			display_upper(tl);
		}
	}else if(poly[0].x >= right){
		const double tr = lc::binary_search(
			0.0, INF, [&](double x) -> bool {
				const auto p =
					(center * area_sum + poly[0] * x) / (area_sum + x);
				return p.x > right;
			});
		const double tl = lc::binary_search(
			0.0, INF, [&](double x) -> bool {
				const auto p =
					(center * area_sum + poly[0] * x) / (area_sum + x);
				return p.x >= left;
			});
		if(center.x > right + lc::EPS){
			cout << "unstable" << endl;
		}else if(center.x >= left - lc::EPS){
			cout << "0 .. ";
			display_upper(tr);
		}else{
			cout << static_cast<ll>(floor(tl + lc::EPS)) << " .. ";
			display_upper(tr);
		}
	}else if(left <= center.x && center.x <= right){
		cout << "0 .. inf" << endl;
	}else{
		const double t = lc::binary_search(
			0.0, INF, [&](double x) -> bool {
				const auto p =
					(center * area_sum + poly[0] * x) / (area_sum + x);
				return (left <= p.x) && (p.x <= right);
			});
		cout << static_cast<ll>(floor(t + lc::EPS)) << " .. inf" << endl;
	}
	return 0;
}

