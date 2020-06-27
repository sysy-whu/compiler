#include "Parse.h"
#include "../util/MyConstants.h"
#include "../util/Error.h"
#include "../util/Dump.h"

Parse::Parse() {
    Lex lex;
    Token token(lex.getToken());
    while (token.getType() != TOKEN_EOF) {
        tokens.emplace_back(token);  /// no need to create new class
        token = Token(lex.getToken());
    }
}

/**
 * AST -> {ConstDecl | VarDecl |FuncDef }
 * @param ast 返回值
 */
void Parse::parseAST() {
    auto *decls = new std::vector<Decl *>();
    for (step = 0; step < this->tokens.size();) {
        switch (tokens.at(step).getType()) {
            case TOKEN_INT: {
                int stepAux = step;
                if (tokens.at(++step).getType() == TOKEN_IDENTIFIER) {
                    std::string ident = tokens.at(step).getIdentifierStr();
                    if (tokens.at(++step).getType() == CHAR_L_PARENTHESIS) {
                        step = stepAux;
                        FuncDef *funcDef = parseFuncDef();
                        auto *decl = new Decl(nullptr, nullptr, funcDef);
                        decls->emplace_back(decl);
                    } else {
                        step = stepAux;
                        VarDecl *varDecl = parseVarDecl();
                        auto *decl = new Decl(nullptr, varDecl, nullptr);
                        decls->emplace_back(decl);
                    }
                } else {
                    Error::errorParse("Identifier Expected!", tokens.at(step));
                }
                break;
            }
            case TOKEN_VOID: {
                FuncDef *funcDef = parseFuncDef();
                auto *decl = new Decl(nullptr, nullptr, funcDef);
                decls->emplace_back(decl);
                break;
            }
            case TOKEN_CONST: {
                ConstDecl *constDecl = parseConstDecl();
                auto *decl = new Decl(constDecl, nullptr, nullptr);
                decls->emplace_back(decl);
                break;
            }
            case TOKEN_EOF:
                break;
            default:
                std::vector<std::string> expects{KEYWORD_INT, KEYWORD_VOID, KEYWORD_CONST};
                Error::errorParse(expects, tokens.at(step));
        }
    }

    ast = new AST(decls);

//    std::cout << &this->ast << std::endl;
//    std::cout << this->ast->getDecls()->size() << std::endl;
}

/**
 * 常量声明 ConstDecl -> 'const' BType ConstDef { ',' ConstDef } ';'
 * 此刻 token 应为 const
 * @return
 */
ConstDecl *Parse::parseConstDecl() {
    auto *locs = new std::vector<SourceLocation *>();
    auto *constDefs = new std::vector<ConstDef *>();

    if (tokens.at(step).getType() == TOKEN_CONST) {
        auto *constLoc = new SourceLocation(tokens.at(step).getRow(), tokens.at(step).getStartColumn());
        locs->push_back(constLoc);
    }

    if (tokens.at(++step).getType() == TOKEN_INT) {
        auto *intLoc = new SourceLocation(tokens.at(step).getRow(), tokens.at(step).getStartColumn());
        locs->push_back(intLoc);
    }

    step++;  // eat BType
    constDefs->push_back(parseConstDef());
    while (tokens.at(step).getType() == CHAR_COMMA) {
        step++;  // eat ','
        constDefs->push_back(parseConstDef());
    }

    if (tokens.at(step).getType() != CHAR_SEPARATOR) {
        Error::errorParse("; Expected", tokens.at(step));
    }
    step++;  // eat ';'

    auto *constDecl = new ConstDecl(TYPE_INT, constDefs, locs);
    return constDecl;
}

/**
 * 常数定义 ConstDef → Ident { '[' ConstExp ']' } '=' ConstInitVal
 * @return
 */
ConstDef *Parse::parseConstDef() {
    auto *locs = new std::vector<SourceLocation *>();
    auto *constExpsInner = new std::vector<ConstExp *>();
    std::string ident;

    if (tokens.at(step).getType() == TOKEN_IDENTIFIER) {
        auto *identLoc = new SourceLocation(tokens.at(step).getRow(), tokens.at(step).getStartColumn());
        locs->push_back(identLoc);
        ident = tokens.at(step).getIdentifierStr();
    } else {
        Error::errorParse("Identifier Expected", tokens.at(step));
    }

    while (tokens.at(++step).getType() == CHAR_L_BRACKET) {
        auto *lLoc = new SourceLocation(tokens.at(step).getRow(), tokens.at(step).getStartColumn());
        locs->push_back(lLoc);

        step++;  // eat [
        ConstExp *constExp = parseConstExp();  // 返回后位置 ]
        constExpsInner->push_back(constExp);

        if (tokens.at(step).getType() == CHAR_R_BRACKET) {
            auto *rLoc = new SourceLocation(tokens.at(step).getRow(), tokens.at(step).getStartColumn());
            locs->push_back(rLoc);
        } else {
            Error::errorParse("']' Expected", tokens.at(step));
        }
        step++;  // eat ]
    }

    if (tokens.at(step).getType() == OP_ASSIGN) {
        auto *assignLoc = new SourceLocation(tokens.at(step).getRow(), tokens.at(step).getStartColumn());
        locs->emplace_back(assignLoc);
    } else {
        Error::errorParse("'=' or '[' Expected", tokens.at(step));
    }

    step++;  // eat =
    ConstInitVal *constInitVal = parseConstInitVal();

    auto *constDefRet = new ConstDef(ident.c_str(), constExpsInner, constInitVal, locs);
    return constDefRet;
}

