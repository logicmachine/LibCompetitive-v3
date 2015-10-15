/**
 *  @file libcomp/lang/io.hpp
 */
#pragma once
#include <iostream>
#include <tuple>
#include <vector>
#include <list>
#include <deque>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>

/**
 *  @defgroup input_output_helper Input/Output helper functions
 *  @brief    入出力ヘルパー
 *  @ingroup  lang
 *  @{
 */

namespace lc {

template <typename T, std::size_t I = std::tuple_size<T>::value>
struct tuple_reader {
	static std::istream &read(std::istream &is, T &x){
		is >> std::get<std::tuple_size<T>::value - I>(x);
		return tuple_reader<T, I - 1>::read(is, x);
	}
};
template <typename T>
struct tuple_reader<T, 0> {
	static std::istream &read(std::istream &is, T &){ return is; }
};

template <typename Iterator>
std::istream &input_range(std::istream &is, Iterator begin, Iterator end){
	for(Iterator it = begin; it != end; ++it){ is >> *it; }
	return is;
}

template <typename T, std::size_t I = std::tuple_size<T>::value>
struct tuple_writer {
	static std::ostream &write(std::ostream &os, const T &x){
		os << std::get<std::tuple_size<T>::value - I>(x) << (I > 1 ? "," : "");
		return tuple_writer<T, I - 1>::write(os, x);
	}
};
template <typename T>
struct tuple_writer<T, 0> {
	static std::ostream &write(std::ostream &os, const T &){ return os; }
};

template <typename Iterator>
std::ostream &output_range(std::ostream &os, Iterator begin, Iterator end){
	for(Iterator it = begin; it != end; ++it){
		if(it != begin){ os << " "; }
		os << *it;
	}
	return os;
}

}

template <typename T1, typename T2>
inline std::istream &operator>>(std::istream &is, std::pair<T1, T2> &x){
	return is >> x.first >> x.second;
}

template <typename... Ts>
inline std::istream &operator>>(std::istream &is, std::tuple<Ts...> &x){
	return lc::tuple_reader<std::tuple<Ts...>>::read(is, x);
}

template <typename T>
inline std::istream &operator>>(std::istream &is, std::vector<T> &x){
	return lc::input_range(is, x.begin(), x.end());
}

template <typename T>
inline std::istream &operator>>(std::istream &is, std::list<T> &x){
	return lc::input_range(is, x.begin(), x.end());
}

template <typename T>
inline std::istream &operator>>(std::istream &is, std::deque<T> &x){
	return lc::input_range(is, x);
}


template <typename T1, typename T2>
inline std::ostream &operator<<(std::ostream &os, const std::pair<T1, T2> &x){
	return os << "(" << x.first << "," << x.second << ")";
}

template <typename... Ts>
inline std::ostream &operator<<(std::ostream &os, const std::tuple<Ts...> &x){
	os << "(";
	lc::tuple_writer<std::tuple<Ts...>>::write(os, x);
	return os << ")";
}

template <typename T>
inline std::ostream &operator<<(std::ostream &os, const std::vector<T> &x){
	return lc::output_range(os, x.begin(), x.end());
}

template <typename T>
inline std::ostream &operator<<(std::ostream &os, const std::list<T> &x){
	return lc::output_range(os, x.begin(), x.end());
}

template <typename T>
inline std::ostream &operator<<(std::ostream &os, const std::deque<T> &x){
	return lc::output_range(os, x.begin(), x.end());
}

template <typename T>
inline std::ostream &operator<<(std::ostream &os, const std::set<T> &x){
	return lc::output_range(os, x.begin(), x.end());
}

template <typename T1, typename T2>
inline std::ostream &operator<<(std::ostream &os, const std::map<T1, T2> &x){
	return lc::output_range(os, x.begin(), x.end());
}

template <typename T>
inline std::ostream &operator<<(
	std::ostream &os, const std::unordered_set<T> &x)
{
	return lc::output_range(os, x.begin(), x.end());
}

template <typename T1, typename T2>
inline std::ostream &operator<<(
	std::ostream &os, const std::unordered_map<T1, T2> &x)
{
	return lc::output_range(os, x.begin(), x.end());
}

/**
 *  @}
 */

