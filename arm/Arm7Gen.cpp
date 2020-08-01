#include "Arm7Gen.h"
#include "../util/Error.h"

Arm7Gen::Arm7Gen() {
    ast = nullptr;
    symbolTables = nullptr;
    armTree = nullptr;

    whilePos = new std::vector<std::string>();
    whileEndPos = new std::vector<std::string>();
    /// TODO 我不知自什么始
    blockName = 0;
}

void Arm7Gen::startGen(AST *ast_, std::vector<SymbolTable *> *symbolTables_) {
    Arm7Gen::ast = ast_;
    Arm7Gen::symbolTables = symbolTables_;

    auto *armGlobals = new std::vector<ArmGlobal *>();

    for (Decl *decl:*(ast->getDecls())) {
        ArmGlobal *armGlobal = nullptr;
        if (decl->getVarDecl() != nullptr) {

        } else if (decl->getConstDecl() != nullptr) {

        }

    }



}

///===-----------------------------------------------------------------------===///
/// 基本声明定义
///===-----------------------------------------------------------------------===///

void Arm7Gen::genArm7Var(Decl *decl, std::vector<ArmStmt *> *armStmts) {

}

void Arm7Gen::genArm7Var(BlockItem *blockItem, std::vector<ArmStmt *> *armStmts) {

}

void Arm7Gen::genArm7Func(FuncDef *funcDef, std::vector<ArmBlock *> *armBlocks) {
    funcNameNow = funcDef->getIdent();
}

///===-----------------------------------------------------------------------===///
/// 语句、语句块
///===-----------------------------------------------------------------------===///

const char *Arm7Gen::genBlock(Block *block, std::vector<ArmBlock *> *basicBlocks, ArmBlock *lastBlock,
                              std::vector<ArmStmt *> *lastBlockStmts) {
    return nullptr;
}

const char *Arm7Gen::genStmt(Stmt *stmt, std::vector<ArmBlock *> *basicBlocks, ArmBlock *lastBlock,
                             std::vector<ArmStmt *> *lastBlockStmts) {
    return nullptr;
}

const char *Arm7Gen::genStmtAuxIf(Stmt *stmt, std::vector<ArmBlock *> *basicBlocks, ArmBlock *lastBlock,
                                  std::vector<ArmStmt *> *lastBlockStmts) {
    return nullptr;
}

const char *Arm7Gen::genStmtAuxWhile(Stmt *stmt, std::vector<ArmBlock *> *basicBlocks, ArmBlock *lastBlock,
                                     std::vector<ArmStmt *> *lastBlockStmts) {
    return nullptr;
}

///===-----------------------------------------------------------------------===///
/// 表达式 不计算 生成代码
///===-----------------------------------------------------------------------===///

std::vector<Exp *> *Arm7Gen::genVarArrayInitVals(InitVal *initVal, std::vector<int> *subs,
                                                 std::vector<ArmStmt *> *ArmStmts) {
    return nullptr;
}

const char *Arm7Gen::genCondExp(Cond *cond, std::vector<ArmStmt *> *ArmStmts) {
    return nullptr;
}

const char *Arm7Gen::genLOrExp(LOrExp *lOrExp, std::vector<ArmStmt *> *ArmStmts) {
    return nullptr;
}

const char *Arm7Gen::genLAndExp(LAndExp *lAndExp, std::vector<ArmStmt *> *ArmStmts) {
    return nullptr;
}

const char *Arm7Gen::genEqExp(EqExp *eqExp, std::vector<ArmStmt *> *ArmStmts) {
    return nullptr;
}

const char *Arm7Gen::genRelExp(RelExp *relExp, std::vector<ArmStmt *> *ArmStmts) {
    return nullptr;
}

const char *Arm7Gen::genAddExp(AddExp *addExp, std::vector<ArmStmt *> *ArmStmts) {
    return nullptr;
}

const char *Arm7Gen::genMulExp(MulExp *mulExp, std::vector<ArmStmt *> *ArmStmts) {
    return nullptr;
}

const char *Arm7Gen::genUnaryExp(UnaryExp *unaryExp, std::vector<ArmStmt *> *ArmStmts) {
    return nullptr;
}

const char *Arm7Gen::genPrimaryExp(PrimaryExp *primaryExp, std::vector<ArmStmt *> *ArmStmts) {
    return nullptr;
}

const char *Arm7Gen::genLVal(LVal *lVal, std::vector<ArmStmt *> *ArmStmts) {
    return nullptr;
}

///===-----------------------------------------------------------------------===///
/// Expr 表达式 CalConst
/// @param one kind of Exp or initVal
/// @return const value of that Exp or initVal
///===-----------------------------------------------------------------------===///

std::vector<int> *Arm7Gen::calConstArrayInitVals(ConstInitVal *constInitVal, std::vector<int> *subs) {
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

std::vector<int> *Arm7Gen::calVarArrayInitVals(InitVal *initVal, std::vector<int> *subs) {
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

int Arm7Gen::calConstExp(ConstExp *constExp) {
    return calAddExp(constExp->getAddExp());
}

int Arm7Gen::calAddExp(AddExp *addExp) {
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

int Arm7Gen::calMulExp(MulExp *mulExp) {
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

int Arm7Gen::calUnaryExp(UnaryExp *unaryExp) {
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

int Arm7Gen::calPrimaryExp(PrimaryExp *primaryExp) {
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

int Arm7Gen::calLVal(LVal *lVal) {
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
