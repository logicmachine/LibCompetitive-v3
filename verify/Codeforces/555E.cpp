// Codeforces 555E - Case of Computer Network
#include <iostream>
#include <vector>
#include <queue>
#include "libcomp/graph/edges.hpp"
#include "libcomp/graph/connected_components.hpp"
#include "libcomp/graph/two_edge_connected_components.hpp"
#include "libcomp/graph/heavy_light_decomposition.hpp"
#include "libcomp/misc/coordinate_compression.hpp"

using namespace std;
typedef pair<int, int> pii;
typedef lc::Edge Edge;

int main(){
	ios_base::sync_with_stdio(false);
	int n, m, q;
	cin >> n >> m >> q;
	lc::AdjacencyList<Edge> conn(n);
	for(int i = 0; i < m; ++i){
		int a, b;
		cin >> a >> b;
		--a; --b;
		conn.add_edge(a, b);
		conn.add_edge(b, a);
	}
	const auto tcc = lc::two_edge_connected_components(conn);
	const auto cont_conn = tcc_contract(conn, tcc);
	lc::CoordinateCompressor<int> comp;
	for(const auto &v : tcc){ comp.push(v); }
	const int k = comp.build();
	lc::AdjacencyList<Edge> forest(k);
	for(int i = 0; i < n; ++i){
		const int ci = comp.compress(i);
		for(const auto &e : cont_conn[i]){
			forest.add_edge(ci, comp.compress(e.to));
		}
	}
	const auto trees = connected_components(forest);
	const int num_trees = trees.size();
	vector<lc::HeavyLightDecomposer> decomps(num_trees);
	vector<vector<vector<pii>>> paths(num_trees);
	for(int i = 0; i < num_trees; ++i){
		decomps[i] = lc::HeavyLightDecomposer(trees.contracted_graph(i), 0);
		const int num_paths = decomps[i].path_count();
		paths[i].assign(num_paths, vector<pii>());
		for(int j = 0; j < num_paths; ++j){
			paths[i][j].assign(decomps[i].path_length(j) + 1, pii(0, 0));
		}
	}
	bool answer = true;
	for(int i = 0; i < q; ++i){
		int u, v;
		cin >> u >> v;
		u = comp.compress(tcc[u - 1]);
		v = comp.compress(tcc[v - 1]);
		if(trees.component_id(u) != trees.component_id(v)){
			answer = false;
			continue;
		}
		const int cid = trees.component_id(u);
		const int cu = trees.local_vertex_id(u);
		const int cv = trees.local_vertex_id(v);
		if(cu == cv){ continue; }
		const auto route = decomps[cid].shortest_path(cu, cv);
		const int route_len = route.size();
		for(int j = 0; j < route_len; ++j){
			const auto &s = route[j];
			auto &path = paths[cid][s.path];
			if(s.first < s.last){
				int first = s.first + 1, last = s.last;
				if(first == 1 && j > 0){
					const int pp = decomps[cid].parent_path_id(s.path);
					if(route[j - 1].path == pp){ --first; }
				}
				++path[first].first;
				--path[last].first;
			}else{
				int first = s.last + 2, last = s.first + 1;
				if(first == 1 && j + 1 < route_len){
					const int pp = decomps[cid].parent_path_id(s.path);
					if(route[j + 1].path == pp){ --first; }
				}
				++path[first].second;
				--path[last].second;
			}
		}
	}
	for(int i = 0; i < num_trees; ++i){
		const int num_paths = paths[i].size();
		for(int j = 0; j < num_paths; ++j){
			const int len = paths[i][j].size();
			for(int k = 1; k < len; ++k){
				paths[i][j][k].first += paths[i][j][k - 1].first;
				paths[i][j][k].second += paths[i][j][k - 1].second;
			}
			for(int k = 0; k < len; ++k){
				if(paths[i][j][k].first && paths[i][j][k].second){
					answer = false;
				}
			}
		}
	}
	cout << (answer ? "Yes" : "No") << endl;
	return 0;
}

