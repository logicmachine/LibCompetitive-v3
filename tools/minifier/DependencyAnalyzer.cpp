#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/ExprCXX.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/ADT/PostOrderIterator.h"
#include "llvm/ADT/SmallString.h"
#include "DependencyAnalyzer.hpp"
#include <unordered_set>

// #define DEBUG_TREE

class DeclarationRange {
private:
	std::string m_source_file;
	int m_first, m_last;
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
		if(m_source_file != rhs.m_source_file){ return false; }
		return m_first == rhs.m_first && m_last == rhs.m_last;
	}
	const std::string &getSourceFile() const { return m_source_file; }
	int getFirstLine() const { return m_first; }
	int getLastLine() const { return m_last; }
};

class DeclarationRangeHash {
public:
	size_t operator()(const DeclarationRange &r) const {
		const size_t a = std::hash<std::string>()(r.getSourceFile());
		const size_t b = r.getFirstLine(), c = r.getLastLine();
		return a ^ (b * 1000000007) ^ (c * 1000000009);
	}
};

class CallGraphConsumer : public clang::ASTConsumer {
private:
	llvm::DenseSet<const clang::Decl *> m_traversed_decls;
	llvm::DenseSet<const clang::Type *> m_traversed_types;
	llvm::DenseSet<const clang::Stmt *> m_traversed_stmts;
	llvm::DenseSet<const clang::Decl *> m_marked_decls;
	std::unordered_set<DeclarationRange, DeclarationRangeHash> m_keep_ranges;
	std::unordered_set<DeclarationRange, DeclarationRangeHash> m_remove_ranges;
	std::unordered_map<std::string, std::vector<std::pair<int, int>>> &m_result_buffer;

