/**
 *  @file libcomp/geometry/line.h
 */
#pragma once
#include "libcomp/geometry/point.hpp"

namespace lc {

/**
 *  @defgroup line Line
 *  @ingroup  geometry_primitives
 *  @{
 */

/**
 *  @brief 直線
 */
struct Line {
	/// 直線状の点
	Point a;
	/// 直線状の点
	Point b;

	/**
	 *  @brief コンストラクタ
	 *  @param[in] a  直線状の点
	 *  @param[in] b  直線状の点 (a != b)
	 */
	explicit Line(const Point &a = Point(), const Point &b = Point()) :
		a(a), b(b)
	{ }

	/**
	 *  @brief 不正な値を示す直線の取得
	 *  @return 不正な直線
	 */
	static Line invalid(){
		Point inv = Point::invalid();
		return Line(inv, inv);
	}
	/**
	 *  @brief 直線データが不正なものではないかの確認
	 *  @retval true   *thisが有効な直線データであった場合
	 *  @retval false  *thisが無効な直線データであった場合
	 */
	bool is_valid() const { return a.is_valid() && b.is_valid(); }

	/**
	 *  @brief 直線の比較 (<, 厳密評価)
	 *
	 *  コンテナで使用するためのもので数学的な意味はないことに注意。
	 *  (同一直線ではあるものの選んでいる点が異なる場合など)
	 *
	 *  @param[in] l      比較する値
	 *  @retval    true   *thisがlより辞書順で小さい場合
	 *  @retval    false  *thisがlより辞書順で小さくない場合
	 */
	bool operator<(const Line &l) const {
		return (a == l.a) ? (b < l.b) : (a < l.a);
	}

	/**
	 *  @brief 点の直線への射影
	 *  @param[in] p  射影する点
	 *  @return    直線上で最も点pに近くなる座標
	 */
	Point projection(const Point &p) const {
		double t = dot(p - a, b - a) / (b - a).norm();
		return a + t * (b - a);
	}
	/**
	 *  @brief 直線を挟んで対称な位置にある点の計算
	 *  @param[in] p  変換する点
	 *  @return    直線を挟んで点pと対称な点
	 */
	Point reflection(const Point &p) const {
		return p + 2.0 * (projection(p) - p);
	}
	/**
	 *  @brief  直線の向きの計算
	 *  @return 直線と同じ向きの単位ベクトル
	 */
	Point direction() const {
		return (b - a).unit();
	}
};

/**
 *  @brief 直線の比較 (==, 誤差許容, 無向)
 *  @param[in] a      比較する値
 *  @param[in] b      比較する値
 *  @retval    true   aとbが同じ直線を表している場合
 *  @retval    false  aとbが同じ直線を表していない場合
 */
inline bool tolerant_eq(const Line &a, const Line &b){
	const double x = abs(cross(a.b - a.a, b.b - b.a));
	const double y = abs(cross(a.b - a.a, a.b - b.a));
	return x < EPS && y < EPS;
}

/**
 *  @brief 直線の比較 (==, 誤差許容, 有向)
 *  @param[in] a      比較する値
 *  @param[in] b      比較する値
 *  @retval    true   aとbが同じ直線を表している場合
 *  @retval    false  aとbが同じ直線を表していない場合
 */
inline bool directed_tolerant_eq(const Line &a, const Line &b){
	if(!tolerant_eq(a, b)){ return false; }
	return tolerant_eq((a.a - a.b).unit(), (b.a - b.b).unit());
}

/**
 *  @brief 2直線が平行かどうかの判定
 *  @param[in] a  判定する直線
 *  @param[in] b  判定する直線
 */
inline bool is_parallel(const Line &a, const Line &b){
	return abs(cross(a.b - a.a, b.b - b.a)) < EPS;
}

/**
 *  @brief 直線と点の進行方向
 *  @param[in] l  直線
 *  @param[in] p  点
 *  @retval    0   曲線(l.a, l.b, p)が点l.bで180度曲がり点pが点l.a, l.bの間にある場合
 *  @retval    1   曲線(l.a, l.b, p)が点l.bで反時計回りに曲がっている場合
 *  @retval    -1  曲線(l.a, l.b, p)が点l.bで時計回りに曲がっている場合
 *  @retval    2   曲線(l.a, l.b, p)が点l.bで180度曲がり点pが点l.aを通り過ぎる場合
 *  @retval    -2  曲線(l.a, l.b, p)が一直線である場合
 */
inline int ccw(const Line &l, const Point &p){
	return ccw(l.a, l.b, p);
}

/**
 *  @}
 */

}

