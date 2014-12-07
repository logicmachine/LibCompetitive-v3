#include <iostream>
#include "libcomp/graph/edges.hpp"
#include "libcomp/graph/adjacency_list.hpp"
#include "libcomp/graph/tree_farthest_table.hpp"

using namespace std;
typedef long long ll;
typedef lc::EdgeWithWeight<int> Edge;

int main(){
	ios_base::sync_with_stdio(false);
	int n, m;
	cin >> n >> m;
	lc::AdjacencyList<Edge> conn(n);
	for(int i = 1; i < n; ++i){
		int a, b;
		cin >> a >> b;
		--a; --b;
		conn.add_edge(a, b, 1);
		conn.add_edge(b, a, 1);
	}
	const lc::TreeFarthestTable<Edge> farthest_table(conn);
	for(int i = 0; i < m; ++i){
		int v, k;
		cin >> v >> k;
		--v;
		const ll d0 = farthest_table.weight(v);
		const ll d1 = farthest_table.weight(farthest_table.vertice(v));
		cout << d0 + d1 * (k - 1) << endl;
	}
	return 0;
}

