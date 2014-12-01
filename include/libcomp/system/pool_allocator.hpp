/**
 *  @file libcomp/system/pool_allocator.hpp
 */
#pragma once
#include <vector>
#include <stack>
#include <utility>
#include <cstdlib>
#include <cstddef>

namespace lc {

class MemoryPool {

private:
	class FixedMemoryPool {
	private:
		static const size_t BLOCK_SIZE = (1 << 16);
		size_t m_element_size;
		std::vector<void *> m_blocks;
		std::stack<void *> m_pool;
		FixedMemoryPool(const FixedMemoryPool &) = delete;
		FixedMemoryPool &operator=(const FixedMemoryPool &) = delete;
		void clear(){
			for(void *p : m_blocks){ free(p); }
			m_blocks.clear();
			while(!m_pool.empty()){ m_pool.pop(); }
		}
	public:
		explicit FixedMemoryPool(size_t element_size = 0)
			: m_element_size(element_size)
		{ }
		FixedMemoryPool(FixedMemoryPool &&obj)
			: m_element_size(obj.m_element_size)
			, m_blocks(std::move(obj.m_blocks))
			, m_pool(std::move(obj.m_pool))
		{ }
		~FixedMemoryPool(){ clear(); }
		void resize(size_t n){
			clear();
			m_element_size = n;
		}
		void *allocate(){
			if(m_pool.empty()){
				void *block = malloc(m_element_size * BLOCK_SIZE);
				for(size_t i = 0; i < BLOCK_SIZE; ++i){
					m_pool.push(static_cast<void *>(
						static_cast<uint8_t *>(block) + m_element_size * i));
				}
			}
			void *p = m_pool.top();
			m_pool.pop();
			return p;
		}
		void release(void *p){ m_pool.push(p); }
	};

	std::vector<FixedMemoryPool> m_pools;

	MemoryPool() = default;
	MemoryPool(const MemoryPool &) = delete;
	MemoryPool &operator=(const MemoryPool &) = delete;

public:
	static MemoryPool &instance(){
		static MemoryPool instance;
		return instance;
	}

	void *allocate(size_t n){
		if(n >= m_pools.size()){
			const size_t t = m_pools.size();
			m_pools.resize(n + 1);
			for(size_t i = t; i <= n; ++i){
				m_pools[i].resize(i);
			}
		}
		return m_pools[n].allocate();
	}
	void release(void *p, size_t n){
		m_pools[n].release(p);
	}

};

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
		return static_cast<pointer>(
			MemoryPool::instance().allocate(n * sizeof(T)));
	}
	void deallocate(pointer p, size_type n){
		return MemoryPool::instance().release(p, n * sizeof(T));
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

}

