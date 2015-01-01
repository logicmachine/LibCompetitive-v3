#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_set>
#include <tuple>
#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Lex/Preprocessor.h"
#include "removal_table.hpp"

class IncludeHandler : public clang::PPCallbacks {
private:
	clang::SourceManager &m_source_manager;
	std::unordered_set<std::string> m_included_files;
	std::unordered_set<std::string> m_skipped_files;
public:
	explicit IncludeHandler(clang::SourceManager &sm)
		: m_source_manager(sm)
	{ }
	virtual void InclusionDirective(
		clang::SourceLocation hash_loc,
		const clang::Token &include_tok,
		clang::StringRef filename,
		bool is_angled,
		clang::CharSourceRange filename_range,
		const clang::FileEntry *file,
		clang::StringRef search_path,
		clang::StringRef relative_path,
		const clang::Module *imported)
	{
		if(!is_angled){ return; }
		m_skipped_files.insert(file->getName());
		const auto from = m_source_manager.getFilename(hash_loc);
		if(m_skipped_files.find(from) == m_skipped_files.end()){
			m_included_files.insert(filename);
		}
	}
	const std::unordered_set<std::string> &IncludedFiles() const {
		return m_included_files;
	}
	const std::unordered_set<std::string> &SkippedFiles() const {
		return m_skipped_files;
	}
};

class TextFile {
private:
	std::vector<std::string> m_lines;
	std::vector<bool> m_remove_flags;
public:
	TextFile()
		: m_lines()
		, m_remove_flags()
	{ }
	TextFile(const std::string &filename, const std::vector<bool> &remove_flags)
		: m_lines()
		, m_remove_flags(remove_flags)
	{
		std::ifstream ifs(filename.c_str());
		std::string line;
		while(std::getline(ifs, line)){ m_lines.push_back(line); }
		const int n = m_lines.size() + 1;
		m_remove_flags.resize(n + 1);
	}
	const std::string &operator[](int i) const { return m_lines[i - 1]; }
	bool is_removed(int i) const { return m_remove_flags[i]; }
	void remove(int i){ m_remove_flags[i] = true; }
};

template <typename OutputStream>
class MinifyAction : public clang::PreprocessorFrontendAction {
private:
	OutputStream &m_ostream;
	const RemovalTable &m_table;
public:
	MinifyAction(OutputStream &os, const RemovalTable &table)
		: clang::PreprocessorFrontendAction()
		, m_ostream(os)
		, m_table(table)
	{ }
	virtual void ExecuteAction(){
		clang::Preprocessor &pp = getCompilerInstance().getPreprocessor();
		clang::SourceManager &sm = getCompilerInstance().getSourceManager();
		IncludeHandler *include_handler = new IncludeHandler(sm);
		pp.addPPCallbacks(include_handler);
		clang::Token tok;
		pp.EnterMainSourceFile();
		std::unordered_map<std::string, TextFile> file_table;
		std::vector<std::string> output_buffer;
		std::string prev_file = "";
		int prev_line = -1, skip_flag = 1;
		for(;;){
			pp.Lex(tok);
			if(tok.is(clang::tok::eof)){ break; }
			const clang::SourceLocation loc = tok.getLocation();
			const std::string cur_file = sm.getFilename(loc);
			const int cur_line = sm.getPresumedLineNumber(loc);
			if(cur_file != prev_file){
				if(cur_file == ""){
					skip_flag = 1;
				}else if(include_handler->SkippedFiles().count(cur_file)){
					skip_flag = 1;
				}else{
					skip_flag = 0;
					if(!file_table.count(cur_file)){
						file_table.insert(std::make_pair(
							cur_file, TextFile(cur_file, m_table[cur_file])));
					}
				}
				auto it = file_table.find(cur_file);
				if(skip_flag == 0 && it != file_table.end()){
					if(!it->second.is_removed(cur_line)){
						it->second.remove(cur_line);
						output_buffer.push_back(it->second[cur_line]);
					}
				}
			}else if(cur_line != prev_line && !skip_flag){
				auto it = file_table.find(cur_file);
				if(skip_flag == 0 && it != file_table.end()){
					if(!it->second.is_removed(cur_line)){
						it->second.remove(cur_line);
						output_buffer.push_back(it->second[cur_line]);
					}
				}
			}
			prev_file = cur_file;
			prev_line = cur_line;
		}
		for(const auto &s : include_handler->IncludedFiles()){
			m_ostream << "#include <" << s << ">" << std::endl;
		}
		for(const auto &s : output_buffer){
			m_ostream << s << std::endl;
		}
	}
};

template <typename OutputStream>
class MinifyActionFactory : public clang::tooling::FrontendActionFactory {
private:
	OutputStream &m_ostream;
	const RemovalTable &m_table;
public:
	MinifyActionFactory(OutputStream &os, const RemovalTable &table)
		: clang::tooling::FrontendActionFactory()
		, m_ostream(os)
		, m_table(table)
	{ }
	virtual clang::FrontendAction *create(){
		return new MinifyAction<OutputStream>(m_ostream, m_table);
	}
};

template <typename OutputStream>
int minify(
	OutputStream &os, clang::tooling::ClangTool &tool,
	const RemovalTable &table)
{
	MinifyActionFactory<OutputStream> factory(os, table);
	return tool.run(&factory);
}

