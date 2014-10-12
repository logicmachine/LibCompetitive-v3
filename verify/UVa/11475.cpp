// UVa 11475 - Extend to Palindrome
#include <iostream>
#include <string>
#include <algorithm>
#include "libcomp/string/kmp.hpp"

using namespace std;

int main(){
	ios_base::sync_with_stdio(false);
	string s;
	while(cin >> s){
		string t(s);
		reverse(t.begin(), t.end());
		lc::KMPMatcher kmp(t);
		int current = 0;
		for(char c : s){
			const auto p = kmp.iterate(c, current);
			if(p.second){
				current = s.size();
			}else{
				current = p.first;
			}
		}
		cout << s << t.substr(current) << endl;
	}
	return 0;
}

