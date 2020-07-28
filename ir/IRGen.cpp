#include "IRGen.h"
#include "../parser/Parse.h"
#include "../util/Error.h"


///===-----------------------------------------------------------------------===///
/// 构造方法
///===-----------------------------------------------------------------------===///

IRGen::IRGen() {
    Parse parse;
    parse.parseAST();
    ast = parse.getAST();

    symbolTables = new std::vector<SymbolTable *>();
    whilePos = new std::vector<std::string>();
    whileEndPos = new std::vector<std::string>();
}

void IRGen::startIrGen() {
    auto *symbolsGlobal = new std::vector<Symbol *>();
    auto *symbolTableGlobal = new SymbolTable(SYMBOL_TABLE_GLOBAL, symbolsGlobal);
    symbolTables->push_back(symbolTableGlobal);

    int globalStepName = 0;
    auto *irGlobals = new std::vector<IRGlobal *>();

    for (Decl *decl:*(ast->getDecls())) {
        IRGlobal *irGlobal = nullptr;
        auto *irStmts = new std::vector<IRStmt *>();

        if (decl->getConstDecl() != nullptr) {
            for (ConstDef *constDef: *decl->getConstDecl()->getConstDefs()) {
                if (constDef->getConstExps()->empty()) {  // 常量变量
                    ConstVar *constVar = genConstVar(constDef, decl->getConstDecl()->getBType(), irStmts,
                                                     REGISTER_GLOBAL, DAG_Con_GLOBAL_i32);
                    auto *symbol = new Symbol(nullptr, nullptr, constVar, nullptr, nullptr);
                    symbolsGlobal->emplace_back(symbol);
                } else {  // 常量数组
                    ConstVarArray *constVarArray =
                            genConstVarArray(constDef, decl->getConstDecl()->getBType(), irStmts,
                                             REGISTER_GLOBAL, DAG_Con_GLOBAL_ARRAY_i32, globalStepName);
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
                    genVar(varDef, irStmts, REGISTER_GLOBAL.c_str(), DAG_GLOBAL_i32, globalStepName);
                } else {  // 数组
                    genVarArray(varDef, irStmts, REGISTER_GLOBAL.c_str(), DAG_GLOBAL_i32_ARRAY, globalStepName);
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
        }
        irGlobals->emplace_back(irGlobal);
    }  // end ast;
    irTree = new IRTree(irGlobals, nullptr);
}

ConstVar *IRGen::genConstVar(ConstDef *constDef, int constType, std::vector<IRStmt *> *irStmts,
                             const std::string &locType, int allocaType) {
    int intRet = GLOBAL_DEFAULT_VALUE;
    if (constDef->getConstInitVal() != nullptr) {
        intRet = calConstExp(constDef->getConstInitVal()->getConstExp());
    }

    auto *irAllocaStmt = new IRStmt(allocaType, (locType + constDef->getIdent()).c_str());
    auto *irStoreStmt = new IRStmt(DAG_STORE, (locType + constDef->getIdent()).c_str(),
                                   std::to_string(intRet).c_str());
    irStmts->emplace_back(irAllocaStmt);
    irStmts->emplace_back(irStoreStmt);

    auto *constVarRet = new ConstVar(constDef->getIdent().c_str(), constType, intRet);
    return constVarRet;
}

ConstVarArray *IRGen::genConstVarArray(ConstDef *constDef, int constType, std::vector<IRStmt *> *irStmts,
                                       const std::string &locType, int allocaType, int &stepName) {
    // sub 相关
    auto *subs = new std::vector<int>();
    for (ConstExp *constExp:*constDef->getConstExps()) {
        subs->push_back(calConstExp(constExp));
    }
    // ir sub 相关
    auto subs_ = new std::vector<std::string>();  // 后续赋值索引用
    int len = 1;
    for (int i:*subs) {
        subs_->push_back(std::to_string(GLOBAL_DEFAULT_VALUE));
        len *= i;
    }
    auto *subs_str = new std::vector<std::string>();
    for (int sub : *subs) {
        subs_str->emplace_back(std::to_string(sub));
    }
    // value 相关
    auto *values = new std::vector<int>();
    if (constDef->getConstInitVal() != nullptr) {
        values = calConstArrayInitVals(constDef->getConstInitVal(), subs, locType, allocaType);
    } else {
        for (int i = 0; i < len; i++) {
            values->push_back(GLOBAL_DEFAULT_VALUE);
        }
    }

    // ir相关
    auto *irAllocaStmt = new IRStmt(allocaType, (locType + constDef->getIdent()).c_str(), OPD_NULL.c_str(), subs_str);
    // 初始化赋值，考虑到 int a[4][2] = {} 无嵌套 与 values 已存数值之情况，constArray统一在此计算（ VarArray 则不然）
    // 又:出于地址索引特性，一律按照一维展开方式存数 如: int a[4][2] 的 a[1][1] = 1 在此表示为 a[0][3] = 1
    auto *irAssignStmts = new std::vector<IRStmt *>();
    for (int i = 0; i < len; i++) {
        subs_->pop_back();
        subs_->push_back(std::to_string(i));
        auto subAux = new std::vector<std::string>();
        subAux->assign(subs_->begin(), subs_->end());
        auto *getStmt = new IRStmt(DAG_GETPTR, (locType + std::to_string(stepName)).c_str(),
                                   (locType + constDef->getIdent()).c_str(), subAux);
        auto *storeStmt = new IRStmt(DAG_STORE, (locType + std::to_string(stepName)).c_str(),
                                     std::to_string(values->at(i)).c_str());
        irAssignStmts->emplace_back(getStmt);
        irAssignStmts->emplace_back(storeStmt);
        stepName++;
    }
    irStmts->emplace_back(irAllocaStmt);
    irStmts->insert(irStmts->end(), irAssignStmts->begin(), irAssignStmts->end());

    // symbol table 相关
    auto *constVarArray = new ConstVarArray(constDef->getIdent().c_str(), constType, values, subs);
    return constVarArray;
}

void IRGen::genVar(VarDef *varDef, std::vector<IRStmt *> *irStmts, const char *levelLoc, int allocaType,
                   int &stepName) {
    // alloca 相关
    auto *irAllocaStmt = new IRStmt(allocaType, (levelLoc + varDef->getIdent()).c_str());
    irStmts->emplace_back(irAllocaStmt);

    // value 相关
    if (varDef->getInitVal() != nullptr) {  // 有初始化
        const char *rExpRet = genAddExp(varDef->getInitVal()->getExp()->getAddExp(), irStmts, stepName);
        auto *irAssignStmt = new IRStmt(DAG_STORE, (levelLoc + varDef->getIdent()).c_str(), rExpRet);
        irStmts->emplace_back(irAssignStmt);
    } else if (levelLoc == REGISTER_GLOBAL) {  // 无初始化全局变量
        auto *irAssignStmt = new IRStmt(DAG_STORE, (levelLoc + varDef->getIdent()).c_str(),
                                        std::to_string(GLOBAL_DEFAULT_VALUE).c_str());
        irStmts->emplace_back(irAssignStmt);
    }
}

void IRGen::genVarArray(VarDef *varDef, std::vector<IRStmt *> *irStmts, const char *levelLoc,
                        int allocaType, int &stepName) {
    // sub 相关
    auto *subs = new std::vector<int>();
    for (ConstExp *constExp:*varDef->getConstExps()) {
        subs->push_back(calConstExp(constExp));
    }

    // ir sub 相关
    auto subs_ = new std::vector<std::string>();  // 后续赋值索引用
    int len = 1;
    for (int i:*subs) {
        subs_->push_back(std::to_string(GLOBAL_DEFAULT_VALUE));
        len *= i;
    }
    auto *subs_str = new std::vector<std::string>();
    for (int sub : *subs) {
        subs_str->emplace_back(std::to_string(sub));
    }

    // value 相关
    auto *values = new std::vector<Exp *>();
    if (varDef->getInitVal() != nullptr) {
        values = genVarArrayInitVals(varDef->getInitVal(), subs, irStmts, levelLoc, stepName);
    } else {
        for (int i = 0; i < len; i++) {
            // addExp null 表 DEFAULT_VALUE
            auto *exp = new Exp(nullptr);
            values->push_back(exp);
        }
    }

    // ir相关
    auto *IrAllocaStmt =
            new IRStmt(allocaType, (levelLoc + varDef->getIdent()).c_str(), OPD_NULL.c_str(), subs_str);
    // 初始化赋值，考虑到 int a[4][2] = {} 无嵌套 与 values 已存数值之情况，constArray统一在此计算（ VarArray 则不然）
    // 又:出于地址索引特性，一律按照一维展开方式存数 如: int a[4][2] 的 a[1][1] = 1 在此表示为 a[0][3] = 1
    auto *irAssignStmts = new std::vector<IRStmt *>();
    for (int i = 0; i < len; i++) {
        subs_->pop_back();
        subs_->push_back(std::to_string(i));
        auto subAux = new std::vector<std::string>();
        subAux->assign(subs_->begin(), subs_->end());
        auto *getPtrStmt = new IRStmt(DAG_GETPTR, (levelLoc + std::to_string(stepName)).c_str(),
                                      (levelLoc + varDef->getIdent()).c_str(), subAux);
        IRStmt *storeStmt = nullptr;
        if (values->at(i)->getAddExp() != nullptr) {
            const char *expRet = genAddExp(values->at(i)->getAddExp(), irStmts, stepName);
            storeStmt = new IRStmt(DAG_STORE, (levelLoc + std::to_string(stepName)).c_str(), expRet);
        } else {
            storeStmt = new IRStmt(DAG_STORE, (levelLoc + std::to_string(stepName)).c_str(),
                                   std::to_string(GLOBAL_DEFAULT_VALUE).c_str());
        }
        irAssignStmts->emplace_back(getPtrStmt);
        irAssignStmts->emplace_back(storeStmt);
        stepName++;
    }
    irStmts->emplace_back(IrAllocaStmt);
    irStmts->insert(irStmts->end(), irAssignStmts->begin(), irAssignStmts->end());
}

void IRGen::genFunc(FuncDef *funcDef, std::vector<IRLocalBlock *> *basicBlocks) {
    auto *irStmts = new std::vector<IRStmt *>();
    auto *preLocs = new std::set<std::string>();
    auto *blockEntry = new IRLocalBlock(BLOCK_ENTRY.c_str(), irStmts, preLocs);
    basicBlocks->emplace_back(blockEntry);
    // llvm 在实参和内部变量间隔一个
    int localStepName = 0;
    if(funcDef->getFuncFParams() != nullptr){
        localStepName = funcDef->getFuncFParams()->getFuncFParams()->size();
    }

    if (funcDef->getFuncFParams() != nullptr) {
        for (int i = 0; i < funcDef->getFuncFParams()->getFuncFParams()->size(); i++) {
            if (funcDef->getFuncFParams()->getFuncFParams()->at(i)->getExps() == nullptr) {
                auto *irAllocaStmt = new IRStmt(DAG_ALLOCA_i32,
                                                (REGISTER_LOCAL + std::to_string(localStepName)).c_str());
                irStmts->emplace_back(irAllocaStmt);
            } else {
                auto *irAllocaArrayStmt = new IRStmt(DAG_ALLOCA_i32_ARRAY,
                                                     (REGISTER_LOCAL + std::to_string(localStepName)).c_str());
                irStmts->emplace_back(irAllocaArrayStmt);
            }
            auto *irStoreStmt = new IRStmt(DAG_STORE, (REGISTER_LOCAL + std::to_string(i)).c_str(),
                                           (REGISTER_LOCAL + std::to_string(localStepName)).c_str());
            irStmts->emplace_back(irStoreStmt);
            i++;
            localStepName++;
        }
    }

    genBlock(funcDef->getBlock(), basicBlocks, blockEntry, irStmts, localStepName);
}

///===-----------------------------------------------------------------------===///
/// 语句、语句块
///===-----------------------------------------------------------------------===///

const char *IRGen::genStmt(Stmt *stmt, std::vector<IRLocalBlock *> *basicBlocks, IRLocalBlock *lastBlock,
                           std::vector<IRStmt *> *lastBlockStmts, int &stepName) {
    switch (stmt->getStmtType()) {
        case STMT_EXP: {
            genAddExp(stmt->getExp()->getAddExp(), lastBlockStmts, stepName);
            return lastBlock->getBlockName().c_str();
        }
        case STMT_LVAL_ASSIGN: {
            const char *rValueRet = genAddExp(stmt->getExp()->getAddExp(), lastBlockStmts, stepName);
            const char *lValueRet = genLVal(stmt->getLVal(), lastBlockStmts, stepName);
            auto *irAssignStmt = new IRStmt(DAG_STORE, lValueRet, rValueRet);
            return lastBlock->getBlockName().c_str();
        }
        case STMT_BLOCK:
            return genBlock(stmt->getBlock(), basicBlocks, lastBlock, lastBlockStmts, stepName);
        case STMT_IF: {
            return genStmtAuxIf(stmt, basicBlocks, lastBlock, lastBlockStmts, stepName);
        }
        case STMT_WHILE: {
            return genStmtAuxWhile(stmt, basicBlocks, lastBlock, lastBlockStmts, stepName);
        }
        case STMT_RETURN: {
            if (stmt->getExp() == nullptr) {
                auto *irStmtRet = new IRStmt(STMT_RETURN, OPD_NULL.c_str(), OPD_NULL.c_str());
                lastBlockStmts->emplace_back(irStmtRet);
            } else {
                std::string registerRet = genAddExp(stmt->getExp()->getAddExp(), lastBlockStmts, stepName);
                auto *irStmtRet = new IRStmt(STMT_RETURN, registerRet.c_str(), OPD_NULL.c_str());
                lastBlockStmts->emplace_back(irStmtRet);
            }
            return lastBlock->getBlockName().c_str();
        }
        case STMT_EXP_BLANK: {
            return lastBlock->getBlockName().c_str();
        }
        case STMT_CONTINUE: {
            auto *irBrWhileBlock = new IRStmt(DAG_BR, whilePos->at(whilePos->size() - 1).c_str());
            lastBlockStmts->emplace_back(irBrWhileBlock);
            return lastBlock->getBlockName().c_str();
        }
        case STMT_BREAK: {
            auto *irBrWhileEndBlock = new IRStmt(DAG_BR, whileEndPos->at(whileEndPos->size() - 1).c_str());
            lastBlockStmts->emplace_back(irBrWhileEndBlock);
            return lastBlock->getBlockName().c_str();
        }
        default:
            return lastBlock->getBlockName().c_str();
    }
}

const char *IRGen::genStmtAuxIf(Stmt *stmt, std::vector<IRLocalBlock *> *basicBlocks, IRLocalBlock *lastBlock,
                                std::vector<IRStmt *> *lastBlockStmts, int &stepName) {
    const char *ifCondRet = genCondExp(stmt->getCond(), lastBlockStmts, stepName);

    auto *irBodyStmts = new std::vector<IRStmt *>();
    auto *irBodyPreLocs = new std::set<std::string>();
    irBodyPreLocs->insert(lastBlock->getBlockName());
    int bodyStep = stepName++;
    auto *bodyBlock = new IRLocalBlock((REGISTER_LOCAL + std::to_string(bodyStep)).c_str(), irBodyStmts,
                                       irBodyPreLocs);
    basicBlocks->emplace_back(bodyBlock);

    genStmt(stmt->getStmtBrBody(), basicBlocks, lastBlock, lastBlockStmts, stepName);

    if (stmt->getElseBody() != nullptr) {
        auto *irElseStmts = new std::vector<IRStmt *>();
        auto *irElsePreLocs = new std::set<std::string>();
        irElsePreLocs->insert(lastBlock->getBlockName());
        int elseStep = stepName++;
        auto *elseBlock = new IRLocalBlock((REGISTER_LOCAL + std::to_string(elseStep)).c_str(), irElseStmts,
                                           irElsePreLocs);
        basicBlocks->emplace_back(elseBlock);

        genStmt(stmt->getElseBody(), basicBlocks, lastBlock, lastBlockStmts, stepName);

        // lastBlockStmts (原if之前的语句) 添加最后跳转 br
        auto *elseStepArray = new std::vector<std::string>();
        elseStepArray->emplace_back(REGISTER_LOCAL + std::to_string(elseStep));
        auto *irBrCondStmt = new IRStmt(DAG_BR, ifCondRet, (REGISTER_LOCAL + std::to_string(bodyStep)).c_str(),
                                        elseStepArray);
        lastBlockStmts->emplace_back(irBrCondStmt);

        // if {...} else {...} 各自最后一句添加跳转
        auto *afterEndStepStr = new std::string(REGISTER_LOCAL + std::to_string(++stepName));
        auto *irIfBodyStmt = new IRStmt(DAG_BR, afterEndStepStr->c_str());
        irBodyStmts->emplace_back(irIfBodyStmt);
        auto *irIfElseBodyStmt = new IRStmt(DAG_BR, afterEndStepStr->c_str());
        irElseStmts->emplace_back(irIfElseBodyStmt);
        return afterEndStepStr->c_str();
    } else {
        // lastBlockStmts (原if之前的语句) 添加最后跳转 br
        stepName++;
        auto *afterIfStepArray = new std::vector<std::string>();
        afterIfStepArray->emplace_back(REGISTER_LOCAL + std::to_string(stepName));
        auto *irBrCondStmt = new IRStmt(DAG_BR, ifCondRet, (REGISTER_LOCAL + std::to_string(bodyStep)).c_str(),
                                        afterIfStepArray);
        lastBlockStmts->emplace_back(irBrCondStmt);

        // if {...} 最后一句添加跳转
        auto *afterEndStepStr = new std::string(REGISTER_LOCAL + std::to_string(++stepName));
        auto *irIfBodyStmt = new IRStmt(DAG_BR, afterEndStepStr->c_str());
        irBodyStmts->emplace_back(irIfBodyStmt);
        return afterEndStepStr->c_str();
    }
}

const char *IRGen::genStmtAuxWhile(Stmt *stmt, std::vector<IRLocalBlock *> *basicBlocks, IRLocalBlock *lastBlock,
                                   std::vector<IRStmt *> *lastBlockStmts, int &stepName) {
    auto *irCondStmts = new std::vector<IRStmt *>();
    auto *irCondPreLocs = new std::set<std::string>();
    irCondPreLocs->insert(lastBlock->getBlockName());
    int condStep = stepName++;
    auto *condBlock = new IRLocalBlock((REGISTER_LOCAL + std::to_string(condStep)).c_str(), irCondStmts, irCondPreLocs);
    basicBlocks->emplace_back(condBlock);
    whilePos->push_back(condBlock->getBlockName());

    const char *ifCondRet = genCondExp(stmt->getCond(), lastBlockStmts, stepName);

    auto *irBodyStmts = new std::vector<IRStmt *>();
    auto *irBodyPreLocs = new std::set<std::string>();
    irBodyPreLocs->insert(condBlock->getBlockName());
    int bodyStep = stepName++;
    auto *bodyBlock = new IRLocalBlock((REGISTER_LOCAL + std::to_string(bodyStep)).c_str(), irBodyStmts, irBodyPreLocs);
    basicBlocks->emplace_back(bodyBlock);
    auto *afterEndStepStr = new std::string ("tmp");
    whileEndPos->push_back(*afterEndStepStr);

    genStmt(stmt->getStmtBrBody(), basicBlocks, bodyBlock, irBodyStmts, stepName);

    afterEndStepStr = new std::string(REGISTER_LOCAL + std::to_string(++stepName));
    auto *bodyStepArray = new std::vector<std::string>();
    bodyStepArray->emplace_back(afterEndStepStr->c_str());
    auto *irWhileCondStmt = new IRStmt(DAG_BR, ifCondRet, bodyBlock->getBlockName().c_str(), bodyStepArray);
    irCondStmts->emplace_back(irWhileCondStmt);

    whilePos->pop_back();
    whileEndPos->pop_back();
    return afterEndStepStr->c_str();
}

const char *IRGen::genBlock(Block *block, std::vector<IRLocalBlock *> *basicBlocks, IRLocalBlock *lastBlock,
                            std::vector<IRStmt *> *lastBlockStmts, int &stepName) {
    auto *symbolsLocal = new std::vector<Symbol *>();
    auto *symbolTableLocal = new SymbolTable(SYMBOL_TABLE_GLOBAL, symbolsLocal);
    symbolTables->emplace_back(symbolTableLocal);

    IRLocalBlock *tmpBlock = lastBlock;
    std::vector<IRStmt *> *tmpIrStmts = lastBlockStmts;

    for (BlockItem *blockItem:*block->getBlockItems()) {
        if (blockItem->getConstDecl() != nullptr) {
            for (ConstDef *constDef: *blockItem->getConstDecl()->getConstDefs()) {
                if (constDef->getConstExps()->empty()) {  // 常量变量
                    ConstVar *constVar = genConstVar(constDef, blockItem->getVarDecl()->getBType(), tmpIrStmts,
                                                     REGISTER_LOCAL, DAG_Con_ALLOCA_i32);
                    auto *symbol = new Symbol(nullptr, nullptr, constVar, nullptr, nullptr);
                    symbolsLocal->emplace_back(symbol);
                } else {  // 常量数组
                    ConstVarArray *constVarArray =
                            genConstVarArray(constDef, blockItem->getConstDecl()->getBType(), tmpIrStmts,
                                             REGISTER_LOCAL, DAG_Con_ALLOCA_ARRAY_i32, stepName);
                    auto *symbol = new Symbol(nullptr, nullptr, nullptr, constVarArray, nullptr);
                    symbolsLocal->emplace_back(symbol);
                }
            } //  end constDefs
        } else if (blockItem->getVarDecl() != nullptr) {
            for (VarDef *varDef: *blockItem->getVarDecl()->getVarDefs()) {
                if (varDef->getConstExps()->empty()) {
                    genVar(varDef, tmpIrStmts, REGISTER_LOCAL.c_str(), DAG_ALLOCA_i32, stepName);
                } else {  // 数组
                    genVarArray(varDef, tmpIrStmts, REGISTER_LOCAL.c_str(), DAG_ALLOCA_i32_ARRAY, stepName);
                }
            }  // end varDefs
        } else if (blockItem->getStmt() != nullptr) {
            const char *newBlockName = genStmt(blockItem->getStmt(), basicBlocks, tmpBlock, tmpIrStmts, stepName);
            if (strcmp(newBlockName, tmpBlock->getBlockName().c_str()) != 0) {  // 返回得知当前 basicBlock 有变化
                auto *newStmts = new std::vector<IRStmt *>();
                auto *newPreLocs = new std::set<std::string>();
                auto *newBlock = new IRLocalBlock(newBlockName, newStmts, newPreLocs);
                tmpBlock = newBlock;
                tmpIrStmts = newStmts;
            }
        }
    }
    symbolTables->pop_back();
    return lastBlock->getBlockName().c_str();
}

///===-----------------------------------------------------------------------===///
/// 表达式 计算 不生成代码
///===-----------------------------------------------------------------------===///

std::vector<int> *IRGen::calConstArrayInitVals(ConstInitVal *constInitVal, std::vector<int> *subs,
                                               const std::string &locType, int allocaType) {
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
                    std::vector<int> *subs_values_inner = calConstArrayInitVals(initValInner, subs, locType,
                                                                                allocaType);
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
        }
    } else {
        intRet = calPrimaryExp(unaryExp->getPrimaryExp());
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
                    int pos = 0;  // 计算索引一维位置
                    for (int j = 0; j < subs->size(); j++) {
                        int subs_subs_len = 1;
                        for (int k = j + 1; k < subs->size(); k++) {
                            subs_subs_len *= symbol->getConstVarArrayInner()->getSubs()->at(k);
                        }
                        pos += subs->at(j) * subs_subs_len;
                    }
                    return symbol->getConstVarArrayInner()->getValue()->at(pos);
                }  // end find constArray ident
            }  // end one symbol-table iter
        }  // end all symbol-table except EXTERN
    }  // end find lVal
    Error::errorSim("something wrong when semantic, occur in irGen");
    exit(-1);
}

