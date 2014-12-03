/**
 *  @file libcomp/structure/persistent_rbst_list.hpp
 */
#pragma once
#include <limits>
#include <utility>
#include <memory>
#include "libcomp/misc/xorshift128.hpp"
#include "libcomp/system/pool_allocator.hpp"

namespace lc {

/**
 *  @defgroup persistent_rbst_list Persistent list (RBST)
 *  @brief    RBST による永続リストの実装
 *  @ingroup  structure
 *  @{
 */

/**
 *  @brief  RBSTを用いた永続リスト
 *  @tparam Traits  リストの動作を示す型
 */
template <class Traits>
class PersistentRBSTList {

public:
	/// 自身の型
	typedef PersistentRBSTList<Traits> self_type;
	/// 値型
	typedef typename Traits::value_type value_type;

protected:
	struct node_type {
		value_type value;
		value_type cache;
		size_t size;
		std::shared_ptr<node_type> children[2];
	};
	typedef std::shared_ptr<node_type> node_ptr;
	typedef PoolAllocator<node_type> allocator_type;

	Traits m_traits;
	node_ptr m_root;

	void initialize_node(node_ptr &p, const value_type &v) const {
		p->value = p->cache = v;
		p->size = 1;
		p->children[0] = p->children[1] = node_ptr();
	}

	node_ptr copy_node(const node_ptr &p) const {
		return std::allocate_shared<node_type>(allocator_type(), *p);
	}

	size_t size(const node_ptr &p) const {
		if(!p){ return 0; }
		return p->size;
	}

	node_ptr kth(const node_ptr &p, size_t k) const {
		if(k >= size(p)){ return node_ptr(); }
		const size_t ls = size(p->children[0]);
		if(k == ls){
			return p;
		}else if(k < ls){
			return kth(p->children[0], k);
		}else{
			return kth(p->children[1], k - 1 - ls);
		}
	}

	node_ptr refresh(node_ptr &p) const {
		if(!p){ return node_ptr(); }
		p->cache = p->value;
		if(p->children[0]){
			p->cache = m_traits(p->children[0]->cache, p->cache);
		}
		if(p->children[1]){
			p->cache = m_traits(p->cache, p->children[1]->cache);
		}
		p->size = 1 + size(p->children[0]) + size(p->children[1]);
		return p;
	}

	node_ptr merge(const node_ptr &l, const node_ptr &r) const {
		if(!l){ return r; }
		if(!r){ return l; }
		const size_t limit = std::numeric_limits<unsigned int>::max();
		const size_t ls = size(l), rs = size(r);
		if(xorshift128() * (ls + rs) < ls * limit){
			node_ptr t = copy_node(l);
			t->children[1] = merge(t->children[1], r);
			return refresh(t);
		}else{
			node_ptr t = copy_node(r);
			t->children[0] = merge(l, r->children[0]);
			return refresh(t);
		}
	}
	std::pair<node_ptr, node_ptr> split(const node_ptr &t, size_t k) const {
		if(!t){ return std::make_pair(node_ptr(), node_ptr()); }
		node_ptr p = copy_node(t);
		if(k <= size(t->children[0])){
			const auto s = split(p->children[0], k);
			p->children[0] = s.second;
			return std::make_pair(s.first, refresh(p));
		}else{
			const auto s = split(p->children[1], k - size(p->children[0]) - 1);
			p->children[1] = s.first;
			return std::make_pair(refresh(p), s.second);
		}
	}

	node_ptr update(const node_ptr &t, size_t k, const value_type &v) const {
		if(!t){ return node_ptr(); }
		const size_t ls = size(t->children[0]);
		node_ptr p = copy_node(t);
		if(k == ls){
			p->value = v;
		}else if(k < ls){
			p->children[0] = update(p->children[0], k, v);
		}else{
			p->children[1] = update(p->children[1], k - ls - 1, v);
		}
		return refresh(p);
	}

	template <class Iterator>
	std::pair<node_ptr, Iterator> range_copy(
		Iterator first, Iterator last, int depth) const
	{
		if(depth == 0){ return std::make_pair(node_ptr(), first); }
		const auto l = range_copy(first, last, depth - 1);
		first = l.second;
		if(first == last){ return l; }
		node_ptr c = std::allocate_shared<node_type>(allocator_type());
		initialize_node(c, *(first++));
		c->children[0] = l.first;
		const auto r = range_copy(first, last, depth - 1);
		c->children[1] = r.first;
		return std::make_pair(refresh(c), r.second);
	}

