#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include "libcomp/math/fft_convolution.hpp"

using namespace std;

int main(){
	ios_base::sync_with_stdio(false);
	int n, m, k;
	string s, t;
	cin >> n >> m >> k >> s >> t;
	vector<int> accept(n - m + 1, 1);
	for(int cc = 0; cc < 4; ++cc){
		const char c = "ACGT"[cc];
		vector<int> integral(n + 1);
		for(int i = 0; i < n; ++i){
			integral[i + 1] = integral[i] + (s[i] == c ? 1 : 0);
		}
		vector<int> s_match(n), t_match(m);
		for(int i = 0; i < n; ++i){
			const int l = max(0, i - k), r = min(n, i + k + 1);
			if(integral[r] != integral[l]){ s_match[i] = 1; }
		}
		for(int i = 0; i < m; ++i){
			t_match[i] = (t[i] == c ? 1 : 0);
		}
		const int count = accumulate(t_match.begin(), t_match.end(), 0);
		const auto conv = lc::fft_convolution(s_match, t_match);
		for(int i = 0; i <= n - m; ++i){
			if(conv[i].real() + 1e-6 < count){ accept[i] = 0; }
		}
	}
	cout << accumulate(accept.begin(), accept.end(), 0) << endl;
	return 0;
}

