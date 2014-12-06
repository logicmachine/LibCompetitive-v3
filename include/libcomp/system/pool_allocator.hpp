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
	typedef T value_type;

	PoolAllocator() noexcept { }
	PoolAllocator(const PoolAllocator<value_type> &) noexcept { }
	template <class U>
	PoolAllocator(const PoolAllocator<U> &) noexcept { }
	~PoolAllocator() noexcept { }

	value_type *allocate(size_t n){
		return FixedMemoryPool<T>::instance().allocate();
	}
	void deallocate(value_type *p, size_t){
		FixedMemoryPool<T>::instance().deallocate(p);
	}

	size_t max_size() const noexcept { return 1; }

	template <typename U>
	bool operator==(const PoolAllocator<U> &){ return false; }
	template <typename U>
	bool operator!=(const PoolAllocator<U> &){ return true; }

};

/**
 *  @}
 */

}