/**
 * 常量初值 ConstInitVal → ConstExp | '{' [ ConstInitVal { ',' ConstInitVal } ] '}'
 * @return
 */
ConstInitVal *Parse::parseConstInitVal() {
    auto *locs = new std::vector<SourceLocation *>();
    auto *constInitValsInner = new std::vector<ConstInitVal *>();
    ConstExp *constExpInner = nullptr;

    if (tokens.at(step).getType() == CHAR_L_BRACE) {  // constArray
        auto *lBraceLoc = new SourceLocation(tokens.at(step).getRow(), tokens.at(step).getStartColumn());
        locs->push_back(lBraceLoc);
        step++;  // eat '{'

        if (tokens.at(step).getType() == CHAR_R_BRACE) {  // {}
            auto *rBraceLoc = new SourceLocation(tokens.at(step).getRow(), tokens.at(step).getStartColumn());
            locs->push_back(rBraceLoc);
        } else {
            ConstInitVal *constInitValFirst = parseConstInitVal();
            constInitValsInner->push_back(constInitValFirst);

            while (tokens.at(step).getType() == CHAR_COMMA) {
                step++;  // eat ,
                ConstInitVal *constInitValBack = parseConstInitVal();
                constInitValsInner->push_back(constInitValBack);
            }

            if (tokens.at(step).getType() == CHAR_R_BRACE) {
                auto *rBraceLoc = new SourceLocation(tokens.at(step).getRow(), tokens.at(step).getStartColumn());
                locs->push_back(rBraceLoc);
            } else {
                Error::errorParse("'}' or ',' Expected!", tokens.at(step));
            }
        }
    } else {  // const
        constExpInner = parseConstExp();
    }

    auto *constInitValRet = new ConstInitVal(constExpInner, constInitValsInner, locs);
    return constInitValRet;
}

/**
 * 变量声明 VarDecl → BType VarDef { ',' VarDef } ';'
 * @return
 */
VarDecl *Parse::parseVarDecl() {
    int varType = TOKEN_INT;
    auto *varDefsInner = new std::vector<VarDef *>();

    if (tokens.at(step).getType() == TOKEN_INT) {
        step++;
    } else {
        Error::errorParse("VarType Expected!", tokens.at(step));
    }

    VarDef *varDefFirst = parseVarDef();
    varDefsInner->emplace_back(varDefFirst);
    while (tokens.at(step).getType() == CHAR_COMMA) {
        step++;  // eat ','
        VarDef *varDefBack = parseVarDef();
        varDefsInner->emplace_back(varDefBack);
    }

    if (tokens.at(step).getType() != CHAR_SEPARATOR) {
        Error::errorParse("; Expected", tokens.at(step));
    }
    step++;  // eat ';'

    auto *varDeclRet = new VarDecl(varType, varDefsInner);
    return varDeclRet;
}

/**
 * 变量定义 VarDef → Ident { '[' ConstExp ']' }
 *               | Ident { '[' ConstExp ']' } '=' InitVal
 * @return
 */
VarDef *Parse::parseVarDef() {
    std::string ident;
    InitVal *initValInner = nullptr;
    auto *constExps = new std::vector<ConstExp *>();
    auto *locs = new std::vector<SourceLocation *>();

    if (tokens.at(step).getType() == TOKEN_IDENTIFIER) {
        ident = tokens.at(step).getIdentifierStr();\
        step++;
    }

    while (tokens.at(step).getType() == CHAR_L_BRACKET) {
        auto *lBracketLoc = new SourceLocation(tokens.at(step).getRow(), tokens.at(step).getStartColumn());
        locs->emplace_back(lBracketLoc);
        step++;  // eat '['

        ConstExp *constExp = parseConstExp();
        constExps->emplace_back(constExp);

        if (tokens.at(step).getType() == CHAR_R_BRACKET) {
            auto *rBracketLoc = new SourceLocation(tokens.at(step).getRow(), tokens.at(step).getStartColumn());
            locs->emplace_back(rBracketLoc);
            step++;  // eat ']'
        }
    }

    if (tokens.at(step).getType() == OP_ASSIGN) {
        auto *assignLoc = new SourceLocation(tokens.at(step).getRow(), tokens.at(step).getStartColumn());
        locs->emplace_back(assignLoc);
        step++;  // eat '['
        initValInner = parseInitVal();
    }

    auto *varDef = new VarDef(ident.c_str(), constExps, initValInner, locs);
    return varDef;
}

