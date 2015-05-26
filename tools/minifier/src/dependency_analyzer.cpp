#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/ExprCXX.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/CompilerInstance.h"
#include "dependency_analyzer.hpp"

// #define DUMP_DEBUG_TREE

class CallGraphConsumer : public clang::ASTConsumer {
	static const int MARK_WEAK_KEEP = 0;
	static const int MARK_REMOVE = 1;
	static const int MARK_KEEP = 2;

	llvm::DenseMap<const clang::Decl *, const clang::Decl *> m_parent_table;
	llvm::DenseSet<const clang::Decl *> m_traversed_decls;
	llvm::DenseSet<const clang::Stmt *> m_traversed_stmts;
	llvm::DenseSet<const clang::Type *> m_traversed_types;
	std::unordered_map<std::string, std::vector<int>> m_marks;
	RemovalTable &m_result;

	void BuildParentTable(const clang::Decl *decl){
		if(clang::isa<clang::DeclContext>(decl)){
			const auto ctx = clang::dyn_cast<clang::DeclContext>(decl);
			for(const auto child : ctx->decls()){
				m_parent_table.insert(std::make_pair(child, decl));
				BuildParentTable(child);
			}
		}
		if(clang::isa<clang::FunctionTemplateDecl>(decl)){
			const auto ft_decl =
				clang::dyn_cast<clang::FunctionTemplateDecl>(decl);
			for(const auto spec : ft_decl->specializations()){
				m_parent_table.insert(std::make_pair(spec, decl));
				BuildParentTable(spec);
			}
		}
		if(clang::isa<clang::RecordDecl>(decl)){
			const auto record_decl =
				clang::dyn_cast<clang::RecordDecl>(decl);
			for(const auto field : record_decl->fields()){
				m_parent_table.insert(std::make_pair(field, decl));
				BuildParentTable(field);
			}
		}
		if(clang::isa<clang::CXXRecordDecl>(decl)){
			const auto record_decl =
				clang::dyn_cast<clang::CXXRecordDecl>(decl);
			for(const auto method : record_decl->methods()){
				m_parent_table.insert(std::make_pair(method, decl));
				BuildParentTable(method);
			}
		}
		if(clang::isa<clang::ClassTemplateDecl>(decl)){
			const auto ct_decl =
				clang::dyn_cast<clang::ClassTemplateDecl>(decl);
			for(const auto spec : ct_decl->specializations()){
				m_parent_table.insert(std::make_pair(spec, decl));
				BuildParentTable(spec);
			}
		}
	}

