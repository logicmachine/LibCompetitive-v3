/**
 *  @file libcomp/geometry/arc.h
 */
#pragma once
#include "libcomp/geometry/point.hpp"

namespace lc {

/**
 *  @defgroup arc Arc
 *  @ingroup  geometry_primitives
 *  @{
 */

/**
 *  @brief 弧
 */
struct Arc {
	/// 中心の座標
	Point c;
	/// 弧の半径
	double r;
	/// 弧の始点
	double a;
	/// 弧の終点
	double b;

	/**
	 *  @brief コンストラクタ
	 *  @param[in] c  中心の座標
	 *  @param[in] r  弧の半径
	 *  @param[in] a  弧の始点
	 *  @param[in] b  弧の終点
	 */
	explicit Arc(
		const Point &c = Point(), double r = 0.0,
		double a = 0.0, double b = 0.0)
		: c(c), r(r), a(a), b(b)
	{ }

	/**
	 *  @brief 不正な値を示す弧の取得
	 *  @return 不正な弧
	 */
	static Arc invalid(){
		double qnan = std::numeric_limits<double>::quiet_NaN();
		return Arc(Point::invalid(), qnan, qnan, qnan);
	}
	/**
	 *  @brief 弧データが不正なものではないかの確認
	 *  @retval true   *thisが有効な弧データであった場合
	 *  @retval false  *thisが無向な弧データであった場合
	 */
	bool is_valid() const {
		return c.is_valid() && !isnan(r) && !isnan(a) && !isnan(b);
	}

	/**
	 *  @brief 円の比較 (<, 厳密評価)
	 *
	 *  コンテナで使用するためのもので数学的な意味はないことに注意。
	 *
	 *  @param[in] i      比較する値
	 *  @retval    true   *thisがiより辞書順で小さい場合
	 *  @retval    false  *thisがiより辞書順で大きい場合
	 */
	bool operator<(const Arc &i) const {
		if(r != i.r){ return r < i.r; }
		if(c != i.c){ return c < i.c; }
		if(a != i.a){ return a < i.a; }
		return b < i.b;
	}
};

/**
 *  @brief 弧の比較 (==, 誤差許容)
 *  @param[in] a      比較する値
 *  @param[in] b      比較する値
 *  @retval    true   aとbが同じ弧を表している場合
 *  @retval    false  aとbが同じ弧を表している場合
 */
inline bool tolerant_eq(const Arc &a, const Arc &b){
	return
		tolerant_eq(a.c, b.c) && tolerant_eq(a.r, b.r) &&
		tolerant_eq(a.a, b.a) && tolerant_eq(a.b, b.b);
}

/**
 *  @}
 */

}

