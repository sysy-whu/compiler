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

///===------------------------------------------------------------------------------------===///
/// @FuncName -> about funcDef, funcParams
///===------------------------------------------------------------------------------------===///

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
        /// token should be [ here, or others
        while (tokens.at(step).getType() == CHAR_L_BRACKET) {
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

///===------------------------------------------------------------------------------------===///
/// Variable -> about varDecl, varDef
///===------------------------------------------------------------------------------------===///

/// 变量声明 VarDecl → varType varDefs ';'
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

/// 常量声明 ConstDecl → 'const' varDecl
/// in this time, step should be constant
ConstantDecl Parse::parseConstantDecl() {
    SourceLocation constantLoc(tokens.at(step).getStartRow(), tokens.at(step).getStartColumn());
    VarDecl varDecl = parseVarDecl();
    step++;  /// jump constant
    return ConstantDecl(varDecl, constantLoc);
}

/// 变量声明 VarDecl → varType varDefs ';'
/// in this time, step is in the token after the first Identifier
VarDecl Parse::parseVarDecl(const std::string &ident, SourceLocation typeLoc, SourceLocation firstIdentLoc) {
    std::vector<VarDef> varDefs = parseVarDef(ident, firstIdentLoc);
    return VarDecl(TYPE_INT, varDefs, typeLoc);
}

/// 变量声明之 VarDefs -> VarDef { ',' VarDef } ';'
/// in this time, step is in the token after the first Identifier
std::vector<VarDef> Parse::parseVarDef(const std::string &firstIdent, SourceLocation firstIdentLoc) {
    std::vector<VarDef> varDefs;
    VarDef firstVarDef = parseOneVarDef(firstIdent, firstIdentLoc);
    varDefs.push_back(firstVarDef);
    while (tokens.at(step).getType() == CHAR_COMMA) {
        VarDef varDef = parseOneVarDef();
        varDefs.push_back(varDef);
    }
    if (tokens.at(step).getType() == CHAR_SEPARATOR) {
        step++;  /// jump ';'
        return varDefs;
    } else {
        Error::errorParse("\";\" Expected", tokens.at(step));
        exit(-1);
    }

}  /// in fact, here is no need to return anything

/// 单个变量定义 VarDef → Ident { '[' ConstExp ']' } | Ident { '[' ConstExp ']' } '=' InitVal
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

/// 单个变量定义 VarDef → Ident { '[' ConstExp ']' } | Ident { '[' ConstExp ']' } '=' InitVal
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

/// in this time, token should be {
ArrayInitListExpr Parse::parseArrayInitListExpr() {
    std::vector<Expr> subs;
    return ArrayInitListExpr(subs, SourceLocation(), SourceLocation());
}

///===------------------------------------------------------------------===///
/// Expr
///===------------------------------------------------------------------===///

/// in this time, token should be the first token in a expression
/// 表达式 Exp → AddExp 注：SysY 表达式是 int 型表达式
Expr Parse::parseExpr() {
    return parseAddExpr();
}

/// 加减表达式 AddExp → MulExp | AddExp ('+' | '−') MulExp
AddExpr Parse::parseAddExpr() {
    int opType = 0;
    SourceLocation opLoc(-1, -1);
    int stepAux = step;

    AddExpr addExpr = parseAddExpr();
    if (tokens.at(step).getType() == OP_BO_ADD || tokens.at(step).getType() == OP_BO_SUB) {
        opType = tokens.at(step).getType();
        opLoc = SourceLocation(tokens.at(step).getStartRow(), tokens.at(step).getStartColumn());
        step++;  /// jump op
        MulExpr mulExpr = parseMulExpr();
        return AddExpr(mulExpr, &addExpr, opType, opLoc);
    } else {
        step = stepAux;
        MulExpr mulExpr = parseMulExpr();
        return AddExpr(mulExpr, nullptr, opType, opLoc);
    }
}

/// 乘除模表达式 MulExp → UnaryExp | MulExp ('*' | '/' | '%') UnaryEx
MulExpr Parse::parseMulExpr() {
    int opType = 0;
    SourceLocation opLoc(-1, -1);
    int stepAux = step;

    MulExpr mulExpr = parseMulExpr();
    if (tokens.at(step).getType() == OP_BO_MUL || tokens.at(step).getType() == OP_BO_DIV ||
        tokens.at(step).getType() == OP_BO_REM) {
        opType = tokens.at(step).getType();
        opLoc = SourceLocation(tokens.at(step).getStartRow(), tokens.at(step).getStartColumn());
        step++;  /// jump op
        UnaryExpr unaryExpr = parseUnaryExpr();
        return MulExpr(unaryExpr, &mulExpr, opType, opLoc);
    } else {
        step = stepAux;
        UnaryExpr unaryExpr = parseUnaryExpr();
        return MulExpr(unaryExpr, nullptr, opType, opLoc);
    }
}

/// 一元表达式 UnaryExp → '(' Exp ')' | LValExpr | NumberExpr | CallExpr | UnaryOpType UnaryExp
/// CallExpr -> ident ( ;;; LValExpr -> ident / ident [ ;;; NumExpr -> constant
UnaryExpr Parse::parseUnaryExpr() {
    int opType = 0;
    SourceLocation opLoc(-1, -1), lParenthesisLoc(-1, -1), rParenthesisLoc(-1, -1);
    int stepAux = step;

    switch (tokens.at(step).getType()) {
        case CHAR_ADD:
        case CHAR_SUB:
        case CHAR_NOT: {
            opType = tokens.at(step).getType();
            opLoc = SourceLocation(tokens.at(step).getStartRow(), tokens.at(step).getStartColumn());
            UnaryExpr unaryExpr = parseUnaryExpr();
            return UnaryExpr(unaryExpr, opType, opLoc, lParenthesisLoc, rParenthesisLoc);
        }
        case CHAR_L_PARENTHESIS: {
            opType = tokens.at(step).getType();
            lParenthesisLoc = SourceLocation(tokens.at(step).getStartRow(), tokens.at(step).getStartColumn());
            step++;
            Expr expr = parseExpr();

            if (tokens.at(step).getType() == CHAR_R_PARENTHESIS) {
                rParenthesisLoc = SourceLocation(tokens.at(step).getStartRow(), tokens.at(step).getStartColumn());
                return UnaryExpr(expr, opType, opLoc, lParenthesisLoc, rParenthesisLoc);
            } else {
                Error::errorParse("\")\" Expected!", tokens.at(step));
            }
        }
        case TOKEN_NUMBER: {
            NumberExpr numberExpr = parseNumberExpr();
            return UnaryExpr(numberExpr, opType, opLoc, lParenthesisLoc, rParenthesisLoc);
        }
        case TOKEN_IDENTIFIER: {
            if (tokens.at(++step).getType() == CHAR_L_PARENTHESIS) {
                step = stepAux;  /// back to funcName
                CallExpr callExpr = parseCallExpr();
                return UnaryExpr(callExpr, opType, opLoc, lParenthesisLoc, rParenthesisLoc);
            } else {
                step = stepAux;  /// back to funcName
                LValExpr lValExpr = parseLValExpr();
                return UnaryExpr(lValExpr, opType, opLoc, lParenthesisLoc, rParenthesisLoc);
            }
        }
        default: {
            Error::errorParse("UnaryExpr Expected!", tokens.at(step));
            exit(-1);
        }
    }
}

/// 左值表达式 LVal → Ident {'[' Exp ']'}
LValExpr Parse::parseLValExpr() {
    std::string ident;
    std::vector<Expr> subs;
    SourceLocation identLoc, firstBracketLoc(-1, -1), lastBracketLoc(-1, -1);

    if (tokens.at(step).getType() == TOKEN_IDENTIFIER) {
        ident = tokens.at(step).getIdentifierStr();
        identLoc = SourceLocation(tokens.at(step).getStartRow(), tokens.at(step).getStartColumn());
    } else {
        Error::errorParse("Identifier expected!", tokens.at(step));
    }

    /// maybe [
    if (tokens.at(++step).getType() == CHAR_L_BRACKET) {
        firstBracketLoc = SourceLocation(tokens.at(step).getStartRow(), tokens.at(step).getStartColumn());
        while (tokens.at(step).getType() == CHAR_L_BRACKET) {
            if (tokens.at(++step).getType() == CHAR_R_BRACKET) {   /// a[]...
                subs.emplace_back(Expr());                            /// Here is not very sure
            } else {
                Expr arraySize = parseExpr();
                subs.emplace_back(arraySize);
                if (tokens.at(step).getType() == CHAR_R_BRACKET) {
                    lastBracketLoc = SourceLocation(tokens.at(step).getStartRow(), tokens.at(step).getStartColumn());
                } else {
                    Error::errorParse("\"]\" expected!", tokens.at(step));
                }
            }
            step++;  /// jump ']'
        }
    }

    return LValExpr(ident, subs, identLoc, firstBracketLoc, lastBracketLoc);

}

/// 数值 Number → IntConst
NumberExpr Parse::parseNumberExpr() {
    if (tokens.at(step).getType() == TOKEN_NUMBER) {
        SourceLocation numLoc(tokens.at(step).getStartRow(), tokens.at(step).getStartColumn());
        int numVal = tokens.at(step).getNumVal();
        step++;
        return NumberExpr(numVal, numLoc);
    } else {
        Error::errorParse("Number expected!", tokens.at(step));
        exit(-1);  /// in fact, it's no need.
    }
}

/// 逻辑或表达式 LOrExp → LAndExp | LOrExp '||' LAndExp
LOrExpr Parse::parseLOrExpr() {
    SourceLocation opLoc(-1, -1);
    int stepAux = step;

    LOrExpr lOrExpr = parseLOrExpr();
    if (tokens.at(step).getType() == OP_BO_OR) {
        opLoc = SourceLocation(tokens.at(step).getStartRow(), tokens.at(step).getStartColumn());
        step++;  /// jump op
        LAndExpr lAndExpr = parseLAndExpr();
        return LOrExpr(lAndExpr, &lOrExpr, opLoc);
    } else {
        step = stepAux;
        LAndExpr lAndExpr = parseLAndExpr();
        return LOrExpr(lAndExpr, nullptr, opLoc);
    }
}

/// 逻辑与表达式 LAndExp → EqExp | LAndExp '&&' EqExp
LAndExpr Parse::parseLAndExpr() {
    SourceLocation opLoc(-1, -1);
    int stepAux = step;

    LAndExpr lAndExpr = parseLAndExpr();
    if (tokens.at(step).getType() == OP_BO_AND) {
        opLoc = SourceLocation(tokens.at(step).getStartRow(), tokens.at(step).getStartColumn());
        step++;  /// jump op
        EqExpr eqExpr = parseEqExpr();
        return LAndExpr(eqExpr, &lAndExpr, opLoc);
    } else {
        step = stepAux;
        EqExpr eqExpr = parseEqExpr();
        return LAndExpr(eqExpr, nullptr, opLoc);
    }
}

/// 相等性表达式 EqExp → RelExp | EqExp ('==' | '!=') RelExp
EqExpr Parse::parseEqExpr() {
    int opType = 0;
    SourceLocation opLoc(-1, -1);
    int stepAux = step;

    EqExpr eqExpr = parseEqExpr();
    if (tokens.at(step).getType() == OP_BO_EQ || tokens.at(step).getType() == OP_BO_NEQ) {
        opLoc = SourceLocation(tokens.at(step).getStartRow(), tokens.at(step).getStartColumn());
        step++;  /// jump op
        RelExpr relExpr = parseRelExpr();
        return EqExpr(relExpr, &eqExpr, opType, opLoc);
    } else {
        step = stepAux;
        RelExpr relExpr = parseRelExpr();
        return EqExpr(relExpr, nullptr, opType, opLoc);
    }
}

/// 关系表达式 RelExp → AddExp | RelExp ('<' | '>' | '<=' | '>=') AddExp
RelExpr Parse::parseRelExpr() {
    int opType = 0;
    SourceLocation opLoc(-1, -1);
    int stepAux = step;

    RelExpr relExpr = parseRelExpr();
    if (tokens.at(step).getType() == OP_BO_GT || tokens.at(step).getType() == OP_BO_GTE ||
        tokens.at(step).getType() == OP_BO_LT || tokens.at(step).getType() == OP_BO_LTE) {
        opLoc = SourceLocation(tokens.at(step).getStartRow(), tokens.at(step).getStartColumn());
        step++;  /// jump op
        AddExpr addExpr = parseAddExpr();
        return RelExpr(addExpr, &relExpr, opType, opLoc);
    } else {
        step = stepAux;
        AddExpr addExpr = parseAddExpr();
        return RelExpr(addExpr, nullptr, opType, opLoc);
    }
}

/// 函数调用一元表达式 CallExpr -> Ident '(' [FuncRParams] ')'
CallExpr Parse::parseCallExpr() {
    std::string ident;
    SourceLocation callLoc;

    if (tokens.at(step).getType() == TOKEN_IDENTIFIER) {
        ident = tokens.at(step).getIdentifierStr();
        callLoc = SourceLocation(tokens.at(step).getStartRow(), tokens.at(step).getStartColumn());
    } else {
        Error::errorParse("Identifier expected!", tokens.at(step));
    }

    step++;  /// jump Identifier
    FuncRParams funcRParams = parseFuncRParams();

    return CallExpr(ident, funcRParams, callLoc);
}

/// 函数实参表 FuncRParams → '(' Exp { ',' Exp } ')'
FuncRParams Parse::parseFuncRParams() {
    std::vector<Expr> args;
    SourceLocation lParenthesisLoc, rParenthesisLoc;

    if (tokens.at(step).getType() == CHAR_L_PARENTHESIS) {
        lParenthesisLoc = SourceLocation(tokens.at(step).getStartRow(), tokens.at(step).getStartColumn());
    }
    step++;  /// jump (

    while (tokens.at(step).getType() != CHAR_R_PARENTHESIS) {
        Expr item = parseExpr();
        /// in this time, step is , or )
        if (tokens.at(step).getType() == CHAR_COMMA) {
            step++;
        }
        args.push_back(item);
    }

    /// parse )
    if (tokens.at(step).getType() == CHAR_R_PARENTHESIS) {
        rParenthesisLoc = SourceLocation(tokens.at(step).getStartRow(), tokens.at(step).getStartColumn());
    } else {
        Error::errorParse("\")\" or \",\" Expected in Function.", tokens.at(step));
    }
    step++;   /// jump ) in function

    return FuncRParams(args, lParenthesisLoc, rParenthesisLoc);
}

///===------------------------------------------------------------------------------------===///
/// Stmts
///===------------------------------------------------------------------------------------===///

/// in this time, step should be {
/// 语句块 Block -> '{' { BlockItem } '}'
Stmt Parse::parseBlockStmts() {
    SourceLocation lBraceLoc, rBraceLoc;

    if (tokens.at(step).getType() == CHAR_L_BRACE) {
        lBraceLoc = SourceLocation(tokens.at(step).getStartRow(), tokens.at(step).getStartColumn());
        step++;  /// jump {
    } else {
        Error::errorParse("\"{\" expected", tokens.at(step));
    }

    std::vector<Stmt> stmts = parseBlockItems();

    if (tokens.at(step).getType() == CHAR_R_BRACE) {
        rBraceLoc = SourceLocation(tokens.at(step).getStartRow(), tokens.at(step).getStartColumn());
        step++;  /// jump }
    } else {
        Error::errorParse("\"}\" expected", tokens.at(step));
    }

    return BlockStmt(stmts, lBraceLoc, rBraceLoc);
}

/// 语句块项 BlockItem → VarDecl | ConstantDecl | Stmt
std::vector<Stmt> Parse::parseBlockItems() {
    std::vector<Stmt> stmts;

    /// 要求 parseItem 返回前吃掉 ';'
    while (tokens.at(step).getType() != CHAR_R_BRACE) {
        if (tokens.at(step).getType() == TOKEN_CONST) {
            ConstantDecl constantDecl = parseConstantDecl();
            stmts.push_back(DeclStmt(&constantDecl));
        } else if (tokens.at(step).getType() == TOKEN_INT) {
            VarDecl varDecl = parseVarDecl();
            stmts.push_back(DeclStmt(&varDecl));
        } else {
            stmts.push_back(parseOneStmt());
        }
    }

    return stmts;
}

/// 语句 Stmt -> LVal '=' Exp ';'
///           | [Exp] ';'
///           | Block
///           | 'if' '( Cond ')' Stmt [ 'else' Stmt ]
///           | 'while' '(' Cond ')' Stmt
///           | 'break' ';'
///           | 'continue' ';'
///           | 'return' [Exp] ';'
/// 条件表达式 Cond → LOrExp
Stmt Parse::parseOneStmt() {
    switch (tokens.at(step).getType()) {
        case CHAR_L_BRACE:
            return parseBlockStmts();
        case TOKEN_IF: {
            if (tokens.at(++step).getType() != CHAR_L_PARENTHESIS) {
                Error::errorParse("\"(\" expected!", tokens.at(step));
            }
            SourceLocation lParenthesisLoc = SourceLocation(tokens.at(step).getStartRow(),
                                                            tokens.at(step).getStartColumn());

            step++;  /// jump "("
            Expr condExpr = parseLOrExpr();

            if (tokens.at(++step).getType() != CHAR_L_PARENTHESIS) {
                Error::errorParse("\"(\" expected!", tokens.at(step));
            }
            SourceLocation rParenthesisLoc = SourceLocation(tokens.at(step).getStartRow(),
                                                            tokens.at(step).getStartColumn());
            step++;  /// jump ")"
            Stmt thenStmt = parseOneStmt();

            if (tokens.at(step).getType() == TOKEN_ELSE) {
                step++;   /// jump "else"
                Stmt elseStmt = parseOneStmt();
                return IfStmt(&condExpr, thenStmt, &elseStmt, lParenthesisLoc, rParenthesisLoc);
            } else {
                return IfStmt(&condExpr, thenStmt, nullptr, lParenthesisLoc, rParenthesisLoc);
            }
        }
        case TOKEN_WHILE: {
            if (tokens.at(++step).getType() != CHAR_L_PARENTHESIS) {
                Error::errorParse("\"(\" expected!", tokens.at(step));
            }
            SourceLocation lParenthesisLoc = SourceLocation(tokens.at(step).getStartRow(),
                                                            tokens.at(step).getStartColumn());

            step++;  /// jump "("
            Expr condExpr = parseLOrExpr();

            if (tokens.at(++step).getType() != CHAR_L_PARENTHESIS) {
                Error::errorParse("\"(\" expected!", tokens.at(step));
            }
            SourceLocation rParenthesisLoc = SourceLocation(tokens.at(step).getStartRow(),
                                                            tokens.at(step).getStartColumn());
            step++;  /// jump ")"
            Stmt bodyStmt = parseOneStmt();

            return WhileStmt(&condExpr, bodyStmt, lParenthesisLoc, rParenthesisLoc);
        }
        case TOKEN_BREAK: {
            SourceLocation breakLoc(tokens.at(step).getStartRow(), tokens.at(step).getStartColumn());
            if (tokens.at(++step).getType() != CHAR_SEPARATOR) {
                Error::errorParse("\";\" expected", tokens.at(step));
            }
            step++;  /// jump ';'
            return BreakStmt(breakLoc);
        }
        case TOKEN_CONTINUE: {
            SourceLocation continueLoc(tokens.at(step).getStartRow(), tokens.at(step).getStartColumn());
            if (tokens.at(++step).getType() != CHAR_SEPARATOR) {
                Error::errorParse("\";\" expected", tokens.at(step));
            }
            step++;  /// jump ';'
            return ContinueStmt(continueLoc);
        }
        case TOKEN_RETURN: {
            SourceLocation returnLoc(tokens.at(step).getStartRow(), tokens.at(step).getStartColumn());
            if (tokens.at(++step).getType() == CHAR_SEPARATOR) {
                return ReturnExpr(nullptr, returnLoc);
            }
            Expr retValue = parseExpr();

            if (tokens.at(step).getType() != CHAR_SEPARATOR) {
                Error::errorParse("\";\" expected", tokens.at(step));
            }
            step++;  /// jump ';'
            return ReturnExpr(&retValue, returnLoc);
        }
        default:
            Error::errorParse("Error statement!", tokens.at(step));
            exit(-1);  /// in fact, here is no need to return or exit
    }
}


