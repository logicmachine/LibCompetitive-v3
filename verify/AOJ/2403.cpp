// AOJ 2403 - The Enemy of My Enemy is My Friend
#include <iostream>
#include "libcomp/graph/edges.hpp"
#include "libcomp/graph/enumerate_maximal_independent_sets.hpp"
#include "libcomp/string/name_table.hpp"

using namespace std;
typedef lc::Edge Edge;

int main(){
	ios_base::sync_with_stdio(false);
	while(true){
		int n;
		cin >> n;
		if(n == 0){ break; }
		lc::NameTable name_table;
		vector<int> powers(n);
		lc::AdjacencyList<Edge> graph(n);
		for(int i = 0; i < n; ++i){
			string u_name;
			int p, m;
			cin >> u_name >> p >> m;
			const int u = name_table[u_name];
			powers[u] = p;
			for(int j = 0; j < m; ++j){
				string v_name;
				cin >> v_name;
				const int v = name_table[v_name];
				graph.add_edge(u, v);
			}
		}
		int answer = 0;
		lc::enumerate_maximal_independent_sets(graph, [&](uint64_t s){
			if((s & 1) == 0){ return; }
			int sum = 0;
			for(int i = 0; i < n; ++i){
				if(s & (1ull << i)){ sum += powers[i]; }
			}
			answer = max(answer, sum);
		});
		cout << answer << endl;
	}
	return 0;
}

