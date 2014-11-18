#include <iostream>
#include "libcomp/string/longest_common_prefix.hpp"

using namespace std;
static const int MOD = 1000000009;

int main(){
	ios_base::sync_with_stdio(false);
	int T;
	cin >> T;
	while(T--){
		int n;
		cin >> n;
		vector<int> h(n);
		for(int i = 0; i < n; ++i){ cin >> h[i]; }
		vector<int> diff(n - 1);
		for(int i = 0; i + 1 < n; ++i){
			diff[i] = (h[i + 1] - h[i] + 101);
		}
		const lc::SuffixArray sa(diff);
		vector<int> lcp = lc::longest_common_prefix(diff, sa);
		lcp.push_back(0);
		int answer = 0;
		for(int i = lcp.size() - 2; i >= 0; --i){
			const int length = n - sa[i + 1];
			if(length > lcp[i + 1]){
				answer = (answer + length - lcp[i + 1] - 1) % MOD;
			}
		}
		cout << answer << endl;
	}
	return 0;
}