	void Traverse(
		const clang::Decl *decl, const clang::SourceManager &sm, int depth)
	{
		if(!decl){ return; }
		if(!m_traversed_decls.insert(decl).second){ return; }
#ifdef DUMP_DEBUG_TREE
		llvm::errs() << std::string(depth * 2, ' ');
		llvm::errs() << "D: " << decl->getDeclKindName();
		if(clang::isa<clang::NamedDecl>(decl)){
			llvm::errs() << " (" << *clang::dyn_cast<clang::NamedDecl>(decl) << ")";
		}
		llvm::errs() << "\n";
#endif
		// 親があればそれを辿る
		const auto parent_it = m_parent_table.find(decl);
		if(parent_it != m_parent_table.end()){
			Traverse(parent_it->second, sm, depth + 1);
		}
		// ValueDecl
		if(clang::isa<clang::ValueDecl>(decl)){
			const auto value_decl = clang::dyn_cast<clang::ValueDecl>(decl);
			// 対応する型を辿る
			Traverse(value_decl->getType(), sm, depth + 1);
		}
		// VarDecl
		if(clang::isa<clang::VarDecl>(decl)){
			const auto var_decl = clang::dyn_cast<clang::VarDecl>(decl);
			// 初期化式があればそれを辿る
			if(var_decl->hasInit()){
				Traverse(var_decl->getInit(), sm, depth + 1);
			}
		}
		// ParmVarDecl
		if(clang::isa<clang::ParmVarDecl>(decl)){
			const auto pv_decl = clang::dyn_cast<clang::ParmVarDecl>(decl);
			// デフォルト引数があれば辿る
			if(pv_decl->hasDefaultArg()){
				Traverse(pv_decl->getDefaultArg(), sm, depth + 1);
			}
		}
		// FunctionDecl
		if(clang::isa<clang::FunctionDecl>(decl)){
			const auto func_decl = clang::dyn_cast<clang::FunctionDecl>(decl);
			// テンプレートを特殊化したものならその特殊化元をたどる
			if(func_decl->isFunctionTemplateSpecialization()){
				Traverse(func_decl->getPrimaryTemplate(), sm, depth + 1);
			}
			// 引数を辿る
			for(const auto param : func_decl->params()){
				Traverse(param, sm, depth + 1);
			}
			// 中に含まれる処理を辿る
			if(func_decl->hasBody()){
				Traverse(func_decl->getBody(), sm, depth + 1);
			}
			// 戻り値の型を辿る
			Traverse(func_decl->getReturnType(), sm, depth + 1);
		}
		// FieldDecl
		if(clang::isa<clang::FieldDecl>(decl)){
			const auto field_decl = clang::dyn_cast<clang::FieldDecl>(decl);
			// 親を辿る
			Traverse(field_decl->getParent(), sm, depth + 1);
			// ビットフィールドなら幅の計算式を辿る
			if(field_decl->isBitField()){
				Traverse(field_decl->getBitWidth(), sm, depth + 1);
			}
			// メンバ初期化子を持つならその式を辿る
			if(field_decl->hasInClassInitializer()){
				Traverse(field_decl->getInClassInitializer(), sm, depth + 1);
			}
		}
		// CXXMethodDecl
		if(clang::isa<clang::CXXMethodDecl>(decl)){
			const auto method_decl = clang::dyn_cast<clang::CXXMethodDecl>(decl);
			// 親を辿る
			Traverse(method_decl->getParent(), sm, depth + 1);
		}
		// CXXConstructorDecl
		if(clang::isa<clang::CXXConstructorDecl>(decl)){
			const auto ctor_decl =
				clang::dyn_cast<clang::CXXConstructorDecl>(decl);
			// 初期化リストを辿る
			for(const auto init : ctor_decl->inits()){
				if(init->getMember()){
					Traverse(init->getMember(), sm, depth + 1);
				}
				Traverse(init->getInit(), sm, depth + 1);
			}
		}
		// CXXRecordDecl
		if(clang::isa<clang::CXXRecordDecl>(decl)){
			const auto record_decl = clang::dyn_cast<clang::CXXRecordDecl>(decl);
			// 基底クラスを全て辿る
			if(record_decl->getNumBases()){
				for(const auto &base : record_decl->bases()){
					Traverse(base.getType(), sm, depth + 1);
				}
			}
			// 仮想基底クラスを全て辿る
			if(record_decl->getNumVBases()){
				for(const auto &vbase : record_decl->vbases()){
					Traverse(vbase.getType(), sm, depth + 1);
				}
			}
			// ユーザ定義デストラクタがあるなら辿る
			if(record_decl->hasUserDeclaredDestructor()){
				Traverse(record_decl->getDestructor(), sm, depth + 1);
			}
		}
		// TypedefNameDecl
		if(clang::isa<clang::TypedefNameDecl>(decl)){
			const auto tn_decl = clang::dyn_cast<clang::TypedefNameDecl>(decl);
			// 別名をつけられた型を辿る
			Traverse(tn_decl->getUnderlyingType(), sm, depth + 1);
		}
		// TemplateDecl
		if(clang::isa<clang::TemplateDecl>(decl)){
			const auto template_decl =
				clang::dyn_cast<clang::TemplateDecl>(decl);
			// テンプレート引数を辿る
			for(const auto param : *template_decl->getTemplateParameters()){
				Traverse(param, sm, depth + 1);
			}
		}
		// ClassTemplateDecl
		if(clang::isa<clang::ClassTemplateDecl>(decl)){
			const auto ct_decl =
				clang::dyn_cast<clang::ClassTemplateDecl>(decl);
			// 特殊化元となったクラス定義を辿る
			Traverse(ct_decl->getTemplatedDecl(), sm, depth + 1);
		}
		// ClassTemplateSpecializationDecl
		if(clang::isa<clang::ClassTemplateSpecializationDecl>(decl)){
			const auto cts_decl =
				clang::dyn_cast<clang::ClassTemplateSpecializationDecl>(decl);
			// 特殊化元となったクラス定義を辿る
			Traverse(cts_decl->getSpecializedTemplate(), sm, depth + 1);
		}
		// TemplateTypeParmDecl
		if(clang::isa<clang::TemplateTypeParmDecl>(decl)){
			const auto ttp_decl =
				clang::dyn_cast<clang::TemplateTypeParmDecl>(decl);
			// デフォルト引数があるならそれを辿る
			if(ttp_decl->hasDefaultArgument()){
				Traverse(ttp_decl->getDefaultArgument(), sm, depth + 1);
			}
		}
		// NonTypeTemplateParmDecl
		if(clang::isa<clang::NonTypeTemplateParmDecl>(decl)){
			const auto nttp_decl =
				clang::dyn_cast<clang::NonTypeTemplateParmDecl>(decl);
			// デフォルト引数があるならそれを辿る
			if(nttp_decl->hasDefaultArgument()){
				Traverse(nttp_decl->getDefaultArgument(), sm, depth + 1);
			}
		}
	}

