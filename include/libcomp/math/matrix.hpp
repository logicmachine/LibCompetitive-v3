/**
 *  @file libcomp/math/matrix.hpp
 */
#pragma once
#include <algorithm>
#include <functional>
#include <cassert>

namespace lc {

/**
 *  @defgroup matrix Matrix
 *  @brief    行列
 *  @ingroup  math
 *  @{
 */

template <class T>
class Matrix {

private:
	int m_rows;
	int m_cols;
	std::vector<T> m_data;

	template <class Func>
	Matrix<T> apply(Func f) const {
		Matrix<T> ret(m_rows, m_cols);
		std::transform(m_data.begin(), m_data.end(), ret.m_data.begin(), f);
		return ret;
	}
	template <class Func>
	Matrix<T> apply_scalar(const T &s, Func f) const {
		return apply(std::bind2nd(f, s));
	}
	template <class Func>
	Matrix<T> apply_mat(const Matrix<T> &m, Func f) const {
		assert(m_rows == m.m_rows && m_cols == m.m_cols);
		Matrix<T> ret(m_rows, m_cols);
		std::transform(
			m_data.begin(), m_data.end(), m.m_data.begin(),
			ret.m_data.begin(), f);
		return ret;
	}

public:
	Matrix()
		: m_rows(0)
		, m_cols(0)
		, m_data()
	{ }
	Matrix(int n, int m)
		: m_rows(n)
		, m_cols(m)
		, m_data(n * m)
	{ }
	template <typename U>
	Matrix(std::initializer_list<std::initializer_list<U>> a)
		: m_rows(a.size())
		, m_cols(a.begin()->size())
		, m_data(m_rows * m_cols)
	{
		auto it = a.begin();
		for(int i = 0; it != a.end(); ++i, ++it){
			std::copy(it->begin(), it->end(), m_data.begin() + m_cols * i);
		}
	}

	int rows() const { return m_rows; }
	int cols() const { return m_cols; }

	const T &operator()(int i, int j) const {
		return m_data[i * m_cols + j];
	}
	T &operator()(int i, int j){
		return m_data[i * m_cols + j];
	}

	Matrix<T> operator-() const {
		return apply(std::negate<T>());
	}

	Matrix<T> operator*(const T &s) const {
		return apply_scalar(s, std::multiplies<T>());
	}
	Matrix<T> operator/(const T &s) const {
		return apply_scalar(s, std::divides<T>());
	}
	Matrix<T> operator%(const T &s) const {
		return apply_scalar(s, std::modulus<T>());
	}

	Matrix<T> &operator*=(const T &s){ return (*this = *this * s); }
	Matrix<T> &operator/=(const T &s){ return (*this = *this / s); }
	Matrix<T> &operator%=(const T &s){ return (*this = *this % s); }

	Matrix<T> operator+(const Matrix<T> &m) const {
		return apply_mat(m, std::plus<T>());
	}
	Matrix<T> operator-(const Matrix<T> &m) const {
		return apply_mat(m, std::minus<T>());
	}
	Matrix<T> operator*(const Matrix<T> &m) const {
		assert(m_cols == m.m_rows);
		Matrix<T> ret(m_rows, m.m_cols);
		for(int i = 0; i < m_rows; ++i){
			for(int k = 0; k < m_cols; ++k){
				for(int j = 0; j < m.m_cols; ++j){
					ret(i, j) += (*this)(i, k) * m(k, j);
				}
			}
		}
		return ret;
	}

	Matrix<T> &operator+=(const Matrix<T> &m){ return (*this = *this + m); }
	Matrix<T> &operator-=(const Matrix<T> &m){ return (*this = *this - m); }
	Matrix<T> &operator*=(const Matrix<T> &m){ return (*this = *this * m); }

};

template <class T>
inline Matrix<T> pow(Matrix<T> x, long long y){
	assert(x.rows() == x.cols());
	Matrix<T> z(x.rows(), x.cols());
	for(int i = 0; i < x.rows(); ++i){ z(i, i) = 1; }
	while(y > 0){
		if(y & 1){ z *= x; }
		x *= x;
		y >>= 1;
	}
	return z;
}

/**
 *  @}
 */

}