///===-----------------------------------------------------------------------===///
/// 表达式 不计算 生成代码
///===-----------------------------------------------------------------------===///

std::vector<Exp *> *IRGen::genVarArrayInitVals(InitVal *initVal, std::vector<int> *subs, std::vector<IRStmt *> *irStmts,
                                               const char *levelLoc, int &stepName) {
    auto *valuesRet = new std::vector<Exp *>();
    int len = 1;  // 维度展开一维的长度
    for (int i : *subs) {
        len *= i;
    }
    // 所有值用零填充
    if (initVal->getInitVals()->empty()) {  // {}
        for (int i = 0; i < len; i++) {
            // addExp null 表 DEFAULT_VALUE
            auto *exp = new Exp(nullptr);
            valuesRet->emplace_back(exp);
        }

    } else {
        for (InitVal *initValInner : *initVal->getInitVals()) {
            if (initValInner->getExp() != nullptr) {  // 实值
                valuesRet->emplace_back(initValInner->getExp());
            } else {  // 数组嵌套
                int sub_first_len = 1;  // 子维展开一维的长度
                for (int i = 1; i < subs->size(); i++) {
                    sub_first_len *= subs->at(i);
                }
                if (valuesRet->size() % sub_first_len == 0) {  // 保证嵌套子维前，已有长度为子维整数倍
                    int tmp = subs->at(0);  // 当前第一维度值
                    subs->erase(subs->begin());
                    std::vector<Exp *> *subs_values_inner =
                            genVarArrayInitVals(initValInner, subs, irStmts, levelLoc, stepName);
                    subs->insert(subs->begin(), tmp);
                    valuesRet->insert(valuesRet->end(), subs_values_inner->begin(), subs_values_inner->end());  // 拼接
                }
            }
        }
    }
    if (valuesRet->size() < len) {  // 长度不足补零
        for (int i = 0; i < len - valuesRet->size(); i++) {
            // addExp null 表 DEFAULT_VALUE
            auto *exp = new Exp(nullptr);
            valuesRet->emplace_back(exp);
        }
    }
    return valuesRet;
}

