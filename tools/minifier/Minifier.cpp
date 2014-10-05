#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Lex/Preprocessor.h"
#include "Minifier.hpp"
#include <fstream>
#include <stack>
#include <utility>

class IncludeHandler : public clang::PPCallbacks {
private:
	clang::SourceManager &m_source_manager;
	std::set<std::string> m_included_files;
	std::set<std::string> m_passed_files;
public:
	explicit IncludeHandler(clang::SourceManager &sm)
		: m_source_manager(sm)
	{ }
	virtual void InclusionDirective(
		clang::SourceLocation hash_loc, const clang::Token &include_tok,
		clang::StringRef filename, bool is_angled,
		clang::CharSourceRange filename_range, const clang::FileEntry *file,
		clang::StringRef search_path, clang::StringRef relative_path,
		const clang::Module *imported)
	{
		if(!is_angled){ return; }
		m_passed_files.insert(file->getName());
		std::string from = m_source_manager.getFilename(hash_loc);
		if(m_passed_files.find(from) == m_passed_files.end()){
			m_included_files.insert(filename);
		}
	}
	const std::set<std::string> &IncludedFiles() const {
		return m_included_files;
	}
	const std::set<std::string> &SkippedFiles() const {
		return m_passed_files;
	}
};

class TextFile {
private:
	std::vector<std::string> m_lines;
	std::vector<int> m_removal_flags;
public:
	TextFile() : m_lines(), m_removal_flags() { }
	template <typename Iterator>
	TextFile(const std::string &filename, Iterator first, Iterator last)
		: m_lines()
		, m_removal_flags()
	{
		std::ifstream ifs(filename.c_str());
		std::string line;
		while(std::getline(ifs, line)){ m_lines.push_back(line); }
		const int n = m_lines.size() + 1;
		m_removal_flags.resize(n + 1);
		for(auto it = first; it != last; ++it){
			++m_removal_flags[it->first];
			--m_removal_flags[it->second+ 1];
		}
		for(int i = 0, s = 0; i <= n; ++i){
			s += m_removal_flags[i];
			m_removal_flags[i] = s;
		}
	}
	const std::string &operator[](int i) const { return m_lines[i - 1]; }
	bool is_removed(int i) const { return m_removal_flags[i] != 0; }
	void remove(int i){ m_removal_flags[i] = 1; }
};

class MinifyAction : public clang::PreprocessorFrontendAction {
private:
	const DependencyAnalyzer &m_dependency_analyzer;
public:
	explicit MinifyAction(const DependencyAnalyzer &dep)
		: clang::PreprocessorFrontendAction()
		, m_dependency_analyzer(dep)
	{ }
	virtual void ExecuteAction(){
		const DependencyAnalyzer &dep = m_dependency_analyzer;
		clang::Preprocessor &pp = getCompilerInstance().getPreprocessor();
		clang::SourceManager &sm = getCompilerInstance().getSourceManager();
		IncludeHandler *include_handler = new IncludeHandler(sm);
		pp.addPPCallbacks(include_handler);
		clang::Token tok;
		pp.EnterMainSourceFile();
		std::string prev_file = "";
		int prev_line = -1, skip_flag = 1;
		std::vector<std::string> output_buffer;
		std::unordered_map<std::string, TextFile> loaded_files;
		do {
			pp.Lex(tok);
			const clang::SourceLocation loc = tok.getLocation();
			std::string cur_file = sm.getFilename(loc);
			const int cur_line = sm.getPresumedLineNumber(loc);
			if(tok.isNot(clang::tok::eof) && cur_file != prev_file){
				if(cur_file == "" || include_handler->SkippedFiles().count(cur_file) > 0){
					skip_flag = 1;
				}else{
					skip_flag = 0;
					if(loaded_files.count(cur_file) == 0){
						std::vector<std::pair<int, int>> ranges;
						if(dep.UnusedRanges().count(cur_file) > 0){
							ranges = dep.UnusedRanges().find(cur_file)->second;
						}
						loaded_files.insert(std::make_pair(
							cur_file, TextFile(cur_file, ranges.begin(), ranges.end())));
					}
				}
				if(skip_flag == 0 && !loaded_files[cur_file].is_removed(cur_line)){
					const auto it = loaded_files.find(cur_file);
					if(it != loaded_files.end()){
						it->second.remove(cur_line);
						output_buffer.push_back(it->second[cur_line]);
					}
				}
			}else if(tok.isNot(clang::tok::eof) && cur_line != prev_line && skip_flag == 0){
				if(skip_flag == 0 && !loaded_files[cur_file].is_removed(cur_line)){
					const auto it = loaded_files.find(cur_file);
					if(it != loaded_files.end()){
						it->second.remove(cur_line);
						output_buffer.push_back(it->second[cur_line]);
					}
				}
			}
			cur_file.swap(prev_file);
			prev_line = cur_line;
		} while(tok.isNot(clang::tok::eof));
		for(const auto &s : include_handler->IncludedFiles()){
			llvm::outs() << "#include <" << s << ">\n";
		}
		for(const auto &s : output_buffer){
			llvm::outs() << s << "\n";
		}
	}
};

class MinifyActionFactory : public clang::tooling::FrontendActionFactory {
private:
	const DependencyAnalyzer &m_dependency_analyzer;
public:
	MinifyActionFactory(const DependencyAnalyzer &dep)
		: clang::tooling::FrontendActionFactory()
		, m_dependency_analyzer(dep)
	{ }
	virtual clang::FrontendAction *create(){
		return new MinifyAction(m_dependency_analyzer);
	}
};

int Minifier::Minify(
	clang::tooling::ClangTool &tool, const DependencyAnalyzer &dep)
{
	MinifyActionFactory factory(dep);
	return tool.run(&factory);
}