	void Traverse(
		const clang::Stmt *stmt, const clang::SourceManager &sm, int depth)
	{
		if(!stmt){ return; }
		if(!m_traversed_stmts.insert(stmt).second){ return; }
#ifdef DUMP_DEBUG_TREE
		llvm::errs() << std::string(depth * 2, ' ');
		llvm::errs() << "S: " << stmt->getStmtClassName();
		llvm::errs() << "\n";
#endif
		// 子要素を全て辿る
		for(const auto child : stmt->children()){
			Traverse(child, sm, depth + 1);
		}
		// DeclStmt
		if(clang::isa<clang::DeclStmt>(stmt)){
			const auto decl_stmt = clang::dyn_cast<clang::DeclStmt>(stmt);
			// 子として含まれる定義を全て辿る
			for(const auto decl : decl_stmt->decls()){
				Traverse(decl, sm, depth + 1);
			}
		}
		// DeclRefExpr
		if(clang::isa<clang::DeclRefExpr>(stmt)){
			const auto dr_expr = clang::dyn_cast<clang::DeclRefExpr>(stmt);
			// 定義への参照ならその定義を辿る
			Traverse(dr_expr->getDecl(), sm, depth + 1);
			// テンプレートの特殊化ならその引数も辿る
			Traverse(
				dr_expr->getTemplateArgs(), dr_expr->getNumTemplateArgs(),
				sm, depth + 1);
			// NestedNameQualifier
			if(dr_expr->hasQualifier()){
				Traverse(dr_expr->getQualifier(), sm, depth + 1);
			}
		}
		// MemberExpr
		if(clang::isa<clang::MemberExpr>(stmt)){
			const auto member_expr = clang::dyn_cast<clang::MemberExpr>(stmt);
			// メンバの定義を辿る
			Traverse(member_expr->getMemberDecl(), sm, depth + 1);
			// テンプレートの特殊化ならその引数も辿る
			Traverse(
				member_expr->getTemplateArgs(), member_expr->getNumTemplateArgs(),
				sm, depth + 1);
		}
		// CallExpr
		if(clang::isa<clang::CallExpr>(stmt)){
			const auto call_expr = clang::dyn_cast<clang::CallExpr>(stmt);
			// 関数呼び出しなら呼ばれた関数を辿る
			Traverse(call_expr->getCallee(), sm, depth + 1);
		}
		// CXXConstructExpr
		if(clang::isa<clang::CXXConstructExpr>(stmt)){
			const auto ct_expr =
				clang::dyn_cast<clang::CXXConstructExpr>(stmt);
			//コンストラクタの定義を辿る
			Traverse(ct_expr->getConstructor(), sm, depth + 1);
		}
		// ExplicitCastExpr
		if(clang::isa<clang::ExplicitCastExpr>(stmt)){
			const auto ec_expr =
				clang::dyn_cast<clang::ExplicitCastExpr>(stmt);
			// キャスト先の型の定義を辿る
			Traverse(ec_expr->getTypeAsWritten(), sm, depth + 1);
		}
		// UnaryExprOrTypeTraitExpr
		if(clang::isa<clang::UnaryExprOrTypeTraitExpr>(stmt)){
			const auto uett_expr =
				clang::dyn_cast<clang::UnaryExprOrTypeTraitExpr>(stmt);
			if(uett_expr->isArgumentType()){
				Traverse(uett_expr->getArgumentType(), sm, depth + 1);
			}else{
				Traverse(uett_expr->getArgumentExpr(), sm, depth + 1);
			}
		}
	}