/**
 * 变量初值 InitVal → Exp
 *                | '{' [ InitVal { ',' InitVal } ] '}'
 * @return
 */
InitVal *Parse::parseInitVal() {
    Exp *expInner = nullptr;
    auto *initVals = new std::vector<InitVal *>();
    auto *locs = new std::vector<SourceLocation *>();

    if (tokens.at(step).getType() == CHAR_L_BRACE) {
        auto *lBraceLoc = new SourceLocation(tokens.at(step).getRow(), tokens.at(step).getStartColumn());
        locs->push_back(lBraceLoc);
        step++;

        if (tokens.at(step).getType() == CHAR_R_BRACE) {  // 元素需要全部初始化为0
            auto *rBraceLoc = new SourceLocation(tokens.at(step).getRow(), tokens.at(step).getStartColumn());
            locs->push_back(rBraceLoc);
            step++;
        } else {
            InitVal *initValFirst = parseInitVal();
            initVals->emplace_back(initValFirst);

            while (tokens.at(step).getType() == CHAR_COMMA) {
                step++;  // eat ,
                InitVal *initValBack = parseInitVal();
                initVals->emplace_back(initValBack);
            }

            if (tokens.at(step).getType() == CHAR_R_BRACE) {
                auto *rBraceLoc = new SourceLocation(tokens.at(step).getRow(), tokens.at(step).getStartColumn());
                locs->push_back(rBraceLoc);
                step++;
            } else {
                Error::errorParse("'}' or ',' Expected!", tokens.at(step));
            }
        }
    } else {
        expInner = parseExp();
    }

    auto *initValRet = new InitVal(expInner, initVals, locs);
    return initValRet;
}

/**
 * 函数定义 FuncDef → funcType Ident '(' [FuncFParams] ')' Block
 * @return
 */
FuncDef *Parse::parseFuncDef() {
    int funcType = -1;
    std::string ident;
    FuncFParams *funcFParamsInner = nullptr;
    auto *locs = new std::vector<SourceLocation *>();

    if (tokens.at(step).getType() == TOKEN_INT || tokens.at(step).getType() == TOKEN_VOID) {
        funcType = tokens.at(step).getType();
        step++;
    } else {
        Error::errorParse("Function return type Expected!", tokens.at(step));
    }

    if (tokens.at(step).getType() == TOKEN_IDENTIFIER) {
        ident = tokens.at(step).getIdentifierStr();
        step++;
    } else {
        Error::errorParse("Identifier Wxpected!", tokens.at(step));
    }

    if (tokens.at(step).getType() == CHAR_L_PARENTHESIS) {
        auto *lParenthesisLoc = new SourceLocation(tokens.at(step).getRow(), tokens.at(step).getStartColumn());
        locs->emplace_back(lParenthesisLoc);
        step++;
    } else {
        Error::errorParse("'(' expected!", tokens.at(step));
    }

    if (tokens.at(step).getType() != CHAR_R_PARENTHESIS) {
        funcFParamsInner = parseFuncFParams();
    }

    if (tokens.at(step).getType() == CHAR_R_PARENTHESIS) {
        auto *rParenthesisLoc = new SourceLocation(tokens.at(step).getRow(), tokens.at(step).getStartColumn());
        locs->emplace_back(rParenthesisLoc);
        step++;
    } else {
        Error::errorParse("')' expected!", tokens.at(step));
    }

    Block *blockInner = parseBlock();

    auto *funcDef = new FuncDef(funcType, ident.c_str(), funcFParamsInner, blockInner, locs);
    return funcDef;
}

/**
 * 函数形参表 FuncFParams → FuncFParam { ',' FuncFParam }
 * @return
 */
FuncFParams *Parse::parseFuncFParams() {
    auto *funcFParamsInner = new std::vector<FuncFParam *>();

    FuncFParam *funcFParamFirst = parseFuncFParam();
    funcFParamsInner->emplace_back(funcFParamFirst);
    while (tokens.at(step).getType() == CHAR_COMMA) {
        step++;  // eat ','
        FuncFParam *funcFParamBack = parseFuncFParam();
        funcFParamsInner->emplace_back(funcFParamBack);
    }

    auto *funcParamRet = new FuncFParams(funcFParamsInner);
    return funcParamRet;
}

