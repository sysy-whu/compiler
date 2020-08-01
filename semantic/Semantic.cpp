#include <iostream>
#include "Semantic.h"
#include "../util/Error.h"

Semantic::Semantic() {
    Parse parse;
    parse.parseAST();
    ast = parse.getAST();
    std::cout << ast->getDecls()->size() << std::endl;
    symbolTables = new std::vector<SymbolTable *>();

    capacity = 4;
    levelNow = 0;
    funcParamNow = nullptr;
    symbolTableNow = nullptr;
}

///===-----------------------------------------------------------------------===///
/// startSemantic 启动接口
///===-----------------------------------------------------------------------===///

void Semantic::startSemantic() {
    /// 在这里无脑设置 sylib.h 声明函数
    setSyLib_H();
    std::cout << "after setSyLib_H; begin semantic" << std::endl;

    auto *symbolsGlobal = new std::vector<Symbol *>();
    auto *symbolTableGlobal = new SymbolTable(SYMBOL_TABLE_GLOBAL, SYMBOL_TABLE_GLOBAL_STR.c_str(), symbolsGlobal);
    symbolTables->emplace_back(symbolTableGlobal);
    // 处理 ast
    for (Decl *decl: *(ast->getDecls())) {
        funcNameNow = GLOBAL_VAR_NO_FUNC;
        if (decl->getVarDecl() != nullptr || decl->getConstDecl() != nullptr) {  // 变量
            semanticArm7Var(decl, symbolsGlobal);
        } else if (decl->getFuncDef() != nullptr) {  // 函数
            Arm7Func *arm7Func = semanticArm7Func(decl->getFuncDef());
            auto *symbol = new Symbol(nullptr, arm7Func);
            symbolsGlobal->emplace_back(symbol);
        } else {
            Error::errorSemanticDecl("Error Semantic", decl);
        }
    }
}

///===-----------------------------------------------------------------------===///
/// 分析 ast 得到符号表相关内容
///===-----------------------------------------------------------------------===///

void Semantic::semanticArm7Var(Decl *decl, std::vector<Symbol *> *symbols) {
    if (decl->getConstDecl() != nullptr) {
        for (ConstDef *constDef: *decl->getConstDecl()->getConstDefs()) {
            auto *values = new std::vector<int>();
            Arm7Var *arm7Var;
            if (constDef->getConstExps()->empty()) {  /// 常量变量
                int constValue = calConstExp(constDef->getConstInitVal()->getConstExp());
                values->push_back(constValue);
                arm7Var = new Arm7Var(constDef->getIdent().c_str(), funcNameNow.c_str(),
                                      decl->getConstDecl()->getBType(), levelNow,
                                      CONST_TRUE, ARRAY_FALSE, nullptr, values);
                if (funcNameNow == GLOBAL_VAR_NO_FUNC) {
                    arm7Var->setMemoryLoc(GLOBAL_VAR_POS);
                } else {
                    capacity += 4;
                    arm7Var->setMemoryLoc(capacity);
                }
            } else {  /// 常量数组
                auto *subs = new std::vector<int>();  /// 下标
                int len = 1;  /// 元素总个数
                for (ConstExp *constExp:*constDef->getConstExps()) {
                    int sub = calConstExp(constExp);
                    len *= sub;
                    subs->push_back(sub);
                }
                values = calConstArrayInitVals(constDef->getConstInitVal(), subs);
                arm7Var = new Arm7Var(constDef->getIdent().c_str(), funcNameNow.c_str(),
                                      decl->getConstDecl()->getBType(), levelNow,
                                      CONST_TRUE, ARRAY_TRUE, subs, values);
                if (funcNameNow == GLOBAL_VAR_NO_FUNC) {
                    arm7Var->setMemoryLoc(GLOBAL_VAR_POS);
                } else {
                    capacity += len * 4;
                    arm7Var->setMemoryLoc(capacity);
                }
            }
            auto *symbol = new Symbol(arm7Var, nullptr);
            symbols->emplace_back(symbol);
        } //  end constDefs
    } else if (decl->getVarDecl() != nullptr) {
        for (VarDef *varDef: *decl->getVarDecl()->getVarDefs()) {
            Arm7Var *arm7Var;
            if (varDef->getConstExps()->empty()) {
                if (varDef->getInitVal() != nullptr &&
                    semanticAddExp(varDef->getInitVal()->getExp()->getAddExp()) != decl->getVarDecl()->getBType()) {
                    Error::errorSim("var init wrong when assign");
                    exit(-1);
                }
                arm7Var = new Arm7Var(varDef->getIdent().c_str(), funcNameNow.c_str(),
                                      decl->getVarDecl()->getBType(), levelNow,
                                      CONST_FALSE, ARRAY_FALSE, nullptr, nullptr);
                if (funcNameNow == GLOBAL_VAR_NO_FUNC) {
                    arm7Var->setMemoryLoc(GLOBAL_VAR_POS);
                } else {
                    capacity += 4;
                    arm7Var->setMemoryLoc(capacity);
                }
            } else {
                auto *subs = new std::vector<int>();  /// 下标
                int len = 1;  /// 元素总个数
                for (ConstExp *constExp:*varDef->getConstExps()) {
                    int sub = calConstExp(constExp);
                    len *= sub;
                    subs->push_back(sub);
                }
                if (funcNameNow == GLOBAL_VAR_NO_FUNC) {
                    auto *values = calVarArrayInitVals(varDef->getInitVal(), subs);
                    arm7Var = new Arm7Var(varDef->getIdent().c_str(), funcNameNow.c_str(),
                                          decl->getVarDecl()->getBType(), levelNow,
                                          CONST_FALSE, ARRAY_TRUE, subs, values);
                    arm7Var->setMemoryLoc(GLOBAL_VAR_POS);
                } else {
                    arm7Var = new Arm7Var(varDef->getIdent().c_str(), funcNameNow.c_str(),
                                          decl->getVarDecl()->getBType(), levelNow,
                                          CONST_FALSE, ARRAY_TRUE, subs, nullptr);
                    capacity += len * 4;
                    arm7Var->setMemoryLoc(capacity);
                }
            }
            auto *symbol = new Symbol(arm7Var, nullptr);
            symbols->emplace_back(symbol);
        }  // end varDefs
    }
}


