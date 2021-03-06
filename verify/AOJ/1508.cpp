#include <cstdio>
#include <vector>
#include <algorithm>
#include "libcomp/structure/traits/min.hpp"
#include "libcomp/structure/treap_list.hpp"

using namespace std;

int main(){
	int n, q;
	scanf("%d%d", &n, &q);
	lc::TreapList<lc::MinTraits<int>> b;
	for(int i = 0; i < n; ++i){
		int ai;
		scanf("%d", &ai);
		b.insert(i, ai);
	}
	while(q--){
		int x, y, z;
		scanf("%d%d%d", &x, &y, &z);
		if(x == 0){
			lc::TreapList<lc::MinTraits<int>> c = b.split(y);
			lc::TreapList<lc::MinTraits<int>> d = c.split(z - y);
			lc::TreapList<lc::MinTraits<int>> e = d.split(1);
			b.merge(std::move(d));
			b.merge(std::move(c));
			b.merge(std::move(e));
		}else if(x == 1){
			printf("%d\n", b.query(y, z + 1));
		}else{
			b.update(y, z);
		}
	}
	return 0;
}

