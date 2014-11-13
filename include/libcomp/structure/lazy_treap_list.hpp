/**
 *  @file libcomp/structure/lazy_treap_list.hpp
 */
#pragma once
#include <utility>
#include "libcomp/misc/xorshift128.hpp"

namespace lc {

/**
 *  @defgroup lazy_treap_list List (treap, lazy)
 *  @brief    Treap によるリストの実装 (遅延更新あり)
 *  @ingroup  structure
 *  @{
 */

/**
 *  @brief  Treapを用いた遅延更新リスト
 *  @tparam Traits  リストの動作を示す型
 */
template <class Traits>
class LazyTreapList {

public:
	/// 値型
	typedef typename Traits::value_type value_type;
	/// 更新クエリ型
	typedef typename Traits::modifier_type modifier_type;

private:
	struct node_type {
		value_type value;
		value_type cache;
		modifier_type modifier;
		unsigned int priority;
		size_t size;
		node_type *children[2];
	};

	Traits m_traits;
	node_type *m_root;

	void initialize_node(node_type *p, const value_type &v){
		p->value = p->cache = v;
		p->modifier = m_traits.default_modifier();
		p->priority = xorshift128();
		p->size = 1;
		p->children[0] = p->children[1] = nullptr;
	}

	node_type *clone(const node_type *p) const {
		if(p == nullptr){ return nullptr; }
		node_type *node = new node_type;
		node->value    = p->value;
		node->cache    = p->cache;
		node->modifier = p->modifier;
		node->priority = p->priority;
		node->size     = p->size;
		node->children[0] = clone(p->children[0]);
		node->children[1] = clone(p->children[1]);
		return node;
	}
	void destroy(node_type *p) const {
		if(p == nullptr){ return; }
		destroy(p->children[0]);
		destroy(p->children[1]);
		delete p;
	}

	size_t size(const node_type *p) const {
		if(p == nullptr){ return 0; }
		return p->size;
	}
	const node_type *kth(const node_type *p, size_t k) const {
		if(k >= size(p)){ return nullptr; }
		if(k < size(p->children[0])){ return kth(p->children[0], k); }
		if(k == size(p->children[0])){ return p; }
		return kth(p->children[1], k - 1 - size(p->children[0]));
	}
	node_type *kth(node_type *p, size_t k) const {
		if(k >= size(p)){ return nullptr; }
		if(k < size(p->children[0])){ return kth(p->children[0], k); }
		if(k == size(p->children[0])){ return p; }
		return kth(p->children[1], k - 1 - size(p->children[0]));
	}

	node_type *refresh(node_type *p) const {
		if(p == nullptr){ return nullptr; }
		value_type cache = p->value;
		if(p->children[0] != nullptr){
			cache = m_traits(p->children[0]->cache, cache);
		}
		if(p->children[1] != nullptr){
			cache = m_traits(cache, p->children[1]->cache);
		}
		p->size = 1 + size(p->children[0]) + size(p->children[1]);
		p->cache = m_traits.resolve(p->size, cache, p->modifier);
		return p;
	}
	void propagate(node_type *p) const {
		if(p == nullptr){ return; }
		const size_t k = size(p->children[0]);
		const auto mod_l_cr = m_traits.split_modifier(p->modifier, k);
		const auto mod_c_r  = m_traits.split_modifier(mod_l_cr.second, 1);
		if(p->children[0] != nullptr){
			p->children[0]->modifier = m_traits.merge_modifier(
				p->children[0]->modifier, mod_l_cr.first);
			refresh(p->children[0]);
		}
		if(p->children[1] != nullptr){
			p->children[1]->modifier = m_traits.merge_modifier(
				p->children[1]->modifier, mod_c_r.second);
			refresh(p->children[1]);
		}
		p->value = m_traits.resolve(1, p->value, mod_c_r.first);
		p->modifier= m_traits.default_modifier();
	}

	node_type *merge(node_type *l, node_type *r) const {
		if(l == nullptr){ return r; }
		if(r == nullptr){ return l; }
		if(l->priority > r->priority){
			propagate(l);
			l->children[1] = merge(l->children[1], r);
			return refresh(l);
		}else{
			propagate(r);
			r->children[0] = merge(l, r->children[0]);
			return refresh(r);
		}
	}
	std::pair<node_type *, node_type *> split(node_type *t, size_t k) const {
		if(t == nullptr){ return std::make_pair(nullptr, nullptr); }
		propagate(t);
		if(k <= size(t->children[0])){
			const auto s = split(t->children[0], k);
			t->children[0] = s.second;
			return std::make_pair(s.first, refresh(t));
		}else{
			const auto s = split(
				t->children[1], k - size(t->children[0]) - 1);
			t->children[1] = s.first;
			return std::make_pair(refresh(t), s.second);
		}
	}

public:
	/**
	 *  @brief デフォルトコンストラクタ
	 *
	 *  要素数0のリストとして初期化する。
	 *    - 時間計算量: \f$ O(1) \f$
	 *
	 *  @param[in] traits  処理内容を示すオブジェクト
	 */
	explicit LazyTreapList(const Traits &traits = Traits())
		: m_traits(traits)
		, m_root(nullptr)
	{ }

