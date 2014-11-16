#include <iostream>
#include "libcomp/string/suffix_array.hpp"

using namespace std;

int main(){
	ios_base::sync_with_stdio(false);
	string s;
	cin >> s;
	lc::SuffixArray sa(s);
	for(size_t i = 1; i < sa.size(); ++i){
		cout << sa[i] << "\n";
	}
	return 0;
}

