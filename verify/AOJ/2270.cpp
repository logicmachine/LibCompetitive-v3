// AOJ 2270 - The L-th Number
#include <iostream>
#include "libcomp/graph/edges.hpp"
#include "libcomp/graph/adjacency_list.hpp"
#include "libcomp/graph/topological_sort.hpp"
#include "libcomp/graph/lowest_common_ancestor.hpp"
#include "libcomp/structure/persistent_rbst_list.hpp"
#include "libcomp/structure/traits/sum.hpp"
#include "libcomp/misc/coordinate_compression.hpp"

using namespace std;

class PersistentList : public lc::PersistentRBSTList<lc::SumTraits<int>> {
private:
	int cache(const node_ptr &p) const {
		if(!p){ return 0; }
		return p->cache;
	}
public:
	PersistentList()
		: lc::PersistentRBSTList<lc::SumTraits<int>>()
	{ }
	PersistentList(const PersistentList &p)
		: lc::PersistentRBSTList<lc::SumTraits<int>>(p)
	{ }
	PersistentList(const lc::PersistentRBSTList<lc::SumTraits<int>> &p)
		: lc::PersistentRBSTList<lc::SumTraits<int>>(p)
	{ }
	template <class Iterator>
	PersistentList(Iterator first, Iterator last)
		: lc::PersistentRBSTList<lc::SumTraits<int>>(first, last)
	{ }
	int solve(
		const PersistentList &a, const PersistentList &b,
		const PersistentList &p, int k) const
	{
		node_ptr cur_a = a.m_root, cur_b = b.m_root;
		node_ptr cur_c = m_root, cur_p = p.m_root;
		int answer = 0;
		while(true){
			const int la = cache(cur_a->children[0]);
			const int lb = cache(cur_b->children[0]);
			const int lc = cache(cur_c->children[0]);
			const int lp = cache(cur_p->children[0]);
			const int lsum = la + lb - lc - lp;
			if(k < lsum){
				cur_a = cur_a->children[0];
				cur_b = cur_b->children[0];
				cur_c = cur_c->children[0];
				cur_p = cur_p->children[0];
				continue;
			}else{
				answer += size(cur_a->children[0]);
				k -= lsum;
			}
			const int csum =
				cur_a->value + cur_b->value - cur_c->value - cur_p->value;
			if(k < csum){
				return answer;
			}else{
				cur_a = cur_a->children[1];
				cur_b = cur_b->children[1];
				cur_c = cur_c->children[1];
				cur_p = cur_p->children[1];
				k -= csum;
				answer += 1;
			}
		}
		return -1;
	}
};

typedef lc::Edge Edge;
typedef lc::AdjacencyList<Edge> Graph;

int main(){
	int n, q;
	cin >> n >> q;
	vector<int> x(n);
	for(int i = 0; i < n; ++i){ cin >> x[i]; }
	lc::CoordinateCompressor<int> comp(x.begin(), x.end());
	const int m = comp.size();
	for(int i = 0; i < n; ++i){ x[i] = comp.compress(x[i]); }
	Graph conn(n);
	for(int i = 1; i < n; ++i){
		int a, b;
		cin >> a >> b;
		--a; --b;
		conn.add_edge(a, b);
		conn.add_edge(b, a);
	}
	lc::LowestCommonAncestor lca(0, conn);
	Graph tree(n);
	for(int i = 1; i < n; ++i){
		const int p = lca.get_parent(i);
		tree.add_edge(p, i);
	}
	const vector<int> topo = lc::topological_sort(tree);
	vector<int> init(m, 0);
	PersistentList init_list(init.begin(), init.end());
	vector<PersistentList> lists(n);
	lists[0] = init_list;
	for(int i = 0; i < n; ++i){
		const int u = topo[i];
		if(i != 0){ lists[u] = lists[lca.get_parent(u)]; }
		lists[u] = lists[u].update(x[u], lists[u][x[u]] + 1);
	}
	for(int i = 0; i < q; ++i){
		int a, b, l;
		cin >> a >> b >> l;
		--a; --b; --l;
		const int c = lca.query(a, b);
		const int p = lca.get_parent(c);
		const int answer = lists[c].solve(
			lists[a], lists[b], p < 0 ? init_list : lists[p], l);
		cout << comp.decompress(answer) << endl;
	}
	return 0;
}

