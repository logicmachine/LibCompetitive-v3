/**
 *  @file geometry/essential/common.h
 */
#pragma once
#include <cmath>

namespace lc {

/**
 *  @defgroup common Common
 *  @ingroup  geometry
 *  @{
 */

/**
 *  @brief 許容誤差
 */
static const double EPS = 1e-9;

/**
 *  @brief 絶対値の計算
 *  @param[in] x  入力
 *  @return    xの絶対値
 */
inline double abs(double x){ return std::abs(x); }

/**
 *  @brief 誤差を許容する比較 (実数)
 *  @param[in] a      比較する数値
 *  @param[in] b      比較する数値
 *  @retval    true   aとbの差の絶対値がEPS未満
 *  @retval    false  aとbの差の絶対値がEPS以上
 */
inline bool tolerant_eq(double a, double b){ return abs(a - b) < EPS; }

/**
 *  @}
 */

}

