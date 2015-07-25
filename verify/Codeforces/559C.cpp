// Codeforces 559C - Gerald and Giant Chess
#include <iostream>
#include <vector>
#include <algorithm>
#include <utility>
#include "libcomp/math/modulus_integer.hpp"
#include "libcomp/math/modulus_utility.hpp"

using namespace std;
static const int MOD = 1000000007;
typedef pair<int, int> pii;
typedef lc::ModulusInteger<MOD> mint;

int main(){
	ios_base::sync_with_stdio(false);
	int h, w, n;
	cin >> h >> w >> n;
	vector<pii> v(n);
	for(int i = 0; i < n; ++i){
		int r, c;
		cin >> r >> c;
		v[i] = pii(r - 1, c - 1);
	}
	v.emplace_back(0, 0);
	v.emplace_back(h - 1, w - 1);
	sort(v.begin(), v.end());
	const int m = v.size();
	vector<vector<mint>> dp(m, vector<mint>(2));
	dp[0][0] = 1;
	for(int i = 0; i < m; ++i){
		const int ri = v[i].first, ci = v[i].second;
		for(int j = i + 1; j < m; ++j){
			const int rj = v[j].first, cj = v[j].second;
			if(rj < ri || cj < ci){ continue; }
			const int dr = rj - ri, dc = cj - ci;
			const mint comb = lc::modulus_combination<MOD>(dr + dc, dc);
			dp[j][0] += dp[i][1] * comb;
			dp[j][1] += dp[i][0] * comb;
		}
	}
	cout << dp[m - 1][1] - dp[m - 1][0] << endl;
	return 0;
}

