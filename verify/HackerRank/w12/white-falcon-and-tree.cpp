#include <iostream>
#include <vector>
#include <tuple>
#include "libcomp/graph/edges.hpp"
#include "libcomp/graph/heavy_light_decomposition.hpp"
#include "libcomp/structure/lazy_segment_tree.hpp"

using namespace std;
typedef long long ll;
typedef tuple<int, int> tii;
typedef tuple<int, int, int> tiii;
static const int MOD = 1000000007;

inline tii merge_function(const tii &f, const tii &g){
	return make_tuple<int, int>(
		(static_cast<ll>(get<0>(f)) * get<0>(g)) % MOD,
		(static_cast<ll>(get<0>(g)) * get<1>(f) + get<1>(g)) % MOD);
}

struct LazyTraits {
	typedef tii value_type;
	typedef tiii modifier_type;
	value_type default_value() const { return make_tuple(1, 0); }
	modifier_type default_modifier() const { return make_tuple(-1, 0, 0); }
	pair<modifier_type, modifier_type>
	split_modifier(const modifier_type &m, size_t k) const {
		return make_pair(m, m);
	}
	modifier_type merge_modifier(
		const modifier_type &a, const modifier_type &b) const
	{
		return max(a, b);
	}
	value_type operator()(const value_type &a, const value_type &b) const {
		return merge_function(a, b);
	}
	value_type resolve(
		size_t n, const value_type &v, const modifier_type &m) const
	{
		if(get<0>(m) < 0){ return v; }
		tii x = tii(get<1>(m), get<2>(m)), y = tii(1, 0);
		while(n > 0){
			if(n & 1){ y = merge_function(y, x); }
			x = merge_function(x, x);
			n >>= 1;
		}
		return y;
	}
};

int main(){
	ios_base::sync_with_stdio(false);
	int n;
	cin >> n;
	vector<int> a(n), b(n);
	for(int i = 0; i < n; ++i){ cin >> a[i] >> b[i]; }
	lc::AdjacencyList<lc::Edge> graph(n);
	for(int i = 1; i < n; ++i){
		int u, v;
		cin >> u >> v;
		--u; --v;
		graph.add_edge(u, v);
		graph.add_edge(v, u);
	}
	const lc::HeavyLightDecomposer decomp(graph);
	vector<lc::LazySegmentTree<LazyTraits>> paths(decomp.path_count());
	vector<lc::LazySegmentTree<LazyTraits>> rev_paths(decomp.path_count());
	for(int i = 0; i < decomp.path_count(); ++i){
		const int m = decomp.path_length(i);
		vector<tii> init(m);
		for(int j = 0; j < m; ++j){
			const int v = decomp.vertex_id(i, j);
			init[j] = make_tuple(a[v], b[v]);
		}
		paths[i] = lc::LazySegmentTree<LazyTraits>(init.begin(), init.end());
		reverse(init.begin(), init.end());
		rev_paths[i] = lc::LazySegmentTree<LazyTraits>(init.begin(), init.end());
	}
	int q;
	cin >> q;
	for(int i = 0; i < q; ++i){
		int t;
		cin >> t;
		if(t == 1){
			int u, v, a, b;
			cin >> u >> v >> a >> b;
			--u; --v;
			const auto path = decomp.shortest_path(u, v);
			for(const auto &s : path){
				const int p = s.path, l = s.first, r = s.last;
				const int m = decomp.path_length(p);
				const int rev_l = m - l, rev_r = m - r;
				if(l < r){
					paths[p].modify(l, r, tiii(i, a, b));
					rev_paths[p].modify(rev_r, rev_l, tiii(i, a, b));
				}else{
					paths[p].modify(r + 1, l + 1, tiii(i, a, b));
					rev_paths[p].modify(rev_l - 1, rev_r - 1, tiii(i, a, b));
				}
			}
		}else if(t == 2){
			int u, v, x;
			cin >> u >> v >> x;
			--u; --v;
			const auto path = decomp.shortest_path(u, v);
			tii result(1, 0);
			for(const auto &s : path){
				const int p = s.path, l = s.first, r = s.last;
				const int m = decomp.path_length(p);
				const int rev_l = m - l, rev_r = m - r;
				if(l < r){
					result = merge_function(result, paths[p].query(l, r));
				}else{
					result = merge_function(
						result, rev_paths[p].query(rev_l - 1, rev_r - 1));
				}
			}
			const ll a = get<0>(result), b = get<1>(result);
			cout << (a * x + b) % MOD << endl;
		}
	}
	return 0;
}

