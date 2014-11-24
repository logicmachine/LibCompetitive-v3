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

namespace lc {

class Visualizer {

private:
	double m_scale;
	double m_offset_x;
	double m_offset_y;

	double trans_x(double x) const { return x * m_scale + m_offset_x; }
	double trans_y(double y) const { return y * m_scale + m_offset_y; }

public:
	explicit Visualizer(
		double scale = 1.0, double offset_x = 0.0, double offset_y = 0.0)
		: m_scale(scale)
		, m_offset_x(offset_x)
		, m_offset_y(offset_y)
	{ }

	void operator()(const Point &p){
		std::cerr << "Point(";
		std::cerr << trans_x(p.x) << ", " << trans_y(p.y) << ");" << std::endl;
	}
	void operator()(const Line &l){
		std::cerr << "Line(";
		std::cerr << trans_x(l.a.x) << ", " << trans_y(l.a.y) << ", ";
		std::cerr << trans_x(l.b.x) << ", " << trans_y(l.b.y) << ");" << std::endl;
	}
	void operator()(const Segment &s){
		std::cerr << "Segment(";
		std::cerr << trans_x(s.a.x) << ", " << trans_y(s.a.y) << ", ";
		std::cerr << trans_x(s.b.x) << ", " << trans_y(s.b.y) << ");" << std::endl;
	}
	void operator()(const Circle &c){
		std::cerr << "Circle(";
		std::cerr << trans_x(c.c.x) << ", " << trans_y(c.c.y) << ", ";
		std::cerr << c.r * m_scale << ");" << std::endl;
	}
	void operator()(const Arc &a){
		std::cerr << "Arc(";
		std::cerr << trans_x(a.c.x) << ", " << trans_y(a.c.y) << ", ";
		std::cerr << a.r * m_scale << ", " << a.a << ", " << a.b << ");" << std::endl;
	}

};

}

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