void Semantic::semanticArm7Var(BlockItem *blockItem, std::vector<Symbol *> *symbols) {
    if (blockItem->getConstDecl() != nullptr) {
        for (ConstDef *constDef: *blockItem->getConstDecl()->getConstDefs()) {
            auto *values = new std::vector<int>();
            Arm7Var *arm7Var;
            if (constDef->getConstExps()->empty()) {  /// 常量变量
                int constValue = calConstExp(constDef->getConstInitVal()->getConstExp());
                values->push_back(constValue);
                arm7Var = new Arm7Var(constDef->getIdent().c_str(), funcNameNow.c_str(),
                                      blockItem->getConstDecl()->getBType(), levelNow,
                                      CONST_TRUE, ARRAY_FALSE, nullptr, values);
                if (funcNameNow == GLOBAL_VAR_NO_FUNC) {
                    arm7Var->setMemoryLoc(GLOBAL_VAR_POS);
                } else {
                    capacity += 4;
                    arm7Var->setMemoryLoc(capacity);
                }
            } else {  /// 常量数组
                auto *subs = new std::vector<int>();  /// 下标
                int len = 1;  /// 元素总个数
                for (ConstExp *constExp:*constDef->getConstExps()) {
                    int sub = calConstExp(constExp);
                    len *= sub;
                    subs->push_back(sub);
                }
                values = calConstArrayInitVals(constDef->getConstInitVal(), subs);
                arm7Var = new Arm7Var(constDef->getIdent().c_str(), funcNameNow.c_str(),
                                      blockItem->getConstDecl()->getBType(), levelNow,
                                      CONST_TRUE, ARRAY_TRUE, subs, values);
                if (funcNameNow == GLOBAL_VAR_NO_FUNC) {
                    arm7Var->setMemoryLoc(GLOBAL_VAR_POS);
                } else {
                    capacity += len * 4;
                    arm7Var->setMemoryLoc(capacity);
                }
            }
            auto *symbol = new Symbol(arm7Var, nullptr);
            symbols->emplace_back(symbol);
        } //  end constDefs
    } else if (blockItem->getVarDecl() != nullptr) {
        for (VarDef *varDef: *blockItem->getVarDecl()->getVarDefs()) {
            Arm7Var *arm7Var;
            if (varDef->getConstExps()->empty()) {
                if (varDef->getInitVal() != nullptr &&
                    semanticAddExp(varDef->getInitVal()->getExp()->getAddExp()) != blockItem->getVarDecl()->getBType()) {
                    Error::errorSim("var init wrong when assign");
                    exit(-1);
                }
                arm7Var = new Arm7Var(varDef->getIdent().c_str(), funcNameNow.c_str(),
                                      blockItem->getVarDecl()->getBType(), levelNow,
                                      CONST_FALSE, ARRAY_FALSE, nullptr, nullptr);
                if (funcNameNow == GLOBAL_VAR_NO_FUNC) {
                    arm7Var->setMemoryLoc(GLOBAL_VAR_POS);
                } else {
                    capacity += 4;
                    arm7Var->setMemoryLoc(capacity);
                }
            } else {
                auto *subs = new std::vector<int>();  /// 下标
                int len = 1;  /// 元素总个数
                for (ConstExp *constExp:*varDef->getConstExps()) {
                    int sub = calConstExp(constExp);
                    len *= sub;
                    subs->push_back(sub);
                }
                if (funcNameNow == GLOBAL_VAR_NO_FUNC) {
                    auto *values = calVarArrayInitVals(varDef->getInitVal(), subs);
                    arm7Var = new Arm7Var(varDef->getIdent().c_str(), funcNameNow.c_str(),
                                          blockItem->getVarDecl()->getBType(), levelNow,
                                          CONST_FALSE, ARRAY_TRUE, subs, values);
                    arm7Var->setMemoryLoc(GLOBAL_VAR_POS);
                } else {
                    arm7Var = new Arm7Var(varDef->getIdent().c_str(), funcNameNow.c_str(),
                                          blockItem->getVarDecl()->getBType(), levelNow,
                                          CONST_FALSE, ARRAY_TRUE, subs, nullptr);
                    capacity += len * 4;
                    arm7Var->setMemoryLoc(capacity);
                }
            }
            auto *symbol = new Symbol(arm7Var, nullptr);
            symbols->emplace_back(symbol);
        }  // end varDefs
    }
}

