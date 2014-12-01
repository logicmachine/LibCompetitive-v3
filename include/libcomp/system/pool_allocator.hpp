/**
 *  @file libcomp/system/pool_allocator.hpp
 */
#pragma once
#include <iostream>
#include <vector>
#include <stack>
#include <utility>
#include <cstdlib>
#include <cstddef>

namespace lc {

/**
 *  @defgroup pool_allocator Pool allocator
 *  @brief    プールを用いた固定サイズアロケータ
 *  @ingroup  system
 *  @{
 */

/**
 *  @brief プールを用いた固定サイズアロケータ
 *
 *  プールを利用した固定サイズ専用アロケータ。
 *  シングルトンパターンを適用しているので、必要に応じて
 *  instance() からインスタンスへの参照を取得する。
 *
 *  @tparam T  確保する型
 */
template <class T>
class FixedMemoryPool {

private:
	static const size_t BLOCK_SIZE = (1 << 18);
	std::vector<T *> m_blocks;
	std::stack<T *> m_pool;

	FixedMemoryPool() = default;
	FixedMemoryPool(const FixedMemoryPool<T> &) = delete;
	FixedMemoryPool<T> &operator=(const FixedMemoryPool<T> &) = delete;

public:
	/**
	 *  @brief  インスタンスの取得
	 *  @return 型T用の固定サイズアロケータへの参照
	 */
	static FixedMemoryPool<T> &instance(){
		static FixedMemoryPool<T> self;
		return self;
	}

	/**
	 *  @brief デストラクタ
	 */
	~FixedMemoryPool(){
		for(T *p : m_blocks){ free(p); }
	}

	/**
	 *  @brief  メモリ確保
	 *  @return 確保された領域を指すポインタ
	 */
	T *allocate(){
		if(m_pool.empty()){
			T *block = static_cast<T *>(malloc(sizeof(T) * BLOCK_SIZE));
			for(size_t i = 0; i < BLOCK_SIZE; ++i){
				m_pool.push(block + i);
			}
		}
		T *p = m_pool.top();
		m_pool.pop();
		return p;
	}

	/**
	 *  @brief     メモリ解放
	 *  @param[in] p  解放する領域を指すポインタ
	 */
	void deallocate(T *p){ m_pool.push(p); }

};

/**
 *  @brief  固定サイズアロケータのSTL用ラッパー
 *  @tparam T  確保する要素の型
 */
template <class T>
class PoolAllocator {

public:
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef T *pointer;
	typedef const T *const_pointer;
	typedef T &reference;
	typedef const T &const_reference;
	typedef T value_type;

	template <class U>
	struct rebind {
		typedef PoolAllocator<U> other;
	};

	PoolAllocator(){ }
	template <class U>
	PoolAllocator(const PoolAllocator<U> &){ }

	pointer allocate(size_type n, const void *hint = 0){
		return FixedMemoryPool<T>::instance().allocate();
	}
	void deallocate(pointer p, size_type){
		FixedMemoryPool<T>::instance().deallocate(p);
	}

	template <class U, class... Args>
	void construct(U *p, Args&&... args){
		new(static_cast<void *>(p)) T(std::forward<Args>(args)...);
	}
	template <class U> void destroy(U *p){ p->~U(); }

	pointer address(reference x) const noexcept {
		return std::addressof(x);
	}
	const_pointer address(const_reference x) const noexcept {
		return std::addressof(x);
	}

	size_type max_size() const noexcept { return 1; }

};

/**
 *  @}
 */

}