/**
 * 函数形参 FuncFParam → BType Ident ['[' ']' { '[' Exp ']' }]
 * @return
 */
FuncFParam *Parse::parseFuncFParam() {
    int paramType = TOKEN_INT;
    std::string ident;
    auto *expsInner = new std::vector<Exp *>();
    auto *locs = new std::vector<SourceLocation *>();

    if (tokens.at(step).getType() == TOKEN_INT) {
        auto *paramTypeLoc = new SourceLocation(tokens.at(step).getRow(), tokens.at(step).getStartColumn());
        locs->emplace_back(paramTypeLoc);
        step++;
    } else {
        Error::errorParse("Parameter type Expected!", tokens.at(step));
    }

    if (tokens.at(step).getType() == TOKEN_IDENTIFIER) {
        auto *identLoc = new SourceLocation(tokens.at(step).getRow(), tokens.at(step).getStartColumn());
        locs->emplace_back(identLoc);
        step++;
    } else {
        Error::errorParse("Parameter type Expected!", tokens.at(step));
    }

    if (tokens.at(step).getType() == CHAR_L_BRACKET) {  // array
        auto *lBracketLocFirst = new SourceLocation(tokens.at(step).getRow(), tokens.at(step).getStartColumn());
        locs->emplace_back(lBracketLocFirst);
        step++;
        if (tokens.at(step).getType() == CHAR_R_BRACKET) {  // array
            auto *rBracketLocFirst = new SourceLocation(tokens.at(step).getRow(), tokens.at(step).getStartColumn());
            locs->emplace_back(rBracketLocFirst);
            step++;
        } else {
            Error::errorParse("']' Expected!", tokens.at(step));
        }

        // 多维数组，后续维数
        while (tokens.at(step).getType() == CHAR_L_BRACKET) {
            auto *lBracketLocBack = new SourceLocation(tokens.at(step).getRow(), tokens.at(step).getStartColumn());
            locs->emplace_back(lBracketLocBack);
            step++;
            Exp *exp = parseExp();
            expsInner->emplace_back(exp);
            if (tokens.at(step).getType() == CHAR_R_BRACKET) {  // array
                auto *rBracketLocBack = new SourceLocation(tokens.at(step).getRow(), tokens.at(step).getStartColumn());
                locs->emplace_back(rBracketLocBack);
                step++;
            } else {
                Error::errorParse("']' Expected!", tokens.at(step));
            }
        }
    }

    auto *funcFParamRet = new FuncFParam(paramType, ident.c_str(), expsInner, locs);
    return funcFParamRet;
}

/**
 * 语句块 Block → '{' { BlockItem } '}'
 * @return
 */
Block *Parse::parseBlock() {
    auto *locs = new std::vector<SourceLocation *>();
    auto *blockItemInner = new std::vector<BlockItem *>();

    if (tokens.at(step).getType() == CHAR_L_BRACE) {
        auto *lBraceLoc = new SourceLocation(tokens.at(step).getRow(), tokens.at(step).getStartColumn());
        locs->emplace_back(lBraceLoc);
        step++;  // eat '{'
    }

    while (tokens.at(step).getType() != CHAR_R_BRACE) {
        BlockItem *blockItem = parseBlockItem();
        blockItemInner->emplace_back(blockItem);
    }

    if (tokens.at(step).getType() == CHAR_R_BRACE) {
        auto *rBraceLoc = new SourceLocation(tokens.at(step).getRow(), tokens.at(step).getStartColumn());
        locs->emplace_back(rBraceLoc);
        step++;  // eat '}'
    }

    auto *blockRet = new Block(blockItemInner, locs);
    return blockRet;
}

/**
 * 语句块项 BlockItem → VarDecl | ConstVarDecl | Stmt
 * @return
 */
BlockItem *Parse::parseBlockItem() {
    ConstDecl *constDecl = nullptr;
    VarDecl *varDecl = nullptr;
    Stmt *stmtInner = nullptr;

    if (tokens.at(step).getType() == TOKEN_INT) {
        varDecl = parseVarDecl();
    } else if (tokens.at(step).getType() == TOKEN_CONST) {
        constDecl = parseConstDecl();
    } else {
        stmtInner = parseStmt();
    }

    auto *blockItemRet = new BlockItem(constDecl, varDecl, stmtInner);
    return blockItemRet;
}

/**
 * 语句 Stmt → LVal '=' Exp ';'
 *          | [Exp] ';'
 *          | Block
 *          | 'if' '(' Cond ')' Stmt [ 'else' Stmt ]
 *          | 'while' '(' Cond ')' Stmt
 *          | 'break' ';'
 *          | 'continue' ';'
 *          | 'return' [Exp] ';'
 * @return
 */
