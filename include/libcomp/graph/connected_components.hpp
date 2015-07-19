/**
 *  @file libcomp/graph/connected_components.hpp
 */
#pragma once
#include <vector>
#include <queue>
#include <utility>
#include "libcomp/graph/adjacency_list.hpp"
#include "libcomp/misc/coordinate_compression.hpp"

namespace lc {

/**
 *  @defgroup connected_components Connected components
 *  @brief    連結成分分解
 *  @ingroup  graph
 *  @{
 */

template <typename EdgeType>
class ConnectedComponents {

public:
	typedef lc::AdjacencyList<EdgeType> GraphType;

private:
	std::vector<std::pair<int, int>> m_compress_table;
	std::vector<std::vector<int>> m_decompress_table;
	std::vector<GraphType> m_contracted_graphs;

public:
	ConnectedComponents()
		: m_compress_table()
		, m_decompress_table()
		, m_contracted_graphs()
	{ }

	ConnectedComponents(const GraphType &graph)
		: m_compress_table(graph.size(), std::make_pair(-1, -1))
		, m_decompress_table()
		, m_contracted_graphs()
	{
		const int n = graph.size();
		int current_component = 0;
		for(int root = 0; root < n; ++root){
			if(m_compress_table[root].first >= 0){ continue; }
			CoordinateCompressor<int> comp;
			std::queue<int> q;
			q.push(root);
			m_compress_table[root].first = current_component;
			while(!q.empty()){
				const int u = q.front();
				q.pop();
				comp.push(u);
				for(const auto &e : graph[u]){
					if(m_compress_table[e.to].first >= 0){ continue; }
					m_compress_table[e.to].first = current_component;
					q.push(e.to);
				}
			}
			const int m = comp.build();
			m_decompress_table.emplace_back(m);
			GraphType contracted(m);
			for(int cu = 0; cu < m; ++cu){
				const int u = comp.decompress(cu);
				m_compress_table[u].second = cu;
				m_decompress_table[current_component][cu] = u;
				for(const auto &e : graph[u]){
					const int v = e.to, cv = comp.compress(v);
					EdgeType f(e);
					f.to = cv;
					contracted.add_edge(cu, f);
				}
			}
			m_contracted_graphs.emplace_back(std::move(contracted));
			++current_component;
		}
	}

	int size() const { return m_contracted_graphs.size(); }

	const GraphType &contracted_graph(int component_id) const {
		return m_contracted_graphs[component_id];
	}

	int component_id(int v) const { return m_compress_table[v].first; }

	int local_vertex_id(int v) const { return m_compress_table[v].second; }

	int global_vertex_id(int component, int local_id) const {
		return m_decompress_table[component][local_id];
	}

};

template <typename EdgeType>
ConnectedComponents<EdgeType> connected_components(
	const AdjacencyList<EdgeType> &graph)
{
	return ConnectedComponents<EdgeType>(graph);
}

/**
 *  @}
 */

}

