#include "Arm7Gen.h"
#include "../util/Error.h"

Arm7Gen::Arm7Gen() {
    ast = nullptr;
    symbolTables = nullptr;
    armTree = nullptr;

    whilePos = new std::vector<std::string>();
    whileEndPos = new std::vector<std::string>();
    levelNow = 0;
    armRegManager = new ArmRegManager();
    /// TODO 我不知自什么始
    blockName = 0;
}

void Arm7Gen::startGen(AST *ast_, std::vector<SymbolTable *> *symbolTables_) {
    ast = ast_;
    symbolTables = symbolTables_;

    auto *armGlobals = new std::vector<ArmGlobal *>();

    /// 一次性声明所有全局变量
    for (Symbol *symbol:*symbolTables->at(1)->getSymbols()) {
        if (symbol->getArm7Var() != nullptr) {
            auto *arm7GlobalVar =
                    new Arm7GlobalVar(symbol->getArm7Var()->getIdent().c_str(), symbol->getArm7Var()->getIfConst(),
                                      symbol->getArm7Var()->getSubs(), symbol->getArm7Var()->getValue());
            auto *armGlobal = new ArmGlobal(arm7GlobalVar, nullptr);
            armGlobals->emplace_back(armGlobal);
        }
    }

    for (Decl *decl:*(ast->getDecls())) {
        if (decl->getFuncDef() != nullptr) {
            auto *armBlocks = new std::vector<ArmBlock *>();
            genArm7Func(decl->getFuncDef(), armBlocks);

            auto *arm7GlobalFunc =
                    new Arm7GlobalFunc(decl->getFuncDef()->getIdent().c_str(), armBlocks);
            auto *armGlobal = new ArmGlobal(nullptr, arm7GlobalFunc);
            armGlobals->emplace_back(armGlobal);
        }
    }
    armTree = new Arm7Tree(armGlobals);
}

///===-----------------------------------------------------------------------===///
/// 基本声明定义
///===-----------------------------------------------------------------------===///


void Arm7Gen::genArm7Func(FuncDef *funcDef, std::vector<ArmBlock *> *armBlocks) {
    funcNameNow = funcDef->getIdent();
    levelNow++;

    auto *armStmts = new std::vector<ArmStmt *>();
    auto *blockEntry = new ArmBlock(BLOCK_ENTRY.c_str(), armStmts);
    armBlocks->emplace_back(blockEntry);


    for (Symbol *symbol:*symbolTables->at(1)->getSymbols()) {
        if (symbol->getArm7Func() != nullptr && symbol->getArm7Func()->getIdent() == funcDef->getIdent()) {
            /// push	{r4, fp, lr}
            /// add	fp, sp, #PUSH_NUM_DEFUALT * 4
            /// sub	sp, sp, #DIGIT_CAPACITY
            auto *armStmtPush = new ArmStmt(ARM_STMT_PUSH, "{r4, fp, lr}");
            auto *armStmtAdd = new ArmStmt(ARM_STMT_ADD, "sp", ("#" + std::to_string(PUSH_NUM_DEFAULT * 4)).c_str());
            /// 此句负数 capacity 转正
            auto *armStmtSub = new ArmStmt(ARM_STMT_SUB, "sp", "sp",
                                           ("#" + std::to_string(0 - symbol->getArm7Func()->getCapacity())).c_str());
            armStmts->emplace_back(armStmtPush);
            armStmts->emplace_back(armStmtAdd);
            armStmts->emplace_back(armStmtSub);

            /// 寄存器传递的参数
            auto *params = symbol->getArm7Func()->getParams();
            for (int i = 0; i < symbol->getArm7Func()->getParams()->size(); i++) {
                if (i < 4) {
                    auto *armStmt =
                            new ArmStmt(ARM_STMT_STR, ("r" + std::to_string(i)).c_str(),
                                        ("[fp, #" + std::to_string(params->at(i)->getMemoryLoc()) + "]").c_str());
                    armStmts->emplace_back(armStmt);
                    armRegManager->getArmRegs()->at(i)->setArm7Var(params->at(i));
                } else {
                    break;   /// 加快跳出不循环完
                }
            }
        }
    }

    genBlock(funcDef->getBlock(), armBlocks, blockEntry, armStmts);

    /// 考虑到如下固定格式结尾无需计算，保留在 output 时输出到文件
    /// sub	sp, fp, #4
    /// @ sp needed
    /// pop	{fp, pc}
    /// .size	whileFunc, .-whileFunc
    /// .align	2

    funcNameNow = nullptr;
    levelNow--;
}

///===-----------------------------------------------------------------------===///
/// 语句块中声明语句
///===-----------------------------------------------------------------------===///

