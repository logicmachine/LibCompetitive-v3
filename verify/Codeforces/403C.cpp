// Codeforces 403C - Strictly Positive Matrix
#include <iostream>
#include "libcomp/graph/edges.hpp"
#include "libcomp/graph/strongly_connected_components.hpp"

using namespace std;
typedef lc::Edge Edge;

int main(){
	ios_base::sync_with_stdio(false);
	int n;
	cin >> n;
	lc::AdjacencyList<Edge> conn(n);
	for(int i = 0; i < n; ++i){
		for(int j = 0; j < n; ++j){
			int a;
			cin >> a;
			if(i != j && a){ conn.add_edge(i, j); }
		}
	}
	const auto scc = lc::strongly_connected_components(conn);
	bool answer = true;
	for(int i = 0; i < n; ++i){
		if(scc[i] != scc[0]){ answer = false; }
	}
	cout << (answer ? "YES" : "NO") << endl;
	return 0;
}

