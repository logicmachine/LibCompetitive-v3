#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Analysis/CallGraph.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/ADT/PostOrderIterator.h"
#include "llvm/ADT/SmallString.h"
#include "DependencyAnalyzer.hpp"
#include <unordered_set>

class DeclarationRange {
private:
	std::string m_source_file;
	int m_first;
	int m_last;
public:
	DeclarationRange()
		: m_source_file()
		, m_first(0)
		, m_last(0)
	{ }
	DeclarationRange(const std::string &file, int first, int last)
		: m_source_file(file)
		, m_first(first)
		, m_last(last)
	{ }
	bool operator==(const DeclarationRange &rhs) const {
		return
			m_source_file == rhs.m_source_file &&
			m_first == rhs.m_first && m_last == rhs.m_last;
	}
	const std::string &getSourceFile() const { return m_source_file; }
	int getFirstLine() const { return m_first; }
	int getLastLine() const { return m_last; }
};

class DeclarationRangeHash {
public:
	size_t operator()(const DeclarationRange &r) const {
		return
			std::hash<std::string>()(r.getSourceFile()) ^
			static_cast<size_t>(r.getFirstLine()) * 1000000007 ^
			static_cast<size_t>(r.getLastLine()) * 1000000009;
	}
};

class CallGraphConsumer : public clang::ASTConsumer {
private:
	std::unordered_set<DeclarationRange, DeclarationRangeHash> m_keeping_ranges;
	std::unordered_set<DeclarationRange, DeclarationRangeHash> m_removing_ranges;
	std::unordered_map<std::string, std::vector<std::pair<int, int>>> &m_result_buffer;

