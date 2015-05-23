#include <iostream>
#include <vector>
#include "libcomp/math/fft_convolution.hpp"
#include "libcomp/structure/segment_tree.hpp"

using namespace std;
typedef long long ll;
typedef pair<ll, int> pli;

struct SegTreeTraits {
	typedef pli value_type;
	pli default_value() const { return pli(-1, -1); }
	pli operator()(const pli &a, const pli &b) const { return max(a, b); }
};

int main(){
	ios_base::sync_with_stdio(false);
	const int len = (1 << 20);
	vector<int> a(len);
	for(int i = 1; i <= 500000; ++i){
		int count = 0;
		for(int j = 1; j * j <= i; ++j){
			if(i % j != 0){ continue; }
			if(j * j != i){ ++count; }
			++count;
		}
		a[i] = count;
	}
	const auto b = lc::fft_convolution(a, a);
	vector<pli> c(len);
	for(int i = 0; i < len; ++i){
		c[i] = pli(static_cast<ll>(b[i].real() + 0.5), -i);
	}
	lc::SegmentTree<SegTreeTraits> st(c.begin(), c.end());
	int T;
	cin >> T;
	while(T--){
		int lo, hi;
		cin >> lo >> hi;
		const auto p = st.query(lo, hi + 1);
		cout << -p.second << " " << p.first << endl;
	}
	return 0;
}

