/********************************************
* Parse.cpp                              *
* Created by gyj on 2020/6/5.
* Modified by lugf027 on 2020/6/21.            *
*********************************************/

///===------------------------------------------------------------------------------------===///
/// 注：在 SysY 源程序中不出现对 sylib.h 的文件包含；(尚未处理)
///    由 SysY 编译器分析和处理 SysY 程序静态库 libsysy.a 中7个库函数的调用
///===------------------------------------------------------------------------------------===///
#include "Parse.h"

#include <utility>

#include "../util/MyConstants.h"
#include "../util/Error.h"
#include "../ast/Expr.h"
#include "../ast/Stmt.h"

Parse::Parse() {
    Token token(this->lex.getToken());
    while (token.getType() != TOKEN_EOF) {
        tokens.emplace_back(token);  /// no need to create new class
        token = Token(this->lex.getToken());
    }
}

void Parse::startParse() {
    for (step = 0; step < this->tokens.size(); step++) {
        switch (tokens.at(step).getType()) {
            case TOKEN_INT:
                decls.push_back(parseFuncOrVar());
                break;
            case TOKEN_VOID:
                decls.push_back(parseFunc());
                break;
            case TOKEN_CONST:
                decls.push_back(parseConstantDecl());
                break;
            default:
                std::vector<std::string> expects{KEYWORD_INT, KEYWORD_VOID, KEYWORD_CONST};
                Error::errorParse(expects, tokens.at(step));
        }
    }
}

///===------------------------------------------------------------------------------------===///
/// @Variable /@FuncName -> can provide external links
///===------------------------------------------------------------------------------------===///

/// only int type/retType can reach here
Decl Parse::parseFuncOrVar() {
    SourceLocation typeLoc(tokens.at(step).getStartRow(), tokens.at(step).getStartColumn());

    std::string ident;
    SourceLocation identLoc;
    if (tokens.at(++step).getType() == TOKEN_IDENTIFIER) {
        ident = tokens.at(step).getIdentifierStr();
        identLoc = SourceLocation(tokens.at(step).getStartRow(), tokens.at(step).getStartColumn());
    } else {
        Error::errorParse("Identifier expected!", tokens.at(step));
    }

    /// int a( ...
    if (tokens.at(++step).getType() == CHAR_L_PARENTHESIS)
        return parseFunc(ident, TYPE_INT, typeLoc, identLoc);
        /// int a; ... int a= ... int a, ... int a[ ...
    else if (tokens.at(step).getType() == CHAR_EQ || tokens.at(step).getType() == CHAR_COMMA ||
             tokens.at(step).getType() == CHAR_L_BRACKET || tokens.at(step).getType() == CHAR_SEPARATOR)
        return parseVarDecl(ident, typeLoc, identLoc);
    else {
        std::vector<int> expects{CHAR_L_PARENTHESIS, CHAR_EQ, CHAR_COMMA, CHAR_L_BRACKET, CHAR_SEPARATOR};
        Error::errorParse(expects, tokens.at(step));
        exit(-1);
    }
} /// in fact, here is no need to return anything

/// only void retType can reach here
FuncDef Parse::parseFunc() {
    SourceLocation typeLoc(tokens.at(step).getStartRow(), tokens.at(step).getStartColumn());

    std::string ident;
    SourceLocation identLoc;
    if (tokens.at(++step).getType() == TOKEN_IDENTIFIER) {
        ident = tokens.at(step).getIdentifierStr();
        identLoc = SourceLocation(tokens.at(step).getStartRow(), tokens.at(step).getStartColumn());
    } else {
        Error::errorParse("Identifier expected!", tokens.at(step));
    }

    if (tokens.at(++step).getType() == CHAR_L_PARENTHESIS)
        return parseFunc(ident, TYPE_INT, typeLoc, identLoc);
    else {
        std::vector<int> expects{CHAR_L_PARENTHESIS};
        Error::errorParse(expects, tokens.at(step));
        exit(-1);
    }
}  /// in fact, here is no need to return anything

/// in this time, step is in the token ( after the function Identifier
FuncDef Parse::parseFunc(const std::string &ident, int retType, SourceLocation retTypeLoc, SourceLocation identLoc) {
    FuncFParams funcFParams = parseFunFParams();
    Stmt block = parseBlockStmts();
    return FuncDef(retType, ident, funcFParams, &block, retTypeLoc, identLoc);
}