Stmt *Parse::parseStmt() {
    int stmtType;
//    std::string ident;
    Exp *expInner = nullptr;
    LVal *lValInner = nullptr;
    Cond *condInner = nullptr;
    Stmt *stmtBodyInner = nullptr;
    Stmt *stmtElseInner = nullptr;
    Block *blockInner = nullptr;
    auto *locs = new std::vector<SourceLocation *>();

    switch (tokens.at(step).getType()) {
        case CHAR_L_BRACE: {  // Block
            stmtType = STMT_BLOCK;
            blockInner = parseBlock();
            break;
        }
        case TOKEN_IF: {  // 'if' '(' Cond ')' Stmt [ 'else' Stmt ]
            stmtType = STMT_IF;
            auto *ifLoc = new SourceLocation(tokens.at(step).getRow(), tokens.at(step).getStartColumn());
            locs->emplace_back(ifLoc);
            step++;  // eat 'if'

            if (tokens.at(step).getType() == CHAR_L_PARENTHESIS) {
                auto *lParenthesisLoc = new SourceLocation(tokens.at(step).getRow(), tokens.at(step).getStartColumn());
                locs->emplace_back(lParenthesisLoc);
                step++;  // eat '('
            } else {
                Error::errorParse("'(' Expected!", tokens.at(step));
            }
            condInner = parseCond();  // if->condition
            if (tokens.at(step).getType() == CHAR_R_PARENTHESIS) {
                auto *rParenthesisLoc = new SourceLocation(tokens.at(step).getRow(), tokens.at(step).getStartColumn());
                locs->emplace_back(rParenthesisLoc);
                step++;  // eat ')'
            } else {
                Error::errorParse("')' Expected!", tokens.at(step));
            }
            stmtBodyInner = parseStmt();  // if->body

            if (tokens.at(step).getType() == TOKEN_ELSE) {
                auto *elseParenthesisLoc = new SourceLocation(tokens.at(step).getRow(),
                                                              tokens.at(step).getStartColumn());
                locs->emplace_back(elseParenthesisLoc);
                step++;  // eat 'else'
                stmtElseInner = parseStmt();  // else->body
            }
            break;
        }
        case TOKEN_WHILE: {  // 'while' '(' Cond ')' Stmt
            stmtType = STMT_WHILE;
            auto *whileLoc = new SourceLocation(tokens.at(step).getRow(), tokens.at(step).getStartColumn());
            locs->emplace_back(whileLoc);
            step++;  // eat 'while'

            if (tokens.at(step).getType() == CHAR_L_PARENTHESIS) {
                auto *lParenthesisLoc = new SourceLocation(tokens.at(step).getRow(), tokens.at(step).getStartColumn());
                locs->emplace_back(lParenthesisLoc);
                step++;  // eat '('
            } else {
                Error::errorParse("'(' Expected!", tokens.at(step));
            }
            condInner = parseCond();  // while->condition
            if (tokens.at(step).getType() == CHAR_R_PARENTHESIS) {
                auto *rParenthesisLoc = new SourceLocation(tokens.at(step).getRow(), tokens.at(step).getStartColumn());
                locs->emplace_back(rParenthesisLoc);
                step++;  // eat ')'
            } else {
                Error::errorParse("')' Expected!", tokens.at(step));
            }
            stmtBodyInner = parseStmt();  // while->body
            break;
        }
        case TOKEN_RETURN: {  // 'return' [Exp] ';'
            stmtType = STMT_RETURN;
            auto *continueLoc = new SourceLocation(tokens.at(step).getRow(), tokens.at(step).getStartColumn());
            locs->emplace_back(continueLoc);
            step++;  // eat 'return'

            if (tokens.at(step).getType() != CHAR_SEPARATOR) {
                expInner = parseExp();
            }

            if (tokens.at(step).getType() != CHAR_SEPARATOR) {
                Error::errorParse("';' Expected", tokens.at(step));
            }
            step++;  // eat ';'
            break;
        }
        case TOKEN_BREAK: {  // 'break' ';'
            stmtType = STMT_BREAK;
            auto *breakLoc = new SourceLocation(tokens.at(step).getRow(), tokens.at(step).getStartColumn());
            locs->emplace_back(breakLoc);
            step++;  // eat 'break'

            if (tokens.at(step).getType() != CHAR_SEPARATOR) {
                Error::errorParse("';' Expected", tokens.at(step));
            }
            step++;  // eat ';'
            break;
        }
        case TOKEN_CONTINUE: {  // 'continue' ';'
            stmtType = STMT_CONTINUE;
            auto *continueLoc = new SourceLocation(tokens.at(step).getRow(), tokens.at(step).getStartColumn());
            locs->emplace_back(continueLoc);
            step++;  // eat 'continue'

            if (tokens.at(step).getType() != CHAR_SEPARATOR) {
                Error::errorParse("';' Expected", tokens.at(step));
            }
            step++;  // eat ';'
            break;
        }
        case TOKEN_IDENTIFIER: {  // LVal '=' Exp ';' || [Exp] ';'
            // LVal: ident {'['Exp']'}
            // Exp->Add->MulExp->UnaryExp: ident '('[funcRParam]')'
            // Exp->Add->MulExp->UnaryExp->PrimaryExp->LVal: ident {'['Exp']'}
            int stepAux = step;
//            ident = tokens.at(step).getIdentifierStr();
            step++;  // eat ident
            if (tokens.at(step).getType() == CHAR_L_BRACKET || tokens.at(step).getType() == OP_ASSIGN) {
                stmtType = STMT_LVAL_ASSIGN;
                step = stepAux;
                lValInner = parseLVal();

                if (tokens.at(step).getType() != OP_ASSIGN) {
                    Error::errorParse("'=' Expected", tokens.at(step));
                }
                step++;  // eat '='

                expInner = parseExp();

                if (tokens.at(step).getType() != CHAR_SEPARATOR) {
                    Error::errorParse("';' Expected", tokens.at(step));
                }
                step++;  // eat ';'

                break;
            } else {
                step = stepAux;
                expInner = parseExp();
                stmtType = STMT_EXP;

                if (tokens.at(step).getType() != CHAR_SEPARATOR) {
                    Error::errorParse("';' Expected", tokens.at(step));
                }
                step++;  // eat ';'
                break;
            }
        }
        case CHAR_SEPARATOR: {  // ';'
            stmtType = STMT_EXP_BLANK;
            step++;  // eat ';'
            break;
        }
        default: { // Exp ';'
            stmtType = STMT_EXP;
            expInner = parseExp();

            if (tokens.at(step).getType() != CHAR_SEPARATOR) {
                Error::errorParse("';' Expected", tokens.at(step));
            }
            step++;  // eat ';'
        }
    }

    auto *stmtRet = new Stmt(stmtType, lValInner, expInner, blockInner, condInner, stmtBodyInner, stmtElseInner, locs);
    return stmtRet;
}

