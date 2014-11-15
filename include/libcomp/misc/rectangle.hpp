/**
 *  @file libcomp/misc/rectangle.hpp
 */
#pragma once

namespace lc {

/**
 *  @defgroup rectangle Rectangle
 *  @brief    矩形
 *  @ingroup  misc
 *  @{
 */

/**
 *  @brief 矩形
 *  @tparam T  座標の型
 */
template <class T>
struct Rectangle {
	/// 矩形の左上x座標
	T x1;
	/// 矩形の左上y座標
	T y1;
	/// 矩形の右下x座標
	T x2;
	/// 矩形の右下y座標
	T y2;
	/// デフォルトコンストラクタ
	Rectangle() : x1(), y1(), x2(), y2() { }
	/// コンストラクタ
	Rectangle(const T &x1, const T &y1, const T &x2, const T &y2)
		: x1(x1), y1(y1), x2(x2), y2(y2)
	{ }
	/// 比較演算子 (==)
	bool operator==(const Rectangle<T> &r) const {
		return y1 == r.y1 && x1 == r.x1 && y2 == r.y2 && x2 == r.x2;
	}
	/// 比較演算子 (!=)
	bool operator!=(const Rectangle<T> &r) const {
		return y1 != r.y1 && x1 != r.x1 && y2 != r.y2 && x2 != r.x2;
	}
	/// 比較演算子 (<)
	bool operator<(const Rectangle<T> &r) const {
		if(y1 != r.y1){ return y1 < r.y1; }
		if(x1 != r.x1){ return x1 < r.x1; }
		if(y2 != r.y2){ return y2 < r.y2; }
		return x2 < r.x2;
	}
	/// 比較演算子 (<=)
	bool operator<=(const Rectangle<T> &r) const {
		return (*this < r) || (*this == r);
	}
	/// 比較演算子 (>)
	bool operator>(const Rectangle<T> &r) const {
		return r < *this;
	}
	/// 比較演算子 (>=)
	bool operator>=(const Rectangle<T> &r) const {
		return r <= *this;
	}
};

/**
 *  @}
 */

}

