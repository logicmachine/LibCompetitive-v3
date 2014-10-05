#ifndef MINIFIER_MINIFIER_HPP
#define MINIFIER_MINIFIER_HPP

#include "DependencyAnalyzer.hpp"

class Minifier {

public:
	Minifier(){ }
	int Minify(clang::tooling::ClangTool &tool, const DependencyAnalyzer &dep);

};

#endif