	DeclarationRange MakeRange(
		const clang::SourceRange &range, const clang::SourceManager &sm) const
	{
		return DeclarationRange(
			sm.getFilename(range.getBegin()),
			sm.getPresumedLineNumber(range.getBegin()),
			sm.getPresumedLineNumber(range.getEnd()));
	}
	void TraverseDecl(
		const clang::Decl *decl, const clang::SourceManager &sm, int depth = 0)
	{
		if(decl == nullptr){ return; }
		if(!m_traversed_decls.insert(decl).second){ return; }
#ifdef DEBUG_TREE
		llvm::errs() << std::string(depth * 2, ' ');
		if(clang::isa<clang::NamedDecl>(decl)){
			llvm::errs() << *clang::dyn_cast<clang::NamedDecl>(decl) << " ";
		}
		llvm::errs() << "(" << decl->getDeclKindName() << ")\n";
#endif
		m_marked_decls.insert(decl);
		if(clang::isa<clang::CXXConstructorDecl>(decl)){
			const auto ctor_decl = clang::dyn_cast<clang::CXXConstructorDecl>(decl);
			for(const auto &init : ctor_decl->inits()){
				TraverseStmt(init->getInit(), sm, depth + 1);
			}
		}
		if(clang::isa<clang::FunctionDecl>(decl)){
			const auto func_decl = clang::dyn_cast<clang::FunctionDecl>(decl);
			if(func_decl->hasBody()){
				TraverseStmt(func_decl->getBody(), sm, depth + 1);
			}
		}
		if(clang::isa<clang::VarDecl>(decl)){
			const auto var_decl = clang::dyn_cast<clang::VarDecl>(decl);
			if(var_decl->hasInit()){
				TraverseStmt(var_decl->getAnyInitializer(), sm, depth + 1);
			}
			TraverseType(var_decl->getType().getTypePtr(), sm, depth + 1);
			for(unsigned i = 0; i < var_decl->getNumTemplateParameterLists(); ++i){
				for(const auto param : *var_decl->getTemplateParameterList(i)){
					TraverseDecl(param, sm, depth + 1);
				}
			}
		}
		if(clang::isa<clang::FieldDecl>(decl)){
			const auto field_decl = clang::dyn_cast<clang::FieldDecl>(decl);
			TraverseType(field_decl->getType().getTypePtr(), sm, depth + 1);
		}
		if(clang::isa<clang::TypedefDecl>(decl)){
			const auto typedef_decl = clang::dyn_cast<clang::TypedefDecl>(decl);
			const auto tsinfo = typedef_decl->getTypeSourceInfo();
			TraverseType(tsinfo->getType().getTypePtr(), sm, depth + 1);
		}
	}
	void TraverseStmt(
		const clang::Stmt *stmt, const clang::SourceManager &sm, int depth = 0)
	{
		if(stmt == nullptr){ return; }
		if(!m_traversed_stmts.insert(stmt).second){ return; }
#ifdef DEBUG_TREE
		llvm::errs() << std::string(depth * 2, ' ');
		llvm::errs() << stmt->getStmtClassName() << "\n";
#endif
		if(clang::isa<clang::CallExpr>(stmt)){
			const auto call_expr = clang::dyn_cast<clang::CallExpr>(stmt);
			TraverseDecl(call_expr->getCalleeDecl(), sm, depth + 1);
		}
		if(clang::isa<clang::CXXConstructExpr>(stmt)){
			const auto construct_expr =
				clang::dyn_cast<clang::CXXConstructExpr>(stmt);
			TraverseDecl(construct_expr->getConstructor(), sm, depth + 1);
		}
		if(clang::isa<clang::DeclRefExpr>(stmt)){
			const auto declref_expr = clang::dyn_cast<clang::DeclRefExpr>(stmt);
			TraverseDecl(declref_expr->getDecl(), sm, depth + 1);
			const auto template_args = declref_expr->getTemplateArgs();
			for(unsigned i = 0; i < declref_expr->getNumTemplateArgs(); ++i){
				switch(template_args[i].getArgument().getKind()){
				case clang::TemplateArgument::Type:
					TraverseType(
						template_args[i].getTypeSourceInfo()->getType().getTypePtr(),
						sm, depth + 1);
					break;
				case clang::TemplateArgument::Expression:
					TraverseStmt(template_args[i].getSourceExpression(), sm, depth + 1);
					break;
				case clang::TemplateArgument::Declaration:
					TraverseStmt(template_args[i].getSourceDeclExpression(), sm, depth + 1);
					break;
				case clang::TemplateArgument::NullPtr:
					TraverseStmt(template_args[i].getSourceNullPtrExpression(), sm, depth + 1);
					break;
				case clang::TemplateArgument::Integral:
					TraverseStmt(template_args[i].getSourceIntegralExpression(), sm, depth + 1);
					break;
				default:
					break;
				}
			}
		}
		if(clang::isa<clang::DeclStmt>(stmt)){
			const auto decl_stmt = clang::dyn_cast<clang::DeclStmt>(stmt);
			for(const auto &child : decl_stmt->decls()){
				TraverseDecl(child, sm, depth + 1);
			}
		}
		for(const auto &child : stmt->children()){
			TraverseStmt(child, sm, depth + 1);
		}
	}
	void TraverseType(
		const clang::Type *type, const clang::SourceManager &sm, int depth = 0)
	{
		if(type == nullptr){ return; }
		if(!m_traversed_types.insert(type).second){ return; }
		if(clang::isa<clang::PointerType>(type)){
			const auto pointer_type = clang::dyn_cast<clang::PointerType>(type);
			TraverseType(pointer_type->getPointeeType().getTypePtr(), sm, depth + 1);
		}else if(clang::isa<clang::ReferenceType>(type)){
			const auto ref_type = clang::dyn_cast<clang::ReferenceType>(type);
			TraverseType(ref_type->getPointeeType().getTypePtr(), sm, depth + 1);
		}else if(clang::isa<clang::ArrayType>(type)){
			const auto array_type = clang::dyn_cast<clang::ArrayType>(type);
			TraverseType(array_type->getElementType().getTypePtr(), sm, depth + 1);
		}else if(clang::isa<clang::TypedefType>(type)){
			const auto typedef_type = clang::dyn_cast<clang::TypedefType>(type);
			TraverseDecl(typedef_type->getDecl(), sm, depth + 1);
		}else if(clang::isa<clang::RecordType>(type)){
			const auto record_type = clang::dyn_cast<clang::RecordType>(type);
			TraverseDecl(record_type->getDecl(), sm, depth + 1);
		}else if(clang::isa<clang::SubstTemplateTypeParmType>(type)){
			const auto sttp_type = clang::dyn_cast<clang::SubstTemplateTypeParmType>(type);
			TraverseType(sttp_type->getReplacementType().getTypePtr(), sm, depth + 1);
		}else if(clang::isa<clang::ElaboratedType>(type)){
			const auto el_type = clang::dyn_cast<clang::ElaboratedType>(type);
			TraverseType(el_type->getNamedType().getTypePtr(), sm, depth + 1);
		}else if(clang::isa<clang::TemplateSpecializationType>(type)){
			const auto ts_type = clang::dyn_cast<clang::TemplateSpecializationType>(type);
			for(const auto arg : *ts_type){
				if(arg.getKind() == clang::TemplateArgument::Type){
					TraverseType(arg.getAsType().getTypePtr(), sm, depth + 1);
				}
			}
		}
	}
	bool PropagateDecl(
		const clang::Decl *decl, const clang::SourceManager &sm, int depth = 0)
	{
		if(decl == nullptr){ return false; }
#ifdef DEBUG_TREE
		llvm::errs() << std::string(depth * 2, ' ');
		if(clang::isa<clang::NamedDecl>(decl)){
			llvm::errs() << *clang::dyn_cast<clang::NamedDecl>(decl) << " ";
		}
		llvm::errs() << "(" << decl->getDeclKindName() << ")\n";
#endif
		bool enabled = (m_marked_decls.find(decl) != m_marked_decls.end());
		if(clang::isa<clang::DeclContext>(decl)){
			const auto ctx = clang::dyn_cast<clang::DeclContext>(decl);
			for(const auto &child : ctx->decls()){
				if(PropagateDecl(child, sm, depth + 1)){ enabled = true; }
			}
		}
		if(clang::isa<clang::ClassTemplateDecl>(decl)){
			const auto ctdecl = clang::dyn_cast<clang::ClassTemplateDecl>(decl);
			for(const auto &spec : ctdecl->specializations()){
				if(PropagateDecl(spec, sm, depth + 1)){ enabled = true; }
			}
		}
		if(clang::isa<clang::FunctionTemplateDecl>(decl)){
			const auto ftdecl = clang::dyn_cast<clang::FunctionTemplateDecl>(decl);
			for(const auto &spec : ftdecl->specializations()){
				if(PropagateDecl(spec, sm, depth + 1)){ enabled = true; }
			}
		}
		if(enabled){ m_marked_decls.insert(decl); }
		if(enabled && clang::isa<clang::CXXRecordDecl>(decl)){
			const auto ctx = clang::dyn_cast<clang::DeclContext>(decl);
			if(enabled){
				const auto record_decl =
					clang::dyn_cast<clang::CXXRecordDecl>(decl);
				TraverseDecl(record_decl->getDestructor(), sm, depth + 1);
				for(const auto &ctor : record_decl->ctors()){
					TraverseDecl(ctor, sm, depth + 1);
				}
			}
		}
		if(enabled && clang::isa<clang::RecordDecl>(decl)){
			const auto ctx = clang::dyn_cast<clang::DeclContext>(decl);
			for(const auto &child : ctx->decls()){
				if(!clang::isa<clang::FunctionDecl>(child)){
					TraverseDecl(child, sm, depth + 1);
				}
			}
		}
		return enabled;
	}
	bool RemoveDecl(
		const clang::Decl *decl, const clang::SourceManager &sm, int depth = 0)
	{
		if(decl == nullptr){ return false; }
		bool enabled = (m_marked_decls.find(decl) != m_marked_decls.end());
		if(!enabled){
			DeclarationRange dr = MakeRange(decl->getSourceRange(), sm);
			if(m_keep_ranges.find(dr) != m_keep_ranges.end()){
				enabled = true;
			}else if(!decl->isImplicit()){
				m_remove_ranges.insert(dr);
			}
		}
		if(!enabled){ return false; }
		if(clang::isa<clang::DeclContext>(decl)){
			const auto ctx = clang::dyn_cast<clang::DeclContext>(decl);
			for(const auto &child : ctx->decls()){
				if(RemoveDecl(child, sm, depth + 1)){ enabled = true; }
			}
		}
		if(clang::isa<clang::RecordDecl>(decl)){
			const auto ctx = clang::dyn_cast<clang::DeclContext>(decl);
			for(const auto &child : ctx->decls()){
				if(clang::isa<clang::VarDecl>(child)){ TraverseDecl(child, sm); }
				if(clang::isa<clang::FieldDecl>(child)){ TraverseDecl(child, sm); }
			}
		}
		if(clang::isa<clang::ClassTemplateDecl>(decl)){
			const auto ctdecl = clang::dyn_cast<clang::ClassTemplateDecl>(decl);
			for(const auto &spec : ctdecl->specializations()){
				RemoveDecl(spec, sm, depth + 1);
			}
		}
		if(clang::isa<clang::FunctionTemplateDecl>(decl)){
			const auto ftdecl = clang::dyn_cast<clang::FunctionTemplateDecl>(decl);
			for(const auto &spec : ftdecl->specializations()){
				RemoveDecl(spec, sm, depth + 1);
			}
		}
		return true;
	}
public:
	explicit CallGraphConsumer(
		std::unordered_map<std::string, std::vector<std::pair<int, int>>> &buffer)
		: clang::ASTConsumer()
		, m_result_buffer(buffer)
	{ }
	virtual void HandleTranslationUnit(clang::ASTContext &context){
		const clang::SourceManager &sm = context.getSourceManager();
		clang::TranslationUnitDecl *tu = context.getTranslationUnitDecl();
		m_traversed_decls.clear();
		m_traversed_types.clear();
		m_traversed_stmts.clear();
		m_marked_decls.clear();
		for(const auto &decl : tu->decls()){
			if(clang::isa<clang::VarDecl>(decl)){
				TraverseDecl(decl, sm);
			}else if(clang::isa<clang::UsingDirectiveDecl>(decl)){
				TraverseDecl(decl, sm);
			}else if(clang::isa<clang::FunctionDecl>(decl)){
				const auto *function_decl =
					clang::dyn_cast<clang::FunctionDecl>(decl);
				if(function_decl->isMain()){
					TraverseDecl(decl, sm);
				}
			}
		}
		while(true){
			size_t marked_count = m_marked_decls.size();
			PropagateDecl(tu, sm);
			if(m_marked_decls.size() == marked_count){ break; }
		}
		m_keep_ranges.clear();
		m_remove_ranges.clear();
		for(const auto &decl : m_marked_decls){
			const clang::SourceRange sr = decl->getSourceRange();
			m_keep_ranges.insert(MakeRange(sr, sm));
		}
		RemoveDecl(tu, sm);
		m_result_buffer.clear();
		for(const auto r : m_remove_ranges){
			const auto file = r.getSourceFile();
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