/// in this time, step is in the token ( after the function Identifier
FuncFParams Parse::parseFunFParams() {
    std::vector<FuncFParam> funcFParams;
    SourceLocation lParenthesis, rParenthesis;

    /// parse (
    if (tokens.at(step).getType() == CHAR_L_PARENTHESIS)
        lParenthesis = SourceLocation(tokens.at(step).getStartRow(), tokens.at(step).getStartColumn());
    else
        Error::errorParse("\"(\" Expected in Function.", tokens.at(step));

    /// parse params, in this time, step is varType
    while (tokens.at(step).getType() == TOKEN_INT) {
        FuncFParam oneParam = parseFuncFParam();
        funcFParams.push_back(oneParam);
        /// in this time, step is , or )
        if (tokens.at(step).getType() == CHAR_COMMA) {
            step++;
        }
    }

    /// parse )
    if (tokens.at(step).getType() == CHAR_R_PARENTHESIS) {
        rParenthesis = SourceLocation(tokens.at(step).getStartRow(), tokens.at(step).getStartColumn());
    } else {
        Error::errorParse("\")\" or \",\" Expected in Function.", tokens.at(step));
    }
    step++;   /// jump ) in function

    return FuncFParams(funcFParams, lParenthesis, rParenthesis);
}

/// in this time, step is in the token varType after ( or ,
FuncFParam Parse::parseFuncFParam() {
    std::string ident;
    SourceLocation typeLoc, identLoc, lBracketLoc(-1, -1), rBracketLoc(-1, -1);
    if (tokens.at(step).getType() == TOKEN_INT) {
        typeLoc = SourceLocation(tokens.at(step).getStartRow(), tokens.at(step).getStartColumn());
    } else {
        Error::errorParse("Identifier type expected!", tokens.at(step));
    }

    if (tokens.at(++step).getType() == TOKEN_IDENTIFIER) {
        ident = tokens.at(step).getIdentifierStr();
        identLoc = SourceLocation(tokens.at(step).getStartRow(), tokens.at(step).getStartColumn());
    } else {
        Error::errorParse("Identifier expected!", tokens.at(step));
    }

    std::vector<Expr> subs;
    if (tokens.at(++step).getType() == CHAR_L_BRACKET) {
        lBracketLoc = SourceLocation(tokens.at(step).getStartRow(), tokens.at(step).getStartColumn());
        while (tokens.at(++step).getType() == CHAR_L_BRACKET) {
            if (tokens.at(++step).getType() == CHAR_R_BRACKET) {   /// a[]...
                subs.emplace_back(Expr());                            /// Here is not very sure
            } else {
                Expr arraySize = parseExpr();
                subs.emplace_back(arraySize);
                if (tokens.at(step).getType() == CHAR_R_BRACKET) {
                    rBracketLoc = SourceLocation(tokens.at(step).getStartRow(), tokens.at(step).getStartColumn());
                } else {
                    Error::errorParse("\"]\" expected!", tokens.at(step));
                }
            }
            step++;  /// jump ']'
        }
    }

    return FuncFParam(TYPE_INT, ident, subs, typeLoc, identLoc, lBracketLoc, rBracketLoc);
}

/// in this time, step should be varType
VarDecl Parse::parseVarDecl() {
    std::string ident;
    SourceLocation typeLoc, identLoc;

    if (tokens.at(step).getType() == TOKEN_INT) {
        typeLoc = SourceLocation(tokens.at(step).getStartRow(), tokens.at(step).getStartColumn());
    } else {
        Error::errorParse("Identifier type expected!", tokens.at(step));
    }

    if (tokens.at(++step).getType() == TOKEN_IDENTIFIER) {
        ident = tokens.at(step).getIdentifierStr();
        identLoc = SourceLocation(tokens.at(step).getStartRow(), tokens.at(step).getStartColumn());
    } else {
        Error::errorParse("Identifier expected!", tokens.at(step));
    }

    step++;  /// jump identifier
    return parseVarDecl(ident, typeLoc, identLoc);
}


/// in this time, step should be constant
ConstantDecl Parse::parseConstantDecl() {
    SourceLocation constantLoc(tokens.at(step).getStartRow(), tokens.at(step).getStartColumn());
    VarDecl varDecl = parseVarDecl();
    step++;  /// jump constant
    return ConstantDecl(varDecl, constantLoc);
}

