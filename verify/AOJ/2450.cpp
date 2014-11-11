#include <iostream>
#include <tuple>
#include "libcomp/graph/edges.hpp"
#include "libcomp/graph/adjacency_list.hpp"
#include "libcomp/graph/heavy_light_decomposition.hpp"
#include "libcomp/structure/lazy_segment_tree.hpp"

using namespace std;
typedef long long ll;
typedef lc::Edge Edge;
typedef tuple<ll, ll> tll;				// timestamp, value
typedef tuple<ll, ll, ll, ll> tllll;	// left, right, partial, all

const ll INF = 1000000000000000000ll;

struct SegtreeTraits {
	typedef tllll value_type;
	typedef tll modifier_type;
	value_type default_value() const { return tllll(0, 0, -INF, 0); }
	modifier_type default_modifier() const { return tll(-1, 0); }
	pair<tll, tll> split_modifier(const tll &m, size_t k) const {
		return make_pair(m, m);
	}
	tll merge_modifier(const tll &a, const tll &b) const { return max(a, b); }
	tllll operator()(const tllll &a, const tllll &b) const {
		if(get<2>(a) < get<3>(a)){ return b; }
		if(get<2>(b) < get<3>(b)){ return a; }
		const tllll result(
			max(get<0>(a), get<3>(a) + get<0>(b)),
			max(get<1>(b), get<1>(a) + get<3>(b)),
			max(max(get<2>(a), get<2>(b)), get<1>(a) + get<0>(b)),
			get<3>(a) + get<3>(b));
		return result;
	}
	tllll resolve(size_t n, const tllll &v, const tll &m) const {
		if(get<0>(m) < 0){ return v; }
		if(get<1>(m) > 0){
			const ll sum = get<1>(m) * n;
			return tllll(sum, sum, sum, sum);
		}else{
			const ll val = get<1>(m);
			return tllll(val, val, val, val * n);
		}
	}
};

int main(){
	ios_base::sync_with_stdio(false);
	int n, q;
	cin >> n >> q;
	vector<int> w(n);
	for(int i = 0; i < n; ++i){ cin >> w[i]; }
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
	vector<lc::LazySegmentTree<SegtreeTraits>> segtrees(m);
	for(int i = 0; i < m; ++i){
		const int len = decomp.path_length(i);
		vector<tllll> values(len);
		for(int j = 0; j < len; ++j){
			const int x = w[decomp.vertex_id(i, j)];
			values[j] = tllll(x, x, x, x);
		}
		segtrees[i] = lc::LazySegmentTree<SegtreeTraits>(
			values.begin(), values.end());
	}
	for(int ts = 0; ts < q; ++ts){
		int t, a, b, c;
		cin >> t >> a >> b >> c;
		--a; --b;
		const vector<lc::HeavyLightDecomposer::Segment> route =
			decomp.shortest_path(a, b);
		if(t == 1){
			for(size_t i = 0; i < route.size(); ++i){
				const int p = route[i].path;
				int first = route[i].first, last = route[i].last;
				if(first > last){
					swap(first, last);
					++first; ++last;
				}
				segtrees[p].modify(first, last, tll(ts, c));
			}
		}else if(t == 2){
			const int len = route.size();
			vector<tllll> values(len);
			vector<ll> integral(len + 1);
			ll answer = -INF;
			for(int i = 0; i < len; ++i){
				const int p = route[i].path;
				int first = route[i].first, last = route[i].last;
				if(first > last){
					const tllll v = segtrees[p].query(last + 1, first + 1);
					values[i] = tllll(
						get<1>(v), get<0>(v), get<2>(v), get<3>(v));
				}else{
					const tllll v = segtrees[p].query(first, last);
					values[i] = v;
				}
				integral[i] = get<3>(values[i]);
				answer = max(answer, get<2>(values[i]));
			}
			ll sum = 0;
			for(int i = 0; i < len; ++i){
				const ll t = integral[i];
				integral[i] = sum;
				sum += t;
			}
			integral.back() = sum;
			for(int l = 0; l < len; ++l){
				for(int r = l + 1; r < len; ++r){
					const ll x = integral[r] - integral[l + 1];
					answer = max(
						answer, get<1>(values[l]) + get<0>(values[r]) + x);
				}
			}
			cout << answer << endl;
		}
	}
	return 0;
}