	DeclarationRange ConvertRange(
		const clang::SourceRange &range, const clang::SourceManager &sm)
	{
		return DeclarationRange(
			sm.getFilename(range.getBegin()),
			sm.getPresumedLineNumber(range.getBegin()),
			sm.getPresumedLineNumber(range.getEnd()));
	}
	void AddKeepingRange(
		const clang::SourceRange &range, const clang::SourceManager &sm)
	{
		m_keeping_ranges.insert(ConvertRange(range, sm));
	}
	void TraverseRecordDecl(
		const clang::RecordDecl *decl, const clang::SourceManager &sm,
		llvm::DenseSet<const clang::Decl *> &passed_decl,
		llvm::DenseSet<const clang::Type *> &passed_type)
	{
		const clang::DeclContext *context =
			clang::dyn_cast<clang::DeclContext>(decl);
		for(auto it = context->decls_begin(); it != context->decls_end(); ++it){
			if(clang::isa<clang::FieldDecl>(*it)){
				const clang::FieldDecl *field_decl =
					clang::dyn_cast<clang::FieldDecl>(*it);
				if(field_decl->getTypeSourceInfo() == nullptr){ continue; }
				TraverseType(
					field_decl->getType().getTypePtr(),
					sm, passed_decl, passed_type);
			}
		}
	}
	void TraverseType(
		const clang::Type *type, const clang::SourceManager &sm,
		llvm::DenseSet<const clang::Decl *> &passed_decl,
		llvm::DenseSet<const clang::Type *> &passed_type)
	{
		if(!passed_type.insert(type).second){ return; }
		if(clang::isa<clang::BuiltinType>(type)){
			// nothing to do
		}else if(clang::isa<clang::AutoType>(type)){
			// nothing to do
		}else if(clang::isa<clang::PointerType>(type)){
			const clang::PointerType *pointer_type =
				clang::dyn_cast<clang::PointerType>(type);
			TraverseType(
				pointer_type->getPointeeType().getTypePtr(),
				sm, passed_decl, passed_type);
		}else if(clang::isa<clang::ReferenceType>(type)){
			const clang::ReferenceType *reference_type =
				clang::dyn_cast<clang::ReferenceType>(type);
			TraverseType(
				reference_type->getPointeeType().getTypePtr(),
				sm, passed_decl, passed_type);
		}else if(clang::isa<clang::ArrayType>(type)){
			const clang::ArrayType *array_type =
				clang::dyn_cast<clang::ArrayType>(type);
			TraverseType(
				array_type->getElementType().getTypePtr(),
				sm, passed_decl, passed_type);
		}else if(clang::isa<clang::TypedefType>(type)){
			const clang::TypedefType *typedef_type =
				clang::dyn_cast<clang::TypedefType>(type);
			const clang::TypeSourceInfo *tsinfo =
				typedef_type->getDecl()->getTypeSourceInfo();
			AddKeepingRange(tsinfo->getTypeLoc().getSourceRange(), sm);
			TraverseType(
				typedef_type->desugar().getTypePtr(),
				sm, passed_decl, passed_type);
		}else if(clang::isa<clang::EnumType>(type)){
			// TODO
		}else if(clang::isa<clang::RecordType>(type)){
			const clang::RecordType *record_type =
				clang::dyn_cast<clang::RecordType>(type);
			AddKeepingRange(record_type->getDecl()->getSourceRange(), sm);
			TraverseRecordDecl(
				record_type->getDecl(), sm, passed_decl, passed_type);
			if(clang::isa<clang::CXXRecordDecl>(record_type->getDecl())){
				const clang::CXXRecordDecl *record_decl =
					clang::dyn_cast<clang::CXXRecordDecl>(record_type->getDecl());
				if(record_decl->isCompleteDefinition()){
					for(const auto &base : record_decl->bases()){
						const clang::Type *base_type = base.getType().getTypePtr();
						TraverseType(base_type, sm, passed_decl, passed_type);
					}
				}
			}
			if(clang::isa<clang::ClassTemplateSpecializationDecl>(record_type->getDecl())){
				const clang::ClassTemplateSpecializationDecl *record_decl =
					clang::dyn_cast<clang::ClassTemplateSpecializationDecl>(record_type->getDecl());
				const auto &template_args = record_decl->getTemplateInstantiationArgs();
				for(unsigned i = 0; i < template_args.size(); ++i){
					const auto &arg = template_args.get(i);
					if(arg.getKind() == clang::TemplateArgument::Type){
						TraverseType(
							arg.getAsType().getTypePtr(),
							sm, passed_decl, passed_type);
					}else{
						TraverseCallGraph(
							arg.getAsDecl(), sm, passed_decl, passed_type);
					}
				}
			}
		}else if(clang::isa<clang::FunctionType>(type)){
			// TODO
		}else if(clang::isa<clang::ElaboratedType>(type)){
			const clang::ElaboratedType *elaborated_type =
				clang::dyn_cast<clang::ElaboratedType>(type);
			if(elaborated_type->isSugared()){
				TraverseType(
					elaborated_type->desugar().getTypePtr(),
					sm, passed_decl, passed_type);
			}
		}else if(clang::isa<clang::TemplateSpecializationType>(type)){
			const clang::TemplateSpecializationType *specialization_type =
				clang::dyn_cast<clang::TemplateSpecializationType>(type);
			if(specialization_type->isSugared()){
				TraverseType(
					specialization_type->desugar().getTypePtr(),
					sm, passed_decl, passed_type);
			}
			for(auto it : *specialization_type){
				if(it.getKind() != clang::TemplateArgument::Type){ continue; }
				TraverseType(
					it.getAsType().getTypePtr(),
					sm, passed_decl, passed_type);
			}
		}else if(clang::isa<clang::SubstTemplateTypeParmType>(type)){
			const clang::SubstTemplateTypeParmType *subst_type =
				clang::dyn_cast<clang::SubstTemplateTypeParmType>(type);
			if(subst_type->isSugared()){
				TraverseType(
					subst_type->desugar().getTypePtr(),
					sm, passed_decl, passed_type);
			}
		}else{
			// llvm::errs() << "Unknown type class: " << type->getTypeClassName() << "\n";
		}
	}
	void TraverseGlobalVariables(
		const clang::TranslationUnitDecl *tu, const clang::SourceManager &sm,
		llvm::DenseSet<const clang::Decl *> &passed_decl,
		llvm::DenseSet<const clang::Type *> &passed_type)
	{
		for(auto it = tu->decls_begin(); it != tu->decls_end(); ++it){
			const clang::Decl *decl = *it;
			if(clang::isa<clang::VarDecl>(decl)){
				const clang::VarDecl *var_decl =
					clang::dyn_cast<clang::VarDecl>(decl);
				TraverseType(
					var_decl->getTypeSourceInfo()->getType().getTypePtr(),
					sm, passed_decl, passed_type);
			}
		}
	}
	template <bool TraverseCallExpr = false>
	void TraverseBody(
		const clang::Stmt *stmt, const clang::SourceManager &sm,
		llvm::DenseSet<const clang::Decl *> &passed_decl,
		llvm::DenseSet<const clang::Type *> &passed_type)
	{
		if(stmt == nullptr){ return; }
		if(clang::isa<clang::DeclStmt>(stmt)){
			const clang::DeclStmt *decl_stmt =
				clang::dyn_cast<clang::DeclStmt>(stmt);
			for(auto it = decl_stmt->decl_begin(); it != decl_stmt->decl_end(); ++it){
				const clang::Decl *decl = *it;
				if(clang::isa<clang::VarDecl>(*it)){
					const clang::VarDecl *var_decl =
						clang::dyn_cast<clang::VarDecl>(decl);
					TraverseType(
						var_decl->getTypeSourceInfo()->getType().getTypePtr(),
						sm, passed_decl, passed_type);
				}
			}
		}else if(TraverseCallExpr && clang::isa<clang::CallExpr>(stmt)){
			const clang::CallExpr *call_expr =
				clang::dyn_cast<clang::CallExpr>(stmt);
			TraverseCallGraph(call_expr->getCalleeDecl(), sm, passed_decl, passed_type);
		}
		for(auto it = stmt->child_begin(); it != stmt->child_end(); ++it){
			TraverseBody(*it, sm, passed_decl, passed_type);
		}
	}
	void TraverseCallGraph(
		const clang::Decl *decl, const clang::SourceManager &sm,
		llvm::DenseSet<const clang::Decl *> &passed_decl,
		llvm::DenseSet<const clang::Type *> &passed_type,
		int depth = 0)
	{
		if(decl == nullptr){ return; }
		if(!passed_decl.insert(decl).second){ return; }
		if(decl->hasBody()){
			TraverseBody(decl->getBody(), sm, passed_decl, passed_type);
		}
		if(clang::isa<clang::VarDecl>(decl)){
			const clang::VarDecl *var_decl =
				clang::dyn_cast<clang::VarDecl>(decl);
			TraverseBody<true>(
				var_decl->getAnyInitializer(),
				sm, passed_decl, passed_type);
		}else if(clang::isa<clang::FunctionDecl>(decl)){
			const clang::FunctionDecl *function_decl =
				clang::dyn_cast<clang::FunctionDecl>(decl);
			AddKeepingRange(function_decl->getSourceRange(), sm);
			TraverseType(
				function_decl->getReturnType().getTypePtr(),
				sm, passed_decl, passed_type);
			for(auto var_decl : function_decl->parameters()){
				if(var_decl->getTypeSourceInfo() == nullptr){ continue; }
				TraverseType(
					var_decl->getTypeSourceInfo()->getType().getTypePtr(),
					sm, passed_decl, passed_type);
			}
			const auto *template_args =
				function_decl->getTemplateSpecializationArgs();
			if(template_args != nullptr){
				for(unsigned i = 0; i < template_args->size(); ++i){
					const auto &arg = template_args->get(i);
					if(arg.getKind() == clang::TemplateArgument::Type){
						TraverseType(
							arg.getAsType().getTypePtr(),
							sm, passed_decl, passed_type);
					}
				}
			}
		}else if(clang::isa<clang::NonTypeTemplateParmDecl>(decl)){
			const clang::NonTypeTemplateParmDecl *param_decl =
				clang::dyn_cast<clang::NonTypeTemplateParmDecl>(decl);
			if(param_decl->hasDefaultArgument()){
				TraverseBody(
					param_decl->getDefaultArgument(),
					sm, passed_decl, passed_type);
			}
		}
		clang::CallGraph call_graph;
		call_graph.addToCallGraph(const_cast<clang::Decl *>(decl));
		llvm::ReversePostOrderTraversal<clang::CallGraph *> rpot(&call_graph);
		for(const auto &node : rpot){
			clang::Decl *p = node->getDecl();
			if(p == nullptr){ continue; }
			TraverseCallGraph(p, sm, passed_decl, passed_type, depth + 1);
		}
	}
	void TraverseASTBody(
		clang::Stmt *stmt, const clang::SourceManager &sm)
	{
		if(stmt == nullptr){ return; }
		if(clang::isa<clang::DeclStmt>(stmt)){
			clang::DeclStmt *decl_stmt =
				clang::dyn_cast<clang::DeclStmt>(stmt);
			for(auto it = decl_stmt->decl_begin(); it != decl_stmt->decl_end(); ++it){
				TraverseAST(*it, sm);
			}
		}
		for(auto it = stmt->child_begin(); it != stmt->child_end(); ++it){
			TraverseASTBody(*it, sm);
		}
	}
	bool TraverseAST(
		clang::Decl *decl, const clang::SourceManager &sm)
	{
		bool keep_flag = false;
		if(decl->isImplicit()){
			// nothing to do
		}else if(clang::isa<clang::CXXConstructorDecl>(decl)){
			// nothing to do
		}else if(clang::isa<clang::CXXDestructorDecl>(decl)){
			// nothing to do
		}else if(clang::isa<clang::FunctionDecl>(decl)){
			const DeclarationRange range =
				ConvertRange(decl->getSourceRange(), sm);
			if(m_keeping_ranges.find(range) == m_keeping_ranges.end()){
				m_removing_ranges.insert(range);
				return false;
			}else{
				keep_flag = true;
			}
		}else if(clang::isa<clang::RecordDecl>(decl)){
			const DeclarationRange range =
				ConvertRange(decl->getSourceRange(), sm);
			if(m_keeping_ranges.find(range) == m_keeping_ranges.end()){
				m_removing_ranges.insert(range);
				return false;
			}else{
				keep_flag = true;
			}
		}else if(clang::isa<clang::TemplateDecl>(decl)){
			const DeclarationRange range =
				ConvertRange(decl->getSourceRange(), sm);
			if(m_keeping_ranges.find(range) == m_keeping_ranges.end()){
				clang::TemplateDecl *template_decl =
					clang::dyn_cast<clang::TemplateDecl>(decl);
				const DeclarationRange child_range = ConvertRange(
					template_decl->getTemplatedDecl()->getSourceRange(), sm);
				if(m_keeping_ranges.find(child_range) == m_keeping_ranges.end()){
					m_removing_ranges.insert(range);
					return false;
				}else{
					keep_flag = true;
				}
			}else{
				keep_flag = true;
			}
		}
		if(decl->hasBody()){
			clang::Stmt *stmt = decl->getBody();
			TraverseASTBody(stmt, sm);
		}
		if(clang::isa<clang::DeclContext>(decl)){
			clang::DeclContext *context =
				clang::dyn_cast<clang::DeclContext>(decl);
			for(auto it = context->decls_begin(); it != context->decls_end(); ++it){
				if(TraverseAST(*it, sm)){ keep_flag = true; }
			}
		}
		if(clang::isa<clang::NamespaceDecl>(decl) && !keep_flag){
			const DeclarationRange range =
				ConvertRange(decl->getSourceRange(), sm);
			m_removing_ranges.insert(range);
		}
		return keep_flag;
	}
public:
	explicit CallGraphConsumer(
		std::unordered_map<std::string, std::vector<std::pair<int, int>>> &buffer)
		: clang::ASTConsumer()
		, m_keeping_ranges()
		, m_removing_ranges()
		, m_result_buffer(buffer)
	{ }
	virtual void HandleTranslationUnit(clang::ASTContext &context){
		m_keeping_ranges.clear();
		m_removing_ranges.clear();
		const clang::SourceManager &sm = context.getSourceManager();
		clang::TranslationUnitDecl *tu = context.getTranslationUnitDecl();
		llvm::DenseSet<const clang::Type *> type_passed;
		llvm::DenseSet<const clang::Decl *> decl_passed;
		TraverseGlobalVariables(tu, sm, decl_passed, type_passed);
		for(auto it = tu->decls_begin(); it != tu->decls_end(); ++it){
			if(clang::isa<clang::VarDecl>(*it)){
				TraverseCallGraph(*it, sm, decl_passed, type_passed);
			}else if(clang::isa<clang::FunctionDecl>(*it)){
				clang::FunctionDecl *function_decl =
					clang::dyn_cast<clang::FunctionDecl>(*it);
				if(function_decl->getNameInfo().getAsString() == "main"){
					TraverseCallGraph(*it, sm, decl_passed, type_passed);
				}
			}
		}
		TraverseAST(tu, sm);
		m_result_buffer.clear();
		for(const auto r : m_removing_ranges){
			const std::string file = r.getSourceFile();
			const int first = r.getFirstLine(), last = r.getLastLine();
			m_result_buffer[file].emplace_back(first, last);
		}
	}
};

