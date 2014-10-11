// UVa 793 - Network Connections
#include <iostream>
#include <string>
#include <cstdlib>
#include "libcomp/structure/union_find_tree.hpp"

using namespace std;

int main(){
	ios_base::sync_with_stdio(false);
	int T;
	cin >> T;
	int n;
	cin >> n;
	for(int case_num = 0; case_num < T; ++case_num){
		lc::UnionFindTree uft(n + 1);
		int succ = 0, fail = 0;
		while(true){
			string t;
			if(!(cin >> t)){
				n = -1;
				break;
			}
			if(t == "c"){
				int a, b;
				cin >> a >> b;
				uft.unite(a, b);
			}else if(t == "q"){
				int a, b;
				cin >> a >> b;
				if(uft.same(a, b)){
					++succ;
				}else{
					++fail;
				}
			}else{
				n = atoi(t.c_str());
				break;
			}
		}
		if(case_num != 0){ cout << endl; }
		cout << succ << "," << fail << endl;
	}
	return 0;
}

