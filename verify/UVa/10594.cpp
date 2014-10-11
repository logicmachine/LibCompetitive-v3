// UVa 10594 - Data flow
#include <iostream>
#include "libcomp/graph/edges.hpp"
#include "libcomp/graph/mincostflow_primal_dual.hpp"

using namespace std;
typedef long long ll;
typedef lc::EdgeWithWeightAndCapacity<ll, ll> Edge;

int main(){
	ios_base::sync_with_stdio(false);
	while(true){
		int n, m;
		if(!(cin >> n >> m)){ break; }
		vector<int> a(m), b(m);
		vector<ll> w(m);
		for(int i = 0; i < m; ++i){ cin >> a[i] >> b[i] >> w[i]; }
		ll d, k;
		cin >> d >> k;
		lc::AdjacencyList<Edge> conn(n + 1);
		for(int i = 0; i < m; ++i){
			conn.add_edge(a[i], b[i], w[i], k);
			conn.add_edge(b[i], a[i], w[i], k);
		}
		auto residual = lc::make_residual(conn);
		const auto answer = lc::mincostflow_primal_dual(1, n, d, residual);
		if(answer < 0){
			cout << "Impossible." << endl;
		}else{
			cout << answer << endl;
		}
	}
	return 0;
}