Arm7Func *Semantic::semanticArm7Func(FuncDef *funcDef) {
    auto *params = new std::vector<Arm7Var *>();
    funcNameNow = funcDef->getIdent();
    levelNow++;
    capacity = 4;

    if (funcDef->getFuncFParams() != nullptr) {
        for (FuncFParam *funcFParam: *funcDef->getFuncFParams()->getFuncFParams()) {
            Arm7Var *arm7Var;
            if (funcFParam->getBType() == TYPE_INT) {  // 变量
                arm7Var = new Arm7Var(funcFParam->getIdent().c_str(), funcNameNow.c_str(),
                                      funcFParam->getBType(), levelNow,
                                      CONST_FALSE, ARRAY_FALSE, nullptr, nullptr);

            } else if (funcFParam->getBType() == TYPE_INT_STAR) {  // 数组 （第一维为空）
                auto *subs = new std::vector<int>();
                for (Exp *exp: *funcFParam->getExps()) {
                    subs->push_back(calAddExp(exp->getAddExp()));
                }
                arm7Var = new Arm7Var(funcFParam->getIdent().c_str(), funcNameNow.c_str(),
                                      funcFParam->getBType(), levelNow,
                                      CONST_FALSE, ARRAY_TRUE, subs, nullptr);
            }else{
                Error::errorSim("something wrong in semanticArm7Func funcFParam->getBType()");
                exit(-1);
            }
            capacity += 4;  /// 由于所存为引用变量，此处无论参数类型均分配四字节数
            arm7Var->setMemoryLoc(capacity);
            params->emplace_back(arm7Var);
        }
    }
    funcParamNow = params;

    semanticBlock(funcDef->getBlock(), funcDef->getFuncType(), params);

    levelNow--;
    funcParamNow = nullptr;
    auto *arm7Func = new Arm7Func(funcNameNow.c_str(), funcDef->getFuncType(), params);
    return arm7Func;
}

///===-----------------------------------------------------------------------===///
/// Stmt 语句
///===-----------------------------------------------------------------------===///

void Semantic::semanticBlock(Block *block, int funcRetType, std::vector<Arm7Var *> *params) {
    levelNow++;
    auto *symbolsLocal = new std::vector<Symbol *>();
    auto *symbolTableLocal = new SymbolTable(SYMBOL_TABLE_LOCAL, funcNameNow.c_str(), symbolsLocal);
    symbolTables->emplace_back(symbolTableLocal);
    SymbolTable *symbolTableTmp = symbolTableNow;
    symbolTableNow = symbolTableLocal;

    for (BlockItem *blockItem:*block->getBlockItems()) {
        if (blockItem->getConstDecl() != nullptr || blockItem->getVarDecl() != nullptr) {
            semanticArm7Var(blockItem, symbolsLocal);
        } else if (blockItem->getStmt() != nullptr) {
            semanticStmt(blockItem->getStmt(), funcRetType);
        }
    }
    levelNow--;
    /// 这里 symbolTable 不再 pop_back
    symbolTableNow = symbolTableTmp;
}

void Semantic::semanticStmt(Stmt *stmt, int funcRetType) {
    switch (stmt->getStmtType()) {
        case STMT_EXP:
            semanticAddExp(stmt->getExp()->getAddExp());
            break;
        case STMT_BLOCK:
            semanticBlock(stmt->getBlock(), funcRetType, nullptr);
            break;
        case STMT_IF: {
            semanticCondExp(stmt->getCond());
            semanticStmt(stmt->getStmtBrBody(), funcRetType);
            if (stmt->getElseBody() != nullptr)
                semanticStmt(stmt->getElseBody(), funcRetType);
            break;
        }
        case STMT_WHILE: {
            semanticCondExp(stmt->getCond());
            semanticStmt(stmt->getStmtBrBody(), funcRetType);
            break;
        }
        case STMT_LVAL_ASSIGN: {
            int lType = semanticLVal(stmt->getLVal());
            int rType = semanticAddExp(stmt->getExp()->getAddExp());
            if (lType != TYPE_INT || rType != TYPE_INT) {  // 好像 C 子集不能一次赋个数组
                Error::errorSim("semanticStmt STMT_LVAL_ASSIGN");
            }
            break;
        }
        case STMT_RETURN: {
            if (stmt->getExp() == nullptr) {
                if (funcRetType != TYPE_VOID) {
                    Error::errorSim("Error func ret type void");
                }
            } else {
                int retType = semanticAddExp(stmt->getExp()->getAddExp());
                if (retType != TYPE_INT || funcRetType != TYPE_INT) {
                    Error::errorSim("Error func ret type int");
                }
            }
            break;
        }
        case STMT_EXP_BLANK:
        case STMT_CONTINUE:
        case STMT_BREAK:
            break;
    }
}

///===-----------------------------------------------------------------------===///
/// Expr 表达式 CalConst
/// @param one kind of Exp or initVal
/// @return const value of that Exp or initVal
///===-----------------------------------------------------------------------===///

std::vector<int> *Semantic::calConstArrayInitVals(ConstInitVal *constInitVal, std::vector<int> *subs) {
    auto *valuesRet = new std::vector<int>();
    int len = 1;  // 维度展开一维的长度
    for (int i : *subs) {
        len *= i;
    }
    if (constInitVal->getConstInitVals()->empty()) {  // {}
        for (int i = 0; i < len; i++) {
            valuesRet->push_back(0);
        }
    } else {
        for (ConstInitVal *initValInner: *constInitVal->getConstInitVals()) {
            if (initValInner->getConstExp() != nullptr) {  // 实值
                int valueInner = calConstExp(initValInner->getConstExp());
                valuesRet->push_back(valueInner);
            } else {  // 数组嵌套
                int sub_first_len = 1;  // 子维展开一维的长度
                for (int i = 1; i < subs->size(); i++) {
                    sub_first_len *= subs->at(i);
                }
                if (valuesRet->size() % sub_first_len == 0) {  // 保证嵌套子维前，已有长度为子维整数倍
                    int tmp = subs->at(0);  // 当前第一维度值
                    subs->erase(subs->begin());
                    std::vector<int> *subs_values_inner = calConstArrayInitVals(initValInner, subs);
                    subs->insert(subs->begin(), tmp);
                    valuesRet->insert(valuesRet->end(), subs_values_inner->begin(), subs_values_inner->end());  // 拼接
                } else {
                    Error::errorSim("calConstArrayInitVals");
                    exit(-1);
                }
            }
        }
    }
    if (valuesRet->size() < len) {  // 长度不足补零
        for (int i = 0; i < len - valuesRet->size(); i++) {
            valuesRet->push_back(0);
        }
    } else if (valuesRet->size() > len) {
        Error::errorSim("ConstArray len error");
    }
    return valuesRet;
}

