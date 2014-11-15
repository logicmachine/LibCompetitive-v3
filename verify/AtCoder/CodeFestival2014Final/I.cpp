#include <iostream>
#include <map>
#include <utility>
#include "libcomp/graph/edges.hpp"
#include "libcomp/graph/lowest_common_ancestor.hpp"
#include "libcomp/misc/containing_tree.hpp"

using namespace std;
typedef pair<int, int> pii;
typedef lc::Edge Edge;

int main(){
	ios_base::sync_with_stdio(false);
	int n;
	cin >> n;
	vector<lc::Rectangle<int>> rects;
	for(int i = 0; i < n; ++i){
		int x, y, r;
		cin >> x >> y >> r;
		const int x1 = x - r, y1 = y;
		const int x2 = x + r, y2 = y;
		rects.push_back(lc::Rectangle<int>(
			x1 - y1, x1 + y1, x2 - y2 + 1, x2 + y2 + 1));
	}
	map<pii, int> qmap;
	vector<pii> queries;
	int m;
	cin >> m;
	for(int i = 0; i < m; ++i){
		int x1, y1, x2, y2;
		cin >> x1 >> y1 >> x2 >> y2;
		const pii p1(x1 - y1, x1 + y1), p2(x2 - y2, x2 + y2);
		int k1 = -1, k2 = -1;
		const auto it = qmap.find(p1);
		if(it == qmap.end()){
			k1 = qmap.size();
			qmap.insert(std::make_pair(p1, k1));
			rects.push_back(lc::Rectangle<int>(
				p1.first, p1.second, p1.first, p1.second));
		}else{
			k1 = it->second;
		}
		const auto jt = qmap.find(p2);
		if(jt == qmap.end()){
			k2 = qmap.size();
			qmap.insert(std::make_pair(p2, k2));
			rects.push_back(lc::Rectangle<int>(
				p2.first, p2.second, p2.first, p2.second));
		}else{
			k2 = jt->second;
		}
		queries.push_back(pii(k1, k2));
	}
	const vector<int> parents = lc::containing_tree(rects);
	const int k = parents.size() + 1;
	lc::AdjacencyList<Edge> graph(k);
	for(int i = 0; i + 1 < k; ++i){
		const int p = (parents[i] == -1) ? k - 1 : parents[i];
		graph.add_edge(p, i);
		graph.add_edge(i, p);
	}
	const lc::LowestCommonAncestor lca(k - 1, graph);
	for(int i = 0; i < m; ++i){
		const int u = queries[i].first + n;
		const int v = queries[i].second + n;
		const int p = lca.query(u, v);
		const int ud = lca.get_depth(u);
		const int vd = lca.get_depth(v);
		const int pd = lca.get_depth(p);
		cout << (ud - pd - 1) + (vd - pd - 1) << endl;
	}
	return 0;
}

