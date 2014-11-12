#include <iostream>
#include <tuple>
#include "libcomp/math/modulus_integer.hpp"
#include "libcomp/math/matrix.hpp"
#include "libcomp/structure/lazy_segment_tree.hpp"

using namespace std;
typedef lc::ModulusInteger<1000000009> mint;

static mint fib_coeff_f0[(1 << 19) + 1];
static mint fib_coeff_f1[(1 << 19) + 1];
static mint fib_sum_coeff_f0[(1 << 19) + 1];
static mint fib_sum_coeff_f1[(1 << 19) + 1];

inline void initialize_tables(){
	const lc::Matrix<mint> m = {
		{ 0, 1, 0 },
		{ 1, 1, 0 },
		{ 1, 0, 1 }
	};
	lc::Matrix<mint> t = {
		{ 1, 0, 0 },
		{ 0, 1, 0 },
		{ 0, 0, 1 }
	};
	for(int i = 0; i <= (1 << 19); ++i){
		fib_coeff_f0[i] = t(0, 0);
		fib_coeff_f1[i] = t(0, 1);
		fib_sum_coeff_f0[i] = t(2, 0);
		fib_sum_coeff_f1[i] = t(2, 1);
		t *= m;
	}
}

inline mint fibonacci(mint f0, mint f1, int k){
	return fib_coeff_f0[k] * f0 + fib_coeff_f1[k] * f1;
}
inline mint fibonacci_sum(mint f0, mint f1, int k){
	return fib_sum_coeff_f0[k] * f0 + fib_sum_coeff_f1[k] * f1;
}

struct LazySumFibAddTraits {
	typedef mint value_type;
	typedef tuple<mint, mint> modifier_type;
	value_type default_value() const { return mint(); }
	modifier_type default_modifier() const {
		return make_tuple(mint(), mint());
	}
	pair<modifier_type, modifier_type>
	split_modifier(const modifier_type &m, size_t k) const {
		const mint fk0 = fibonacci(get<0>(m), get<1>(m), k);
		const mint fk1 = fibonacci(get<0>(m), get<1>(m), k + 1);
		return make_pair(m, make_tuple(fk0, fk1));
	}
	modifier_type merge_modifier(
		const modifier_type &a, const modifier_type &b) const
	{
		return make_tuple(get<0>(a) + get<0>(b), get<1>(a) + get<1>(b));
	}
	value_type operator()(const value_type &a, const value_type &b) const {
		return a + b;
	}
	value_type resolve(
		size_t n, const value_type &v, const modifier_type &m) const
	{
		return v + fibonacci_sum(get<0>(m), get<1>(m), n);
	}
};

int main(){
	ios_base::sync_with_stdio(false);
	initialize_tables();
	int n, m;
	cin >> n >> m;
	vector<int> a(n);
	for(int i = 0; i < n; ++i){ cin >> a[i]; }
	lc::LazySegmentTree<LazySumFibAddTraits> st(a.begin(), a.end());
	while(m--){
		int t, l, r;
		cin >> t >> l >> r;
		--l;
		if(t == 1){
			st.modify(l, r, make_tuple(1, 1));
		}else{
			cout << *(st.query(l, r)) << endl;
		}
	}
	return 0;
}