std::vector<int> *Semantic::calVarArrayInitVals(InitVal *initVal, std::vector<int> *subs) {
    auto *valuesRet = new std::vector<int>();
    int len = 1;  // 维度展开一维的长度
    for (int i : *subs) {
        len *= i;
    }
    if (initVal->getInitVals()->empty()) {
        for (int i = 0; i < len; i++) {
            valuesRet->push_back(0);
        }
    } else {
        for (InitVal *initValInner: *initVal->getInitVals()) {
            if (initValInner->getExp() != nullptr) {  // 实值
                int valueInner = calAddExp(initValInner->getExp()->getAddExp());
                valuesRet->push_back(valueInner);
            } else {  // 数组嵌套
                int sub_first_len = 1;  // 子维展开一维的长度
                for (int i = 1; i < subs->size(); i++) {
                    sub_first_len *= subs->at(i);
                }
                if (valuesRet->size() % sub_first_len == 0) {  // 保证嵌套子维前，已有长度为子维整数倍
                    int tmp = subs->at(0);  // 当前第一维度值
                    subs->erase(subs->begin());
                    std::vector<int> *subs_values_inner = calVarArrayInitVals(initValInner, subs);
                    subs->insert(subs->begin(), tmp);
                    valuesRet->insert(valuesRet->end(), subs_values_inner->begin(), subs_values_inner->end());  // 拼接
                }
            }
        }
    }
    if (valuesRet->size() < len) {  // 长度不足补零
        for (int i = 0; i < len - valuesRet->size(); i++) {
            valuesRet->push_back(0);
        }
    }
    return valuesRet;
}

int Semantic::calConstExp(ConstExp *constExp) {
    return calAddExp(constExp->getAddExp());
}

int Semantic::calAddExp(AddExp *addExp) {
    int intRet;
    if (addExp->getOpType() == OP_NULL) {
        intRet = calMulExp(addExp->getMulExp());
    } else {
        int mulRet = calMulExp(addExp->getMulExp());
        int addRet = calAddExp(addExp->getAddExp());
        switch (addExp->getOpType()) {
            case OP_BO_ADD:
                intRet = mulRet + addRet;
                break;
            case OP_BO_SUB:
                intRet = mulRet - addRet;
                break;
            default:
                Error::errorSim("semantic.cpp -> calAddExp");
                exit(-1);
        }
    }
    return intRet;
}

int Semantic::calMulExp(MulExp *mulExp) {
    int intRet;
    if (mulExp->getOpType() == OP_NULL) {
        intRet = calUnaryExp(mulExp->getUnaryExp());
    } else {
        int unaryRet = calUnaryExp(mulExp->getUnaryExp());
        int mulRet = calMulExp(mulExp->getMulExp());
        switch (mulExp->getOpType()) {
            case OP_BO_MUL:
                intRet = unaryRet * mulRet;
                break;
            case OP_BO_DIV:
                intRet = unaryRet / mulRet;
                break;
            case OP_BO_REM:
                intRet = unaryRet % mulRet;
                break;
            default:
                Error::errorSim("semantic.cpp -> calMulExp");
                exit(-1);
        }
    }
    return intRet;
}

int Semantic::calUnaryExp(UnaryExp *unaryExp) {
    int intRet;
    if (unaryExp->getOpType() != OP_NULL) {
        switch (unaryExp->getOpType()) {
            case OP_BO_ADD:
                intRet = calUnaryExp(unaryExp->getUnaryExp());
                break;
            case OP_BO_SUB:
                intRet = -calUnaryExp(unaryExp->getUnaryExp());
                break;
            case OP_UO_NOT:
                Error::errorSim("\"!\" can't be in here");
                exit(-1);
            default:
                Error::errorSim("Semantic.cpp calUnaryExp Op");
                exit(-1);
        }
    } else if (unaryExp->getPrimaryExp() != nullptr) {
        intRet = calPrimaryExp(unaryExp->getPrimaryExp());
    } else {
        Error::errorSim("Semantic.cpp calUnaryExp call");
        exit(-1);
    }
    return intRet;
}

int Semantic::calPrimaryExp(PrimaryExp *primaryExp) {
    int intRet;
    if (primaryExp->getExp() != nullptr) {
        intRet = calAddExp(primaryExp->getExp()->getAddExp());
    } else if (primaryExp->getLVal() != nullptr) {
        intRet = calLVal(primaryExp->getLVal());
    } else {
        intRet = primaryExp->getNumber();
    }
    return intRet;
}