void Arm7Gen::genVar(VarDef *varDef, std::vector<ArmStmt *> *armStmts) {
    /// 仅初始化时 str
    if (varDef->getInitVal() != nullptr) {
        ///	str rRX, [fp, #LOC]
        auto *rReg = genAddExp(varDef->getInitVal()->getExp()->getAddExp(), armStmts);
        auto *armSTRStmt = new ArmStmt(ARM_STMT_STR, rReg->getRegName().c_str(),
                                       ("[fp, #" + std::to_string(varDef->getBaseMemoryPos()) + "]").c_str());
        armStmts->emplace_back(armSTRStmt);
    }
}

void Arm7Gen::genConstVar(ConstDef *constDef, std::vector<ArmStmt *> *armStmts) {
    int value = GLOBAL_DEFAULT_VALUE;
    if (constDef->getConstInitVal() != nullptr) {
        value = calConstExp(constDef->getConstInitVal()->getConstExp());
    }

    ///	mov	rX, #CONST_VALUE
    ///	str	rX, [fp, #LOC]
    ArmReg *armReg = armRegManager->getFreeArmReg(armStmts);
    auto *armStmtMove = new ArmStmt(ARM_STMT_MOV, armReg->getRegName().c_str(),
                                    ("#" + std::to_string(value)).c_str());
    auto *armStmtStr = new ArmStmt(ARM_STMT_STR, armReg->getRegName().c_str(),
                                   ("[fp, #" + std::to_string(constDef->getBaseMemoryPos()) + "]").c_str());
    armStmts->emplace_back(armStmtMove);
    armStmts->emplace_back(armStmtStr);
}

void Arm7Gen::genVarArray(VarDef *varDef, std::vector<ArmStmt *> *armStmts) {
    // sub 相关
    auto *subs = new std::vector<int>();
    int len = 1;
    for (ConstExp *constExp:*varDef->getConstExps()) {
        int sub = calConstExp(constExp);
        len *= sub;
        subs->push_back(sub);
    }

    // value arm 相关
    if (varDef->getInitVal() != nullptr) {
        for (int i = 0; i < len;) {
            int subPosNow = varDef->getBaseMemoryPos() + i;
            ///	str rRX, [fp, #SUB_LOC]
            auto *rReg = genAddExp(varDef->getInitVal()->getExp()->getAddExp(), armStmts);
            auto *armSTRStmt = new ArmStmt(ARM_STMT_STR, rReg->getRegName().c_str(),
                                           ("[fp, #" + std::to_string(subPosNow) + "]").c_str());
            armStmts->emplace_back(armSTRStmt);
            i += 4;
        }
    } else {
        ArmReg *armReg = armRegManager->getFreeArmReg(armStmts);
        ///	mov	rX, #GLOBAL_DEFAULT_VALUE <--> 0
        auto *armStmtMove = new ArmStmt(ARM_STMT_MOV, armReg->getRegName().c_str(),
                                        ("#" + std::to_string(GLOBAL_DEFAULT_VALUE)).c_str());
        armStmts->emplace_back(armStmtMove);
        for (int i = 0; i < 4 * len;) {
            int subPosNow = varDef->getBaseMemoryPos() + i;
            ///	str	rX, [fp, #SUB_LOC]
            auto *armStmtStr = new ArmStmt(ARM_STMT_STR, armReg->getRegName().c_str(),
                                           ("[fp, #" + std::to_string(subPosNow) + "]").c_str());
            armStmts->emplace_back(armStmtStr);
            i += 4;
        }
    }


}

void Arm7Gen::genConstVarArray(ConstDef *constDef, std::vector<ArmStmt *> *armStmts) {
    // sub 相关
    auto *subs = new std::vector<int>();
    int len = 1;
    for (ConstExp *constExp:*constDef->getConstExps()) {
        int sub = calConstExp(constExp);
        len *= sub;
        subs->push_back(sub);
    }

    // value arm 相关
    if (constDef->getConstInitVal() != nullptr) {
        auto *values = calConstArrayInitVals(constDef->getConstInitVal(), subs);
        ArmReg *armReg = armRegManager->getFreeArmReg(armStmts);
        for (int i = 0; i < 4 * len;) {
            int subPosNow = constDef->getBaseMemoryPos() + i;
            ///	mov	rX, #SUB_VALUE
            ///	str	rX, [fp, #SUB_LOC]
            auto *armStmtMove = new ArmStmt(ARM_STMT_MOV, armReg->getRegName().c_str(),
                                            ("#" + std::to_string(values->at(i))).c_str());
            auto *armStmtStr = new ArmStmt(ARM_STMT_STR, armReg->getRegName().c_str(),
                                           ("[fp, #" + std::to_string(subPosNow) + "]").c_str());
            armStmts->emplace_back(armStmtMove);
            armStmts->emplace_back(armStmtStr);
            i += 4;
        }
    } else {
        ArmReg *armReg = armRegManager->getFreeArmReg(armStmts);
        ///	mov	rX, #GLOBAL_DEFAULT_VALUE <--> 0
        auto *armStmtMove = new ArmStmt(ARM_STMT_MOV, armReg->getRegName().c_str(),
                                        ("#" + std::to_string(GLOBAL_DEFAULT_VALUE)).c_str());
        armStmts->emplace_back(armStmtMove);
        for (int i = 0; i < 4 * len;) {
            int subPosNow = constDef->getBaseMemoryPos() + i;
            ///	str	rX, [fp, #SUB_LOC]
            auto *armStmtStr = new ArmStmt(ARM_STMT_STR, armReg->getRegName().c_str(),
                                           ("[fp, #" + std::to_string(subPosNow) + "]").c_str());
            armStmts->emplace_back(armStmtStr);
            i += 4;
        }
    }
}