const char *IRGen::genCondExp(Cond *cond, std::vector<IRStmt *> *irStmts, int &stepName) {
    return genLOrExp(cond->getLOrExp(), irStmts, stepName);
}

const char *IRGen::genLOrExp(LOrExp *lOrExp, std::vector<IRStmt *> *irStmts, int &stepName) {
    if (lOrExp->getLOrExp() == nullptr) {
        return genLAndExp(lOrExp->getLAndExp(), irStmts, stepName);
    } else {
        const char *lAndRet = genLAndExp(lOrExp->getLAndExp(), irStmts, stepName);
        auto *lOrRetArray = new std::vector<std::string>();
        lOrRetArray->emplace_back(genLOrExp(lOrExp->getLOrExp(), irStmts, stepName));

        auto registerLOrRet = new std::string(REGISTER_LOCAL + std::to_string(++stepName));
        auto *irLOrStmt = new IRStmt(DAG_OR, registerLOrRet->c_str(), lAndRet, lOrRetArray);
        irStmts->emplace_back(irLOrStmt);
        return registerLOrRet->c_str();
    }
}

const char *IRGen::genLAndExp(LAndExp *lAndExp, std::vector<IRStmt *> *irStmts, int &stepName) {
    if (lAndExp->getLAndExp() == nullptr) {
        return genEqExp(lAndExp->getEqExp(), irStmts, stepName);
    } else {
        const char *eqRet = genEqExp(lAndExp->getEqExp(), irStmts, stepName);
        auto *lAndRetArray = new std::vector<std::string>();
        lAndRetArray->emplace_back(genLAndExp(lAndExp->getLAndExp(), irStmts, stepName));

        auto registerLAndRet = new std::string(REGISTER_LOCAL + std::to_string(++stepName));
        auto *irLAndStmt = new IRStmt(DAG_AND, registerLAndRet->c_str(), eqRet, lAndRetArray);
        irStmts->emplace_back(irLAndStmt);
        return registerLAndRet->c_str();
    }
}

