/**
 *  @file libcomp/geometry/extract_polygons.hpp
 */
#pragma once
#include <vector>
#include <algorithm>
#include <limits>
#include <tuple>
#include "libcomp/graph/adjacency_list.hpp"
#include "libcomp/geometry/point.hpp"
#include "libcomp/geometry/polygon.hpp"

namespace lc {

/**
 *  @brief 点群と接続関係からの多角形列挙
 *
 *  点群pointsとその接続関係graphが与えられたとき、
 *  graphに含まれる線分からなる他の点を内部に含まない多角形を列挙する。
 *    - 時間計算量: \f$ O(|V|^2 \log{|V|}) \f$
 *
 *  @param[in] points  点群
 *  @param[in] graph   点同士の接続関係
 *  @return    求められた多角形の集合
 */
template <class EdgeType>
std::vector<Polygon> extract_polygons(
	const std::vector<Point> &points, const AdjacencyList<EdgeType> &graph)
{
	const int n = points.size();
	std::vector<std::vector<int>> next_table(n, std::vector<int>(n, -1));
	for(int i = 0; i < n; ++i){
		const int m = graph[i].size();
		std::vector<std::tuple<double, int>> args(m);
		for(int j = 0; j < m; ++j){
			const int v = graph[i][j].to;
			args[j] = std::make_tuple((points[v] - points[i]).arg(), v);
		}
		std::sort(args.begin(), args.end());
		for(int j = 0; j < m; ++j){
			const int k = std::get<1>(args[(j + 1) % m]);
			next_table[i][k] = std::get<1>(args[j]);
		}
	}
	std::vector<std::vector<bool>> done(n, std::vector<bool>(n));
	std::vector<Polygon> result;
	for(int u = 0; u < n; ++u){
		for(const auto &e : graph[u]){
			const int v = e.to;
			if(done[u][v]){ continue; }
			int cur = v, prev = u;
			std::vector<Point> buffer;
			do {
				buffer.push_back(points[cur]);
				done[prev][cur] = true;
				const int next = next_table[cur][prev];
				prev = cur;
				cur = next;
			} while(cur != v);
			const Polygon poly(buffer.begin(), buffer.end());
			if(poly.area() > 0.0){
				result.emplace_back(poly);
			}
		}
	}
	return result;
}

}

