/**
 *  @file libcomp/math/bit_tools.hpp
 */
#pragma once
#include <cstdint>

namespace lc {

/**
 *  @defgroup bit_tools Bit manipulation tools
 *  @brief    ビット演算を用いたトリック各種
 *  @ingroup  math
 *  @{
 */

/**
 *  @brief 次の2冪への切り下げ
 *  @param[in] x  切り下げる前の値
 *  @return    切り下げた後の値
 */
template <typename T>
inline T flp2(T x){
	if(x == 0){ return 0; }
	return (1u << (31 - __builtin_clz(x)));
}
/**
 *  @brief 次の2冪への切り下げ
 *  @param[in] x  切り下げる前の値
 *  @return    切り下げた後の値
 */
template <>
inline int64_t flp2<int64_t>(int64_t x){
	if(x == 0){ return 0; }
	return (1ull << (63 - __builtin_clzll(x)));
}
/**
 *  @brief 次の2冪への切り下げ
 *  @param[in] x  切り下げる前の値
 *  @return    切り下げた後の値
 */
template <>
inline uint64_t flp2<uint64_t>(uint64_t x){
	if(x == 0){ return 0; }
	return (1ull << (63 - __builtin_clzll(x)));
}

/**
 *  @brief 次の2冪への切り上げ
 *  @param[in] x  切り上げる前の値
 *  @return    切り上げた後の値
 */
template <typename T>
inline T clp2(T x){
	if(x <= 1){ return x; }
	return (1u << (31 - (__builtin_clz(x - 1) - 1)));
}
/**
 *  @brief 次の2冪への切り上げ
 *  @param[in] x  切り上げる前の値
 *  @return    切り上げた後の値
 */
template <>
inline int64_t clp2(int64_t x){
	if(x <= 1){ return x; }
	return (1ull << (63 - (__builtin_clzll(x - 1) - 1)));
}
/**
 *  @brief 次の2冪への切り上げ
 *  @param[in] x  切り上げる前の値
 *  @return    切り上げた後の値
 */
template <>
inline uint64_t clp2(uint64_t x){
	if(x <= 1){ return x; }
	return (1ull << (63 - (__builtin_clzll(x - 1) - 1)));
}

/**
 *  @}
 */

}