const char *IRGen::genEqExp(EqExp *eqExp, std::vector<IRStmt *> *irStmts, int &stepName) {
    if (eqExp->getOpType() == OP_NULL) {
        return genRelExp(eqExp->getRelExp(), irStmts, stepName);
    } else {
        const char *relRet = genRelExp(eqExp->getRelExp(), irStmts, stepName);
        auto *eqRetArray = new std::vector<std::string>();
        eqRetArray->emplace_back(genEqExp(eqExp->getEqExp(), irStmts, stepName));

        auto registerEqRet = new std::string(REGISTER_LOCAL + std::to_string(++stepName));
        auto *irEqStmt = new IRStmt(eqExp->getOpType(), registerEqRet->c_str(), relRet, eqRetArray);
        irStmts->emplace_back(irEqStmt);
        return registerEqRet->c_str();
    }
}

const char *IRGen::genRelExp(RelExp *relExp, std::vector<IRStmt *> *irStmts, int &stepName) {
    if (relExp->getOpType() == OP_NULL) {
        return genAddExp(relExp->getAddExp(), irStmts, stepName);
    } else {
        const char *addRet = genAddExp(relExp->getAddExp(), irStmts, stepName);
        auto *relRetArray = new std::vector<std::string>();
        relRetArray->emplace_back(genRelExp(relExp->getRelExp(), irStmts, stepName));

        auto registerRelRet = new std::string(REGISTER_LOCAL + std::to_string(++stepName));
        auto *irRelStmt = new IRStmt(relExp->getOpType(), registerRelRet->c_str(), addRet, relRetArray);
        irStmts->emplace_back(irRelStmt);
        return registerRelRet->c_str();
    }
}

