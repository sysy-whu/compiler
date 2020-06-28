#include "Semantic.h"
#include "../util/Error.h"

///===-----------------------------------------------------------------------===///
/// 构造方法
///===-----------------------------------------------------------------------===///
Semantic::Semantic() {
    Parse parse;
    parse.parseAST();
    ast = parse.getAST();
    std::cout << ast->getDecls()->size() << std::endl;
    symbolTables = new std::vector<SymbolTable *>();
}

///===-----------------------------------------------------------------------===///
/// 启动接口
///===-----------------------------------------------------------------------===///
void Semantic::startSemantic() {
    /// 在这里无脑设置 sylib.h 声明函数
    setSyLib_H();
    std::cout << "hello" << std::endl;
    auto *symbolsGlobal = new std::vector<Symbol *>();
    auto *symbolTableGlobal = new SymbolTable(SYMBOL_TABLE_GLOBAL, symbolsGlobal);
    symbolTables->emplace_back(symbolTableGlobal);
    /// 处理ast
    for (Decl *decl:*(ast->getDecls())) {
        if (decl->getConstDecl() != nullptr) {
            for (ConstDef *constDef: *decl->getConstDecl()->getConstDefs()) {
                if (constDef->getConstExps()->empty()) {  // 常量变量
                    ConstVar *constVar = semanticConstVar(constDef, decl->getConstDecl()->getBType());
                    auto *symbol = new Symbol(nullptr, nullptr, constVar, nullptr, nullptr);
                    symbolsGlobal->emplace_back(symbol);
                } else {  // 常量数组
                    ConstVarArray *constVarArray = semanticConstVarArray(constDef, decl->getConstDecl()->getBType());
                    auto *symbol = new Symbol(nullptr, nullptr, nullptr, constVarArray, nullptr);
                    symbolsGlobal->emplace_back(symbol);
                }
            } //  end constDefs
        } else if (decl->getVarDecl() != nullptr) {
            for (VarDef *varDef: *decl->getVarDecl()->getVarDefs()) {
                if (varDef->getConstExps()->empty()) {
                    Var *var = semanticVar(varDef, decl->getVarDecl()->getBType());
                    auto *symbol = new Symbol(var, nullptr, nullptr, nullptr, nullptr);
                    symbolsGlobal->emplace_back(symbol);
                } else {
                    VarArray *varArray = semanticVarArray(varDef, decl->getVarDecl()->getBType());
                    auto *symbol = new Symbol(nullptr, varArray, nullptr, nullptr, nullptr);
                    symbolsGlobal->emplace_back(symbol);
                }
            }  // end varDefs
        } else if (decl->getFuncDef() != nullptr) {
            Func *func = semanticFunc(decl->getFuncDef());
            auto *symbol = new Symbol(nullptr, nullptr, nullptr, nullptr, func);
            symbolsGlobal->emplace_back(symbol);
        } else {
            Error::errorSemanticDecl("Error Semantic", decl);
        }
    }  // end ast;
}

///===-----------------------------------------------------------------------===///
/// 分析 ast 得到符号表相关内容
///===-----------------------------------------------------------------------===///

Var *Semantic::semanticVar(VarDef *varDef, int varType) {
//    int initState = varDef->getInitVal() == nullptr ? INIT_FALSE : INIT_TRUE;
    if (varDef->getInitVal() != nullptr &&
        semanticAddExp(varDef->getInitVal()->getExp()->getAddExp()) != varType) {
        Error::errorSim("var init wrong when assign");
        exit(-1);
    }
    Var *varRet = new Var(varDef->getIdent().c_str(), varType,
                          varDef->getLocs()->at(0)->getLine(), varDef->getLocs()->at(0)->getCol());
    return varRet;
}

VarArray *Semantic::semanticVarArray(VarDef *varDef, int varType) {
//    auto *initState = new std::vector<int>();
    auto *subs = new std::vector<int>();
    // 计算下标
    for (ConstExp *constExp:*varDef->getConstExps()) {
        subs->push_back(calConstExp(constExp));
    }
    // 核算初始值
    if (varDef->getInitVal() != nullptr) {
        semanticVarArrayInitVals(varDef->getInitVal(), subs);
    }

    auto *varArray = new VarArray(varDef->getIdent().c_str(), varType, subs,
                                  varDef->getLocs()->at(0)->getLine(), varDef->getLocs()->at(0)->getCol());
    return varArray;
}

