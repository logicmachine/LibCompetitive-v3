/**
 *  @file libcomp/math/modulus_integer.hpp
 */
#pragma once
#include <iostream>
#include "libcomp/math/modulus_utility.hpp"

namespace lc {

/**
 *  @defgroup modulus_integer Modulus integer
 *  @brief    法演算ラッパー
 *  @ingroup  math
 *  @{
 */

/**
 *  @brief  自動で法演算を行う整数
 *  @tparam MOD  法とする値
 */
template <int MOD>
class ModulusInteger {

public:
	typedef ModulusInteger<MOD> self_type;

private:
	int m_value;

	static self_type unsafe_construct(int x){
		self_type y;
		y.m_value = x;
		return y;
	}

public:
	ModulusInteger()
		: m_value(0)
	{ }
	ModulusInteger(int x)
		: m_value(x % MOD)
	{
		if(m_value < 0){ m_value += MOD; }
	}

	int operator*() const { return m_value; }

	self_type &operator=(const self_type &x){
		m_value = x.m_value;
		return *this;
	}

	bool operator==(const self_type &x) const { return m_value == x.m_value; }
	bool operator!=(const self_type &x) const { return m_value != x.m_value; }
	bool operator< (const self_type &x) const { return m_value <  x.m_value; }
	bool operator<=(const self_type &x) const { return m_value <= x.m_value; }
	bool operator> (const self_type &x) const { return m_value >  x.m_value; }
	bool operator>=(const self_type &x) const { return m_value >= x.m_value; }

	self_type operator+() const { return *this; }
	self_type operator-() const {
		return unsafe_construct(m_value > 0 ? MOD - m_value : 0);
	}

	self_type operator+(const self_type &x) const {
		const int y = m_value + x.m_value;
		return unsafe_construct(y >= MOD ? y - MOD : y);
	}
	self_type operator-(const self_type &x) const {
		const int y = m_value - x.m_value;
		return unsafe_construct(y < 0 ? y + MOD : y);
	}
	self_type operator*(const self_type &x) const {
		return unsafe_construct(
			static_cast<long long>(m_value) * x.m_value % MOD);
	}
	self_type operator/(const self_type &x) const {
		return (*this) * unsafe_construct(modulus_inverse<MOD>(x.m_value));
	}

	self_type &operator+=(const self_type &x){ return (*this = *this + x); }
	self_type &operator-=(const self_type &x){ return (*this = *this - x); }
	self_type &operator*=(const self_type &x){ return (*this = *this * x); }
	self_type &operator/=(const self_type &x){ return (*this = *this / x); }

	self_type &operator++(){
		if(++m_value >= MOD){ m_value = 0; }
		return *this;
	}
	self_type &operator--(){
		if(--m_value < 0){ m_value = MOD - 1; }
		return *this;
	}
	self_type operator++(int){
		self_type t = *this;
		if(++m_value >= MOD){ m_value = 0; }
		return t;
	}
	self_type operator--(int){
		self_type t = *this;
		if(--m_value < 0){ m_value = MOD - 1; }
		return t;
	}

};

template <int MOD>
std::ostream& operator<<(std::ostream &os, const ModulusInteger<MOD> &x){
	os << *x;
    return os;
}

/**
 *  @}
 */

}

