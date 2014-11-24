/**
 *  @file libcomp/geometry/segment.h
 */
#pragma once
#include "libcomp/geometry/point.hpp"
#include "libcomp/geometry/line.hpp"

namespace lc {

/**
 *  @defgroup segment Segment
 *  @ingroup  geometry_primitives
 *  @{
 */

/**
 *  @brief 線分
 */
struct Segment {
	/// 線分の始点
	Point a;
	/// 線分の終点
	Point b;

	/**
	 *  @brief コンストラクタ
	 *  @param[in] a  線分の始点
	 *  @param[in] b  線分の終点
	 */
	explicit Segment(const Point &a = Point(), const Point &b = Point()) :
		a(a), b(b)
	{ }

	/**
	 *  @brief 不正な値を示す線分の取得
	 *  @return 不正な線分
	 */
	static Segment invalid(){
		Point inv = Point::invalid();
		return Segment(inv, inv);
	}
	/**
	 *  @brief 線分データが不正なものではないかの確認
	 *  @retval true   *thisが有効な線分データであった場合
	 *  @retval false  *thisが無効な線分データであった場合
	 */
	bool is_valid() const { return a.is_valid() && b.is_valid(); }

	/**
	 *  @brief 線分の比較 (<, 厳密評価)
	 *
	 *  コンテナで使用するためのもので数学的な意味はないことに注意。
	 *
	 *  @param[in] s      比較する値
	 *  @retval    true   *thisがsより辞書順で小さい場合
	 *  @retval    false  *thisがsより辞書順で大きい場合
	 */
	bool operator<(const Segment &s) const {
		return (a == s.a) ? (b < s.b) : (a < s.a);
	}

	/**
	 *  @brief 線分を含む直線の生成
	 *  @return 線分 *this を含む直線
	 */
	Line to_line() const { return Line(a, b); }

	/**
	 *  @brief 線分の長さの計算
	 *  @return 線分の長さ
	 */
	double length() const { return (b - a).abs(); }

	/**
	 *  @brief  線分の向きの計算
	 *  @return 線分と同じ向きの単位ベクトル
	 */
	Point direction() const {
		return (b - a).unit();
	}
};

/**
 *  @brief 線分の比較 (==, 誤差許容, 無向)
 *  @param[in] a      比較する値
 *  @param[in] b      比較する値
 *  @retval    true   aとbが同じ線分を表している場合
 *  @retval    false  aとbが同じ線分を表していない場合
 */
inline bool tolerant_eq(const Segment &a, const Segment &b){
	if(tolerant_eq(a.a, b.a) && tolerant_eq(a.b, b.b)){ return true; }
	if(tolerant_eq(a.a, b.b) && tolerant_eq(a.b, b.a)){ return true; }
	return false;
}

/**
 *  @brief 線分の比較 (==, 誤差許容, 有向)
 *  @param[in] a      比較する値
 *  @param[in] b      比較する値
 *  @retval    true   aとbが同じ線分を表している場合
 *  @retval    false  aとbが同じ線分を表していない場合
 */
inline bool directed_tolerant_eq(const Segment &a, const Segment &b){
	return tolerant_eq(a.a, b.a) && tolerant_eq(a.b, b.b);
}

/**
 *  @}
 */

}

