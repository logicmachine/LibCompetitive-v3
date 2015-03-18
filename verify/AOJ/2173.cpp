#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <limits>
#include "libcomp/graph/edges.hpp"
#include "libcomp/graph/adjacency_list.hpp"
#include "libcomp/graph/sssp_dijkstra.hpp"
#include "libcomp/geometry/point.hpp"
#include "libcomp/geometry/segment.hpp"
#include "libcomp/geometry/polygon.hpp"
#include "libcomp/geometry/distance.hpp"

using namespace std;
typedef lc::EdgeWithWeight<double> Edge;

int main(){
	ios_base::sync_with_stdio(false);
	cout << setiosflags(ios::fixed) << setprecision(10);
	const double inf = numeric_limits<double>::infinity();
	while(true){
		int w, n;
		cin >> w >> n;
		if(w == 0 && n == 0){ break; }
		vector<lc::Polygon> piles(n);
		for(int i = 0; i < n; ++i){
			int m;
			cin >> m;
			piles[i] = lc::Polygon(m);
			for(int j = 0; j < m; ++j){
				cin >> piles[i][j].x >> piles[i][j].y;
			}
		}
		lc::AdjacencyList<Edge> graph(n + 2);
		const int source = n, sink = n + 1;
		graph.add_edge(source, sink, w);
		graph.add_edge(sink, source, w);
		for(int i = 0; i < n; ++i){
			const int m = piles[i].size();
			double left = inf, right = inf;
			for(int j = 0; j < m; ++j){
				left = min(left, piles[i][j].x);
				right = min(right, w - piles[i][j].x);
			}
			graph.add_edge(source, i, left);
			graph.add_edge(i, source, left);
			graph.add_edge(sink, i, right);
			graph.add_edge(i, sink, right);
			for(int j = i + 1; j < n; ++j){
				const int r = piles[j].size();
				double d = inf;
				for(int k = 0; k < m; ++k){
					for(int l = 0; l < r; ++l){
						d = min(d, lc::distance(
							piles[i].side(k), piles[j].side(l)));
					}
				}
				graph.add_edge(i, j, d);
				graph.add_edge(j, i, d);
			}
		}
		cout << lc::sssp_dijkstra(source, graph)[sink] << endl;
	}
	return 0;
}