	PersistentRBSTList(node_ptr root, const Traits &traits)
		: m_traits(traits)
		, m_root(root)
	{ }

public:
	/**
	 *  @brief デフォルトコンストラクタ
	 *
	 *  要素数0のリストとして初期化する。
	 *    - 時間計算量: \f$ O(1) \f$
	 *
	 *  @param[in] traits  処理内容を示すオブジェクト
	 */
	explicit PersistentRBSTList(const Traits &traits = Traits())
		: m_traits(traits)
		, m_root()
	{ }

	/**
	 *  @brief 要素列からの初期化
	 *
	 *  [first, last) の区間内の要素で初期化する。
	 *  必ず平衡した木を返すため、初期化後の処理も
	 *  insert を繰り返して初期化するより高速になる。
	 *    - 時間計算量: \f$ O(n) \f$
	 *
	 *  @param[in] first   区間の先頭を指すイテレータ
	 *  @param[in] last    区間の終端を指すイテレータ
	 *  @param[in] traits  処理内容を示すオブジェクト
	 */
	template <class Iterator>
	PersistentRBSTList(
		Iterator first, Iterator last, const Traits &traits = Traits())
		: m_traits(traits)
		, m_root()
	{
		const int n = std::distance(first, last);
		const int depth = (n == 0 ? 0 : (32 - __builtin_clz(n)));
		m_root = range_copy(first, last, depth).first;
	}

	/**
	 *  @brief コピーコンストラクタ
	 *    - 時間計算量: \f$ O(n) \f$
	 *  @param[in] x  コピー元オブジェクト
	 */
	PersistentRBSTList(const self_type &x)
		: m_traits(x.m_traits)
		, m_root(x.m_root)
	{ }

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
	bool empty() const { return !m_root; }

	/**
	 *  @brief インデックスアクセス
	 *    - 時間計算量: \f$ O(\log{n}) \f$
	 *  @param[in] k  アクセスしたい要素のインデックス
	 *  @return    リスト中でk番目の要素の値
	 */
	const value_type &operator[](size_t k) const {
		return kth(m_root, k)->value;
	}

	/**
	 *  @brief 要素の挿入
	 *
	 *  k番目の要素とk+1番目の要素の間にxを挿入する。
	 *    - 時間計算量: \f$ O(\log{n}) \f$
	 *
	 *  @param[in] k  挿入する位置
	 *  @param[in] x  挿入する値
	 *  @return    挿入した結果のリスト
	 */
	self_type insert(size_t k, const value_type &x) const {
		const auto p = split(m_root, k);
		node_ptr n = std::allocate_shared<node_type>(allocator_type());
		initialize_node(n, x);
		return self_type(merge(merge(p.first, n), p.second), m_traits);
	}

	/**
	 *  @brief 要素の削除
	 *
	 *  k番目の要素を削除する。
	 *    - 時間計算量: \f$ O(\log{n}) \f$
	 *
	 *  @param[in] k  削除する要素のインデックス
	 *  @return    削除した結果のリスト
	 */
	self_type erase(size_t k) const {
		const auto p = split(m_root, k);
		const auto q = split(p.second, 1);
		return self_type(merge(p.first, q.second), m_traits);
	}

	/**
	 *  @brief 要素の更新
	 *
	 *  k番目の要素の値をxに更新する
	 *    - 時間計算量: \f$ O(\log{n}) \f$
	 *
	 *  @param[in] k  更新される要素のインデックス
	 *  @param[in] x  更新後の値
	 *  @return    更新した結果のリスト
	 */
	self_type update(size_t k, const value_type &x) const {
		return self_type(update(m_root, k, x), m_traits);
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
		const auto p = split(m_root, l);
		const auto q = split(p.second, r - l);
		const value_type result = q.first->cache;
		m_root = merge(merge(p.first, q.first), q.second);
		return result;
	}

	/**
	 *  @brief リストのマージ
	 *
	 *  リストの終端にリストtを追加する。
	 *    - 時間計算量: \f$ O(\log{n} + \log{m)}) \f$
	 *
	 *  @param[in] t  結合するリスト
	 *  @return    結合した結果のリスト
	 */
	self_type merge(const self_type &t) const {
		return self_type(merge(m_root, t.m_root), m_traits);
	}

	/**
	 *  @brief リストの分割
	 *
	 *  リストを [0, k) と [k, n) で分割する。
	 *    - 時間計算量: \f$ O(\log{n}) \f$
	 *
	 *  @param[in] k  分割する位置
	 *  @return    もともとのリストの [0, k), [k, n) 番目の要素からなるリスト
	 */
	std::pair<self_type, self_type> split(size_t k) const {
		const auto p = split(m_root, k);
		return std::make_pair(
			self_type(p.first, m_traits), self_type(p.second, m_traits));
	}

};

/**
 *  @}
 */

}

