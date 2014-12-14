/**
 *  @file libcomp/geometry/debug.hpp
 */
#pragma once
#include <iostream>
#include "libcomp/geometry/point.hpp"
#include "libcomp/geometry/line.hpp"
#include "libcomp/geometry/segment.hpp"
#include "libcomp/geometry/circle.hpp"
#include "libcomp/geometry/arc.hpp"

std::ostream &operator<<(std::ostream &os, const lc::Point &p){
	os << "(" << p.x << ", " << p.y << ")";
	return os;
}
std::ostream &operator<<(std::ostream &os, const lc::Line &l){
	os << "(" << l.a << ", " << l.b << ")";
	return os;
}
std::ostream &operator<<(std::ostream &os, const lc::Segment &s){
	os << "(" << s.a << ", " << s.b << ")";
	return os;
}
std::ostream &operator<<(std::ostream &os, const lc::Circle &c){
	os << "(" << c.c << ", " << c.r << ")";
	return os;
}

