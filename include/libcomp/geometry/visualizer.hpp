#pragma once
#include <iomanip>
#include <fstream>
#include <map>
#include <cstdint>
#include "libcomp/geometry/point.hpp"
#include "libcomp/geometry/line.hpp"
#include "libcomp/geometry/segment.hpp"
#include "libcomp/geometry/circle.hpp"
#include "libcomp/geometry/polygon.hpp"

namespace lc {

class Visualizer {

public:
	struct Style {
		// Stroke
		std::string stroke_color;
		double stroke_opacity;
		std::string stroke_style;
		double stroke_width;
		// Fill
		std::string fill_color;
		double fill_opacity;

		Style()
			: stroke_color("#000000")
			, stroke_opacity(1.0)
			, stroke_style("solid")
			, stroke_width(1.0)
			, fill_color("#000000")
			, fill_opacity(0.0)
		{ }

		static Style stroke(const std::string &color, double opacity = 1.0){
			Style style;
			style.stroke_color = color;
			style.stroke_opacity = opacity;
			return style;
		}
		static Style fill(const std::string &color, double opacity = 1.0){
			Style style;
			style.stroke_color = color;
			style.fill_color = color;
			style.fill_opacity = opacity;
			return style;
		}
	};

private:
	struct Shape {
		std::string kind;
		std::vector<double> coords;
		double radius; // Circle
		Style style;
	};

	struct Layer {
		std::string name;
		std::vector<Shape> shapes;
	};
	typedef std::map<int, Layer> scene_type;

	std::string m_filename;
	scene_type m_scene;

	void dump_scene(std::ofstream &ofs, const scene_type &scene) const {
		ofs << "[";
		bool is_first = true;
		for(const auto &p : scene){
			if(!is_first){ ofs << ","; }
			is_first = false;
			dump_layer(ofs, p.second);
		}
		ofs << "]";
	}
	void dump_layer(std::ofstream &ofs, const Layer &layer) const {
		ofs << "{";
		ofs << "\"name\":\"" << layer.name << "\",";
		ofs << "\"shapes\":[";
		bool is_first = true;
		for(const auto &s : layer.shapes){
			if(!is_first){ ofs << ","; }
			is_first = false;
			dump_shape(ofs, s);
		}
		ofs << "]}";
	}
	void dump_shape(std::ofstream &ofs, const Shape &shape) const {
		ofs << "{";
		ofs << "\"kind\":\"" << shape.kind << "\"";
		ofs << ",\"coords\":[";
		bool is_first = true;
		for(const auto &x : shape.coords){
			if(!is_first){ ofs << ","; }
			is_first = false;
			ofs << x;
		}
		ofs << "]";
		if(shape.kind == "circle"){ ofs << ",\"radius\":" << shape.radius; }
		ofs << ",\"stroke_color\":\"" << shape.style.stroke_color << "\"";
		ofs << ",\"stroke_opacity\":" << shape.style.stroke_opacity;
		if(shape.kind != "point"){
			ofs << ",\"stroke_style\":\"" << shape.style.stroke_style << "\"";
			ofs << ",\"stroke_width\":" << shape.style.stroke_width;
		}
		if(shape.kind == "circle" || shape.kind == "polygon"){
			ofs << ",\"fill_color\":\"" << shape.style.fill_color << "\"";
			ofs << ",\"fill_opacity\":" << shape.style.fill_opacity;
		}
		ofs << "}";
	}

	Visualizer(const Visualizer &) = delete;
	Visualizer &operator=(const Visualizer &) = delete;

public:
	explicit Visualizer(const std::string &filename = std::string("out.json"))
		: m_filename(filename)
		, m_scene()
	{ }
	~Visualizer(){
		std::ofstream ofs(m_filename.c_str());
		dump_scene(ofs, m_scene);
	}

	void create_layer(int index, const std::string &name){
		m_scene[index].name = name;
	}

	void operator()(
		const Point &p, int layer = 0, const Style &style = Style())
	{
		Shape shape;
		shape.kind = "point";
		shape.coords.push_back(p.x);
		shape.coords.push_back(p.y);
		shape.style = style;
		m_scene[layer].shapes.push_back(shape);
	}
	void operator()(
		const Line &l, int layer = 0, const Style &style = Style())
	{
		Shape shape;
		shape.kind = "line";
		shape.coords.push_back(l.a.x);
		shape.coords.push_back(l.a.y);
		shape.coords.push_back(l.b.x);
		shape.coords.push_back(l.b.y);
		shape.style = style;
		m_scene[layer].shapes.push_back(shape);
	}
	void operator()(
		const Segment &s, int layer = 0, const Style &style = Style())
	{
		Shape shape;
		shape.kind = "segment";
		shape.coords.push_back(s.a.x);
		shape.coords.push_back(s.a.y);
		shape.coords.push_back(s.b.x);
		shape.coords.push_back(s.b.y);
		shape.style = style;
		m_scene[layer].shapes.push_back(shape);
	}
	void operator()(
		const Circle &c, int layer = 0, const Style &style = Style())
	{
		Shape shape;
		shape.kind = "circle";
		shape.coords.push_back(c.c.x);
		shape.coords.push_back(c.c.y);
		shape.radius = c.r;
		shape.style = style;
		m_scene[layer].shapes.push_back(shape);
	}
	void operator()(
		const Polygon &p, int layer = 0, const Style &style = Style())
	{
		Shape shape;
		shape.kind = "polygon";
		for(int i = 0; i < p.size(); ++i){
			shape.coords.push_back(p[i].x);
			shape.coords.push_back(p[i].y);
		}
		shape.style = style;
		m_scene[layer].shapes.push_back(shape);
	}

};

}