void Arm7Gen::genArm7Var(BlockItem *blockItem, std::vector<ArmStmt *> *armStmts) {
    if (blockItem->getConstDecl() != nullptr) {
        for (ConstDef *constDef:*blockItem->getConstDecl()->getConstDefs()) {
            if (constDef->getConstExps()->empty()) {
                genConstVar(constDef, armStmts);
            } else {
                genConstVarArray(constDef, armStmts);
            }
        }
    } else if (blockItem->getVarDecl() != nullptr) {
        for (VarDef *varDef: *blockItem->getVarDecl()->getVarDefs()) {
            if (varDef->getConstExps()->empty()) {
                genVar(varDef, armStmts);
            } else {
                genVarArray(varDef, armStmts);
            }
        }
    }
}

///===-----------------------------------------------------------------------===///
/// 语句、语句块
///===-----------------------------------------------------------------------===///

const char *Arm7Gen::genBlock(Block *block, std::vector<ArmBlock *> *basicBlocks, ArmBlock *lastBlock,
                              std::vector<ArmStmt *> *lastBlockStmts) {
    levelNow++;

    ArmBlock *tmpBlock = lastBlock;
    std::vector<ArmStmt *> *tmpArmStmts = lastBlockStmts;

    for (BlockItem *blockItem: *block->getBlockItems()) {
        if (blockItem->getStmt() != nullptr) {
            const char *newBlockName = genStmt(blockItem->getStmt(), basicBlocks, tmpBlock, tmpArmStmts);
            if (strcmp(newBlockName, tmpBlock->getBlockName().c_str()) != 0) {  // 返回得知当前 basicBlock 有变化
                auto *newStmts = new std::vector<ArmStmt *>();
                auto *newBlock = new ArmBlock(newBlockName, newStmts);
                tmpBlock = newBlock;
                tmpArmStmts = newStmts;
            }
        } else if (blockItem->getConstDecl() != nullptr || blockItem->getVarDecl() != nullptr) {
            genArm7Var(blockItem, tmpArmStmts);
        }
    }


    levelNow--;
    return lastBlock->getBlockName().c_str();
}

const char *Arm7Gen::genStmt(Stmt *stmt, std::vector<ArmBlock *> *basicBlocks, ArmBlock *lastBlock,
                             std::vector<ArmStmt *> *lastBlockStmts) {
    switch (stmt->getStmtType()) {
        case STMT_EXP: {
            genAddExp(stmt->getExp()->getAddExp(), lastBlockStmts);
            return lastBlock->getBlockName().c_str();
        }
        case STMT_LVAL_ASSIGN: {
            auto *rRet = genAddExp(stmt->getExp()->getAddExp(), lastBlockStmts);
            if (stmt->getLVal()->getExps()->empty()) {
                /// str rRet [lVal]
                auto *armStmtStr = new ArmStmt(ARM_STMT_STR, rRet->getRegName().c_str(),
                                               stmt->getLVal()->getBaseMemoryPos().c_str());
                lastBlockStmts->emplace_back(armStmtStr);
            } else {
                /// str rRet [lVal]
                auto *armRegPos = genLVal(stmt->getLVal(), lastBlockStmts, 1);
                auto *armStmtStr = new ArmStmt(ARM_STMT_STR, rRet->getRegName().c_str(),
                                               ("[" + armRegPos->getRegName() + "]").c_str());
                lastBlockStmts->emplace_back(armStmtStr);
            }
            return lastBlock->getBlockName().c_str();
        }
        case STMT_BLOCK:
            return genBlock(stmt->getBlock(), basicBlocks, lastBlock, lastBlockStmts);
        case STMT_IF: {
            return genStmtAuxIf(stmt, basicBlocks, lastBlock, lastBlockStmts);
        }
        case STMT_WHILE: {
            return genStmtAuxWhile(stmt, basicBlocks, lastBlock, lastBlockStmts);
        }
        case STMT_RETURN: {
            if (stmt->getExp() != nullptr) {
                auto *armRegRet = genAddExp(stmt->getExp()->getAddExp(), lastBlockStmts);
                if (armRegRet->getRegName() != "r0") {
                    /// mov r0 rX
                    auto *armMov0Stmt = new ArmStmt(ARM_STMT_MOV, "r0", armRegRet->getRegName().c_str());
                    lastBlockStmts->emplace_back(armMov0Stmt);
                }
                /// TODO 留给输出文件
                ///	  nop
                ///	  sub	sp, fp, #PUSH_NUM_DEFAULT *4
                ///	  pop	{r4, fp, pc}
            }
            return lastBlock->getBlockName().c_str();
        }
        case STMT_EXP_BLANK: {
            return lastBlock->getBlockName().c_str();
        }
        case STMT_CONTINUE: {
            /// b whileStart
            auto *armBStmt = new ArmStmt(ARM_STMT_B, whilePos->at(whilePos->size() - 1).c_str());
            lastBlockStmts->emplace_back(armBStmt);
            return lastBlock->getBlockName().c_str();
        }
        case STMT_BREAK: {
            /// b whileEnd
            auto *armBStmt = new ArmStmt(ARM_STMT_B, whileEndPos->at(whileEndPos->size() - 1).c_str());
            lastBlockStmts->emplace_back(armBStmt);
            return lastBlock->getBlockName().c_str();
        }
        default:
            return lastBlock->getBlockName().c_str();
    }
}

