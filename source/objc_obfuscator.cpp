//===----------------------------------------------------------------------===//
//                           The MIT License (MIT)
//             Copyright (c) 2020 Douglas Chen <dougpuob@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//===----------------------------------------------------------------------===//

// https://github.com/fenglh/ObjcClassNameObfuscator
#include "objc_obfuscator.hpp"

MatchCallbackHandler::MatchCallbackHandler(Rewriter &aRewriter, CompilerInstance *aCompilerInstance)
: rewriter(aRewriter)
, compilerInstance(aCompilerInstance)
{
}


void MatchCallbackHandler::run(const MatchFinder::MatchResult &Result)
{
    const ObjCInterfaceDecl *interfaceDecl          = Result.Nodes.getNodeAs<ObjCInterfaceDecl>("objcInterfaceDecl");
    const ObjCImplementationDecl *implDecl          = Result.Nodes.getNodeAs<ObjCImplementationDecl>("objcImplementationDecl");
    const ObjCCategoryDecl *categoryDecl            = Result.Nodes.getNodeAs<ObjCCategoryDecl>("objcCategoryDecl");
    const ObjCCategoryImplDecl *categoryImplDecl    = Result.Nodes.getNodeAs<ObjCCategoryImplDecl>("objcCategoryImplDecl");
    const VarDecl *varDecl                          = Result.Nodes.getNodeAs<VarDecl>("varDecl");
    const ObjCIvarDecl *objcIvarDecl                = Result.Nodes.getNodeAs<ObjCIvarDecl>("objcIvarDecl");
    const ObjCPropertyDecl *objcPropertyDecl        = Result.Nodes.getNodeAs<ObjCPropertyDecl>("objcPropertyDecl");
    const ObjCMethodDecl *methodDecl                = Result.Nodes.getNodeAs<ObjCMethodDecl>("objcMethodDecl");
    const ObjCMessageExpr *messageExpr              = Result.Nodes.getNodeAs<ObjCMessageExpr>("objcMessageExpr");
    const ExplicitCastExpr *explicitCastExpr        = Result.Nodes.getNodeAs<ExplicitCastExpr>("explicitCastExpr");
    const clang::StringLiteral *stringLiteral       = Result.Nodes.getNodeAs<clang::StringLiteral>("stringLiteral");
    const TypedefDecl *typedefDecl                  = Result.Nodes.getNodeAs<TypedefDecl>("typedefDecl");
    

    if (isUserSourceDecl(interfaceDecl)) handleInterfaceDecl(interfaceDecl);
    if (isUserSourceDecl(implDecl)) handleImplementationDecl(implDecl);
    if (isUserSourceDecl(categoryDecl)) handleCategoryDecl(categoryDecl);
    if (isUserSourceDecl(categoryImplDecl)) handleCategoryImplDecl(categoryImplDecl);
    if (isUserSourceDecl(explicitCastExpr)) handleExplicitCastExpr(explicitCastExpr);
    if (isUserSourceDecl(varDecl)) handleVarDecl(varDecl);
    if (isUserSourceDecl(objcIvarDecl)) handleObjcIVarDecl(objcIvarDecl);
    if (isUserSourceDecl(objcPropertyDecl)) handleObjcPropertyDecl(objcPropertyDecl);
    if (isUserSourceDecl(methodDecl)) handleMethodDecl(methodDecl);
    if (isUserSourceDecl(messageExpr)) handleMessageExpr(messageExpr);
    if (stringLiteral) handleStringLiteral(stringLiteral);
    if (isUserSourceDecl(typedefDecl)) handleTypedefDecl(typedefDecl);
}

void MatchCallbackHandler::ReplaceText(SourceLocation Start, unsigned OrigLength, StringRef NewStr)
{
    if (compilerInstance->getSourceManager().isMacroBodyExpansion(Start))
    {
    Start = compilerInstance->getSourceManager().getSpellingLoc(Start);
    }
    rewriter.ReplaceText(Start, OrigLength, NewStr);
}

