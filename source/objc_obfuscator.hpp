// https://github.com/fenglh/ObjcClassNameObfuscator
#pragma once

#include <iostream>

#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "llvm/Support/CommandLine.h"

using namespace std;
using namespace llvm;
using namespace clang;
using namespace clang::tooling;
using namespace clang::ast_matchers;

// 匹配回调
class MatchCallbackHandler : public MatchFinder::MatchCallback
{
public:
    MatchCallbackHandler(Rewriter &aRewriter, CompilerInstance *aCompilerInstance);

    virtual void run(const MatchFinder::MatchResult &Result) override;

    void ReplaceText(SourceLocation Start, unsigned OrigLength,StringRef NewStr);
    bool isNeedObfuscateClassName(string name);
    string getNewClassName(string oldName);
    void handleInterfaceDecl(const ObjCInterfaceDecl* interfaceDecl);
    void handleImplementationDecl(const ObjCImplementationDecl* objcImplementationDecl);
    void handleCategoryDecl(const ObjCCategoryDecl* objcCategoryDecl);
    void handleCategoryImplDecl(const ObjCCategoryImplDecl *objcCategoryImplDecl);
    void handleMessageExpr(const ObjCMessageExpr* messageExpr);
    void handleExplicitCastExpr(const ExplicitCastExpr* explicitCastExpr);
    void handleStringLiteral(const clang::StringLiteral* stringLiteral);
    void handleTypedefDecl(const TypedefDecl* typedefDecl);
    void handleVarDecl(const VarDecl* varDecl);
    void handleObjcIVarDecl(const ObjCIvarDecl* objcIvarDecl);
    void handleObjcPropertyDecl(const ObjCPropertyDecl* objcPropertyDecl);
    void handleMethodDecl(const ObjCMethodDecl* methodDecl);
    bool recursiveHandleQualType(SourceLocation start, SourceLocation end, QualType type);
    string getMethodDeclStringOfMethoddecl(const ObjCMethodDecl* methodDecl);
    string getClassNameOfMethodDecl(const ObjCMethodDecl* methodDecl);
   
   //判断是否用户源码，过滤掉系统源码
   template <typename Node>
   bool isUserSourceDecl(const Node node) {
         if(!node)return false;
        string filename = sourcePathNode(node);
        if (filename.empty())
            return false;
        //非XCode中的源码都认为是用户源码
        if(filename.find("/Applications/Xcode.app/") == 0)
            return false;
        return true;
    }
    //获取decl所在的文件
    template <typename Node>
    string sourcePathNode(const Node node ) {
        if(!node)return "";
        
        SourceLocation spellingLoc = compilerInstance->getSourceManager().getSpellingLoc(node->getSourceRange().getBegin());
        string filePath = compilerInstance->getSourceManager().getFilename(spellingLoc).str();
        return filePath;
    }
    
     //获取文件名，截取'/'后面的部分
    template <typename Node>
    string fileNameOfNode(const Node node) {
        string filePath = sourcePathNode(node);
        size_t index = filePath.find_last_of("/");
        if (index == StringRef::npos) {
            return "";
        }
        StringRef fileName = filePath.substr(index+1,-1);
        return fileName.str();
    }

private:
    Rewriter &rewriter;
    CompilerInstance *compilerInstance;
};

// AST 构造器
class ObfASTConsumer : public ASTConsumer
{
public:
    ObfASTConsumer(Rewriter& aRewriter, CompilerInstance* aCI);
    virtual void HandleTranslationUnit(ASTContext& Context) override;
private:
    MatchFinder matcher;
    MatchCallbackHandler handlerMatchCallback;
};

// action
class ObfASTFrontendAction : public ASTFrontendAction
{
public:
    //创建AST Consumer
    std::unique_ptr<ASTConsumer> CreateASTConsumer(clang::CompilerInstance &CI, StringRef file) override;
    //源文件操作结束
    void EndSourceFileAction() override;
private:
    Rewriter rewriter;
};