const char *Arm7Gen::genStmtAuxIf(Stmt *stmt, std::vector<ArmBlock *> *basicBlocks, ArmBlock *lastBlock,
                                  std::vector<ArmStmt *> *lastBlockStmts) {
//    const char *ifCondRet = genCondExp(stmt->getCond(), lastBlockStmts, stepName);
//
//    auto *irBodyStmts = new std::vector<IRStmt *>();
//    auto *irBodyPreLocs = new std::set<std::string>();
//    irBodyPreLocs->insert(lastBlock->getBlockName());
//    int bodyStep = stepName++;
//    auto *bodyBlock = new IRLocalBlock((REGISTER_LOCAL + std::to_string(bodyStep)).c_str(), irBodyStmts,
//                                       irBodyPreLocs);
//    basicBlocks->emplace_back(bodyBlock);
//
//    genStmt(stmt->getStmtBrBody(), basicBlocks, lastBlock, lastBlockStmts, stepName);
//
//    if (stmt->getElseBody() != nullptr) {
//        auto *irElseStmts = new std::vector<IRStmt *>();
//        auto *irElsePreLocs = new std::set<std::string>();
//        irElsePreLocs->insert(lastBlock->getBlockName());
//        int elseStep = stepName++;
//        auto *elseBlock = new IRLocalBlock((REGISTER_LOCAL + std::to_string(elseStep)).c_str(), irElseStmts,
//                                           irElsePreLocs);
//        basicBlocks->emplace_back(elseBlock);
//
//        genStmt(stmt->getElseBody(), basicBlocks, lastBlock, lastBlockStmts, stepName);
//
//        // lastBlockStmts (原if之前的语句) 添加最后跳转 br
//        auto *elseStepArray = new std::vector<std::string>();
//        elseStepArray->emplace_back(REGISTER_LOCAL + std::to_string(elseStep));
//        auto *irBrCondStmt = new IRStmt(DAG_BR, ifCondRet, (REGISTER_LOCAL + std::to_string(bodyStep)).c_str(),
//                                        elseStepArray);
//        lastBlockStmts->emplace_back(irBrCondStmt);
//
//        // if {...} else {...} 各自最后一句添加跳转
//        auto *afterEndStepStr = new std::string(REGISTER_LOCAL + std::to_string(++stepName));
//        auto *irIfBodyStmt = new IRStmt(DAG_BR, afterEndStepStr->c_str());
//        irBodyStmts->emplace_back(irIfBodyStmt);
//        auto *irIfElseBodyStmt = new IRStmt(DAG_BR, afterEndStepStr->c_str());
//        irElseStmts->emplace_back(irIfElseBodyStmt);
//        return afterEndStepStr->c_str();
//    } else {
//        // lastBlockStmts (原if之前的语句) 添加最后跳转 br
//        stepName++;
//        auto *afterIfStepArray = new std::vector<std::string>();
//        afterIfStepArray->emplace_back(REGISTER_LOCAL + std::to_string(stepName));
//        auto *irBrCondStmt = new IRStmt(DAG_BR, ifCondRet, (REGISTER_LOCAL + std::to_string(bodyStep)).c_str(),
//                                        afterIfStepArray);
//        lastBlockStmts->emplace_back(irBrCondStmt);
//
//        // if {...} 最后一句添加跳转
//        auto *afterEndStepStr = new std::string(REGISTER_LOCAL + std::to_string(++stepName));
//        auto *irIfBodyStmt = new IRStmt(DAG_BR, afterEndStepStr->c_str());
//        irBodyStmts->emplace_back(irIfBodyStmt);
//        return afterEndStepStr->c_str();
//    }
    return nullptr;
}

