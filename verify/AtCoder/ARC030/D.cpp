#include <cstdio>
#include <vector>
#include "libcomp/structure/persistent_lazy_treap_list.hpp"
#include "libcomp/structure/lazy_traits/sum_add.hpp"

using namespace std;
typedef long long ll;
typedef lc::PersistentLazyTreapList<lc::LazySumRangeAddTraits<ll>> PersistentList;

int main(){
	int n, q;
	scanf("%d%d", &n, &q);
	PersistentList plist;
	for(int i = 0; i < n; ++i){
		ll xi;
		scanf("%lld", &xi);
		plist = plist.insert(i, xi);
	}
	while(q--){
		int kind;
		scanf("%d", &kind);
		if(kind == 1){
			int a, b, v;
			scanf("%d%d%d", &a, &b, &v);
			--a;
			plist = plist.modify(a, b, v);
		}else if(kind == 2){
			int a, b, c, d;
			scanf("%d%d%d%d", &a, &b, &c, &d);
			--a; --c;
			const auto cd_l_cr = plist.split(c);
			const auto cd_c_r  = cd_l_cr.second.split(d - c);
			const auto ab_l_cr = plist.split(a);
			const auto ab_c_r  = ab_l_cr.second.split(b - a);
			plist = ab_l_cr.first.merge(cd_c_r.first).merge(ab_c_r.second);
		}else if(kind == 3){
			int a, b;
			scanf("%d%d", &a, &b);
			--a;
			printf("%lld\n", plist.query(a, b));
		}
	}
	return 0;
}

