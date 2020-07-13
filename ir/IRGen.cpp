#include <iostream>

#include "IRGen.h"
#include "../parser/Parse.h"
#include "../util/Error.h"
#include "../semantic/Semantic.h"


///===-----------------------------------------------------------------------===///
/// 构造方法
///===-----------------------------------------------------------------------===///

IRGen::IRGen() {
    Parse parse;
    parse.parseAST();
    ast = parse.getAST();

    symbolTables = new std::vector<SymbolTable *>();
}

void IRGen::startIrGen() {
    auto *symbolsGlobal = new std::vector<Symbol *>();
    auto *symbolTableGlobal = new SymbolTable(SYMBOL_TABLE_GLOBAL, symbolsGlobal);
    symbolTables->push_back(symbolTableGlobal);

    auto *irGlobals = new std::vector<IRGlobal *>();

    for (Decl *decl:*(ast->getDecls())) {
        IRGlobal *irGlobal = nullptr;
        auto *irStmts = new std::vector<IRStmt *>();

        if (decl->getConstDecl() != nullptr) {
            for (ConstDef *constDef: *decl->getConstDecl()->getConstDefs()) {
                if (constDef->getConstExps()->empty()) {  // 常量变量
                    ConstVar *constVar = genConstVar(constDef, decl->getConstDecl()->getBType(), irStmts,
                                                     REGISTER_GLOBAL);
                    auto *symbol = new Symbol(nullptr, nullptr, constVar, nullptr, nullptr);
                    symbolsGlobal->emplace_back(symbol);
                } else {  // 常量数组
                    ConstVarArray *constVarArray = genConstVarArray(constDef, decl->getConstDecl()->getBType(), irStmts,
                                                                    REGISTER_GLOBAL);
                    auto *symbol = new Symbol(nullptr, nullptr, nullptr, constVarArray, nullptr);
                    symbolsGlobal->emplace_back(symbol);
                }
                // create irGlobalVar from irStmts
                auto *irGlobalVar = new IRGlobalVar(irStmts);
                irGlobal = new IRGlobal(irGlobalVar, nullptr);
            } //  end constDefs
        } else if (decl->getVarDecl() != nullptr) {
            for (VarDef *varDef: *decl->getVarDecl()->getVarDefs()) {  // 变量
                if (varDef->getConstExps()->empty()) {
                    genVar(varDef, decl->getVarDecl()->getBType(), irStmts);
                } else {  // 数组
                    genVarArray(varDef, decl->getVarDecl()->getBType(), irStmts);
                }
                // create irGlobalVar from irStmts
                auto *irGlobalVar = new IRGlobalVar(irStmts);
                irGlobal = new IRGlobal(irGlobalVar, nullptr);
            }  // end varDefs
        } else if (decl->getFuncDef() != nullptr) {  // 函数
            auto *baseBlocks = new std::vector<IRLocalBlock *>();
            genFunc(decl->getFuncDef(), baseBlocks);
            // create irGlobalFunc from baseBlocks
            auto *irGlobalFunc = new IRGlobalFunc(decl->getFuncDef()->getIdent().c_str(),
                                                  decl->getFuncDef()->getFuncType(), baseBlocks);
            irGlobal = new IRGlobal(nullptr, irGlobalFunc);
        } else {
            Error::errorSemanticDecl("Error Semantic", decl);
        }
        irGlobals->emplace_back(irGlobal);
    }  // end ast;
}

ConstVar *IRGen::genConstVar(ConstDef *constDef, int constType, std::vector<IRStmt *> *irStmts,
                             const std::string &locType) {
    int intRet = calConstExp(constDef->getConstInitVal()->getConstExp());

    auto *globalIrStmt = new IRStmt(DAG_Con_GLOBAL_i32, (locType + constDef->getIdent()).c_str());
    auto *storeIrStmt = new IRStmt(DAG_STORE, (locType + constDef->getIdent()).c_str(),
                                   std::to_string(intRet).c_str());
    irStmts->emplace_back(globalIrStmt);
    irStmts->emplace_back(storeIrStmt);

    auto *constVarRet = new ConstVar(constDef->getIdent().c_str(), constType, intRet);
    return constVarRet;
}