const char *Arm7Gen::genStmtAuxWhile(Stmt *stmt, std::vector<ArmBlock *> *basicBlocks, ArmBlock *lastBlock,
                                     std::vector<ArmStmt *> *lastBlockStmts) {
//    auto *irCondStmts = new std::vector<IRStmt *>();
//    auto *irCondPreLocs = new std::set<std::string>();
//    irCondPreLocs->insert(lastBlock->getBlockName());
//    int condStep = stepName++;
//    auto *condBlock = new IRLocalBlock((REGISTER_LOCAL + std::to_string(condStep)).c_str(), irCondStmts, irCondPreLocs);
//    basicBlocks->emplace_back(condBlock);
//    whilePos->push_back(condBlock->getBlockName());
//
//    const char *ifCondRet = genCondExp(stmt->getCond(), lastBlockStmts, stepName);
//
//    auto *irBodyStmts = new std::vector<IRStmt *>();
//    auto *irBodyPreLocs = new std::set<std::string>();
//    irBodyPreLocs->insert(condBlock->getBlockName());
//    int bodyStep = stepName++;
//    auto *bodyBlock = new IRLocalBlock((REGISTER_LOCAL + std::to_string(bodyStep)).c_str(), irBodyStmts, irBodyPreLocs);
//    basicBlocks->emplace_back(bodyBlock);
//    auto *afterEndStepStr = new std::string("tmp");
//    whileEndPos->push_back(*afterEndStepStr);
//
//    genStmt(stmt->getStmtBrBody(), basicBlocks, bodyBlock, irBodyStmts, stepName);
//
//    afterEndStepStr = new std::string(REGISTER_LOCAL + std::to_string(++stepName));
//    auto *bodyStepArray = new std::vector<std::string>();
//    bodyStepArray->emplace_back(afterEndStepStr->c_str());
//    auto *irWhileCondStmt = new IRStmt(DAG_BR, ifCondRet, bodyBlock->getBlockName().c_str(), bodyStepArray);
//    irCondStmts->emplace_back(irWhileCondStmt);
//
//    whilePos->pop_back();
//    whileEndPos->pop_back();
//    return afterEndStepStr->c_str();
    return nullptr;
}

///===-----------------------------------------------------------------------===///
/// 表达式 不计算 生成代码
///===-----------------------------------------------------------------------===///

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

ArmReg *Arm7Gen::genAddExp(AddExp *addExp, std::vector<ArmStmt *> *ArmStmts) {
    if (addExp->getOpType() == OP_NULL) {
        return genMulExp(addExp->getMulExp(), ArmStmts);
    } else {
        /// 加数乘法式中间结果，Arm7Var 成员变量不一定为 null
        ArmReg *mulRet = genMulExp(addExp->getMulExp(), ArmStmts);
        /// 中间结果压栈，不用管是否释放寄存器，null时可被直接用；非null时可被重复利用
        auto *pushStmt = new ArmStmt(ARM_STMT_PUSH, ("{" + mulRet->getRegName() + " }").c_str());
        ArmStmts->emplace_back(pushStmt);
        /// 加数加法式中间结果，Arm7Var 成员变量可为 null-->因此，可能会被误分配为 armRegRet
        ArmReg *addRet = genAddExp(addExp->getAddExp(), ArmStmts);
        /// 锁定加数加法式中间结果，Arm7Var,防止被误分配为 armRegRet
        addRet->setIfLock(ARM_REG_LOCK_TRUE);

        ArmReg *armRegRet = armRegManager->getFreeArmReg(ArmStmts);
        /// 解锁加数加法式中间结果
        addRet->setIfLock(ARM_REG_LOCK_FALSE);

        auto *popStmt = new ArmStmt(ARM_STMT_POP, ("{" + armRegRet->getRegName() + " }").c_str());
        /// 以 armRegRet 为最终结果，因为 addRet 可能为某变量，其 ArmReg 有对应某个变量地址
        auto *armAddStmt = new ArmStmt(addExp->getOpType(), armRegRet->getRegName().c_str(),
                                       armRegRet->getRegName().c_str(), addRet->getRegName().c_str());
        ArmStmts->emplace_back(popStmt);
        ArmStmts->emplace_back(armAddStmt);

        return armRegRet;
    }
}