	void Traverse(
		const clang::QualType &type, const clang::SourceManager &sm, int depth)
	{
#ifdef DUMP_DEBUG_TREE
		llvm::errs() << std::string(depth * 2, ' ');
		llvm::errs() << "Q: " << type.getAsString();
		llvm::errs() << "\n";
#endif
		Traverse(type.getTypePtr(), sm, depth);
	}
	void Traverse(
		const clang::Type *type, const clang::SourceManager &sm, int depth)
	{
		if(!type){ return; }
		if(!m_traversed_types.insert(type).second){ return; }
#ifdef DUMP_DEBUG_TREE
		llvm::errs() << std::string(depth * 2, ' ');
		llvm::errs() << "T: " << type->getTypeClassName();
		llvm::errs() << "\n";
#endif
		// PointerType
		if(clang::isa<clang::PointerType>(type)){
			const auto ptr_type = clang::dyn_cast<clang::PointerType>(type);
			Traverse(ptr_type->getPointeeType(), sm, depth + 1);
		}
		// ReferenceType
		if(clang::isa<clang::ReferenceType>(type)){
			const auto ref_type = clang::dyn_cast<clang::ReferenceType>(type);
			Traverse(ref_type->getPointeeType(), sm, depth + 1);
		}
		// ArrayType
		if(clang::isa<clang::ArrayType>(type)){
			const auto array_type = clang::dyn_cast<clang::ArrayType>(type);
			Traverse(array_type->getElementType(), sm, depth + 1);
		}
		// AttributedType
		if(clang::isa<clang::AttributedType>(type)){
			const auto attr_type =
				clang::dyn_cast<clang::AttributedType>(type);
			Traverse(attr_type->getModifiedType(), sm, depth + 1);
		}
		// AutoType
		if(clang::isa<clang::AutoType>(type)){
			const auto auto_type = clang::dyn_cast<clang::AutoType>(type);
			Traverse(auto_type->getDeducedType(), sm, depth + 1);
		}
		// DecltypeType
		if(clang::isa<clang::DecltypeType>(type)){
			const auto dt_type = clang::dyn_cast<clang::DecltypeType>(type);
			Traverse(dt_type->getUnderlyingExpr(), sm, depth + 1);
			Traverse(dt_type->getUnderlyingType(), sm, depth + 1);
		}
		// RecordType
		if(clang::isa<clang::RecordType>(type)){
			const auto record_type = clang::dyn_cast<clang::RecordType>(type);
			Traverse(record_type->getDecl(), sm, depth + 1);
		}
		// TemplateSpecializationType
		if(clang::isa<clang::TemplateSpecializationType>(type)){
			const auto ts_type =
				clang::dyn_cast<clang::TemplateSpecializationType>(type);
			Traverse(ts_type->getArgs(), ts_type->getNumArgs(), sm, depth + 1);
		}
		// TypedefType
		if(clang::isa<clang::TypedefType>(type)){
			const auto td_type = clang::dyn_cast<clang::TypedefType>(type);
			Traverse(td_type->getDecl(), sm, depth + 1);
		}
		// ElaboratedType
		if(clang::isa<clang::ElaboratedType>(type)){
			const auto elaborated =
				clang::dyn_cast<clang::ElaboratedType>(type);
			Traverse(elaborated->getNamedType(), sm, depth + 1);
			if(elaborated->getQualifier()){
				Traverse(elaborated->getQualifier(), sm, depth + 1);
			}
		}
	}

	void Traverse(
		const clang::NestedNameSpecifier *spec,
		const clang::SourceManager &sm, int depth)
	{
		switch(spec->getKind()){
		case clang::NestedNameSpecifier::TypeSpec:
		case clang::NestedNameSpecifier::TypeSpecWithTemplate:
    		Traverse(spec->getAsType(), sm, depth);
			break;
		default:
			break;
		}
		if(spec->getPrefix()){
			Traverse(spec->getPrefix(), sm, depth);
		}
	}