ConstVarArray *IRGen::genConstVarArray(ConstDef *constDef, int constType, std::vector<IRStmt *> *irStmts,
                                       const std::string &locType) {
    auto *subs = new std::vector<int>();
    for (ConstExp *constExp:*constDef->getConstExps()) {
        subs->push_back(calConstExp(constExp));
    }

    std::vector<int> *values = calConstArrayInitVals(constDef->getConstInitVal(), subs);


    auto *subs_str = new std::vector<std::string>();
    for (int sub : *subs) {
        subs_str->emplace_back(std::to_string(sub));
    }
    auto *globalStarIrStmt =
            new IRStmt(DAG_Con_GLOBAL_ARRAY_i32, (locType + constDef->getIdent()).c_str(), "", subs_str);
    // 初始化赋值，考虑到 int a[4][2] = {} 无嵌套 与 values 已存数值之情况，constArray统一在此计算（ VarArray 则不然）
    // 又:出于地址索引特性，一律按照一维展开方式存数 如: int a[4][2] 的 a[1][1] = 1 在此表示为 a[0][3] = 1

    auto subs_ = new std::vector<std::string>();  // 后续赋值索引用
    int len = 1;
    for (int i:*subs) {
        subs_->push_back("0");
        len *= i;
    }
    auto *assignIrStmts = new std::vector<IRStmt *>();
    for (int i = 0; i < len; i++) {
        subs_->pop_back();
        subs_->push_back(std::to_string(i));
        auto subAux = new std::vector<std::string>();
        subAux->assign(subs_->begin(), subs_->end());
        auto *getStmt = new IRStmt(DAG_GETPTR, (locType + std::to_string(stepNameGlobal)).c_str(),
                                   (locType + constDef->getIdent()).c_str(), subAux);
        auto *storeStmt = new IRStmt(DAG_STORE, (locType + std::to_string(stepNameGlobal)).c_str(),
                                     std::to_string(values->at(i)).c_str());
        assignIrStmts->emplace_back(getStmt);
        assignIrStmts->emplace_back(storeStmt);
        stepNameGlobal++;
    }
    irStmts->emplace_back(globalStarIrStmt);
    irStmts->insert(irStmts->end(), assignIrStmts->begin(), assignIrStmts->end());

    auto *constVarArray =
            new ConstVarArray(constDef->getIdent().c_str(), constType, values, subs);
    return constVarArray;
}

void IRGen::genVar(VarDef *varDef, int varType, std::vector<IRStmt *> *irStmts) {

}

void IRGen::genVarArray(VarDef *varDef, int varType, std::vector<IRStmt *> *irStmts) {

}

void IRGen::genFunc(FuncDef *funcDef, std::vector<IRLocalBlock *> *basicBlocks) {
    auto *irStmts = new std::vector<IRStmt *>();
    auto *preLocs = new std::set<std::string>();
    auto *blockEntry = new IRLocalBlock(BLOCK_ENTRY.c_str(), irStmts, preLocs);
    basicBlocks->emplace_back(blockEntry);

    if (funcDef->getFuncFParams() != nullptr) {
        for (FuncFParam *funcFParam:*funcDef->getFuncFParams()->getFuncFParams()) {

        }
    }

    genBlock(funcDef->getBlock(), basicBlocks, blockEntry, irStmts);
}

///===-----------------------------------------------------------------------===///
/// 语句、语句块
///===-----------------------------------------------------------------------===///