//需要混淆的类名
bool MatchCallbackHandler::isNeedObfuscateClassName(string name)
{
    return name == "DemoViewController";
}

string MatchCallbackHandler::getNewClassName(string oldName)
{
    return "NewViewController";
}

void MatchCallbackHandler::handleInterfaceDecl(const ObjCInterfaceDecl* interfaceDecl)
{
    string oldClassName = interfaceDecl->getNameAsString();
    if (isNeedObfuscateClassName(oldClassName)) {
        string newClassName = getNewClassName(oldClassName);
        SourceLocation loc = interfaceDecl->getLocation();
        ReplaceText(loc, oldClassName.length(), getNewClassName(oldClassName));
        cout << "cls:" << oldClassName << "->:" << newClassName << endl;
    }
}

void MatchCallbackHandler::handleImplementationDecl(const ObjCImplementationDecl* objcImplementationDecl)
{
    string oldClassName = objcImplementationDecl->getNameAsString();
    if (isNeedObfuscateClassName(oldClassName)) {
        string newClassName = getNewClassName(oldClassName);
        SourceLocation loc = objcImplementationDecl->getLocation();
        ReplaceText(loc, oldClassName.length(), getNewClassName(oldClassName));
        cout << fileNameOfNode(objcImplementationDecl) << ":cls:" << oldClassName << "->:" << newClassName << endl;
    }
}

void MatchCallbackHandler::handleCategoryDecl(const ObjCCategoryDecl* objcCategoryDecl)
{
    string oldClassName = objcCategoryDecl->getClassInterface()->getNameAsString();

    if (isNeedObfuscateClassName(oldClassName)) {
        string newClassName = getNewClassName(oldClassName);
        SourceLocation loc = objcCategoryDecl->getLocation();
        ReplaceText(loc, oldClassName.length(), getNewClassName(oldClassName));
        cout << fileNameOfNode(objcCategoryDecl) << ":Category:" << oldClassName << "->" << newClassName << endl;
    }
}

//处理分类定义
void MatchCallbackHandler::handleCategoryImplDecl(const ObjCCategoryImplDecl* objcCategoryImplDecl) 
{
    string oldClassName = objcCategoryImplDecl->getNameAsString();
    if (isNeedObfuscateClassName(oldClassName)) 
    {
        string newClassName = getNewClassName(oldClassName);
        SourceLocation loc = objcCategoryImplDecl->getLocation();
        ReplaceText(loc, oldClassName.length(), getNewClassName(oldClassName));
        cout << fileNameOfNode(objcCategoryImplDecl) << ":CategoryImpl:" << oldClassName << "->" << newClassName << endl;
    }
}

void MatchCallbackHandler::handleMessageExpr(const ObjCMessageExpr* messageExpr)
{
    if (messageExpr->isClassMessage()) 
    { //是类方法
        const ObjCInterfaceDecl* objcInterfaceDecl = messageExpr->getReceiverInterface();
        if (isUserSourceDecl(objcInterfaceDecl)) 
        {
            string oldClassName = objcInterfaceDecl->getNameAsString();
            if (isNeedObfuscateClassName(oldClassName)) 
            {
                string newClassName = getNewClassName(oldClassName);
                SourceLocation loc = messageExpr->getClassReceiverTypeInfo()->getTypeLoc().getBeginLoc();
                ReplaceText(loc, oldClassName.length(), newClassName);
                cout << "messageExpr:" << "[" << oldClassName << " " 
                    << messageExpr->getSelector().getAsString() 
                    << "]" "->" << "[" << newClassName << " " 
                    << messageExpr->getSelector().getAsString() << "]" << endl;
            }
        }
    }
}

 void MatchCallbackHandler::handleExplicitCastExpr(const ExplicitCastExpr* explicitCastExpr) 
 {
     //        explicitCastExpr->getd
     QualType qualType = explicitCastExpr->getTypeAsWritten();
     SourceLocation typeBeginLoc = explicitCastExpr->getTypeInfoAsWritten()->getTypeLoc().getBeginLoc();
     SourceLocation typeEndLoc = explicitCastExpr->getTypeInfoAsWritten()->getTypeLoc().getEndLoc();
     //宏替换后，是否会处理多余的空格?
     bool success = recursiveHandleQualType(typeBeginLoc, typeEndLoc, qualType);
     if (success) 
     {
         string rewriteString = rewriter.getRewrittenText(SourceRange(explicitCastExpr->getBeginLoc(), explicitCastExpr->getExprLoc()));
         cout << "Explicit:" << qualType.getAsString() << "->" << rewriteString << endl;
     }
 }