class CallGraphAction : public clang::ASTFrontendAction {
private:
	std::unordered_map<std::string, std::vector<std::pair<int, int>>> &m_result_buffer;
public:
	explicit CallGraphAction(
		std::unordered_map<std::string, std::vector<std::pair<int, int>>> &buffer)
		: clang::ASTFrontendAction()
		, m_result_buffer(buffer)
	{ }
	virtual clang::ASTConsumer *CreateASTConsumer(
		clang::CompilerInstance &compiler, llvm::StringRef in_file)
	{
		return new CallGraphConsumer(m_result_buffer);
	}
};

class CallGraphActionFactory : public clang::tooling::FrontendActionFactory {
private:
	std::unordered_map<std::string, std::vector<std::pair<int, int>>> &m_result_buffer;
public:
	explicit CallGraphActionFactory(
		std::unordered_map<std::string, std::vector<std::pair<int, int>>> &buffer)
		: clang::tooling::FrontendActionFactory()
		, m_result_buffer(buffer)
	{ }
	virtual clang::FrontendAction *create(){
		return new CallGraphAction(m_result_buffer);
	}
};

int DependencyAnalyzer::Analyze(clang::tooling::ClangTool &tool){
	CallGraphActionFactory factory(m_unused_ranges);
	return tool.run(&factory);
}