ArmReg *Arm7Gen::genMulExp(MulExp *mulExp, std::vector<ArmStmt *> *ArmStmts) {
    if (mulExp->getOpType() == OP_NULL) {
        return genUnaryExp(mulExp->getUnaryExp(), ArmStmts);
    } else {
        /// 乘数一元表达式中间结果，Arm7Var 成员变量不一定为 null
        ArmReg *unaryRet = genUnaryExp(mulExp->getUnaryExp(), ArmStmts);
        /// 中间结果压栈，不用管是否释放寄存器，null时可被直接用；非null时可被重复利用
        auto *pushStmt = new ArmStmt(ARM_STMT_PUSH, ("{" + unaryRet->getRegName() + " }").c_str());
        ArmStmts->emplace_back(pushStmt);
        /// 乘数乘法式中间结果，Arm7Var 成员变量可为 null-->因此，可能会被误分配为 armRegRet
        ArmReg *mulRet = genMulExp(mulExp->getMulExp(), ArmStmts);
        /// 锁定乘数乘法式中间结果，Arm7Var,防止被误分配为 armRegRet
        mulRet->setIfLock(ARM_REG_LOCK_TRUE);
        /// 此时r0可能被Lock了,故虽然可能div/mod不能一步到位得到r0
        ArmReg *armRegRet = armRegManager->getFreeArmReg(ArmStmts);
        /// 解锁加数加法式中间结果
        mulRet->setIfLock(ARM_REG_LOCK_FALSE);

        auto *popStmt = new ArmStmt(ARM_STMT_POP, ("{" + armRegRet->getRegName() + " }").c_str());

        if (mulExp->getOpType() == OP_BO_MUL) {
            /// 以 armRegRet 为最终结果，因为 mulRet 可能为某变量，其 ArmReg 有对应某个变量地址
            auto *armMulStmt = new ArmStmt(mulExp->getOpType(), armRegRet->getRegName().c_str(),
                                           armRegRet->getRegName().c_str(), mulRet->getRegName().c_str());
            ArmStmts->emplace_back(popStmt);
            ArmStmts->emplace_back(armMulStmt);
        } else {
            /// DIV / REM 需要调用函数,注意为：r0 = r0 OP(/or%) r1;
            /// 注意这里不敢调用 __aeabi_idiv 因为我们知道 mov_zt/dic_zt 用了r0,r1,r2,r3
            /// 故此处:mod_zt/div_zt 函数内记得保护r2、r3现场;或者这里 free/push+pop 掉r2,r3。目前后者
            /// 而若是调用系统库函数，可能会毁了我们的寄存器现场状态。并考虑到运算多时提前push/pop全会得不偿失。

            /// push    {r0,? r1,? r2,? r3}
            for (int i = 0; i < 4; i++) {
                armRegManager->pushOneArmReg(i, ArmStmts);
            }

            if (mulRet->getRegName() == "r0") {
                /// push   {r0 }
                /// mov    r0 rFree
                /// pop    {r1 }
                auto *pushR0Stmt = new ArmStmt(ARM_STMT_PUSH, "{r0 }");
                auto *movR0Stmt = new ArmStmt(ARM_STMT_MOV, "r0", armRegRet->getRegName().c_str());
                auto *popR1Stmt = new ArmStmt(ARM_STMT_POP, "r1");
                ArmStmts->emplace_back(pushR0Stmt);
                ArmStmts->emplace_back(movR0Stmt);
                ArmStmts->emplace_back(popR1Stmt);
            } else {
                if (armRegRet->getRegName() != "r0") {
                    /// mov    r0,    rFree
                    auto *movR0Stmt = new ArmStmt(ARM_STMT_MOV, "r0", armRegRet->getRegName().c_str());
                    ArmStmts->emplace_back(movR0Stmt);
                }
                if (mulRet->getRegName() != "r1") {
                    /// mov    r1,    rR
                    auto *movR1Stmt = new ArmStmt(ARM_STMT_MOV, "r1", mulRet->getRegName().c_str());
                    ArmStmts->emplace_back(movR1Stmt);
                }

            }

            /// bl	   mod_zt / div_zt
            /// mov	   rFree, r0
            if (mulExp->getOpType() == OP_BO_DIV) {
                auto *blDivStmt = new ArmStmt(ARM_STMT_BL, "div_zt");
                ArmStmts->emplace_back(blDivStmt);
            } else if (mulExp->getOpType() == OP_BO_REM) {
                auto *blRemStmt = new ArmStmt(ARM_STMT_BL, "mod_zt");
                ArmStmts->emplace_back(blRemStmt);
            }
            auto *movRFreeStmt = new ArmStmt(ARM_STMT_MOV, armRegRet->getRegName().c_str(), "r0");
            ArmStmts->emplace_back(movRFreeStmt);

            /// pop    {r0,? r1,? r2,? r3}
            for (int i = 0; i < 4; i++) {
                armRegManager->popOneArmReg(i, ArmStmts);
            }
        }
        return armRegRet;
    }
}