void MatchCallbackHandler::handleStringLiteral(const clang::StringLiteral* stringLiteral)
{
     clang::StringRef content = stringLiteral->getString();
     std::string contentString = content.str();
     if (isNeedObfuscateClassName(contentString)) 
     {
         SourceLocation loc = stringLiteral->getBeginLoc();
         string newClassName = getNewClassName(contentString);
         ReplaceText(loc.getLocWithOffset(-1), contentString.length() + 2, newClassName);
         cout << "StringLiteral:" << contentString << " ->" << newClassName << endl;
     }
 }

 void MatchCallbackHandler::handleTypedefDecl(const TypedefDecl* typedefDecl)
 {
     //underlyingType
     QualType type = typedefDecl->getUnderlyingType();
     recursiveHandleQualType(typedefDecl->getBeginLoc(), typedefDecl->getEndLoc(), type);
 }

 void MatchCallbackHandler::handleVarDecl(const VarDecl* varDecl) 
 {
     ////隐式实现的，并非在显示的写源码中则不作处理。例如：编译器会实现 property 的实例变量
     if (varDecl->isImplicit())
     {
         return;
     }
     QualType qualType = varDecl->getType();
     //隐式实现的Decl ，则varDecl->getTypeSourceInfo() 为NULL
     if (qualType.isNull()) 
     {
         return;
     }
     
     TypeSourceInfo* typeSourceInfo = varDecl->getTypeSourceInfo();
     //即使varDecl->isImplicit() == false ，typeSourceInfo 也会为NULL，所以下面再次进行NULL判断
     if (!typeSourceInfo) 
     {
         return;
     }
     SourceLocation typeBeginLoc = typeSourceInfo->getTypeLoc().getBeginLoc();
     SourceLocation typeEndLoc = typeSourceInfo->getTypeLoc().getEndLoc();
     bool success = recursiveHandleQualType(typeBeginLoc, typeEndLoc, qualType);
     if (success) 
     {
         SourceLocation beginLoc = compilerInstance->getSourceManager().getSpellingLoc(varDecl->getBeginLoc());
         SourceLocation endLoc = compilerInstance->getSourceManager().getSpellingLoc(varDecl->getEndLoc());
         string rewriteString = rewriter.getRewrittenText(SourceRange(beginLoc, endLoc));
         cout << "VarDecl:" << qualType.getAsString() << " " << varDecl->getNameAsString() << "->" << rewriteString << endl;
     }
 }
 void MatchCallbackHandler::handleObjcIVarDecl(const ObjCIvarDecl* objcIvarDecl) 
 {
     ////隐式实现的，并非在显示的写源码中则不作处理。例如：编译器会实现 property 的实例变量
     if (objcIvarDecl->isImplicit()) 
     {
         return;
     }
     QualType qualType = objcIvarDecl->getType();
     //隐式实现的Decl ，则varDecl->getTypeSourceInfo() 为NULL
     if (qualType.isNull()) 
     {
         return;
     }
     TypeSourceInfo* typeSourceInfo = objcIvarDecl->getTypeSourceInfo();
     //即使varDecl->isImplicit() == false ，typeSourceInfo 也会为NULL，所以下面再次进行NULL判断
     if (!typeSourceInfo) 
     {
         return;
     }
     SourceLocation typeBeginLoc = typeSourceInfo->getTypeLoc().getBeginLoc();
     SourceLocation typeEndLoc = typeSourceInfo->getTypeLoc().getEndLoc();
     bool success = recursiveHandleQualType(typeBeginLoc, typeEndLoc, qualType);
     if (success) 
     {
         SourceLocation beginLoc = compilerInstance->getSourceManager().getSpellingLoc(objcIvarDecl->getBeginLoc());
         SourceLocation endLoc = compilerInstance->getSourceManager().getSpellingLoc(objcIvarDecl->getEndLoc());
         string rewriteString = rewriter.getRewrittenText(SourceRange(beginLoc, endLoc));
         cout << "ObjcIVar:" << qualType.getAsString() << " " << objcIvarDecl->getNameAsString() << "->" << rewriteString << endl;
     }
 }

 void MatchCallbackHandler::handleObjcPropertyDecl(const ObjCPropertyDecl* objcPropertyDecl)
 {

     QualType qualType = objcPropertyDecl->getType();
     TypeSourceInfo* typeSourceInfo = objcPropertyDecl->getTypeSourceInfo();
     if (!typeSourceInfo) {
         return;
     }
     SourceLocation typeBeginLoc = typeSourceInfo->getTypeLoc().getBeginLoc();
     SourceLocation typeEndLoc = typeSourceInfo->getTypeLoc().getEndLoc();
     bool success = recursiveHandleQualType(typeBeginLoc, typeEndLoc, qualType);
     if (success) {
         SourceLocation beginLoc = compilerInstance->getSourceManager().getSpellingLoc(objcPropertyDecl->getBeginLoc());
         SourceLocation endLoc = compilerInstance->getSourceManager().getSpellingLoc(objcPropertyDecl->getEndLoc());
         string rewriteString = rewriter.getRewrittenText(SourceRange(beginLoc, endLoc));
         cout << "ObjcProperty:" << qualType.getAsString() << " " << objcPropertyDecl->getNameAsString() << "->" << rewriteString << endl;
     }
 }
 void MatchCallbackHandler::handleMethodDecl(const ObjCMethodDecl* methodDecl) 
 {
     if (methodDecl->isImplicit()) {
         return;
     }
     QualType qualType = methodDecl->getReturnType();
     if (qualType.isNull()) {
        return;
     }
     TypeSourceInfo* typeSourceInfo = methodDecl->getReturnTypeSourceInfo();
     if (!typeSourceInfo) {
        return;
     }
     SourceLocation typeBeginLoc = typeSourceInfo->getTypeLoc().getBeginLoc();
     SourceLocation typeEndLoc = typeSourceInfo->getTypeLoc().getEndLoc();
     bool handleReturnTypeSuccess = recursiveHandleQualType(typeBeginLoc, typeEndLoc, qualType);
     ArrayRef<ParmVarDecl*> params = methodDecl->parameters();
     bool handleParamersTypeSuccess = false;
     for (ArrayRef< ParmVarDecl* >::iterator i = params.begin(), e = params.end(); i != e; i++) 
     {
        ParmVarDecl* p = *i;
        QualType type = p->getType();
        handleParamersTypeSuccess = handleParamersTypeSuccess || recursiveHandleQualType(p->getBeginLoc(), p->getEndLoc(), type);
     }

     if (handleReturnTypeSuccess || handleParamersTypeSuccess) 
     {
         auto range = SourceRange(methodDecl->getBeginLoc(), methodDecl->getDeclaratorEndLoc().getLocWithOffset(-1));
         string rewriteString = rewriter.getRewrittenText(range);
         cout << "Method:" << getMethodDeclStringOfMethoddecl(methodDecl) << "->" << rewriteString << endl;
     }
 }

 bool MatchCallbackHandler::recursiveHandleQualType(SourceLocation start, SourceLocation end, QualType type)
 {
     if (start.isInvalid() || end.isInvalid())
     {
         return  false;
     }

     bool success = false;
     SourceLocation slideLoc = start;
     if (isa<ObjCObjectPointerType>(type)) 
     {
         const ObjCObjectPointerType* pointerType = type->getAs<ObjCObjectPointerType>();
         const ObjCInterfaceDecl* IDecl = pointerType->getInterfaceDecl();
         if (isUserSourceDecl(IDecl) && isNeedObfuscateClassName(IDecl->getNameAsString())) 
         {
             string oldClassName = IDecl->getNameAsString();
             string newClassName = getNewClassName(oldClassName);
             const char* startBuffer = compilerInstance->getSourceManager().getCharacterData(slideLoc);
             const char* endBuffer = compilerInstance->getSourceManager().getCharacterData(end);
             int offset = endBuffer - startBuffer;
             string originTypeDefineStr(startBuffer, offset);

             int index = originTypeDefineStr.find(oldClassName, 0);

             slideLoc = slideLoc.getLocWithOffset(index);
             ReplaceText(slideLoc, oldClassName.length(), newClassName);
             slideLoc = slideLoc.getLocWithOffset(index + oldClassName.length());
             success = true;
         }

         if (pointerType->isSpecialized()) 
         {
             const ArrayRef< QualType > params = pointerType->getTypeArgs();
             unsigned index = 0;
             for (ArrayRef< QualType >::iterator i = params.begin(), e = params.end(); i != e; i++, index++) 
             {
                 QualType t = *i;
                 success = recursiveHandleQualType(slideLoc, end, t) || success;
             }
         }
     }
     else if (isa<ObjCObjectType>(type)) 
     {
     }
     else if (isa<AttributedType>(type)) 
     {
         const AttributedType* attributedType = type->getAs<AttributedType>();
         success = recursiveHandleQualType(slideLoc, end, attributedType->getModifiedType());
     }
     return success;
 }

 string MatchCallbackHandler::getMethodDeclStringOfMethoddecl(const ObjCMethodDecl* methodDecl) 
 {
     string methodDeclStr;
     methodDeclStr += (methodDecl->isInstanceMethod() ? "-" : "+");
     methodDeclStr += "(";
     methodDeclStr += methodDecl->getReturnType().getAsString();
     methodDeclStr += ")";
     int numSelectorLocs = methodDecl->getNumSelectorLocs();
     int numParams = methodDecl->getSelector().getNumArgs();
     for (int i = 0; i < numSelectorLocs; i++) 
     {
         methodDeclStr += methodDecl->getSelector().getNameForSlot(i).str();
         if (i < numParams) 
         {
             methodDeclStr += ":";
             const ParmVarDecl* paramVarDecl = methodDecl->getParamDecl(i);
             methodDeclStr += "(";
             methodDeclStr += paramVarDecl->getType().getAsString();
             methodDeclStr += ")";
             methodDeclStr += paramVarDecl->getNameAsString();
         }
         if (i + 1 < numSelectorLocs) 
         {
             methodDeclStr += " ";
         }
     }
     return methodDeclStr;
 }


 string MatchCallbackHandler::getClassNameOfMethodDecl(const ObjCMethodDecl* methodDecl)
 {
     string className;
     methodDecl->getClassInterface();

     if (isa<ObjCCategoryDecl>(methodDecl->getDeclContext())) 
     { //
         const ObjCCategoryDecl* categoryDecl = dyn_cast_or_null<ObjCCategoryDecl>(methodDecl->getDeclContext());
         const ObjCInterfaceDecl* interfaceDecl = categoryDecl->getClassInterface();
         if (interfaceDecl) className = interfaceDecl->getNameAsString();
     }
     else if (isa<ObjCCategoryImplDecl>(methodDecl->getDeclContext())) 
     {
         const ObjCCategoryImplDecl* categoryImplDecl = dyn_cast_or_null<ObjCCategoryImplDecl>(methodDecl->getDeclContext());
         const ObjCInterfaceDecl* interfaceDecl = categoryImplDecl->getClassInterface();
         if (interfaceDecl) className = interfaceDecl->getNameAsString();
     }
     else if (isa<ObjCInterfaceDecl>(methodDecl->getDeclContext())) 
     {
         const ObjCInterfaceDecl* interfaceDecl = dyn_cast_or_null<ObjCInterfaceDecl>(methodDecl->getDeclContext());
         if (interfaceDecl) className = interfaceDecl->getNameAsString();
     }
     else if (isa<ObjCImplementationDecl>(methodDecl->getDeclContext())) 
     {
         const ObjCImplementationDecl* implementationDecl = dyn_cast_or_null<ObjCImplementationDecl>(methodDecl->getDeclContext());
         if (implementationDecl) className = implementationDecl->getNameAsString();
     }
     else 
     { //ObjCProtocolDecl
         const ObjCProtocolDecl* protocolDecl = dyn_cast_or_null<ObjCProtocolDecl>(methodDecl->getDeclContext());
         if (protocolDecl) className = protocolDecl->getNameAsString();
     }
     return className;
 }