	void Traverse(
		const clang::TemplateArgumentLoc *arg_locs, unsigned int num_args,
		const clang::SourceManager &sm, int depth)
	{
		for(unsigned int i = 0; i < num_args; ++i){
			const auto &arg = arg_locs[i].getArgument();
			switch(arg.getKind()){
			case clang::TemplateArgument::Type:
				Traverse(arg_locs[i].getTypeSourceInfo()->getType(), sm, depth);
				break;
			case clang::TemplateArgument::Expression:
				Traverse(arg_locs[i].getSourceExpression(), sm, depth);
				break;
			case clang::TemplateArgument::Declaration:
				Traverse(arg_locs[i].getSourceDeclExpression(), sm, depth);
				break;
			case clang::TemplateArgument::NullPtr:
				Traverse(arg_locs[i].getSourceNullPtrExpression(), sm, depth);
				break;
			case clang::TemplateArgument::Integral:
				Traverse(arg_locs[i].getSourceIntegralExpression(), sm, depth);
				break;
			default:
				break;
			}
		}
	}
	void Traverse(
		const clang::TemplateArgument *args, unsigned int num_args,
		const clang::SourceManager &sm, int depth)
	{
		for(unsigned int i = 0; i < num_args; ++i){
			const auto &arg = args[i];
			switch(arg.getKind()){
			case clang::TemplateArgument::Type:
				Traverse(arg.getAsType(), sm, depth);
				break;
			case clang::TemplateArgument::Expression:
				Traverse(arg.getAsExpr(), sm, depth);
				break;
			case clang::TemplateArgument::Declaration:
				Traverse(arg.getAsDecl(), sm, depth);
				break;
			case clang::TemplateArgument::NullPtr:
				Traverse(arg.getNullPtrType(), sm, depth);
				break;
			default:
				break;
			}
		}
	}

	void WriteMark(
		const clang::SourceRange &range, int mark,
		const clang::SourceManager &sm)
	{
		const std::string filename = sm.getFilename(range.getBegin());
		const int first = sm.getPresumedLineNumber(range.getBegin());
		const int last = sm.getPresumedLineNumber(range.getEnd());
		std::vector<int> &marks = m_marks[filename];
		if(marks.size() <= last){ marks.resize(last + 1); }
		for(int i = first; i <= last; ++i){
			marks[i] = std::max(marks[i], mark);
		}
	}

