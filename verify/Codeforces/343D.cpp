// Codeforces 343D - Water Tree
#include <iostream>
#include <functional>
#include "libcomp/structure/binary_indexed_tree.hpp"
#include "libcomp/graph/heavy_light_decomposition.hpp"
#include "libcomp/graph/edges.hpp"

using namespace std;
typedef lc::Edge Edge;

int main(){
	ios_base::sync_with_stdio(false);
	int n;
	cin >> n;
	lc::AdjacencyList<Edge> conn(n);
	for(int i = 1; i < n; ++i){
		int a, b;
		cin >> a >> b;
		--a; --b;
		conn.add_edge(a, b);
		conn.add_edge(b, a);
	}
	const lc::HeavyLightDecomposer decomp(conn);
	const int m = decomp.path_count();
	function<int(int, int)> max_fun(max<int>);
	vector<lc::BinaryIndexedTree<int, decltype(max_fun)>> fill_bits(m);
	vector<lc::BinaryIndexedTree<int, decltype(max_fun)>> empty_bits(m);
	for(int i = 0; i < m; ++i){
		fill_bits[i] =
			lc::BinaryIndexedTree<int, decltype(max_fun)>(
				decomp.path_length(i), max_fun);
		empty_bits[i] =
			lc::BinaryIndexedTree<int, decltype(max_fun)>(
				decomp.path_length(i), max_fun);
	}
	int q;
	cin >> q;
	for(int ts = 1; ts <= q; ++ts){
		int c, v;
		cin >> c >> v;
		--v;
		if(c == 1){
			const int dn = decomp.path_id(v);
			const int dl = decomp.local_index(v);
			fill_bits[dn].modify(dl, ts);
		}else if(c == 2){
			int cur = v;
			while(cur >= 0){
				const int dn = decomp.path_id(cur);
				const int dl = decomp.local_index(cur);
				const int ln = decomp.path_length(dn);
				empty_bits[dn].modify(ln - dl - 1, ts);
				cur = decomp.parent_vertex_id(dn);
			}
		}else if(c == 3){
			int empty_ts = 0, fill_ts = 0;
			{
				const int dn = decomp.path_id(v);
				const int dl = decomp.local_index(v);
				const int ln = decomp.path_length(dn);
				empty_ts = empty_bits[dn].query(ln - dl);
			}
			int cur = v;
			while(cur >= 0){
				const int dn = decomp.path_id(cur);
				const int dl = decomp.local_index(cur);
				fill_ts = max(fill_ts, fill_bits[dn].query(dl + 1));
				cur = decomp.parent_vertex_id(dn);
			}
			cout << (empty_ts >= fill_ts ? 0 : 1) << endl;
		}
	}
	return 0;
}