const char *IRGen::genAddExp(AddExp *addExp, std::vector<IRStmt *> *irStmts, int &stepName) {
    if (addExp->getOpType() == OP_NULL) {
        return genMulExp(addExp->getMulExp(), irStmts, stepName);
    } else {
        const char *mulRet = genMulExp(addExp->getMulExp(), irStmts, stepName);
        auto *addRetArray = new std::vector<std::string>();
        addRetArray->emplace_back(genAddExp(addExp->getAddExp(), irStmts, stepName));

        auto registerAddRet = new std::string(REGISTER_LOCAL + std::to_string(++stepName));
        auto *irAddStmt = new IRStmt(addExp->getOpType(), registerAddRet->c_str(), mulRet, addRetArray);
        irStmts->emplace_back(irAddStmt);
        return registerAddRet->c_str();
    }
}

const char *IRGen::genMulExp(MulExp *mulExp, std::vector<IRStmt *> *irStmts, int &stepName) {
    if (mulExp->getOpType() == OP_NULL) {
        return genUnaryExp(mulExp->getUnaryExp(), irStmts, stepName);
    } else {
        const char *unaryRet = genUnaryExp(mulExp->getUnaryExp(), irStmts, stepName);
        auto *mulRetArray = new std::vector<std::string>();
        mulRetArray->emplace_back(genMulExp(mulExp->getMulExp(), irStmts, stepName));

        auto registerMulRet = new std::string(REGISTER_LOCAL + std::to_string(++stepName));
        auto *irMulStmt = new IRStmt(mulExp->getOpType(), registerMulRet->c_str(), unaryRet, mulRetArray);
        irStmts->emplace_back(irMulStmt);
        return registerMulRet->c_str();
    }
}