ArmReg *Arm7Gen::genUnaryExp(UnaryExp *unaryExp, std::vector<ArmStmt *> *ArmStmts) {
    if (unaryExp->getPrimaryExp() != nullptr) {
        return genPrimaryExp(unaryExp->getPrimaryExp(), ArmStmts);
    } else if (unaryExp->getOpType() != OP_NULL) {
        if (unaryExp->getOpType() == OP_BO_ADD) {
            return genUnaryExp(unaryExp->getUnaryExp(), ArmStmts);
        } else if (unaryExp->getOpType() == OP_BO_SUB) {
            ArmReg *unaryArmReg = genUnaryExp(unaryExp->getUnaryExp(), ArmStmts);
            /// 取反 rI = -rI
            /// rsb	rI, rI, #0
            auto *armNegStmt = new ArmStmt(ARM_STMT_RSB, unaryArmReg->getRegName().c_str(),
                                           unaryArmReg->getRegName().c_str(), "#0");
            ArmStmts->emplace_back(armNegStmt);
            return unaryArmReg;
        } else if (unaryExp->getOpType() == OP_UO_NOT) {
            /// TODO fix when condExp
            return nullptr;
        }
        return nullptr;
    } else {
        /// 函数调用 有参数
        /// 万法归宗之 LVal 永存真实地址->则无论参数数组还是局部数组,永远先ldr再add好了
        /// 注意此处要求，局部数组声明时。
        if (unaryExp->getFuncRParams() != nullptr) {
            auto *exps = unaryExp->getFuncRParams()->getExps();
            if (exps->size() >= 4) {
                /// sub sp sp #exps->size()*4-16
                auto *armSPStmt = new ArmStmt(ARM_STMT_SUB, "sp", "sp",
                                              ("#" + std::to_string(exps->size() * 4 - 16)).c_str());
                ArmStmts->emplace_back(armSPStmt);

                for (int i = 4; i < exps->size(); i++) {
                    auto *armRagParam = genAddExp(exps->at(i)->getAddExp(), ArmStmts);
                    /// str  rX  [sp, #i*4-12]
                    auto *armStrStmt = new ArmStmt(ARM_STMT_STR, armRagParam->getRegName().c_str(),
                                                   ("[sp, #" + std::to_string(i * 4 - 12) + "]").c_str());
                    ArmStmts->emplace_back(armStrStmt);
                }
            }
            for (int i = 0; i < 4; i++) {
                auto *armRegParam = genAddExp(exps->at(i)->getAddExp(), ArmStmts);
                if (armRegParam->getRegName() != "r" + std::to_string(i)) {
                    /// 释放寄存器并锁定不再分配
                    armRegManager->freeOneArmReg(i, ArmStmts);
                    armRegParam->setIfLock(ARM_REG_LOCK_TRUE);
                    /// mov  rI   aParam
                    auto *armMovStmt = new ArmStmt(ARM_STMT_MOV, ("r" + std::to_string(i)).c_str(),
                                                   armRegParam->getRegName().c_str());
                    ArmStmts->emplace_back(armMovStmt);
                }
            }
            for (int i = 0; i < 4; i++) {
                armRegManager->getArmRegs()->at(i)->setIfLock(ARM_REG_LOCK_FALSE);
            }
        }

        /// bl funcName
        auto *blFuncStmt = new ArmStmt(ARM_STMT_BL, unaryExp->getIdent().c_str());
        ArmStmts->emplace_back(blFuncStmt);
        /// 返回 r0 寄存器
        return armRegManager->getArmRegs()->at(0);
    }
}

ArmReg *Arm7Gen::genPrimaryExp(PrimaryExp *primaryExp, std::vector<ArmStmt *> *ArmStmts) {
    if (primaryExp->getExp() != nullptr) {
        return genAddExp(primaryExp->getExp()->getAddExp(), ArmStmts);
    } else if (primaryExp->getLVal() != nullptr) {
        return genLVal(primaryExp->getLVal(), ArmStmts, 0);
    } else {
        auto *armDegNum = armRegManager->getFreeArmReg(ArmStmts);
        /// mov rX #NUMBER
        auto *armMovStmt = new ArmStmt(ARM_STMT_MOV, armDegNum->getRegName().c_str(),
                                       ("#" + std::to_string(primaryExp->getNumber())).c_str());
        ArmStmts->emplace_back(armMovStmt);
        return armDegNum;
    }
}