ConstVar *Semantic::semanticConstVar(ConstDef *constDef, int constType) {
    int intRet = calConstExp(constDef->getConstInitVal()->getConstExp());
    auto *constVarRet = new ConstVar(constDef->getIdent().c_str(), TYPE_INT, intRet,
                                     constDef->getLocs()->at(0)->getLine(), constDef->getLocs()->at(0)->getCol());
    return constVarRet;
}

ConstVarArray *Semantic::semanticConstVarArray(ConstDef *constDef, int constType) {
    auto *subs = new std::vector<int>();
    for (ConstExp *constExp:*constDef->getConstExps()) {
        subs->push_back(calConstExp(constExp));
    }

    std::vector<int> *values = calConstArrayInitVals(constDef->getConstInitVal(), subs);

    auto *constVarArray =
            new ConstVarArray(constDef->getIdent().c_str(), TYPE_INT, values, subs,
                              constDef->getLocs()->at(0)->getLine(), constDef->getLocs()->at(0)->getCol());
    return constVarArray;
}

Func *Semantic::semanticFunc(FuncDef *funcDef) {
    auto *params = new std::vector<Symbol *>();
    // 处理函数部分，后续返回加入符号表
    if (funcDef->getFuncFParams() != nullptr) {
        for (FuncFParam *funcFParam:*funcDef->getFuncFParams()->getFuncFParams()) {
            Symbol *symbol = nullptr;
            if (funcFParam->getBType() == TYPE_INT) {  // 变量
                auto *var = new Var(funcFParam->getIdent().c_str(), funcFParam->getBType(),
                                    funcFParam->getLocs()->at(0)->getLine(), funcFParam->getLocs()->at(0)->getCol());
                symbol = new Symbol(var, nullptr, nullptr, nullptr, nullptr);
            } else if (funcFParam->getBType() == TYPE_INT_STAR) {  // 数组 （第一维为空）
                auto *subs = new std::vector<int>();
                for (Exp *exp: *funcFParam->getExps()) {
                    subs->push_back(calAddExp(exp->getAddExp()));
                }
                auto *varArray =
                        new VarArray(funcFParam->getIdent().c_str(), funcFParam->getBType(), subs,
                                     funcFParam->getLocs()->at(0)->getLine(), funcFParam->getLocs()->at(0)->getCol());
                symbol = new Symbol(nullptr, varArray, nullptr, nullptr, nullptr);
            }
            params->emplace_back(symbol);
        }
    }

    // 对函数 Block 语义检查
    semanticBlock(funcDef->getBlock(), funcDef->getFuncType(), params);

    auto *func = new Func(funcDef->getIdent().c_str(), funcDef->getFuncType(), params,
                          funcDef->getLocs()->at(0)->getLine(), funcDef->getLocs()->at(0)->getCol());
    return func;
}

///===-----------------------------------------------------------------------===///
/// Stmt 语句
///===-----------------------------------------------------------------------===///

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

