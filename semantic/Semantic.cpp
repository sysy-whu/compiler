#include "Semantic.h"
#include "../util/Error.h"

int calConstExp(ConstExp *pExp);

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
/// 内部解析 ast
///===-----------------------------------------------------------------------===///



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
                if (varDef->getConstExps() == nullptr) {
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

            if (semanticBlock(decl->getFuncDef()->getBlock()) == 0) {
                continue;
            } else {
                Error::errorSemanticDecl("Error Semantic Func", decl);
            }
//            SymbolTable *funcBlockSymbolTable = semanticBlock(decl->getFuncDef()->getBlock());
//            symbolTables->emplace_back(funcBlockSymbolTable);
        } else {
            Error::errorSemanticDecl("Error Semantic", decl);
        }
    }  // end ast;
}

///===-----------------------------------------------------------------------===///
/// 分析 ast 得到符号表相关内容
///===-----------------------------------------------------------------------===///

Var *Semantic::semanticVar(VarDef *varDef, int varType) {
    return nullptr;
}

VarArray *Semantic::semanticVarArray(VarDef *varDef, int varType) {
    return nullptr;
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
                              constDef->getLocs()->at(0)->getCol(), constDef->getLocs()->at(0)->getLine());
    return constVarArray;
}

Func *Semantic::semanticFunc(FuncDef *funcDef) {
    return nullptr;
}

///===-----------------------------------------------------------------------===///
/// Stmt 语句
///===-----------------------------------------------------------------------===///

int Semantic::semanticStmt(Stmt *stmt) {
    return 0;
}

int Semantic::semanticBlock(Block *block) {
    return 0;
}

int Semantic::semanticBlockItem(BlockItem *blockItem) {
    return 0;
}

///===-----------------------------------------------------------------------===///
/// Expr 表达式 CalConst
///===-----------------------------------------------------------------------===///
std::vector<int> *Semantic::calConstArrayInitVals(ConstInitVal *constInitVal, std::vector<int> *subs) {
    auto *valuesRet = new std::vector<int>();
    int len = 1;
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
                int sub_first_len = 1;
                for (int i = 1; i < subs->size(); i++) {
                    sub_first_len *= subs->at(i);
                }
                if (valuesRet->size() % sub_first_len == 0) {
                    int tmp = subs->at(0);  // 当前第一维度值
                    subs->erase(subs->begin());
                    std::vector<int> *subs_values_inner = calConstArrayInitVals(initValInner, subs);
                    subs->insert(subs->begin(), tmp);
                    valuesRet->insert(valuesRet->end(), subs_values_inner->begin(), subs_values_inner->end());  // 拼接
                } else {
                    Error::errorSim("calConstArrayInitVals");
                }
            }
        }
    }
    if (valuesRet->size() < len) {
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
    int intRet = 0;
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
        for (int i = symbolTables->size() - 1; i > 0; i--) {
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
        for (int i = symbolTables->size() - 1; i > 0; i--) {
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
///===-----------------------------------------------------------------------===///

int Semantic::semanticAddExp(AddExp *addExp) {
    return 0;
}

int Semantic::semanticLOrExp(LOrExp *lOrExp) {
    return 0;
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
    auto *getArrayVarArrayParam = new VarArray(avoidNULL.c_str(), TYPE_INT, nullptr, nullptr, 0, 0, 0);
    auto *getArrayVarArraySymbol = new Symbol(nullptr, getArrayVarArrayParam, nullptr, nullptr, nullptr);
    getArrayParams->emplace_back(getArrayVarArraySymbol);
    // int getarray(int[])
    auto *funcGetArray = new Func(getArrayStr.c_str(), TYPE_INT, getArrayParams, 0, 0);
    auto *symbolGetArray = new Symbol(nullptr, nullptr, nullptr, nullptr, funcGetArray);
    symbolsLib->emplace_back(symbolGetArray);

    std::string putIntStr = "putint";
    auto *putIntParams = new std::vector<Symbol *>();
    // int
    auto *putIntVarParam = new Var(avoidNULL.c_str(), TYPE_INT, 0, 0, 0, 0);
    auto *putIntVarSymbol = new Symbol(putIntVarParam, nullptr, nullptr, nullptr, nullptr);
    putIntParams->emplace_back(putIntVarSymbol);
    // void putint(int)
    auto *funcPutInt = new Func(putIntStr.c_str(), TYPE_VOID, putIntParams, 0, 0);
    auto *symbolPutInt = new Symbol(nullptr, nullptr, nullptr, nullptr, funcPutInt);
    symbolsLib->emplace_back(symbolPutInt);

    std::string putChStr = "putch";
    auto *putChParams = new std::vector<Symbol *>();
    // int
    auto *putChVarParam = new Var(avoidNULL.c_str(), TYPE_INT, 0, 0, 0, 0);
    auto *putChVarSymbol = new Symbol(putChVarParam, nullptr, nullptr, nullptr, nullptr);
    putChParams->emplace_back(putChVarSymbol);
    // void putch(int)
    auto *funcPutCh = new Func(putChStr.c_str(), TYPE_VOID, putChParams, 0, 0);
    auto *symbolPutCh = new Symbol(nullptr, nullptr, nullptr, nullptr, funcPutCh);
    symbolsLib->emplace_back(symbolPutCh);

    std::string putArrayStr = "putarray";
    auto *putArrayParams = new std::vector<Symbol *>();
    // int
    auto *putArrayVarParam = new Var(avoidNULL.c_str(), TYPE_INT, 0, 0, 0, 0);
    auto *putArrayVarSymbol = new Symbol(putArrayVarParam, nullptr, nullptr, nullptr, nullptr);
    putArrayParams->emplace_back(putArrayVarSymbol);
    // int[]
    auto *putArrayVarArrayParam = new VarArray(avoidNULL.c_str(), TYPE_INT, nullptr, nullptr, 0, 0, 0);
    auto *putArrayVarArraySymbol = new Symbol(nullptr, putArrayVarArrayParam, nullptr, nullptr, nullptr);
    putArrayParams->emplace_back(putArrayVarArraySymbol);
    // void putarray(int, int[])
    auto *funcPutArray = new Func(putArrayStr.c_str(), TYPE_VOID, putArrayParams, 0, 0);
    auto *symbolPutArray = new Symbol(nullptr, nullptr, nullptr, nullptr, funcPutArray);
    symbolsLib->emplace_back(symbolPutArray);

    std::string putFStr = "putf";
    auto *putFParams = new std::vector<Symbol *>();
    // <格式串>
    auto *putFStrParam = new Var(avoidNULL.c_str(), TYPE_STR, 0, 0, 0, 0);
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