ArmReg *Arm7Gen::genLVal(LVal *lVal, std::vector<ArmStmt *> *ArmStmts, int ifGetPos) {
    if (lVal->getExps()->empty()) {  // 整型
        return armRegManager->getArmRegByNamePos(lVal->getIdent().c_str(), lVal->getIntPos(), ArmStmts);
    } else {  // 数组
        /// 数组起点地址
        ArmReg *armRegLVal;
        armRegLVal = armRegManager->getArmRegByNamePos(lVal->getIdent().c_str(), lVal->getIntPos(),
                                                       ArmStmts);
        armRegLVal->setIfLock(ARM_REG_LOCK_TRUE);
        armRegLVal->setArm7Var(nullptr);
        /// 获得不同情况下数组的位置（内存地址）
        switch (lVal->getType()) {
            case LVAL_ARRAY_LOCAL_INT: {
                /// sub rF fp #Loc
                auto *armSubStmt = new ArmStmt(ARM_STMT_SUB, armRegLVal->getRegName().c_str(),
                                               "fp", ("#" + std::to_string(lVal->getIntPos())).c_str());
                ArmStmts->emplace_back(armSubStmt);
                break;
            }
            case LVAL_ARRAY_PARAM_INT_STAR:
            case LVAL_ARRAY_LOCAL_INT_STAR:
            case LVAL_ARRAY_PARAM_INT: {
                /// ldr rLVal [fp, #Loc]
                auto *armLdrStmt = new ArmStmt(ARM_STMT_LDR, armRegLVal->getRegName().c_str(),
                                               lVal->getBaseMemoryPos().c_str());
                ArmStmts->emplace_back(armLdrStmt);
                break;
            }
            case LVAL_ARRAY_GLOBAL_INT:
            case LVAL_ARRAY_GLOBAL_INT_STAR: {
                /// movw	r3, #:lower16:gR
                /// movt	r3, #:upper16:gR
                auto *armLMovWStmt = new ArmStmt(ARM_STMT_MOVW, armRegLVal->getRegName().c_str(),
                                                 ("#:lower16:" + lVal->getIdent()).c_str());
                ArmStmts->emplace_back(armLMovWStmt);
                auto *armLMovTStmt = new ArmStmt(ARM_STMT_MOVT, armRegLVal->getRegName().c_str(),
                                                 ("#:lower16:" + lVal->getIdent()).c_str());
                ArmStmts->emplace_back(armLMovTStmt);
                break;
            }
            default:
                Error::errorSim("wrong when gen at LVal Array");
                exit(-1);
        }
        /// 索引！索引！
        for (int i = 0; i < lVal->getExps()->size(); i++) {
            auto *armRegRet = genAddExp(lVal->getExps()->at(i)->getAddExp(), ArmStmts);
            int subLenRem = 1;
            for (int j = i + 1; j < lVal->getSubs()->size(); j++) {
                subLenRem *= j;
            }

            if (subLenRem != 1) {
                /// mov  rFree  subLenRem
                auto *armRegLenRem = armRegManager->getFreeArmReg(ArmStmts);
                auto *armLenMovStmt = new ArmStmt(ARM_STMT_MOV, armRegLenRem->getRegName().c_str(),
                                                  ("#" + std::to_string(subLenRem * 4)).c_str());
                ArmStmts->emplace_back(armLenMovStmt);

                /// mul rRegRet rRegRet rFree
                armRegRet->setArm7Var(nullptr);
                auto *armMulStmt = new ArmStmt(ARM_STMT_MUL, armRegRet->getRegName().c_str(),
                                               armRegLenRem->getRegName().c_str(), armRegRet->getRegName().c_str());
                ArmStmts->emplace_back(armMulStmt);
            }
            /// add rRegRet rRegRet rLVal
            auto *armAddStmt = new ArmStmt(ARM_STMT_ADD, armRegRet->getRegName().c_str(),
                                           armRegRet->getRegName().c_str(), armRegLVal->getRegName().c_str());
            ArmStmts->emplace_back(armAddStmt);


        }  // end for exps

        /// 返回值或者地址！
        switch (lVal->getType()) {
            case LVAL_ARRAY_LOCAL_INT:
            case LVAL_ARRAY_PARAM_INT:
            case LVAL_ARRAY_GLOBAL_INT: {
                if (ifGetPos == 0) {
                    /// ldr armRegLVal [armRegLVal]
                    auto *armLdrStmt = new ArmStmt(ARM_STMT_LDR, armRegLVal->getRegName().c_str(),
                                                   ("[" + armRegLVal->getRegName() + "]").c_str());
                    ArmStmts->emplace_back(armLdrStmt);
                }
                armRegLVal->setIfLock(ARM_REG_LOCK_FALSE);
                break;
            }

            case LVAL_ARRAY_LOCAL_INT_STAR:
            case LVAL_ARRAY_PARAM_INT_STAR:
            case LVAL_ARRAY_GLOBAL_INT_STAR: {
                armRegLVal->setIfLock(ARM_REG_LOCK_FALSE);
                break;
            }
            default:
                Error::errorSim("wrong when gen at LVal Array");
                exit(-1);
        }
        return armRegLVal;
    }
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