// action
//创建AST Consumer
std::unique_ptr<ASTConsumer> ObfASTFrontendAction::CreateASTConsumer(clang::CompilerInstance &CI, StringRef file)
{
    size_t index = file.str().find_last_of("/");
    StringRef fileName = file.str().substr(index + 1, -1); //获取文件名，截取'/'后面的部分
    cout << "deal with file:" << fileName.str() << endl;

    rewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
    return std::make_unique<ObfASTConsumer>(rewriter, &CI);
}

void ObfASTFrontendAction::EndSourceFileAction()
{
}

ObfASTConsumer::ObfASTConsumer(Rewriter& aRewriter, CompilerInstance* aCI)
    :handlerMatchCallback(aRewriter, aCI)
{
    //类声明
    matcher.addMatcher(objcInterfaceDecl().bind("objcInterfaceDecl"), &handlerMatchCallback);
    //类定义
    matcher.addMatcher(objcImplementationDecl().bind("objcImplementationDecl"), &handlerMatchCallback);
    //分类声明
    matcher.addMatcher(objcCategoryDecl().bind("objcCategoryDecl"), &handlerMatchCallback);
    //分类定义
    matcher.addMatcher(objcCategoryImplDecl().bind("objcCategoryImplDecl"), &handlerMatchCallback);
    //方法声明
    matcher.addMatcher(objcMethodDecl().bind("objcMethodDecl"), &handlerMatchCallback);
    //变量声明或定义
    matcher.addMatcher(varDecl().bind("varDecl"), &handlerMatchCallback);
    //实例变量
    matcher.addMatcher(objcIvarDecl().bind("objcIvarDecl"), &handlerMatchCallback);
    //属性声明
    matcher.addMatcher(objcPropertyDecl().bind("objcPropertyDecl"), &handlerMatchCallback);
    //类消息表达式
    matcher.addMatcher(objcMessageExpr(isClassMessage()).bind("objcMessageExpr"), &handlerMatchCallback);
    //显式转换表达式
    matcher.addMatcher(explicitCastExpr().bind("explicitCastExpr"), &handlerMatchCallback);
    //typedef 声明
    matcher.addMatcher(typedefDecl().bind("typedefDecl"), &handlerMatchCallback);
    //字符串，小端
    matcher.addMatcher(stringLiteral().bind("stringLiteral"), &handlerMatchCallback);
}

void ObfASTConsumer::HandleTranslationUnit(ASTContext& Context)
{
    //运行匹配器
    matcher.matchAST(Context);
}