	/**
	 *  @brief コピーコンストラクタ
	 *    - 時間計算量: \f$ O(n) \f$
	 *  @param[in] x  コピー元オブジェクト
	 */
	LazyTreapList(const LazyTreapList<Traits> &x)
		: m_traits(x.m_traits)
		, m_root(nullptr)
	{
		m_root = clone(x.m_root);
	}

	/**
	 *  @brief ムーブコンストラクタ
	 *    - 時間計算量: \f$ O(1) \f$
	 *  @param[in] x  ムーブ元オブジェクト
	 */
	LazyTreapList(LazyTreapList<Traits> &&x)
		: m_traits(x.m_traits)
		, m_root(x.m_root)
	{
		x.m_root = nullptr;
	}

	/**
	 *  @brief デストラクタ
	 */
	~LazyTreapList(){ destroy(m_root); }

	/**
	 *  @brief リストに含まれる要素数の取得
	 *    - 時間計算量: \f$ O(1) \f$
	 *  @return リストに含まれている要素の数
	 */
	size_t size() const { return size(m_root); }

	/**
	 *  @brief リストが要素を持つかどうかの判定
	 *    - 時間計算量: \f$ O(1) \f$
	 *  @retval true   リストが1つ以上の要素を持っている
	 *  @retval false  リストが要素を持っていない
	 */
	bool empty() const { return m_root == nullptr; }

	/**
	 *  @brief 要素の挿入
	 *
	 *  k番目の要素とk+1番目の要素の間にxを挿入する。
	 *    - 時間計算量: \f$ O(\log{n}) \f$
	 *
	 *  @param[in] k  挿入する位置
	 *  @param[in] x  挿入する値
	 */
	void insert(size_t k, const value_type &x){
		const auto p = split(m_root, k);
		node_type *n = new node_type();
		initialize_node(n, x);
		m_root = merge(merge(p.first, n), p.second);
	}

	/**
	 *  @brief 要素の削除
	 *
	 *  k番目の要素を削除する。
	 *    - 時間計算量: \f$ O(\log{n}) \f$
	 *
	 *  @param[in] k  削除する要素のインデックス
	 */
	void erase(size_t k){
		const auto p = split(m_root, k);
		const auto q = split(p.second, 1);
		delete q.first;
		m_root = merge(p.first, q.second);
	}

	/**
	 *  @brief 要素の更新
	 *
	 *  k番目の要素の値をxに更新する
	 *    - 時間計算量: \f$ O(\log{n}) \f$
	 *
	 *  @param[in] k  更新される要素のインデックス
	 *  @param[in] x  更新後の値
	 */
	void update(size_t k, const value_type &x){
		erase(k);
		insert(k, x);
	}

	/**
	 *  @brief 区間更新
	 *
	 *  インデックスが区間 [l, r) に含まれる要素すべてxに従って更新する。
	 *    - 時間計算量: \f$ O(\log{n}) \f$
	 *
	 *  @param[in] l  区間の始端
	 *  @param[in] r  区間の終端
	 *  @param[in] x  更新クエリ
	 */
	void modify(size_t l, size_t r, const modifier_type &x){
		const auto p0 = split(m_root, l);
		const auto p1 = split(p0.second, r - l);
		node_type *c = p1.first;
		if(c != nullptr){
			c->modifier = m_traits.merge_modifier(c->modifier, x);
			refresh(c);
		}
		m_root = merge(merge(p0.first, c), p1.second);
	}

	/**
	 *  @brief 区間についての問い合わせ
	 *
	 *  インデックスが区間 [l, r) に含まれる要素すべてを結合した結果を求める。
	 *    - 時間計算量: \f$ O(\log{n}) \f$
	 *
	 *  @param[in] l  区間の始端
	 *  @param[in] r  区間の終端
	 *  @return    計算された結果
	 */
	value_type query(size_t l, size_t r){
		const auto p0 = split(m_root, l);
		const auto p1 = split(p0.second, r - l);
		const value_type result = p1.first->cache;
		m_root = merge(merge(p0.first, p1.first), p1.second);
		return result;
	}

	/**
	 *  @brief リストのマージ
	 *
	 *  リストの終端にリストtを追加する。
	 *    - 時間計算量: \f$ O(m + \log{(n+m)}) \f$
	 *
	 *  @param[in] t  結合するリスト
	 */
	void merge(const LazyTreapList<Traits> &t){
		LazyTreapList<Traits> temporary(t);
		merge(std::move(temporary));
	}

	/**
	 *  @brief リストのマージ
	 *
	 *  リストの終端にリストtを追加する。
	 *    - 時間計算量: \f$ O(\log{(n+m)}) \f$
	 *
	 *  @param[in] t  結合するリスト
	 */
	void merge(LazyTreapList<Traits> &&t){
		m_root = merge(m_root, t.m_root);
		t.m_root = nullptr;
	}

	/**
	 *  @brief リストの分割
	 *
	 *  リストを [0, k) と [k, n) で分割する。
	 *    - 時間計算量: \f$ O(\log{n}) \f$
	 *
	 *  @param[in] k  分割する位置
	 *  @return    もともとのリストの [k, n) 番目の要素からなるリスト
	 */
	LazyTreapList<Traits> split(size_t k){
		LazyTreapList<Traits> result(m_traits);
		const auto p = split(m_root, k);
		m_root = p.first;
		result.m_root = p.second;
		return result;
	}

};

/**
 *  @}
 */

}

