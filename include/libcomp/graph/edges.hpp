/**
 *  @file libcomp/graph/edges.hpp
 */
#pragma once

namespace lc {
struct Edge {
	int to;
	Edge() : to(0) { }
	explicit Edge(int to) : to(to) { }
};

template <class WeightType>
struct EdgeWithWeight {
	int to;
	WeightType weight;
	EdgeWithWeight() : to(0), weight() { }
	EdgeWithWeight(int to, const WeightType &weight)
		: to(to), weight(weight)
	{ }
};

template <class CapacityType>
struct EdgeWithCapacity {
	int to;
	CapacityType capacity;
	EdgeWithCapacity() : to(0), capacity() { }
	EdgeWithCapacity(int to, const CapacityType &capacity)
		: to(to), capacity(capacity)
	{ }
};

template <class WeightType, class CapacityType>
struct EdgeWithWeightAndCapacity {
	int to;
	WeightType weight;
	CapacityType capacity;
	EdgeWithWeightAndCapacity() : to(0), weight(), capacity() { }
	EdgeWithWeightAndCapacity(
		int to, const WeightType &weight, const CapacityType &capacity)
		: to(to), weight(weight), capacity(capacity)
	{ }
};

}

