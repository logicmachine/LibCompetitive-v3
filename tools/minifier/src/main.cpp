#include "clang/Frontend/CompilerInstance.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"
#include "dependency_analyzer.hpp"
#include "minifier.hpp"

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
	const auto removal = analyze_dependency(tool);
	return minify(std::cout, tool, removal);
}