	bool MarkDecl(
		const clang::Decl *decl, const clang::SourceManager &sm, int depth)
	{
#ifdef DUMP_DEBUG_TREE
		llvm::errs() << std::string(depth * 2, ' ');
		llvm::errs() << "M: " << decl->getDeclKindName();
		if(clang::isa<clang::NamedDecl>(decl)){
			llvm::errs() << " (" << *clang::dyn_cast<clang::NamedDecl>(decl) << ")";
		}
		llvm::errs() << " " << (m_traversed_decls.find(decl) == m_traversed_decls.end());
		llvm::errs() << " (";
		llvm::errs() << sm.getPresumedLineNumber(decl->getSourceRange().getBegin());
		llvm::errs() << ":";
		llvm::errs() << sm.getPresumedLineNumber(decl->getSourceRange().getEnd());
		llvm::errs() << ")";
		llvm::errs() << "\n";
#endif
		if(clang::isa<clang::NamespaceDecl>(decl)){
			const clang::DeclContext *ctx =
				clang::dyn_cast<clang::DeclContext>(decl);
			bool keep_flag = false;
			WriteMark(decl->getSourceRange(), MARK_WEAK_KEEP, sm);
			for(const auto child : ctx->decls()){
				if(MarkDecl(child, sm, depth + 1)){ keep_flag = true; }
			}
			if(!keep_flag){
				WriteMark(decl->getSourceRange(), MARK_REMOVE, sm);
			}
			return keep_flag;
		}else if(clang::isa<clang::TranslationUnitDecl>(decl)){
			const auto tu_decl =
				clang::dyn_cast<clang::TranslationUnitDecl>(decl);
			WriteMark(decl->getSourceRange(), MARK_WEAK_KEEP, sm);
			for(const auto child : tu_decl->decls()){
				MarkDecl(child, sm, depth + 1);
			}
			return true;
		}else if(clang::isa<clang::AccessSpecDecl>(decl)){
			WriteMark(decl->getSourceRange(), MARK_KEEP, sm);
			return true;
		}
		if(m_traversed_decls.find(decl) == m_traversed_decls.end()){
			if(!decl->isImplicit()){
				WriteMark(decl->getSourceRange(), MARK_REMOVE, sm);
			}
			return false;
		}
		if(clang::isa<clang::CXXRecordDecl>(decl)){
			const auto record_decl =
				clang::dyn_cast<clang::CXXRecordDecl>(decl);
			WriteMark(decl->getSourceRange(), MARK_WEAK_KEEP, sm);
			for(const auto child : record_decl->decls()){
				MarkDecl(child, sm, depth + 1);
			}
			for(const auto field : record_decl->fields()){
				MarkDecl(field, sm, depth + 1);
			}
			for(const auto method : record_decl->methods()){
				MarkDecl(method, sm, depth + 1);
			}
		}else if(clang::isa<clang::RecordDecl>(decl)){
			const auto record_decl = clang::dyn_cast<clang::RecordDecl>(decl);
			WriteMark(decl->getSourceRange(), MARK_WEAK_KEEP, sm);
			for(const auto child : record_decl->decls()){
				MarkDecl(child, sm, depth + 1);
			}
			for(const auto field : record_decl->fields()){
				MarkDecl(field, sm, depth + 1);
			}
		}else if(clang::isa<clang::ClassTemplateDecl>(decl)){
			const auto ct_decl =
				clang::dyn_cast<clang::ClassTemplateDecl>(decl);
			WriteMark(decl->getSourceRange(), MARK_WEAK_KEEP, sm);
			MarkDecl(ct_decl->getTemplatedDecl(), sm, depth + 1);
			for(const auto spec : ct_decl->specializations()){
				MarkDecl(spec, sm, depth + 1);
			}
		}else if(clang::isa<clang::FunctionTemplateDecl>(decl)){
			const auto ft_decl =
				clang::dyn_cast<clang::FunctionTemplateDecl>(decl);
			WriteMark(decl->getSourceRange(), MARK_KEEP, sm);
			MarkDecl(ft_decl->getTemplatedDecl(), sm, depth + 1);
			for(const auto spec : ft_decl->specializations()){
				MarkDecl(spec, sm, depth + 1);
			}
		}else{
			WriteMark(decl->getSourceRange(), MARK_KEEP, sm);
		}
		return true;
	}

public:
	explicit CallGraphConsumer(RemovalTable &result)
		: m_result(result)
	{ }
	virtual void HandleTranslationUnit(clang::ASTContext &context){
		const clang::SourceManager &sm = context.getSourceManager();
		clang::TranslationUnitDecl *tu = context.getTranslationUnitDecl();
#ifdef DUMP_DEBUG_TREE
		tu->dump();
#endif
		BuildParentTable(tu);
		m_traversed_decls.clear();
		m_traversed_stmts.clear();
		m_traversed_types.clear();
		m_marks.clear();
		for(const auto decl : tu->decls()){
			if(clang::isa<clang::VarDecl>(decl)){
				Traverse(decl, sm, 0);
			}else if(clang::isa<clang::UsingDirectiveDecl>(decl)){
				Traverse(decl, sm, 0);
			}else if(clang::isa<clang::FunctionDecl>(decl)){
				const auto *func_decl =
					clang::dyn_cast<clang::FunctionDecl>(decl);
				if(func_decl->isMain()){ Traverse(decl, sm, 0); }
			}
		}
		MarkDecl(tu, sm, 0);
		for(const auto file : m_marks){
			std::vector<bool> table(file.second.size());
			for(size_t i = 0; i < file.second.size(); ++i){
				table[i] = (file.second[i] == MARK_REMOVE);
			}
			m_result.write(file.first, table);
		}
	}
};

class CallGraphAction : public clang::ASTFrontendAction {
private:
	RemovalTable &m_result;
public:
	CallGraphAction(RemovalTable &result)
		: m_result(result)
	{ }
	virtual clang::ASTConsumer *CreateASTConsumer(
		clang::CompilerInstance &, llvm::StringRef)
	{
		return new CallGraphConsumer(m_result);
	}
};

class CallGraphActionFactory : public clang::tooling::FrontendActionFactory {
private:
	RemovalTable &m_result;
public:
	CallGraphActionFactory(RemovalTable &result)
		: m_result(result)
	{ }
	virtual clang::FrontendAction *create(){
		return new CallGraphAction(m_result);
	}
};

RemovalTable analyze_dependency(clang::tooling::ClangTool &tool){
	RemovalTable result;
	CallGraphActionFactory factory(result);
	tool.run(&factory);
	return result;
}

