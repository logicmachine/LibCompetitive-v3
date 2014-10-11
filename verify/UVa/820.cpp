// UVa 820 - Internet Bandwidth
#include <iostream>
#include "libcomp/graph/edges.hpp"
#include "libcomp/graph/maxflow_dinic.hpp"

using namespace std;
typedef long long ll;
typedef lc::EdgeWithCapacity<ll> Edge;

int main(){
	ios_base::sync_with_stdio(false);
	int case_num = 1;
	while(true){
		int n, s, t, m;
		cin >> n;
		if(n == 0){ break; }
		cin >> s >> t >> m;
		lc::AdjacencyList<Edge> conn(n + 1);
		for(int i = 0; i < m; ++i){
			int a, b, c;
			cin >> a >> b >> c;
			conn.add_edge(a, b, c);
			conn.add_edge(b, a, c);
		}
		auto residual = lc::make_residual(conn);
		const ll answer = lc::maxflow_dinic(s, t, residual);
		cout << "Network " << case_num++ << endl;
		cout << "The bandwidth is " << answer << "." << endl << endl;
	}
	return 0;
}