/**
 * 表达式 Exp → AddExp 注：SysY 表达式是 int 型表达式
 * @return
 */
Exp *Parse::parseExp() {
    AddExp *addExp = parseAddExp();
    auto *exp = new Exp(addExp);
    return exp;
}

/**
 * 条件表达式 Cond → LOrExp
 * @return
 */
Cond *Parse::parseCond() {
    LOrExp *lOrExpInner = parseLOrExp();
    auto *condRet = new Cond(lOrExpInner);
    return condRet;
}

/**
 * 左值表达式 LVal → Ident {'[' Exp ']'}
 * @return
 */
LVal *Parse::parseLVal() {
    std::string ident;
    auto *expsInner = new std::vector<Exp *>();
    auto *locs = new std::vector<SourceLocation *>();

    if (tokens.at(step).getType() == TOKEN_IDENTIFIER) {
        ident = tokens.at(step).getIdentifierStr();
    } else {
        Error::errorParse("Identifier Expected!", tokens.at(step));
    }
    step++; // eat identifier

    while (tokens.at(step).getType() == CHAR_L_BRACKET) {
        auto *lBracketLoc = new SourceLocation(tokens.at(step).getRow(), tokens.at(step).getStartColumn());
        locs->emplace_back(lBracketLoc);
        step++;  // eat [

        Exp *exp = parseExp();
        expsInner->emplace_back(exp);

        if (tokens.at(step).getType() == CHAR_R_BRACKET) {
            auto *rBracketLoc = new SourceLocation(tokens.at(step).getRow(), tokens.at(step).getStartColumn());
            locs->emplace_back(rBracketLoc);
            step++;  // eat ]
        } else {
            Error::errorParse("']' Expected", tokens.at(step));
        }
    }

    auto *lValRet = new LVal(ident.c_str(), expsInner, locs);
    return lValRet;
}

/**
 * 基本表达式 PrimaryExp → '(' Exp ')' | LVal | IntConst
 * @return
 */
