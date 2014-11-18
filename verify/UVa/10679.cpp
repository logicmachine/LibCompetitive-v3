#include <iostream>
#include "libcomp/string/suffix_array.hpp"

using namespace std;

int main(){
	ios_base::sync_with_stdio(false);
	int T;
	cin >> T;
	while(T--){
		string s;
		cin >> s;
		const lc::SuffixArray sa(s);
		int q;
		cin >> q;
		while(q--){
			string t;
			cin >> t;
			int left = 0, right = sa.size();
			for(size_t i = 0; i < t.size(); ++i){
				int l = left, r = right;
				while(l < r){
					const int c = l + (r - l) / 2;
					if(s[sa[c] + i] < t[i]){
						l = c + 1;
					}else{
						r = c;
					}
				}
				left = l;
				r = right;
				while(l < r){
					const int c = l + (r - l) / 2;
					if(s[sa[c] + i] <= t[i]){
						l = c + 1;
					}else{
						r = c;
					}
				}
				right = l;
			}
			cout << (left < right ? "y" : "n") << endl;
		}
	}
	return 0;
}

