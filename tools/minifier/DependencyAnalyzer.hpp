#ifndef MINIFIER_DEPENDENCY_ANALYZER_HPP
#define MINIFIER_DEPENDENCY_ANALYZER_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <utility>

class DependencyAnalyzer {

private:
	std::unordered_map<std::string, std::vector<std::pair<int, int>>> m_unused_ranges;

public:
	DependencyAnalyzer(){ }
	int Analyze(clang::tooling::ClangTool &tool);
	const std::unordered_map<
		std::string, std::vector<std::pair<int, int>>> &UnusedRanges() const
	{
		return m_unused_ranges;
	}

};

#endif

