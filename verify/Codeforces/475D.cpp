#include <iostream>
#include <vector>
#include <map>
#include "libcomp/structure/traits/gcd.hpp"
#include "libcomp/structure/nazo_table.hpp"
#include "libcomp/misc/binary_search.hpp"

using namespace std;
typedef long long ll;

int main(){
	ios_base::sync_with_stdio(false);
	int n;
	cin >> n;
	vector<int> a(n);
	for(int i = 0; i < n; ++i){ cin >> a[i]; }
	lc::NazoTable<lc::GCDTraits<int>> table(a.begin(), a.end());
	map<int, ll> result;
	for(int i = 0; i < n; ++i){
		int g = a[i], tail = i;
		while(tail < n){
			const int t = lc::binary_search(
				tail + 1, n + 1, [&](int x){ return table.query(i, x) < g; });
			result[g] += t - tail - 1;
			g = table.query(i, t);
			tail = t - 1;
		}
	}
	int q;
	cin >> q;
	while(q--){
		int x;
		cin >> x;
		const auto it = result.find(x);
		cout << (it != result.end() ? it->second : 0) << endl;
	}
	return 0;
}

