#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Analysis/CallGraph.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/ReplacementsYaml.h"
#include "clang/Tooling/Refactoring.h"
#include "llvm/ADT/PostOrderIterator.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/Program.h"
#include "llvm/Support/SystemUtils.h"
#include "DependencyAnalyzer.hpp"
#include "Minifier.hpp"

static llvm::cl::OptionCategory MinifierCategory("Minifier options");
static llvm::cl::opt<std::string> SourcePath(
	llvm::cl::Positional, llvm::cl::desc("<source>"),
	llvm::cl::Required, llvm::cl::cat(MinifierCategory));
static llvm::cl::list<std::string> Includes(
	"include", llvm::cl::desc("Specify additional include directories"),
	llvm::cl::Optional, llvm::cl::cat(MinifierCategory));

int main(int argc, const char **argv) {
	llvm::StringMap<llvm::cl::Option *> Options;
	llvm::cl::getRegisteredOptions(Options);
	for(const auto &opt : Options){
		if(opt.first() == "help"){ continue; }
		if(opt.first() == "help-hidden"){ continue; }
		if(opt.first() == "version"){ continue; }
		if(opt.second->Category->getName() != MinifierCategory.getName()){
			opt.second->setHiddenFlag(llvm::cl::ReallyHidden);
		}
	}

	llvm::cl::ParseCommandLineOptions(argc, argv);
	std::vector<std::string> options;
	options.emplace_back("-std=c++11");
	for(const auto &include : Includes){
		options.push_back(std::string("-I") + include);
	}
	std::unique_ptr<clang::tooling::CompilationDatabase> compilations(
		new clang::tooling::FixedCompilationDatabase(".", options));

	clang::tooling::ClangTool tool(*compilations, SourcePath);
	tool.setArgumentsAdjuster(new clang::tooling::ClangSyntaxOnlyAdjuster());
	DependencyAnalyzer analyzer;
	analyzer.Analyze(tool);
	Minifier minifier;
	minifier.Minify(tool, analyzer);
	return 0;
}
