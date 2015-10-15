/**
 *  @file libcomp/lang/integer_range.hpp
 */
#pragma once

namespace lc {

/**
 *  @defgroup integer_range Integer range
 *  @brief    整数の区間
 *  @ingroup  lang
 *  @{
 */

template <typename T>
class IncrementIterator {
	T x;
public:
	explicit IncrementIterator(T x = T()) : x(x) { }
	T operator*() const { return x; }
	IncrementIterator<T> operator++(){ ++x; return *this; }
	IncrementIterator<T> operator++(int){ return IncrementIterator<T>(x++); }
	IncrementIterator<T> operator--(){ --x; return *this; }
	IncrementIterator<T> operator--(int){ return IncrementIterator<T>(x--); }
	bool operator==(IncrementIterator<T> it) const { return x == it.x; }
	bool operator!=(IncrementIterator<T> it) const { return x != it.x; }
};

template <typename T>
class DecrementIterator {
	T x;
public:
	explicit DecrementIterator(T x = T()) : x(x) { }
	T operator*() const { return x; }
	DecrementIterator<T> operator++(){ --x; return *this; }
	DecrementIterator<T> operator++(int){ return DecrementIterator<T>(x--); }
	DecrementIterator<T> operator--(){ ++x; return *this; }
	DecrementIterator<T> operator--(int){ return DecrementIterator<T>(x++); }
	bool operator==(DecrementIterator<T> it) const { return x == it.x; }
	bool operator!=(DecrementIterator<T> it) const { return x != it.x; }
};

template <typename Iterator>
class IntegerRange {
	Iterator m_begin, m_end;
public:
	IntegerRange() : m_begin(), m_end() { }
	template <typename T>
	IntegerRange(T begin, T end) : m_begin(begin), m_end(end) { }
	Iterator begin() const { return m_begin; }
	Iterator end() const { return m_end; }
};

/**
 *  @brief [0, x) の範囲の整数列の生成 (昇順)
 *  @param[in] x  区間の終端
 *  @return    生成された数列を表す区間
 */
template <typename T>
inline IntegerRange<IncrementIterator<T>> inc_range(T x){
	return IntegerRange<IncrementIterator<T>>(T(0), x);
}

/**
 *  @brief [s, t) の範囲の整数列の生成 (昇順)
 *  @param[in] s  区間の始端
 *  @param[in] t  区間の終端
 *  @return    生成された数列を表す区間
 */
template <typename T>
inline IntegerRange<IncrementIterator<T>> inc_range(T s, T t){
	return IntegerRange<IncrementIterator<T>>(s, t);
}

/**
 *  @brief [0, x) の範囲の整数列の生成 (降順)
 *  @param[in] x  区間の終端
 *  @return    生成された数列を表す区間
 */
template <typename T>
inline IntegerRange<DecrementIterator<T>> dec_range(T x){
	return IntegerRange<DecrementIterator<T>>(x - 1, T(-1));
}

/**
 *  @brief [s, t) の範囲の整数列の生成 (降順)
 *  @param[in] s  区間の始端
 *  @param[in] t  区間の終端
 *  @return    生成された数列を表す区間
 */
template <typename T>
inline IntegerRange<DecrementIterator<T>> dec_range(T s, T t){
	return IntegerRange<DecrementIterator<T>>(t - 1, s - 1);
}

/**
 *  @}
 */

}