int Semantic::calLVal(LVal *lVal) {
    if (lVal->getExps()->empty()) {  /// 符号表找常量
        for (int i = (int) symbolTables->size() - 1; i > 0; i--) {
            if (symbolTables->at(i)->getFuncName() == funcNameNow ||
                symbolTables->at(i)->getFuncName() == SYMBOL_TABLE_GLOBAL_STR) {
                for (Symbol *symbol:*symbolTables->at(i)->getSymbols()) {
                    if (symbol->getArm7Var() != nullptr &&
                        symbol->getArm7Var()->getIdent() == lVal->getIdent() &&
                        symbol->getArm7Var()->getIfArray() == ARRAY_FALSE &&
                        symbol->getArm7Var()->getIfConst() == CONST_TRUE) {
                        return symbol->getArm7Var()->getValue()->at(0);
                    }
                }
            }
        }
    } else {  /// 符号表找数组
        // 索引下标
        auto *subs = new std::vector<int>();
        for (Exp *exp:*lVal->getExps()) {
            subs->push_back(calAddExp(exp->getAddExp()));
        }

        for (int i = (int) symbolTables->size() - 1; i > 0; i--) {
            if (symbolTables->at(i)->getFuncName() == funcNameNow ||
                symbolTables->at(i)->getFuncName() == SYMBOL_TABLE_GLOBAL_STR) {
                for (Symbol *symbol:*symbolTables->at(i)->getSymbols()) {
                    if (symbol->getArm7Var() != nullptr &&
                        symbol->getArm7Var()->getIdent() == lVal->getIdent() &&
                        symbol->getArm7Var()->getIfArray() == ARRAY_TRUE &&
                        symbol->getArm7Var()->getIfConst() == CONST_TRUE) {
                        if (subs->size() == symbol->getArm7Var()->getSubs()->size()) {
                            int pos = 0;  // 计算索引一维位置
                            for (int j = 0; j < subs->size(); j++) {
                                int subs_subs_len = 1;
                                for (int k = j + 1; k < subs->size(); k++) {
                                    subs_subs_len *= symbol->getArm7Var()->getSubs()->at(k);
                                }
                                pos += subs->at(j) * subs_subs_len;
                            }
                            return symbol->getArm7Var()->getValue()->at(pos);
                        } else {
                            Error::errorSim("error subs Semantic calLVal");
                            exit(-1);
                        }  // end find and return
                    }  // end find constArray ident
                }  // end symbol-table iter
            }  // end find in same func or global if judge
        }  // end all symbol-table except EXTERN
    }  // end find lVal
    Error::errorSim("undefined ident Semantic calLVal");
    exit(-1);
}

///===-----------------------------------------------------------------------===///
/// Expr 表达式 Semantic
/// @param one kind of Exp
/// @return ret type of that Exp
///===-----------------------------------------------------------------------===///

std::vector<int> *Semantic::semanticVarArrayInitVals(InitVal *initVal, std::vector<int> *subs) {
    auto *valuesRet = new std::vector<int>();
    int len = 1;  // 维度展开一维的长度
    for (int i : *subs) {
        len *= i;
    }
    // 所有值用零填充
    if (initVal->getInitVals()->empty()) {  // {}
        for (int i = 0; i < len; i++) {
            valuesRet->push_back(0);
        }
    } else {
        for (InitVal *initValInner : *initVal->getInitVals()) {
            if (initValInner->getExp() != nullptr) {  // 实值
                valuesRet->push_back(0);
            } else {  // 数组嵌套
                int sub_first_len = 1;  // 子维展开一维的长度
                for (int i = 1; i < subs->size(); i++) {
                    sub_first_len *= subs->at(i);
                }
                if (valuesRet->size() % sub_first_len == 0) {  // 保证嵌套子维前，已有长度为子维整数倍
                    int tmp = subs->at(0);  // 当前第一维度值
                    subs->erase(subs->begin());
                    std::vector<int> *subs_values_inner = semanticVarArrayInitVals(initValInner, subs);
                    subs->insert(subs->begin(), tmp);
                    valuesRet->insert(valuesRet->end(), subs_values_inner->begin(), subs_values_inner->end());  // 拼接
                } else {
                    Error::errorSim("calConstArrayInitVals");
                    exit(-1);
                }
            }
        }
    }
    if (valuesRet->size() < len) {  // 长度不足补零
        for (int i = 0; i < len - valuesRet->size(); i++) {
            valuesRet->push_back(0);
        }
    } else if (valuesRet->size() > len) {
        Error::errorSim("ConstArray len error");
    }
    return valuesRet;
}


int Semantic::semanticCondExp(Cond *cond) {
    return semanticLOrExp(cond->getLOrExp());
}

int Semantic::semanticLOrExp(LOrExp *lOrExp) {
    if (lOrExp->getLOrExp() == nullptr) {
        return semanticLAndExp(lOrExp->getLAndExp());
    } else {
        if (semanticLOrExp(lOrExp->getLOrExp()) == TYPE_INT_BOOL &&
            semanticLAndExp(lOrExp->getLAndExp()) == TYPE_INT_BOOL) {
            return TYPE_INT_BOOL;
        } else {
            Error::errorSim("semanticLOrExp");
            exit(-1);
        }
    }
}

int Semantic::semanticLAndExp(LAndExp *lAndExp) {
    if (lAndExp->getLAndExp() == nullptr) {
        return semanticEqExp(lAndExp->getEqExp());
    } else {
        if (semanticLAndExp(lAndExp->getLAndExp()) == TYPE_INT_BOOL &&
            semanticEqExp(lAndExp->getEqExp()) == TYPE_INT_BOOL) {
            return TYPE_INT_BOOL;
        } else {
            Error::errorSim("semanticLAndExp");
            exit(-1);
        }
    }
}