void IRGen::genStmt(Stmt *stmt, std::vector<IRLocalBlock *> *basicBlocks, IRLocalBlock *lastBlock,
                    std::vector<IRStmt *> *lastBlockStmts) {
    switch (stmt->getStmtType()) {
        case STMT_EXP:
            break;
        case STMT_BLOCK:
            break;
        case STMT_IF: {
            break;
        }
        case STMT_WHILE: {
            break;
        }
        case STMT_LVAL_ASSIGN: {
            break;
        }
        case STMT_RETURN: {
            if (stmt->getExp() == nullptr) {
                auto *irStmtRet = new IRStmt(STMT_RETURN, OPD_NULL.c_str(), OPD_NULL.c_str(), nullptr);
                lastBlockStmts->emplace_back(irStmtRet);
            }else{
                std::string registerRet = genAddExp(stmt->getExp()->getAddExp(), lastBlockStmts);
                auto *irStmtRet = new IRStmt(STMT_RETURN, registerRet.c_str(), OPD_NULL.c_str(), nullptr);
                lastBlockStmts->emplace_back(irStmtRet);
            }
            break;
        }
        case STMT_EXP_BLANK:
        case STMT_CONTINUE:
        case STMT_BREAK:
            break;
    }

}

void IRGen::genBlock(Block *block, std::vector<IRLocalBlock *> *basicBlocks, IRLocalBlock *lastBlock,
                     std::vector<IRStmt *> *lastBlockStmts) {
    auto *symbolsLocal = new std::vector<Symbol *>();
    auto *symbolTableLocal = new SymbolTable(SYMBOL_TABLE_GLOBAL, symbolsLocal);
    symbolTables->emplace_back(symbolTableLocal);

    for (BlockItem *blockItem:*block->getBlockItems()) {
        if (blockItem->getConstDecl() != nullptr) {
            for (ConstDef *constDef: *blockItem->getConstDecl()->getConstDefs()) {
                if (constDef->getConstExps()->empty()) {  // 常量变量
                    ConstVar *constVar = genConstVar(constDef, blockItem->getVarDecl()->getBType(), lastBlockStmts,
                                                     REGISTER_LOCAL);
                    auto *symbol = new Symbol(nullptr, nullptr, constVar, nullptr, nullptr);
                    symbolsLocal->emplace_back(symbol);
                } else {  // 常量数组
                    ConstVarArray *constVarArray = genConstVarArray(constDef, blockItem->getConstDecl()->getBType(),
                                                                    lastBlockStmts, REGISTER_LOCAL);
                    auto *symbol = new Symbol(nullptr, nullptr, nullptr, constVarArray, nullptr);
                    symbolsLocal->emplace_back(symbol);
                }
            } //  end constDefs
        } else if (blockItem->getVarDecl() != nullptr) {
            for (VarDef *varDef: *blockItem->getVarDecl()->getVarDefs()) {
                if (varDef->getConstExps()->empty()) {
                    genVar(varDef, blockItem->getVarDecl()->getBType(), lastBlockStmts);
                } else {  // 数组
                    genVarArray(varDef, blockItem->getVarDecl()->getBType(), lastBlockStmts);
                }
            }  // end varDefs
        } else if (blockItem->getStmt() != nullptr) {
            genStmt(blockItem->getStmt(), basicBlocks, lastBlock, lastBlockStmts);
        }
    }

    symbolTables->pop_back();
}

///===-----------------------------------------------------------------------===///
/// 表达式 计算 不生成代码
///===-----------------------------------------------------------------------===///

std::vector<int> *IRGen::calConstArrayInitVals(ConstInitVal *constInitVal, std::vector<int> *subs) {
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

int IRGen::calConstExp(ConstExp *constExp) {
    return calAddExp(constExp->getAddExp());
}

int IRGen::calAddExp(AddExp *addExp) {
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

int IRGen::calMulExp(MulExp *mulExp) {
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

int IRGen::calUnaryExp(UnaryExp *unaryExp) {
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

int IRGen::calPrimaryExp(PrimaryExp *primaryExp) {
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

int IRGen::calLVal(LVal *lVal) {
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

const char *IRGen::genAddExp(AddExp *addExp, std::vector<IRStmt *> *irStmts) {
    return nullptr;
}