PrimaryExp *Parse::parsePrimaryExp() {
    Exp *expInner = nullptr;
    int numberInner = -1;
    LVal *lValInner = nullptr;
    auto *locs = new std::vector<SourceLocation *>();

    if (tokens.at(step).getType() == TOKEN_NUMBER) {
        numberInner = tokens.at(step).getNumVal();
        step++;  // eat number
    } else if (tokens.at(step).getType() == CHAR_L_PARENTHESIS) {
        auto *lParenthesisLoc = new SourceLocation(tokens.at(step).getRow(), tokens.at(step).getStartColumn());
        locs->emplace_back(lParenthesisLoc);
        step++;  // eat '('

        expInner = parseExp();

        if (tokens.at(step).getType() == CHAR_R_PARENTHESIS) {
            auto *rParenthesisLoc = new SourceLocation(tokens.at(step).getRow(), tokens.at(step).getStartColumn());
            locs->emplace_back(rParenthesisLoc);
            step++;  // eat ')'
        } else {
            Error::errorParse("')' Expected", tokens.at(step));
        }
    } else {
        lValInner = parseLVal();
    }

    auto *primaryExpRet = new PrimaryExp(expInner, lValInner, numberInner, locs);
    return primaryExpRet;
}

/**
 * 一元表达式 UnaryExp → PrimaryExp
 *                   | Ident '(' [FuncRParams] ')'
 *                   | ('+' | '-' | '!') UnaryExp 注：'!'仅出现在条件表达式中
 * @return
 */
UnaryExp *Parse::parseUnaryExp() {
    int opType = OP_NULL;
    std::string ident;
    UnaryExp *unaryExpInner = nullptr;
    PrimaryExp *primaryExpInner = nullptr;
    FuncRParams *funcRParamsInner = nullptr;
    auto *locs = new std::vector<SourceLocation *>();

    if (tokens.at(step).getType() == TOKEN_IDENTIFIER) {
        int stepAux = step;
        step++;  // eat identifier

        if (tokens.at(step).getType() == CHAR_L_PARENTHESIS) {  // 函数调用
            ident = tokens.at(stepAux).getIdentifierStr();

            auto *lParenthesisLoc = new SourceLocation(tokens.at(step).getRow(), tokens.at(step).getStartColumn());
            locs->emplace_back(lParenthesisLoc);
            step++;  // eat '('

            if (tokens.at(step).getType() != CHAR_R_PARENTHESIS) {  // 含有实参
                funcRParamsInner = parseFuncRParams();
            }

            if (tokens.at(step).getType() == CHAR_R_PARENTHESIS) {
                auto *rParenthesisLoc = new SourceLocation(tokens.at(step).getRow(), tokens.at(step).getStartColumn());
                locs->emplace_back(rParenthesisLoc);
                step++;  // eat ')'
            } else {
                Error::errorParse("')' Expected", tokens.at(step));
            }
        } else {  // 左值数组
            step = stepAux;
            primaryExpInner = parsePrimaryExp();
        }
    } else if (tokens.at(step).getType() == OP_BO_ADD || tokens.at(step).getType() == OP_BO_SUB ||
               tokens.at(step).getType() == OP_UO_NOT) {
        opType = tokens.at(step).getType();
        step++;  // eat op
        unaryExpInner = parseUnaryExp();
    } else {
        primaryExpInner = parsePrimaryExp();
    }

    auto *unaryExpRet = new UnaryExp(primaryExpInner, ident.c_str(), funcRParamsInner, opType, unaryExpInner, locs);
    return unaryExpRet;
}

/**
 * 乘除模表达式 MulExp → UnaryExp | UnaryExp ('*' | '/' | '%') MulExp
 * @return
 */
MulExp *Parse::parseMulExp() {
    MulExp *mulExpInner = nullptr;
    auto *locs = new std::vector<SourceLocation *>();
    int opType = OP_NULL;

    UnaryExp *unaryExpInner = parseUnaryExp();
    if (tokens.at(step).getType() == OP_BO_MUL || tokens.at(step).getType() == OP_BO_DIV ||
        tokens.at(step).getType() == OP_BO_REM) {
        opType = tokens.at(step).getType();
        auto *opLoc = new SourceLocation(tokens.at(step).getRow(), tokens.at(step).getStartColumn());
        locs->emplace_back(opLoc);
        step++;  // eat op
        mulExpInner = parseMulExp();
    }

    auto *mulExpRet = new MulExp(mulExpInner, unaryExpInner, opType, locs);
    return mulExpRet;
}

/**
 * 加减表达式 AddExp → MulExp | MulExp ('+' | '−') AddExp
 * @return
 */
AddExp *Parse::parseAddExp() {
    AddExp *addExpInner = nullptr;
    auto *locs = new std::vector<SourceLocation *>();
    int opType = OP_NULL;

    MulExp *mulExpInner = parseMulExp();
    if (tokens.at(step).getType() == OP_BO_ADD || tokens.at(step).getType() == OP_BO_SUB) {
        opType = tokens.at(step).getType();
        auto *opLoc = new SourceLocation(tokens.at(step).getRow(), tokens.at(step).getStartColumn());
        locs->emplace_back(opLoc);
        step++;  // eat op
        addExpInner = parseAddExp();
    }

    auto *addExpRet = new AddExp(addExpInner, mulExpInner, opType, locs);
    return addExpRet;
}

