// AOJ 2257 - Sakura Poetry
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "libcomp/string/name_table.hpp"
#include "libcomp/string/aho_corasick.hpp"

using namespace std;
static const int MOD = 1000000007;

int main(){
	ios_base::sync_with_stdio(false);
	while(true){
		int n, m, k;
		cin >> n >> m >> k;
		if(n == 0 && m == 0 && k == 0){ break; }
		vector< vector<int> > next_words(2 * n + 1);
		lc::NameTable words;
		words.add_name("");
		for(int i = 0; i < n; ++i){
			string from, to;
			cin >> from >> to;
			const int from_key = words[from], to_key = words[to];
			next_words[from_key].push_back(to_key);
		}
		for(size_t i = 1; i < words.size(); ++i){
			next_words[0].push_back(i);
		}
		vector<string> season_words(k);
		for(int i = 0; i < k; ++i){ cin >> season_words[i]; }
		lc::AhoCorasick<> ac(season_words.begin(), season_words.end());
		map<int, int> dp[21][501][2];
		dp[0][0][0].insert(make_pair(0, 1));
		for(int i = 0; i < m; ++i){
			for(size_t j = 0; j < words.size(); ++j){
				for(int f = 0; f < 2; ++f){
					for(const auto &p : dp[i % 21][j][f]){
						for(const int next : next_words[j]){
							const string &word = words[next];
							if(static_cast<int>(i + word.size()) > m){
								continue;
							}
							int state = p.first, sum = f;
							for(const char c : word){
								const auto res = ac.iterate(c, state);
								state = res.first;
								sum += res.second.size();
							}
							if(sum >= 2){ continue; }
							const int rot = (i + word.size()) % 21;
							dp[rot][next][sum][state] += p.second;
							dp[rot][next][sum][state] %= MOD;
						}
					}
					dp[i % 21][j][f].clear();
				}
			}
		}
		int answer = 0;
		for(size_t i = 0; i < words.size(); ++i){
			for(size_t j = 0; j < ac.size(); ++j){
				answer += dp[m % 21][i][1][j];
				answer %= MOD;
			}
		}
		cout << answer << endl;
	}
	return 0;
}