int Semantic::semanticEqExp(EqExp *eqExp) {
    if (eqExp->getOpType() == OP_NULL) {
        return semanticRelExp(eqExp->getRelExp());
    } else {
        if (semanticEqExp(eqExp->getEqExp()) == TYPE_INT_BOOL &&
            semanticRelExp(eqExp->getRelExp()) == TYPE_INT_BOOL) {
            if (eqExp->getOpType() == OP_BO_EQ || eqExp->getOpType() == OP_BO_NEQ) {
                return TYPE_INT_BOOL;
            } else {
                Error::errorSim("semanticEqExp opType");
                exit(-1);
            }
        } else {
            Error::errorSim("semanticEqExp");
            exit(-1);
        }
    }
}

int Semantic::semanticRelExp(RelExp *relExp) {
    if (relExp->getOpType() == OP_NULL) {
        if (semanticAddExp(relExp->getAddExp()) == TYPE_INT) {
            return TYPE_INT_BOOL;
        } else {
            Error::errorSim("semanticRelExp addExpInner type");
            exit(-1);
        }
    } else {
        if (semanticRelExp(relExp->getRelExp()) == TYPE_INT_BOOL &&
            semanticAddExp(relExp->getAddExp()) == TYPE_INT) {
            if (relExp->getOpType() == OP_BO_GT || relExp->getOpType() == OP_BO_GTE ||
                relExp->getOpType() == OP_BO_LT || relExp->getOpType() == OP_BO_LTE) {
                return TYPE_INT_BOOL;
            } else {
                Error::errorSim("semanticRelExp op type");
                exit(-1);
            }
        } else {
            Error::errorSim("semanticRelExp type");
            exit(-1);
        }
    }
}

int Semantic::semanticAddExp(AddExp *addExp) {
    if (addExp->getOpType() == OP_NULL) {
        return semanticMulExp(addExp->getMulExp());
    } else {
        if (semanticAddExp(addExp->getAddExp()) == TYPE_INT && semanticMulExp(addExp->getMulExp()) == TYPE_INT) {
            if (addExp->getOpType() == OP_BO_ADD || addExp->getOpType() == OP_BO_SUB) {
                return TYPE_INT;
            } else {
                Error::errorSim("semanticAddExp op type");
                exit(-1);
            }
        } else {
            Error::errorSim("semanticAddExp type");
            exit(-1);
        }
    }
}

int Semantic::semanticMulExp(MulExp *mulExp) {
    if (mulExp->getOpType() == OP_NULL) {
        return semanticUnaryExp(mulExp->getUnaryExp());
    } else {
        if (semanticMulExp(mulExp->getMulExp()) == TYPE_INT && semanticUnaryExp(mulExp->getUnaryExp()) == TYPE_INT) {
            if (mulExp->getOpType() == OP_BO_MUL || mulExp->getOpType() == OP_BO_DIV) {
                return TYPE_INT;
            } else {
                Error::errorSim("semanticMulExp op type");
                exit(-1);
            }
        } else {
            Error::errorSim("semanticMulExp op type");
            exit(-1);
        }
    }
}

int Semantic::semanticUnaryExp(UnaryExp *unaryExp) {
    if (unaryExp->getPrimaryExp() != nullptr) {
        return semanticPrimaryExp(unaryExp->getPrimaryExp());
    } else if (unaryExp->getOpType() == OP_BO_ADD || unaryExp->getOpType() == OP_BO_SUB ||
               unaryExp->getOpType() == OP_UO_NOT) {
        return semanticUnaryExp(unaryExp->getUnaryExp());
    } else {
        // 注意这里写死了 func 在符号表中的索引位置
        for (int i = 1; i >= 0; i--) {
            for (Symbol *symbol:*symbolTables->at(i)->getSymbols()) {
                if (symbol->getArm7Func() != nullptr &&
                    symbol->getArm7Func()->getIdent() == unaryExp->getIdent()) {
                    // 检查参数
                    checkRParams(unaryExp->getFuncRParams(), symbol->getArm7Func());
                    // 返回函数返回值
                    return symbol->getArm7Func()->getRetType();
                } // 检查到函数名
            } // 某一 symbol table 遍历
        } // tables[1] tables[0] 遍历
        Error::errorSim("semanticUnaryExp func name");
        exit(-1);
    }
}

int Semantic::semanticPrimaryExp(PrimaryExp *primaryExp) {
    if (primaryExp->getExp() != nullptr) {
        return semanticAddExp(primaryExp->getExp()->getAddExp());
    } else if (primaryExp->getLVal() != nullptr) {
        return semanticLVal(primaryExp->getLVal());
    } else {
        return TYPE_INT;
    }
}

