/**
 *  @file libcomp/geometry/point.h
 */
#pragma once
#include <limits>
#include "libcomp/geometry/common.hpp"

namespace lc {

/**
 *  @defgroup point Point
 *  @ingroup  geometry_primitives
 *  @{
 */

/**
 *  @beief 点および2次元ベクトル
 */
struct Point {
	/// X座標
	double x;
	/// Y座標
	double y;

	/**
	 *  @brief コンストラクタ
	 *  @param[in] x  X座標
	 *  @param[in] y  Y座標
	 */
	explicit Point(const double &x = 0.0, const double &y = 0.0) :
		x(x), y(y)
	{ }

	/**
	 *  @brief 不正な点の取得
	 *  @return 不正な点を示すデータ
	 */
	static Point invalid(){
		double qnan = std::numeric_limits<double>::quiet_NaN();
		return Point(qnan, qnan);
	}
	/**
	 *  @brief 不正な点ではないかの確認
	 *  @retval true   *thisが有効な点データを持っている場合
	 *  @retval false  *thisが有効な点データを持っていない場合
	 */
	bool is_valid() const { return !(std::isnan(x) || std::isnan(y)); }

	/**
	 *  @brief 座標同士の和の計算
	 *  @param[in] p  加算する座標
	 *  @return    (this->x + p.x, this->y + p.y)
	 */
	Point operator+(const Point &p) const { return Point(x + p.x, y + p.y); }
	/**
	 *  @brief 座標同士の和の計算 (代入)
	 *  @param[in] p  加算する座標
	 *  @return    自身への参照
	 */
	Point &operator+=(const Point &p){ return *this = *this + p; }
	/**
	 *  @brief 座標同士の差の計算
	 *  @param[in] p  減算する座標
	 *  @return    (this->x - p.x, this->y - p.y)
	 */
	Point operator-(const Point &p) const { return Point(x - p.x, y - p.y); }
	/**
	 *  @brief 座標同士の差の計算 (代入)
	 *  @param[in] p  減算する座標
	 *  @return    自身への参照
	 */
	Point &operator-=(const Point &p){ return *this = *this - p; }
	/**
	 *  @brief 座標値のスカラ倍
	 *  @param[in] s  乗算する値
	 *  @return    (this->x * s, this->y * s)
	 */
	Point operator*(double s) const { return Point(x * s, y * s); }
	/**
	 *  @brief 座標値のスカラ倍 (代入)
	 *  @param[in] s  乗算する値
	 *  @return    自身への参照
	 */
	Point &operator*=(double s){ return *this = *this * s; }
	/**
	 *  @brief 座標値の複素数倍
	 *  @param[in] p  乗算する値
	 *  @return    *this * p;
	 */
	Point operator*(const Point &p) const {
		return Point(x * p.x - y * p.y, x * p.y + y * p.x);
	}
	/**
	 *  @brief 座標値の複素数倍 (代入)
	 *  @param[in] p  乗算する値
	 *  @return    自身への参照
	 */
	Point &operator*=(const Point &p){ return *this = *this * p; }
	/**
	 *  @brief 座標値のスカラ倍 (除算)
	 *  @param[in] s  除算する値
	 *  @return    (this->x / s, this->y / s)
	 */
	Point operator/(double s) const { return Point(x / s, y / s); }
	/**
	 *  @brief 座標値のスカラ倍 (除算・代入)
	 *  @param[in] s  除算する値
	 *  @return    自身への参照
	 */
	Point &operator/=(double s){ return *this = *this / s; }

	/**
	 *  @brief 点同士の比較 (==, 厳密評価)
	 *  @param[in] p  比較する点
	 *  @retval    true   *thisとpが同じ座標を表している
	 *  @retval    false  *thisとpが同じ座標を表していない
	 */
	bool operator==(const Point &p) const { return x == p.x && y == p.y; }
	/**
	 *  @brief 点同士の比較 (!=, 厳密評価)
	 *  @param[in] p  比較する点
	 *  @retval    true   *thisとpが同じ座標を表していない
	 *  @retval    false  *thisとpが同じ座標を表している
	 */
	bool operator!=(const Point &p) const { return x != p.x || y != p.y; }
	/**
	 *  @brief 点同士の比較 (<, 厳密評価)
	 *  @param[in] p  比較する点
	 *  @retval    true   *thisがpより辞書順で小さい
	 *  @retval    false  *thisがpより辞書順で小さくない
	 */
	bool operator<(const Point &p) const {
		return (x == p.x) ? (y < p.y) : (x < p.x);
	}

	/**
	 *  @brief 原点との間のユークリッド距離を求める
	 *  @return (0, 0) と *this のユークリッド距離
	 */
	double abs() const { return sqrt(x * x + y * y); }
	/**
	 *  @brief 原点との間の2乗ノルムを求める
	 *  @return (0, 0) と *this の2乗ノルム
	 */
	double norm() const { return x * x + y * y; }
	/**
	 *  @brief ベクトルの正規化
	 *  @return *this を正規化した結果
	 */
	Point unit() const { return *this / abs(); }
	/**
	 *  @brief 直交するベクトルの計算
	 *  @return *this と直交するベクトル
	 */
	Point ortho() const { return Point(-y, x); }
	/**
	 *  @brief 偏角の計算
	 *  @return ベクトル *this の偏角
	 */
	double arg() const { return atan2(y, x); }
};

/**
 *  @brief 座標値のスカラ倍
 *  @param[in] s  乗算する値
 *  @param[in] p  乗算する座標
 *  @return    (this->x * s, this->y * s)
 */
inline Point operator*(double s, const Point &p){ return p * s; }

/**
 *  @brief 点同士の比較 (==, 誤差許容)
 *  @param[in] a  比較する点
 *  @param[in] b  比較する点
 *  @retval    true   aとbが十分に近い座標を表している
 *  @retval    false  aとbが十分に近い座標を表していない
 */
inline bool tolerant_eq(const Point &a, const Point &b){
	return tolerant_eq(a.x, b.x) && tolerant_eq(a.y, b.y);
}

/**
 *  @brief クロス積の計算
 *  @param[in] a  計算に用いる項
 *  @param[in] b  計算に用いる項
 *  @return       aとbのクロス積
 */
inline double cross(const Point &a, const Point &b){
	return a.x * b.y - a.y * b.x;
}
/**
 *  @brief ドット積の計算
 *  @param[in] a  計算に用いる項
 *  @param[in] b  計算に用いる項
 *  @return       aとbのドット積
 */
inline double dot(const Point &a, const Point &b){
	return a.x * b.x + a.y * b.y;
}
/**
 *  @brief 点の進行方向の計算
 *  @param[in] a   始点の座標
 *  @param[in] b   中点の座標
 *  @param[in] c   終点の座標
 *  @retval    0   曲線(a, b, c)が点bで180度曲がり点cが点a,bの間にある場合
 *  @retval    1   曲線(a, b, c)が点bで反時計回りに曲がっている場合
 *  @retval    -1  曲線(a, b, c)が点bで時計回りに曲がっている場合
 *  @retval    2   曲線(a, b, c)が点bで180度曲がり点cが点aを通り過ぎる場合
 *  @retval    -2  曲線(a, b, c)が一直線である場合
 */ 
inline int ccw(const Point &a, const Point &b, const Point &c){
	Point d = b - a, e = c - a;
	if(cross(d, e) > 0.0){ return 1; }
	if(cross(d, e) < 0.0){ return -1; }
	if(dot(d, e) < 0.0){ return 2; }
	if(d.abs() < e.abs()){ return -2; }
	return 0;
}

/**
 *  @}
 */

}