const char *IRGen::genUnaryExp(UnaryExp *unaryExp, std::vector<IRStmt *> *irStmts, int &stepName) {
    if (unaryExp->getPrimaryExp() != nullptr) {
        return genPrimaryExp(unaryExp->getPrimaryExp(), irStmts, stepName);
    } else if (unaryExp->getOpType() != OP_NULL) {
        const char *unaryRet = genUnaryExp(unaryExp->getUnaryExp(), irStmts, stepName);

        auto registerUnaryRet = new std::string(REGISTER_LOCAL + std::to_string(++stepName));
        auto *irUnaryStmt = new IRStmt(unaryExp->getOpType(), registerUnaryRet->c_str(), unaryRet, nullptr);
        irStmts->emplace_back(irUnaryStmt);
        return registerUnaryRet->c_str();
    } else {
        auto *paramArray = new std::vector<std::string>();

        if(unaryExp->getFuncRParams() != nullptr){
            for (Exp *exp: *unaryExp->getFuncRParams()->getExps()) {
                const char *param = genAddExp(exp->getAddExp(), irStmts, stepName);
                paramArray->emplace_back(param);
            }
        }

        auto registerFuncRet = new std::string(REGISTER_LOCAL + std::to_string(++stepName));
        auto *irCallStmt = new IRStmt(DAG_CALL, registerFuncRet->c_str(), unaryExp->getIdent().c_str(), paramArray);
        irStmts->emplace_back(irCallStmt);
        return registerFuncRet->c_str();
    }
}