/**
 * 关系表达式 RelExp → AddExp | AddExp ('<' | '>' | '<=' | '>=') RelExp
 * @return
 */
RelExp *Parse::parseRelExp() {
    RelExp *relExpInner = nullptr;
    auto *locs = new std::vector<SourceLocation *>();
    int opType = OP_NULL;

    AddExp *addExpInner = parseAddExp();
    if (tokens.at(step).getType() == OP_BO_GT || tokens.at(step).getType() == OP_BO_GTE ||
        tokens.at(step).getType() == OP_BO_LT || tokens.at(step).getType() == OP_BO_LTE) {
        opType = tokens.at(step).getType();
        auto *opLoc = new SourceLocation(tokens.at(step).getRow(), tokens.at(step).getStartColumn());
        locs->emplace_back(opLoc);
        step++;  // eat op
        relExpInner = parseRelExp();
    }

    auto *relExpRet = new RelExp(relExpInner, addExpInner, opType, locs);
    return relExpRet;
}

/**
 * 相等性表达式 EqExp → RelExp | RelExp ('==' | '!=') EqExp
 * @return
 */
EqExp *Parse::parseEqExp() {
    EqExp *eqExpInner = nullptr;
    auto *locs = new std::vector<SourceLocation *>();
    int opType = OP_NULL;

    RelExp *relExpInner = parseRelExp();
    if (tokens.at(step).getType() == OP_BO_EQ || tokens.at(step).getType() == OP_BO_NEQ) {
        opType = tokens.at(step).getType();
        auto *opLoc = new SourceLocation(tokens.at(step).getRow(), tokens.at(step).getStartColumn());
        locs->emplace_back(opLoc);
        step++;  // eat op
        eqExpInner = parseEqExp();
    }

    auto *eqExpRet = new EqExp(eqExpInner, relExpInner, opType, locs);
    return eqExpRet;
}

/**
 * 逻辑与表达式 LAndExp → EqExp | EqExp '&&' LAndExp
 * @return
 */
LAndExp *Parse::parseLAndExp() {
    LAndExp *lAndExpInner = nullptr;
    auto *locs = new std::vector<SourceLocation *>();

    EqExp *eqExpInner = parseEqExp();
    if (tokens.at(step).getType() == OP_BO_AND) {
        auto *opLoc = new SourceLocation(tokens.at(step).getRow(), tokens.at(step).getStartColumn());
        locs->emplace_back(opLoc);
        step++;  // eat op
        lAndExpInner = parseLAndExp();
    }

    auto *lAndExpRet = new LAndExp(lAndExpInner, eqExpInner, locs);
    return lAndExpRet;
}

/**
 * 逻辑或表达式 LOrExp → LAndExp | LAndExp '||' LOrExp
 * @return
 */
LOrExp *Parse::parseLOrExp() {
    LOrExp *lOrExpInner = nullptr;
    auto *locs = new std::vector<SourceLocation *>();

    LAndExp *lAndExpInner = parseLAndExp();
    if (tokens.at(step).getType() == OP_BO_OR) {
        auto *opLoc = new SourceLocation(tokens.at(step).getRow(), tokens.at(step).getStartColumn());
        locs->emplace_back(opLoc);
        step++;  // eat op
        lOrExpInner = parseLOrExp();
    }

    auto *lOrExpRet = new LOrExp(lOrExpInner, lAndExpInner, locs);
    return lOrExpRet;
}

/**
 * 常量表达式 ConstExp → AddExp 注：使用的 Ident 必须是常量
 * @return
 */
ConstExp *Parse::parseConstExp() {
    auto *addExpInner = parseAddExp();
    auto *constExpRet = new ConstExp(addExpInner);
    return constExpRet;
}

/**
 * 函数实参表 FuncRParams →  Exp { ',' Exp }
 *                       | '"' ident '"' { ',' Exp }
 * @return
 */
FuncRParams *Parse::parseFuncRParams() {
    std::string strOut;
    auto *expsInner = new std::vector<Exp *>();

    if (tokens.at(step).getType() == TOKEN_STR) {
        strOut = tokens.at(step).getIdentifierStr();
        step++;  // eat str

        if (tokens.at(step).getType() == CHAR_COMMA) {
            step++;  // eat ','
        } else {
            Error::errorParse("',' Expected!", tokens.at(step));
        }
    }

    Exp *expFirst = parseExp();
    expsInner->emplace_back(expFirst);

    while (tokens.at(step).getType() == CHAR_COMMA) {
        step++;  // eat ','
        Exp *expBack = parseExp();
        expsInner->emplace_back(expBack);
    }

    auto *funcRParamsRet = new FuncRParams(strOut.c_str(), expsInner);
    return funcRParamsRet;
}