int Semantic::semanticLVal(LVal *lVal) {
    if (lVal->getExps()->empty()) {  // 整型
        /// 当前 block 找
        for (Symbol *symbol: *symbolTableNow->getSymbols()) {
            if (symbol->getArm7Var() != nullptr &&
                symbol->getArm7Var()->getIdent() == lVal->getIdent() &&
                symbol->getArm7Var()->getIfArray() == ARRAY_FALSE) {

                lVal->setIdentMemoryPos(symbol->getArm7Var()->getMemoryLoc());
                return symbol->getArm7Var()->getVarType();
            }
        }
        /// 函数里外部层次block找
        for (int i = (int) symbolTables->size() - 1; i > 1; i--) {
            if (symbolTables->at(i)->getFuncName() == funcNameNow) {
                for (Symbol *symbol:*symbolTables->at(i)->getSymbols()) {
                    if (symbol->getArm7Var() != nullptr &&
                        symbol->getArm7Var()->getIdent() == lVal->getIdent() &&
                        symbol->getArm7Var()->getIfArray() == ARRAY_FALSE &&
                        symbol->getArm7Var()->getLevel() < levelNow) {

                        lVal->setIdentMemoryPos(symbol->getArm7Var()->getMemoryLoc());
                        return symbol->getArm7Var()->getVarType();
                    }
                }
            }
        }
        /// 函数形参里找
        for (Arm7Var *arm7Var: *funcParamNow) {
            if (arm7Var->getIdent() == lVal->getIdent() &&
                arm7Var->getIfArray() == ARRAY_FALSE) {

                lVal->setIdentMemoryPos(arm7Var->getMemoryLoc());
                return arm7Var->getVarType();
            }
        }
        /// 全局变量里找
        for (Symbol *symbol:*symbolTables->at(1)->getSymbols()) {
            if (symbol->getArm7Var() != nullptr &&
                symbol->getArm7Var()->getIdent() == lVal->getIdent() &&
                symbol->getArm7Var()->getIfArray() == ARRAY_FALSE) {

                lVal->setIdentMemoryPos(symbol->getArm7Var()->getMemoryLoc());
                return symbol->getArm7Var()->getVarType();
            }
        }
        /// 没找到
        Error::errorSim("no such var");
        exit(-1);
    } else {  // 整型数组
        checkExps(lVal->getExps(), "array sub not int");
        /// 当前 block 找
        for (Symbol *symbol:*symbolTableNow->getSymbols()) {
            if (symbol->getArm7Var() != nullptr &&
                symbol->getArm7Var()->getIdent() == lVal->getIdent() &&
                symbol->getArm7Var()->getIfArray() == ARRAY_TRUE) {

                lVal->setIdentMemoryPos(symbol->getArm7Var()->getMemoryLoc());
                if (lVal->getExps()->size() == symbol->getArm7Var()->getSubs()->size()) {
                    return symbol->getArm7Var()->getVarType();
                } else {
                    return TYPE_STR;
                }
            }
        }
        /// 函数里外部层次block找
        for (int i = (int) symbolTables->size() - 1; i > 1; i--) {
            if (symbolTables->at(i)->getFuncName() == funcNameNow) {
                for (Symbol *symbol:*symbolTables->at(i)->getSymbols()) {
                    if (symbol->getArm7Var() != nullptr &&
                        symbol->getArm7Var()->getIdent() == lVal->getIdent() &&
                        symbol->getArm7Var()->getIfArray() == ARRAY_TRUE &&
                        symbol->getArm7Var()->getLevel() < levelNow) {

                        lVal->setIdentMemoryPos(symbol->getArm7Var()->getMemoryLoc());
                        if (lVal->getExps()->size() == symbol->getArm7Var()->getSubs()->size()) {
                            return symbol->getArm7Var()->getVarType();
                        } else {
                            return TYPE_STR;
                        }
                    }
                }
            }
        }
        /// 函数形参里找
        for (Arm7Var *arm7Var: *funcParamNow) {
            if (arm7Var->getIdent() == lVal->getIdent() &&
                arm7Var->getIfArray() == ARRAY_TRUE) {

                lVal->setIdentMemoryPos(arm7Var->getMemoryLoc());
                if (lVal->getExps()->size() == arm7Var->getSubs()->size() + 1) {  // 形参不写第一维
                    return arm7Var->getVarType();
                } else {
                    return TYPE_STR;
                }
            }
        }
        /// 全局变量里找
        for (Symbol *symbol:*symbolTables->at(1)->getSymbols()) {
            if (symbol->getArm7Var() != nullptr &&
                symbol->getArm7Var()->getIdent() == lVal->getIdent() &&
                symbol->getArm7Var()->getIfArray() == ARRAY_TRUE) {

                lVal->setIdentMemoryPos(symbol->getArm7Var()->getMemoryLoc());
                if (lVal->getExps()->size() == symbol->getArm7Var()->getSubs()->size()) {
                    return symbol->getArm7Var()->getVarType();
                } else {
                    return TYPE_STR;
                }
            }
        }
        /// 没找到
        Error::errorSim("no such array");
        exit(-1);
    }  // 整型、整型数组
}

void Semantic::checkRParams(FuncRParams *funcRParams, Arm7Func *arm7Func) {
    if (funcRParams == nullptr && arm7Func->getParams()->empty()) {
        return;
    } else if (funcRParams->getExps()->size() == arm7Func->getParams()->size()) {
        // j 为索引遍历检查实参
        for (int j = 0; j < funcRParams->getExps()->size(); j++) {
            switch (semanticAddExp(funcRParams->getExps()->at(j)->getAddExp())) {
                case TYPE_INT:
                    if (arm7Func->getParams()->at(j)->getIfArray() == ARRAY_FALSE) {
                        break;
                    } else {
                        Error::errorSim("semanticUnaryExp func param int");
                        exit(-1);
                    }
                case TYPE_INT_STAR:
                    if (arm7Func->getParams()->at(j)->getIfArray() == ARRAY_TRUE) {
                        break;
                    } else {
                        Error::errorSim("semanticUnaryExp func param int star");
                        exit(-1);
                    }
                default:
                    Error::errorSim("semanticUnaryExp func param default");
                    exit(-1);
            }
        }  // 检查各个实参类型
    } else if (arm7Func->getIdent() != putFStr /*&& !funcRParams->getStrOut().empty()*/) {
        Error::errorSim("semanticUnaryExp func param");
        exit(-1);
    }
}

void Semantic::checkExps(std::vector<Exp *> *exps, const char *errMsg) {
    for (Exp *exp: *exps) {
        if (semanticAddExp(exp->getAddExp()) != TYPE_INT) {
            Error::errorSim(errMsg);
            exit(-1);
        }
    }
}