void Semantic::semanticBlock(Block *block, int funcRetType, std::vector<Symbol *> *paramSymbols) {
    auto *symbolsLocal = new std::vector<Symbol *>();
    if (paramSymbols != nullptr)
        symbolsLocal->insert(symbolsLocal->end(), paramSymbols->begin(), paramSymbols->end());
    auto *symbolTableLocal = new SymbolTable(SYMBOL_TABLE_GLOBAL, symbolsLocal);
    symbolTables->emplace_back(symbolTableLocal);

    for (BlockItem *blockItem:*block->getBlockItems()) {
        if (blockItem->getConstDecl() != nullptr) {
            for (ConstDef *constDef: *blockItem->getConstDecl()->getConstDefs()) {
                if (constDef->getConstExps()->empty()) {  // 常量变量
                    ConstVar *constVar = semanticConstVar(constDef, blockItem->getConstDecl()->getBType());
                    auto *symbol = new Symbol(nullptr, nullptr, constVar, nullptr, nullptr);
                    symbolsLocal->emplace_back(symbol);
                } else {  // 常量数组
                    ConstVarArray *constVarArray = semanticConstVarArray(constDef,
                                                                         blockItem->getConstDecl()->getBType());
                    auto *symbol = new Symbol(nullptr, nullptr, nullptr, constVarArray, nullptr);
                    symbolsLocal->emplace_back(symbol);
                }
            } //  end constDefs
        } else if (blockItem->getVarDecl() != nullptr) {
            for (VarDef *varDef: *blockItem->getVarDecl()->getVarDefs()) {
                if (varDef->getConstExps()->empty()) {
                    Var *var = semanticVar(varDef, blockItem->getVarDecl()->getBType());
                    auto *symbol = new Symbol(var, nullptr, nullptr, nullptr, nullptr);
                    symbolsLocal->emplace_back(symbol);
                } else {
                    VarArray *varArray = semanticVarArray(varDef, blockItem->getVarDecl()->getBType());
                    auto *symbol = new Symbol(nullptr, varArray, nullptr, nullptr, nullptr);
                    symbolsLocal->emplace_back(symbol);
                }
            }  // end varDefs
        } else if (blockItem->getStmt() != nullptr) {
            semanticStmt(blockItem->getStmt(), funcRetType);
        }
    }
    // delete & free
    symbolTables->pop_back();
//    symbolTables->erase(symbolTables->cend(), symbolTables->cend());
//    symbolTableLocal->free();
//    std::vector<Symbol *>().swap(*symbolsLocal);
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
    if (lVal->getExps()->empty()) {  // 符号表找常量
        for (int i = (int) symbolTables->size() - 1; i > 0; i--) {
            for (Symbol *symbol:*symbolTables->at(i)->getSymbols()) {
                if (symbol->getConstVarInner() != nullptr &&
                    symbol->getConstVarInner()->getIdent() == lVal->getIdent())
                    return symbol->getConstVarInner()->getValue();
            }
        }
    } else {  // 符号表找数组
        // 索引下标
        auto *subs = new std::vector<int>();
        for (Exp *exp:*lVal->getExps()) {
            subs->push_back(calAddExp(exp->getAddExp()));
        }
        for (int i = (int) symbolTables->size() - 1; i > 0; i--) {
            for (Symbol *symbol:*symbolTables->at(i)->getSymbols()) {
                if (symbol->getConstVarArrayInner() != nullptr &&
                    symbol->getConstVarArrayInner()->getIdent() == lVal->getIdent()) {
                    if (subs->size() == symbol->getConstVarArrayInner()->getSubs()->size()) {
                        int pos = 0;  // 计算索引一维位置
                        for (int j = 0; j < subs->size(); j++) {
                            int subs_subs_len = 1;
                            for (int k = j + 1; k < subs->size(); k++) {
                                subs_subs_len *= symbol->getConstVarArrayInner()->getSubs()->at(k);
                            }
                            pos += subs->at(j) * subs_subs_len;
                        }
                        return symbol->getConstVarArrayInner()->getValue()->at(pos);
                    } else {
                        Error::errorSim("error subs Semantic calLVal");
                        exit(-1);
                    }
                }  // end find constArray ident
            }  // end one symbol-table iter
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
                if (symbol->getFuncInner() != nullptr &&
                    symbol->getFuncInner()->getIdent() == unaryExp->getIdent()) {
                    // 检查参数
                    checkRParams(unaryExp->getFuncRParams(), symbol->getFuncInner());
                    // 返回函数返回值
                    return symbol->getFuncInner()->getRetType();
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
        for (int i = (int) symbolTables->size() - 1; i > 0; i--) {
            for (Symbol *symbol:*symbolTables->at(i)->getSymbols()) {
                if ((symbol->getVarInner() != nullptr &&
                     symbol->getVarInner()->getIdent() == lVal->getIdent()) ||
                    (symbol->getConstVarInner() != nullptr &&
                     symbol->getConstVarInner()->getIdent() == lVal->getIdent())) {
                    return TYPE_INT;
                }
            }
        }
        Error::errorSim("no such var");
        exit(-1);
    } else {  // 整型数组
        for (int i = (int) symbolTables->size() - 1; i > 0; i--) {
            for (Symbol *symbol:*symbolTables->at(i)->getSymbols()) {
                if (symbol->getVarArrayInner() != nullptr &&
                    symbol->getVarArrayInner()->getIdent() == lVal->getIdent()) {
                    checkExps(lVal->getExps(), "array sub not int");
                    if (lVal->getExps()->size() == symbol->getVarArrayInner()->getSubs()->size()) {  // 索引到变量
                        return TYPE_INT;
                    } else {  // 索引到数组
                        return TYPE_INT_STAR;
                    }
                } else if (symbol->getConstVarArrayInner() != nullptr &&
                           symbol->getConstVarArrayInner()->getIdent() == lVal->getIdent()) {
                    checkExps(lVal->getExps(), "const array sub not int");
                    if (lVal->getExps()->size() == symbol->getConstVarArrayInner()->getSubs()->size()) { // 索引到变量
                        return TYPE_INT;
                    } else {  // 索引到数组
                        return TYPE_INT_STAR;
                    }
                }
            }// 遍历 table symbol
        }  // 遍历 tables
        Error::errorSim("no such array");
        exit(-1);
    }  // 整型、整型数组
}

/**
 * 检查数组下标
 * @param exps
 * @return exit(-1): Error
 */
void Semantic::checkExps(std::vector<Exp *> *exps, const char *errMsg) {
    for (Exp *exp: *exps) {
        if (semanticAddExp(exp->getAddExp()) != TYPE_INT) {
            Error::errorSim(errMsg);
            exit(-1);
        }
    }
}

/**
 * 检查函数参数
 * @param funcRParams 实参
 * @param func 形参
 * @Return exit(-1) when wrong
 */
void Semantic::checkRParams(FuncRParams *funcRParams, Func *func) {
    if (funcRParams == nullptr && func->getParams()->empty()) {
        return;
    } else if (funcRParams->getExps()->size() == func->getParams()->size()) {
        // j 为索引遍历检查实参
        for (int j = 0; j < funcRParams->getExps()->size(); j++) {
            switch (semanticAddExp(funcRParams->getExps()->at(j)->getAddExp())) {
                case TYPE_INT:
                    if (func->getParams()->at(j)->getVarInner() != nullptr ||
                        func->getParams()->at(j)->getConstVarInner() != nullptr) {
                        break;
                    } else {
                        Error::errorSim("semanticUnaryExp func param int");
                        exit(-1);
                    }
                case TYPE_INT_STAR:
                    if (func->getParams()->at(j)->getVarArrayInner() != nullptr ||
                        func->getParams()->at(j)->getConstVarArrayInner() != nullptr) {
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
    } else if (func->getIdent() != putFStr /*&& !funcRParams->getStrOut().empty()*/) {
        Error::errorSim("semanticUnaryExp func param");
        exit(-1);
    }
}

///===-----------------------------------------------------------------------===///
/// lib
///===-----------------------------------------------------------------------===///

void Semantic::setSyLib_H() {
    auto *symbolsLib = new std::vector<Symbol *>();
    std::string avoidNULL = "nullptr";

    std::string getIntStr = "getint";
    // int getint()
    auto *funcGetint = new Func(getIntStr.c_str(), TYPE_INT, nullptr, 0, 0);
    auto *symbolGetInt = new Symbol(nullptr, nullptr, nullptr, nullptr, funcGetint);
    symbolsLib->emplace_back(symbolGetInt);

    std::string getChStr = "getch";
    // int getch()
    auto *funcGetCh = new Func(getChStr.c_str(), TYPE_INT, nullptr, 0, 0);
    auto *symbolGetCh = new Symbol(nullptr, nullptr, nullptr, nullptr, funcGetCh);
    symbolsLib->emplace_back(symbolGetCh);

    std::string getArrayStr = "getarray";
    auto *getArrayParams = new std::vector<Symbol *>();
    // int[]
    auto *getArrayVarArraySubs = new std::vector<int>();
    auto *getArrayVarArrayParam = new VarArray(avoidNULL.c_str(), TYPE_INT, getArrayVarArraySubs, 0, 0);
    auto *getArrayVarArraySymbol = new Symbol(nullptr, getArrayVarArrayParam, nullptr, nullptr, nullptr);
    getArrayParams->emplace_back(getArrayVarArraySymbol);
    // int getarray(int[])
    auto *funcGetArray = new Func(getArrayStr.c_str(), TYPE_INT, getArrayParams, 0, 0);
    auto *symbolGetArray = new Symbol(nullptr, nullptr, nullptr, nullptr, funcGetArray);
    symbolsLib->emplace_back(symbolGetArray);

    std::string putIntStr = "putint";
    auto *putIntParams = new std::vector<Symbol *>();
    // int
    auto *putIntVarParam = new Var(avoidNULL.c_str(), TYPE_INT, 0, 0);
    auto *putIntVarSymbol = new Symbol(putIntVarParam, nullptr, nullptr, nullptr, nullptr);
    putIntParams->emplace_back(putIntVarSymbol);
    // void putint(int)
    auto *funcPutInt = new Func(putIntStr.c_str(), TYPE_VOID, putIntParams, 0, 0);
    auto *symbolPutInt = new Symbol(nullptr, nullptr, nullptr, nullptr, funcPutInt);
    symbolsLib->emplace_back(symbolPutInt);

    std::string putChStr = "putch";
    auto *putChParams = new std::vector<Symbol *>();
    // int
    auto *putChVarParam = new Var(avoidNULL.c_str(), TYPE_INT, 0, 0);
    auto *putChVarSymbol = new Symbol(putChVarParam, nullptr, nullptr, nullptr, nullptr);
    putChParams->emplace_back(putChVarSymbol);
    // void putch(int)
    auto *funcPutCh = new Func(putChStr.c_str(), TYPE_VOID, putChParams, 0, 0);
    auto *symbolPutCh = new Symbol(nullptr, nullptr, nullptr, nullptr, funcPutCh);
    symbolsLib->emplace_back(symbolPutCh);

    std::string putArrayStr = "putarray";
    auto *putArrayParams = new std::vector<Symbol *>();
    // int
    auto *putArrayVarParam = new Var(avoidNULL.c_str(), TYPE_INT, 0, 0);
    auto *putArrayVarSymbol = new Symbol(putArrayVarParam, nullptr, nullptr, nullptr, nullptr);
    putArrayParams->emplace_back(putArrayVarSymbol);
    // int[]
    auto *putArrayVarArraySubs = new std::vector<int>();
    auto *putArrayVarArrayParam = new VarArray(avoidNULL.c_str(), TYPE_INT, putArrayVarArraySubs, 0, 0);
    auto *putArrayVarArraySymbol = new Symbol(nullptr, putArrayVarArrayParam, nullptr, nullptr, nullptr);
    putArrayParams->emplace_back(putArrayVarArraySymbol);
    // void putarray(int, int[])
    auto *funcPutArray = new Func(putArrayStr.c_str(), TYPE_VOID, putArrayParams, 0, 0);
    auto *symbolPutArray = new Symbol(nullptr, nullptr, nullptr, nullptr, funcPutArray);
    symbolsLib->emplace_back(symbolPutArray);


    auto *putFParams = new std::vector<Symbol *>();
    // <格式串>
    auto *putFStrParam = new Var(avoidNULL.c_str(), TYPE_STR, 0, 0);
    auto *putFStrSymbol = new Symbol(putFStrParam, nullptr, nullptr, nullptr, nullptr);
    putFParams->emplace_back(putFStrSymbol);
    // int... jump -> 写死判断
    // void putf(<格式串>, int, …)
    auto *funcPutF = new Func(putFStr.c_str(), TYPE_VOID, putFParams, 0, 0);
    auto *symbolPutF = new Symbol(nullptr, nullptr, nullptr, nullptr, funcPutF);
    symbolsLib->emplace_back(symbolPutF);

    std::string startTimeStr = "starttime";
    // void starttime()
    auto *funcStartTime = new Func(startTimeStr.c_str(), TYPE_VOID, nullptr, 0, 0);
    auto *symbolStartTime = new Symbol(nullptr, nullptr, nullptr, nullptr, funcStartTime);
    symbolsLib->emplace_back(symbolStartTime);

    std::string stopTimeStr = "stoptime";
    // void stoptime()
    auto *funcStopTime = new Func(stopTimeStr.c_str(), TYPE_VOID, nullptr, 0, 0);
    auto *symbolStopTime = new Symbol(nullptr, nullptr, nullptr, nullptr, funcStopTime);
    symbolsLib->emplace_back(symbolStopTime);

    auto *symbolLibTable = new SymbolTable(SYMBOL_TABLE_EXTERN, symbolsLib);
    symbolTables->push_back(symbolLibTable);
}

