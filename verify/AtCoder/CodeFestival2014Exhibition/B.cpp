#include <iostream>
#include <string>
#include "libcomp/structure/lazy_traits/min_add.hpp"
#include "libcomp/structure/lazy_treap_list.hpp"

using namespace std;

int main(){
	ios_base::sync_with_stdio(false);
	int q;
	string s;
	cin >> q >> s;
	const int n = s.size();
	lc::LazyTreapList<lc::LazyMinRangeAddTraits<int>> t;
	lc::LazyTreapList<lc::LazyMinRangeAddTraits<int>> b;
	for(int i = 0; i <= n; ++i){ t.insert(i, 0); }
	for(int i = 0; i < n; ++i){ b.insert(i, (s[i] == '(' ? 1 : -1)); }
	for(int i = 0; i < n; ++i){
		t.modify(i + 1, t.size(), (s[i] == '(' ? 1 : -1));
	}
	while(q--){
		string x;
		int y, z;
		cin >> x >> y >> z;
		--y;
		if(x[0] == '('){
			b.insert(y, 1);
			t.insert(y, t.query(y, y + 1));
			t.modify(y + 1, t.size(), 1);
		}else if(x[0] == ')'){
			b.insert(y, -1);
			t.insert(y, t.query(y, y + 1));
			t.modify(y + 1, t.size(), -1);
		}else if(x[0] == 'D'){
			const int k = b.query(y, y + 1);
			b.erase(y);
			t.modify(y + 1, t.size(), (k > 0 ? -1 : 1));
			t.erase(y);
		}else if(x[0] == 'Q'){
			const int l = t.query(y, y + 1);
			const int r = t.query(z, z + 1);
			const int c = t.query(y, z + 1);
			const int answer = (l - c) + (r - c);
			cout << answer << endl;
		}
	}
	return 0;
}

