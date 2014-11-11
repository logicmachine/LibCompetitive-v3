/**
 *  @file libcomp/misc/xorshift128.hpp
 */
#pragma once
#include <cstdint>

namespace lc {

/**
 *  @defgroup xorshift128 Xorshift128
 *  @ingroup  misc
 *  @{
 */

/**
 *  @brief  xorshift128 による乱数生成
 *
 *  \f$ [0, 2^{32}) \f$ の範囲の一様乱数を生成する。
 *    - 時間計算量: \f$ O(1) \f$
 *
 *  @return 生成された乱数
 */
inline unsigned int xorshift128(){
	static uint32_t x = 192479812;
	static uint32_t y = 784892731;
	static uint32_t z = 427398108;
	static uint32_t w = 48382934; 
	const uint32_t t = x ^ (x << 11);
	x = y; y = z; z = w;
	w = (w ^ (w >> 19)) ^ (t ^ (t >> 8)); 
	return w;
}

/**
 *  @}
 */

}

