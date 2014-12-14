#include <iostream>
#include <vector>
#include <tuple>
#include "libcomp/graph/edges.hpp"
#include "libcomp/graph/adjacency_list.hpp"
#include "libcomp/geometry/point.hpp"
#include "libcomp/geometry/circle.hpp"
#include "libcomp/geometry/intersect.hpp"
#include "libcomp/geometry/extract_polygons.hpp"
#include "libcomp/geometry/debug.hpp"
#include "libcomp/geometry/visualizer.hpp"

using namespace std;
typedef lc::Edge Edge;

int main(){
	ios_base::sync_with_stdio(false);
	while(true){
		int n, m;
		cin >> n >> m;
		if(n == 0 && m == 0){ break; }
		vector<lc::Point> points(n);
		vector<lc::Circle> circles(n);
// lc::Visualizer vis("out.json");
// vis.create_layer(0, "circles");
// vis.create_layer(1, "polygons");
// vis.create_layer(2, "queries");
		for(int i = 0; i < n; ++i){
			double x, y, r;
			cin >> x >> y >> r;
			points[i] = lc::Point(x, y);
			circles[i] = lc::Circle(points[i], r);
// vis(circles[i], 0, lc::Visualizer::Style::fill("#ff0000", 0.25));
		}
		lc::AdjacencyList<Edge> graph(n);
		for(int i = 0; i < n; ++i){
			for(int j = i + 1; j < n; ++j){
				if(lc::intersect(circles[i], circles[j])){
					graph.add_edge(i, j);
					graph.add_edge(j, i);
				}
			}
		}
		const vector<lc::Polygon> polygons =
			lc::extract_polygons(points, graph);
		const int k = polygons.size();
// for(int i = 0; i < k; ++i){
// 	vis(polygons[i], 1, lc::Visualizer::Style::fill("#0000ff", 0.25));
// }
		for(int i = 0; i < m; ++i){
			if(i != 0){ cout << " "; }
			lc::Point a, b;
			cin >> a.x >> a.y >> b.x >> b.y;
// vis(a, 2, lc::Visualizer::Style::fill("#00ff00"));
// vis(b, 2, lc::Visualizer::Style::fill("#00ff00"));
			vector<bool> a_circle(n), b_circle(n);
			bool in_circle = false;
			for(int j = 0; j < n; ++j){
				a_circle[j] = circles[j].contains(a);
				b_circle[j] = circles[j].contains(b);
				if(a_circle[j] || b_circle[j]){ in_circle = true; }
			}
			if(in_circle){
				cout << (a_circle == b_circle ? "YES" : "NO");
				continue;
			}
			vector<bool> a_poly(k), b_poly(k);
			for(int j = 0; j < k; ++j){
				a_poly[j] = (polygons[j].contains(a) >= 0);
				b_poly[j] = (polygons[j].contains(b) >= 0);
			}
			cout << (a_poly == b_poly ? "YES" : "NO");
		}
		cout << endl;
	}
	return 0;
}

