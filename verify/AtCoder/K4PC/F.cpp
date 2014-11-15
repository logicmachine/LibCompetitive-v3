#include <iostream>
#include <vector>
#include "libcomp/graph/edges.hpp"
#include "libcomp/graph/sssp_bellman_ford.hpp"
#include "libcomp/misc/coordinate_compression.hpp"

using namespace std;
typedef long long ll;
typedef lc::EdgeWithWeight<ll> Edge;

int main(){
	ios_base::sync_with_stdio(false);
	int n;
	cin >> n;
	vector<ll> x(n), k(n), d(n);
	lc::CoordinateCompressor<ll> comp;
	for(int i = 0; i < n; ++i){
		cin >> x[i] >> k[i] >> d[i];
		comp.push(x[i]);
		comp.push(x[i] - d[i]);
		comp.push(x[i] - d[i] + 1);
		comp.push(x[i] + d[i]);
		comp.push(x[i] + d[i] + 1);
	}
	const int m = comp.build();
	vector<ll> initial_count(m);
	for(int i = 0; i < n; ++i){
		const int t = comp.compress(x[i]);
		++initial_count[t];
	}
	vector<ll> initial_integral(m + 1);
	for(int i = 0; i < m; ++i){
		initial_integral[i + 1] = initial_integral[i] + initial_count[i];
	}
	lc::AdjacencyList<Edge> graph(m + 1);
	for(int i = 0; i < m; ++i){
		graph.add_edge(i + 1, i, 0);
	}
	for(int i = 0; i < n; ++i){
		const int in_l = comp.compress(x[i] - d[i] + 1);
		const int in_r = comp.compress(x[i] + d[i]);
		const ll in_k = initial_integral[in_r] - initial_integral[in_l];
		const int out_l = comp.compress(x[i] - d[i]);
		const int out_r = comp.compress(x[i] + d[i] + 1);
		const ll out_k = initial_integral[out_r] - initial_integral[out_l];
		graph.add_edge(in_l, in_r, (k[i] - in_k) - 1);
		graph.add_edge(out_r, out_l, -(k[i] - out_k));
	}
	const auto result = lc::sssp_bellman_ford(m, graph);
	if(result.empty()){
		cout << "impossible" << endl;
	}else if(result[m] - result[0] > 100000){
		cout << "too many" << endl;
	}else{
		vector<int> solution;
		for(int i = 0; i < m; ++i){
			const int t = result[i + 1] - result[i];
			for(int j = 0; j < t; ++j){
				solution.push_back(comp.decompress(i));
			}
		}
		cout << solution.size() << endl;
		if(!solution.empty()){
			for(size_t i = 0; i < solution.size(); ++i){
				if(i != 0){ cout << " "; }
				cout << solution[i];
			}
			cout << endl;
		}
	}
	return 0;
}