/// in this time, step is in the token after the first Identifier
VarDecl Parse::parseVarDecl(std::string ident, SourceLocation typeLoc, SourceLocation firstIdentLoc) {
    std::vector<VarDef> varDefs = parseVarDef(std::move(ident), firstIdentLoc);
    return VarDecl(TYPE_INT, varDefs, typeLoc);
}

/// in this time, step is in the token after the first Identifier
std::vector<VarDef> Parse::parseVarDef(std::string firstIdent, SourceLocation firstIdentLoc) {
    std::vector<VarDef> varDefs;
    VarDef firstVarDef = parseOneVarDef(std::move(firstIdent), firstIdentLoc);
    varDefs.push_back(firstVarDef);
    while (tokens.at(step).getType() == CHAR_COMMA) {
        VarDef varDef = parseOneVarDef();
        varDefs.push_back(varDef);
    }
    if (tokens.at(step).getType() == CHAR_SEPARATOR)
        return varDefs;
    else {
        Error::errorParse("\";\" Expected", tokens.at(step));
        exit(-1);
    }

}  /// in fact, here is no need to return anything

/// in this time, step is in the token ',' before a new Identifier
VarDef Parse::parseOneVarDef() {
    std::string ident;
    SourceLocation identLoc;
    if (tokens.at(++step).getType() == TOKEN_IDENTIFIER) {
        ident = tokens.at(step).getIdentifierStr();
        identLoc = SourceLocation(tokens.at(step).getStartRow(), tokens.at(step).getStartColumn());
        return parseOneVarDef(ident, identLoc);
    } else {
        Error::errorParse("Identifier expected!", tokens.at(step));
        exit(-1);
    }
}  /// in fact, here is no need to return anything

/// in this time, step is in the token after the latest Identifier
VarDef Parse::parseOneVarDef(const std::string &ident, SourceLocation identLoc) {
    SourceLocation assignLoc(-1, -1);
    switch (tokens.at(step).getType()) {
        case CHAR_SEPARATOR:  /// a; ...
            return VarDef(ident, static_cast<std::vector<Expr>>(0), nullptr, identLoc,
                          SourceLocation(-1, -1));
        case CHAR_COMMA:      /// a, ...
            return VarDef(ident, static_cast<std::vector<Expr>>(0), nullptr, identLoc, assignLoc);
        case CHAR_EQ:         /// a = ...
        {
            assignLoc = SourceLocation(tokens.at(step).getStartRow(), tokens.at(step).getStartColumn());
            Expr initVal = parseExpr();
            return VarDef(ident, static_cast<std::vector<Expr>>(0), &initVal, identLoc, assignLoc);
        }
        case CHAR_L_BRACKET:  /// a[ ...
        {
            std::vector<Expr> subs;
            while (tokens.at(step).getType() == CHAR_L_BRACKET) {
                if (tokens.at(++step).getType() == CHAR_R_BRACKET) {   /// a[]...
                    subs.emplace_back(Expr());                            /// Here is not very sure
                } else {
                    Expr arraySize = parseExpr();
                    subs.emplace_back(arraySize);

                    if (tokens.at(step).getType() != CHAR_R_BRACKET) {
                        Error::errorParse("\"]\" expected!", tokens.at(step));
                    }
                    step++;  /// jump ']'
                }
            }
            if (tokens.at(step).getType() == CHAR_EQ) {
                assignLoc = SourceLocation(tokens.at(step).getStartRow(), tokens.at(step).getStartColumn());
                step++;  /// jump '='
                ArrayInitListExpr initListExpr = parseArrayInitListExpr();
                return VarDef(ident, subs, &initListExpr, identLoc, assignLoc);
            }
            return VarDef(ident, subs, nullptr, identLoc, assignLoc);
        }
        default: {
            std::vector<int> expects{CHAR_EQ, CHAR_COMMA, CHAR_L_BRACKET, CHAR_SEPARATOR};
            Error::errorParse(expects, tokens.at(step));
            exit(-1);
        }
    }
}  /// in fact, here is no need to return anything

/// in this time, step should be {
Stmt Parse::parseBlockStmts() {
    return Stmt();
}

/// in this time, token should be the first token in a expression
Expr Parse::parseExpr() {
    return Expr();
}

/// in this time, token should be {
ArrayInitListExpr Parse::parseArrayInitListExpr() {
    return ArrayInitListExpr(nullptr, SourceLocation(), SourceLocation());
}