const char *IRGen::genPrimaryExp(PrimaryExp *primaryExp, std::vector<IRStmt *> *irStmts, int &stepName) {
    if (primaryExp->getExp() != nullptr) {
        return genAddExp(primaryExp->getExp()->getAddExp(), irStmts, stepName);
    } else if (primaryExp->getLVal() != nullptr) {
        return genLVal(primaryExp->getLVal(), irStmts, stepName);
    } else {
        auto *numberStrRet = new std::string(std::to_string(primaryExp->getNumber()));
        return numberStrRet->c_str();
    }
}

const char *IRGen::genLVal(LVal *lVal, std::vector<IRStmt *> *irStmts, int &stepName) {
    if (lVal->getExps()->empty()) {  // 整型
        auto registerLoadRet = new std::string(REGISTER_LOCAL + std::to_string(++stepName));
        auto *irLoadStmt = new IRStmt(DAG_LOAD, registerLoadRet->c_str(), lVal->getIdent().c_str(), nullptr);
        irStmts->emplace_back(irLoadStmt);
        return registerLoadRet->c_str();
    } else {  // 整型数组
        auto *subArray = new std::vector<std::string>();
        for (Exp *exp:*lVal->getExps()) {
            const char *sub = genAddExp(exp->getAddExp(), irStmts, stepName);
            subArray->emplace_back(sub);
        }

        auto registerLoadRet = new std::string(REGISTER_LOCAL + std::to_string(++stepName));
        auto *irGetPtrStmt = new IRStmt(DAG_GETPTR, registerLoadRet->c_str(), lVal->getIdent().c_str(), subArray);
        irStmts->emplace_back(irGetPtrStmt);
        return registerLoadRet->c_str();
    }
}
