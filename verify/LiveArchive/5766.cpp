// ACM-ICPC Live Archive 5766- GRE Words
#include <cstdio>
#include <string>
#include <vector>
#include "libcomp/string/aho_corasick.hpp"

using namespace std;
static char buffer[300001];

int main(){
	ios_base::sync_with_stdio(false);
	int T;
	scanf("%d", &T);
	for(int case_num = 1; case_num <= T; ++case_num){
		int n;
		scanf("%d", &n);
		vector<string> words(n);
		vector<int> importances(n);
		for(int i = 0; i < n; ++i){
			scanf("%s%d", buffer, &importances[i]);
			words[i] = string(buffer);
		}
		lc::AhoCorasick<> ac(words.begin(), words.end());
		int answer = 0;
		for(int i = 0; i < n; ++i){
			int state = 0, local = max(0, importances[i]);
			for(const int c : words[i]){
				const auto p = ac.iterate(c, state);
				state = p.first;
				for(const int k : p.second){
					if(k >= i){ continue; }
					local = max(local, importances[k] + importances[i]);
				}
			}
			importances[i] = local;
			answer = max(answer, local);
		}
		printf("Case #%d: %d\n", case_num, answer);
	}
	return 0;
}