///===-----------------------------------------------------------------------===///
/// 静态库文件里面的函数
///===-----------------------------------------------------------------------===///
void Semantic::setSyLib_H() {
    auto *symbolsLib = new std::vector<Symbol *>();
    std::string avoidNULL = "nullptr";

    std::string getIntStr = "getint";
    // int getint()
    auto *funcGetint = new Arm7Func(getIntStr.c_str(), TYPE_INT, nullptr);
    auto *symbolGetInt = new Symbol(nullptr, funcGetint);
    symbolsLib->emplace_back(symbolGetInt);

    std::string getChStr = "getch";
    // int getch()
    auto *funcGetCh = new Arm7Func(getChStr.c_str(), TYPE_INT, nullptr);
    auto *symbolGetCh = new Symbol(nullptr, funcGetCh);
    symbolsLib->emplace_back(symbolGetCh);

    std::string getArrayStr = "getarray";
    auto *getArrayParams = new std::vector<Arm7Var *>();
    // int[]
    auto *getArrayVarArraySubs = new std::vector<int>();
    auto *getArrayVarArrayParam = new Arm7Var(avoidNULL.c_str(), getArrayStr.c_str(), TYPE_INT, 1, CONST_FALSE,
                                              ARRAY_TRUE, getArrayVarArraySubs, nullptr);
    getArrayParams->emplace_back(getArrayVarArrayParam);
    // int getarray(int[])
    auto *funcGetArray = new Arm7Func(getArrayStr.c_str(), TYPE_INT, getArrayParams);
    auto *symbolGetArray = new Symbol(nullptr, funcGetArray);
    symbolsLib->emplace_back(symbolGetArray);

    std::string putIntStr = "putint";
    auto *putIntParams = new std::vector<Arm7Var *>();
    // int
    auto *putIntVarParam =
            new Arm7Var(avoidNULL.c_str(), putIntStr.c_str(), TYPE_INT, 1, CONST_FALSE, ARRAY_FALSE, nullptr, nullptr);
    putIntParams->emplace_back(putIntVarParam);
    // void putint(int)
    auto *funcPutInt = new Arm7Func(putIntStr.c_str(), TYPE_VOID, putIntParams);
    auto *symbolPutInt = new Symbol(nullptr, funcPutInt);
    symbolsLib->emplace_back(symbolPutInt);

    std::string putChStr = "putch";
    auto *putChParams = new std::vector<Arm7Var *>();
    // int
    auto *putChVarParam =
            new Arm7Var(avoidNULL.c_str(), putChStr.c_str(), TYPE_INT, 1, CONST_FALSE, CONST_FALSE, nullptr, nullptr);
    putChParams->emplace_back(putChVarParam);
    // void putch(int)
    auto *funcPutCh = new Arm7Func(putChStr.c_str(), TYPE_VOID, putChParams);
    auto *symbolPutCh = new Symbol(nullptr, funcPutCh);
    symbolsLib->emplace_back(symbolPutCh);

    std::string putArrayStr = "putarray";
    auto *putArrayParams = new std::vector<Arm7Var *>();
    // int
    auto *putArrayVarParam =
            new Arm7Var(avoidNULL.c_str(), putArrayStr.c_str(), TYPE_INT, 1, CONST_FALSE, ARRAY_TRUE, nullptr, nullptr);
    putArrayParams->emplace_back(putArrayVarParam);
    // int[]
    auto *putArrayVarArraySubs = new std::vector<int>();
    auto *putArrayVarArrayParam = new Arm7Var(avoidNULL.c_str(), putArrayStr.c_str(), TYPE_INT, 1, CONST_FALSE,
                                              ARRAY_TRUE, putArrayVarArraySubs, nullptr);
    putArrayParams->emplace_back(putArrayVarArrayParam);
    // void putarray(int, int[])
    auto *funcPutArray = new Arm7Func(putArrayStr.c_str(), TYPE_VOID, putArrayParams);
    auto *symbolPutArray = new Symbol(nullptr, funcPutArray);
    symbolsLib->emplace_back(symbolPutArray);

    auto *putFParams = new std::vector<Arm7Var *>();
    // <格式串>
    auto *putFStrParam =
            new Arm7Var(avoidNULL.c_str(), putFStr.c_str(), TYPE_STR, 1, CONST_FALSE, ARRAY_FALSE, nullptr, nullptr);
    putFParams->emplace_back(putFStrParam);
    // int... jump -> 写死判断
    // void putf(<格式串>, int, …)
    auto *funcPutF = new Arm7Func(putFStr.c_str(), TYPE_VOID, putFParams);
    auto *symbolPutF = new Symbol(nullptr, funcPutF);
    symbolsLib->emplace_back(symbolPutF);

    std::string startTimeStr = "starttime";
    auto *startTimeParams = new std::vector<Arm7Var *>();
    // void starttime()
    auto *funcStartTime = new Arm7Func(startTimeStr.c_str(), TYPE_VOID, startTimeParams);
    auto *symbolStartTime = new Symbol(nullptr, funcStartTime);
    symbolsLib->emplace_back(symbolStartTime);

    std::string stopTimeStr = "stoptime";
    auto *stopTimeParams = new std::vector<Arm7Var *>();
    // void stoptime()
    auto *funcStopTime = new Arm7Func(stopTimeStr.c_str(), TYPE_VOID, stopTimeParams);
    auto *symbolStopTime = new Symbol(nullptr, funcStopTime);
    symbolsLib->emplace_back(symbolStopTime);

    auto *symbolLibTable = new SymbolTable(SYMBOL_TABLE_EXTERN, SYMBOL_TABLE_EXTERN_STR.c_str(), symbolsLib);
    symbolTables->push_back(symbolLibTable);
}
