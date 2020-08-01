#include "../ir/IRGlobal.h"
#include "../ir/IRLocalBlock.h"
#include "Arm.h"
#include "ArmStmt.h"


void ArmBuilder::generateArmStmts(const char *blockName, std::vector<IRGlobalFuncParam *> *funcParms) {;
    // 如果blockName为entry就添加函数入栈和获取实参的armstmt
    if (std::string(blockName)== "entry") {
        addArmStmts(genEntryParam(funcParms));
    }

    for (auto stmt:*stmts) {
        // 根据stmt的类型选择相应的
        switch (stmt->getOpt()) {
            case DAG_ALLOCA_i32 :
                addArmStmts(genAlloca(stmt));
                break;
            case DAG_Con_ALLOCA_i32 :
                addArmStmts(genConAlloca(stmt));
                break;
            case DAG_ALLOCA_i32_ARRAY :
                addArmStmts(genAllocaArray(stmt));
                break;
            case DAG_Con_ALLOCA_ARRAY_i32 :
                addArmStmts(genConAllocaArray(stmt));
                break;
            case DAG_ADD :
                addArmStmts(genAddNode(stmt));
                break;
            case DAG_SUB :
                addArmStmts(genSubNode(stmt));
                break;
            case DAG_MUL :
                addArmStmts(genMulNode(stmt));
                break;
            case DAG_DIV :
                addArmStmts(genDivNode(stmt));
                break;
            case DAG_REM :
                addArmStmts(genModNode(stmt));
                break;
            case DAG_AND :
                addArmStmts(genAndNode(stmt));
                break;
            case DAG_OR :
                addArmStmts(genOrNode(stmt));
                break;
            case DAG_EQ :
                addArmStmts(genEQNode(stmt));
                break;
            case DAG_GT :
                addArmStmts(genGTNode(stmt));
                break;
            case DAG_LT :
                addArmStmts(genGTNode(stmt));
                break;
            case DAG_NEQ :
                addArmStmts(genNEQNode(stmt));
                break;
            case DAG_LTE :
                addArmStmts(genLTENode(stmt));
                break;
            case DAG_GTE :
                addArmStmts(genGTENode(stmt));
                break;
            case DAG_NOT:
                addArmStmts(genExclamation(stmt));
                break;
            case DAG_BR:
                addArmStmts(genBRNode(stmt));
                break;
            case DAG_RET:
                addArmStmts(genRetNode(stmt));
                break;
            case DAG_CALL:
                addArmStmts(genCallNode(stmt));
                break;
            case DAG_STORE:
                addArmStmts(genStoreNode(stmt));
                break;
            case DAG_LOAD:
                addArmStmts(genLoadNode(stmt));
                break;
            case DAG_GETPTR:
                addArmStmts(genGetPtrNode(stmt));
                break;
        }
    }

}


/// 生成函数入栈和获取实参的armstmt

std::vector<ArmStmt *> *ArmBuilder::genEntryParam(std::vector<IRGlobalFuncParam *> *parmsInfo) {

    int entryCount = -1;
    auto *currStmts = new std::vector<ArmStmt *>();

    int paramNum = parmsInfo->size();
    if (paramNum != 0) {

        //把实参逐个load到当前栈内并记录  当前栈情况：   param1 | param2 ...

        for (int i = 0; i < paramNum; i++) {

            // ldr r, [fp,#4]
            // sub sp, sp, 4
            // str r, [fp, #能存到的位置]

            std::string addr1 = (std::string) "[FP,#" + std::to_string(paramNum * 4) + "]";
            std::string addr2 = (std::string) "[FP,#" + std::to_string(i * 4) + "]";

            ArmStmt *stmt0 = new ArmStmt(LDR, "$R2", addr1);
            ArmStmt *stmt1 = new ArmStmt(SUB, "$R2", "$SP", "4");
            ArmStmt *stmt2 = new ArmStmt(STR, "$R2", addr2);

            currStmts->emplace_back(stmt0);
            currStmts->emplace_back(stmt1);
            currStmts->emplace_back(stmt2);

            //在stackStatus中记录变量位置 level为1
            std::multimap<std::string, VarInfo> varMap = *status->getVarMap();
            std::string varName = parmsInfo->at(i)->getParamName();
            int loc = status->getCurrentLoc() + 4;
            varMap.insert({varName, VarInfo{loc, 1}});
            status->setCurrentLoc(loc);
            status->setVarMap(&varMap);
        }

        // PUSH {fp,lr}    ADD fp, sp, #4
        ArmStmt *pushStmt = new ArmStmt(PUSH, "{fp,lr}");
        ArmStmt *addStmt = new ArmStmt(ADD, "$FP", "$SP", "4");

        currStmts->emplace_back(pushStmt);
        currStmts->emplace_back(addStmt);
        return currStmts;
    } else { // 无参数

        // PUSH {fp,lr}    ADD fp, sp, #4
        ArmStmt *pushStmt = new ArmStmt(PUSH, "{fp,lr}");
        ArmStmt *addStmt = new ArmStmt(ADD, "$FP", "$SP", "4");

        currStmts->emplace_back(pushStmt);
        currStmts->emplace_back(addStmt);
        return currStmts;

    }
}

/// 全局变量处理

std::vector<std::string> *ArmBuilder::genGlobal(IRGlobalVar *var) {
    auto *armStmt = new std::vector<std::string>();
    if (var->getGlobalValue()->size() == 0) { //未初始化的全局变量
        armStmt->push_back("    .comm " + var->getVarName() + ", 4\n");
    } else { //初始化的全局变量
        std::string name = var->getVarName();
        armStmt->emplace_back("    .global " + name + "\n");
        armStmt->emplace_back("    .data\n");
        armStmt->emplace_back("    .align 2\n");
        armStmt->emplace_back("    .type " + name + ", %object\n");
        armStmt->emplace_back("    .size " + name + "," + "4\n");
        armStmt->emplace_back(var->getVarName() + ":\n");
        armStmt->emplace_back("    .word " + std::to_string(var->getGlobalValue()->at(0)) + "\n");
    }

    return armStmt;
}

std::vector<std::string> *ArmBuilder::genConGlobal(IRGlobalVar *var) {
    auto *armStmt = new std::vector<std::string>();

    const std::string &name = var->getVarName();
    armStmt->emplace_back("    .global " + name + "\n");
    armStmt->emplace_back("    .section .rodata\n");
    armStmt->emplace_back("    .align 2\n");
    armStmt->emplace_back("    .type " + name + ", %object\n");
    armStmt->emplace_back("    .size " + name + "," + "4\n");
    armStmt->emplace_back(var->getVarName() + ":\n");

    armStmt->emplace_back("    .word " + std::to_string(var->getGlobalValue()->at(0)) + "\n");

    return armStmt;
}

std::vector<std::string> *ArmBuilder::genGlobalArray(IRGlobalVar *var) {
    auto *armStmt = new std::vector<std::string>();
    int length = var->getGlobalValue()->size() * 4;

    const std::string &name = var->getVarName();
    armStmt->emplace_back("    .global " + name + "\n");
    armStmt->emplace_back("    .data\n");
    armStmt->emplace_back("    .align 2\n");
    armStmt->emplace_back("    .type " + name + ", %object\n");
    armStmt->emplace_back("    .size " + name + ", " + std::to_string(length) + "\n");
    armStmt->emplace_back(var->getVarName() + ":\n");

    for (int i = 0; i < length / 4; i++) {
        armStmt->emplace_back("    .word " + std::to_string(var->getGlobalValue()->at(i)) + "\n");
    }

    return armStmt;
}

std::vector<std::string> *ArmBuilder::genConGlobalArray(IRGlobalVar *var) {
    auto *armStmt = new std::vector<std::string>();
    int length = var->getGlobalValue()->size() * 4;

    const std::string &name = var->getVarName();
    armStmt->emplace_back("    .global " + name + "\n");
    armStmt->emplace_back("    .section .rodata\n");
    armStmt->emplace_back("    .align 2\n");
    armStmt->emplace_back("    .type " + name + ", %object\n");
    armStmt->emplace_back("    .size " + name + ", " + std::to_string(length) + "\n");
    armStmt->emplace_back(var->getVarName() + ":\n");

    for (int i = 0; i < length / 4; i++) {
        armStmt->emplace_back("    .word " + std::to_string(var->getGlobalValue()->at(i)) + "\n");
    }

    return armStmt;
}

/// 声明语句
std::vector<ArmStmt *> *ArmBuilder::genAlloca(IRStmt *irStmt) {
    auto *currStmts = new std::vector<ArmStmt *>();

    std::string opd1 = irStmt->getOpd1();

    ArmStmt *stmt = new ArmStmt(SUB, "$SP", "$SP", "4");

    //记录变量位置
    std::multimap<std::string, VarInfo> varMap = *(status->getVarMap());
    varMap.emplace(opd1, VarInfo{status->getCurrentLoc(), irStmt->getLevel()});
    int loc = status->getCurrentLoc() + 4;
    status->setCurrentLoc(loc);
    status->setVarMap(&varMap);

    currStmts->emplace_back(stmt);

    return currStmts;
}

std::vector<ArmStmt *> *ArmBuilder::genConAlloca(IRStmt *irStmt) {
    auto *currStmts = new std::vector<ArmStmt *>();

    std::string opd1 = irStmt->getOpd1();

    ArmStmt *stmt = new ArmStmt(SUB, "$SP", "$SP", "4");

    //记录变量位置
    std::multimap<std::string, VarInfo> varMap = *(status->getVarMap());
    varMap.emplace(opd1, VarInfo{status->getCurrentLoc(), irStmt->getLevel()});
    int loc = status->getCurrentLoc() + 4;
    status->setCurrentLoc(loc);
    status->setVarMap(&varMap);

    currStmts->emplace_back(stmt);

    return currStmts;
}

std::vector<ArmStmt *> *ArmBuilder::genAllocaArray(IRStmt *irStmt) {

    auto *currStmts = new std::vector<ArmStmt *>();

    std::string opd1 = irStmt->getOpd1();

    //保存维度信息，并获取维度之积
    std::vector<std::string> *opd3 = irStmt->getOpd3();

    int dim = irStmt->getOpd3()->size();

    std::vector<int> dims;
    int space = 4;

    for (int i = 0; i < dim; i++) {
        int tmp = std::stoi(opd3->at(i));
        dims.emplace_back(tmp);
        space *= tmp;
    }

    std::string spaceStr = "#" + std::to_string(space);

    ArmStmt *stmt = new ArmStmt(SUB, "$SP", "$SP", spaceStr);
    currStmts->emplace_back(stmt);

    //记录变量位置,写入维度信息
    std::multimap<std::string, VarInfo> varMap = *(status->getVarMap());
    std::multimap<std::string, ArrayInfo> dimMap = *(status->getArrDimensionMap());

    varMap.emplace(opd1, VarInfo{status->getCurrentLoc(), irStmt->getLevel()});
    dimMap.emplace(opd1, ArrayInfo{dims, irStmt->getLevel()});

    int loc = status->getCurrentLoc() + space;
    status->setCurrentLoc(loc);
    status->setArrDimensionMap(&dimMap);
    status->setVarMap(&varMap);

    return currStmts;
}

std::vector<ArmStmt *> *ArmBuilder::genConAllocaArray(IRStmt *irStmt) {

    auto *currStmts = new std::vector<ArmStmt *>();

    std::string opd1 = irStmt->getOpd1();

    //保存维度信息，并获取维度之积
    std::vector<std::string> *opd3 = irStmt->getOpd3();

    int dim = irStmt->getOpd3()->size();

    std::vector<int> dims;
    int space = 4;

    for (int i = 0; i < dim; i++) {
        int tmp = std::stoi(opd3->at(i));
        dims.emplace_back(tmp);
        space *= tmp;
    }

    std::string spaceStr = "#" + std::to_string(space);
    ArmStmt *stmt = new ArmStmt(SUB, "$SP", "$SP", spaceStr);
    currStmts->emplace_back(stmt);

    //记录变量位置,写入维度信息
    std::multimap<std::string, VarInfo> varMap = *(status->getVarMap());
    std::multimap<std::string, ArrayInfo> dimMap = *(status->getArrDimensionMap());

    varMap.emplace(opd1, VarInfo{status->getCurrentLoc(), irStmt->getLevel()});
    dimMap.emplace(opd1, ArrayInfo{dims, irStmt->getLevel()});

    int loc = status->getCurrentLoc() + space;
    status->setCurrentLoc(loc);
    status->setArrDimensionMap(&dimMap);
    status->setVarMap(&varMap);

    return currStmts;
}

/// 运算
std::vector<ArmStmt *> *ArmBuilder::genAddNode(IRStmt *irStmt) {

    auto *currStmts = new std::vector<ArmStmt *>();

    std::string opd1 = irStmt->getOpd1();
    std::string opd2 = irStmt->getOpd2();
    std::string opd3 = irStmt->getOpd3()->at(0);

    ArmStmt *stmt = new ArmStmt(ADD, opd1, opd2, opd3);
    currStmts->emplace_back(stmt);

    return currStmts;
}

std::vector<ArmStmt *> *ArmBuilder::genSubNode(IRStmt *irStmt) {
    auto *currStmts = new std::vector<ArmStmt *>();

    std::string opd1 = irStmt->getOpd1();
    std::string opd2 = irStmt->getOpd2();
    std::string opd3 = irStmt->getOpd3()->at(0);

    ArmStmt *stmt = new ArmStmt(SUB, opd1, opd2, opd3);
    currStmts->emplace_back(stmt);

    return currStmts;
}

std::vector<ArmStmt *> *ArmBuilder::genMulNode(IRStmt *irStmt) {

    auto *currStmts = new std::vector<ArmStmt *>();

    std::string opd1 = irStmt->getOpd1();
    std::string opd2 = irStmt->getOpd2();
    std::string opd3 = irStmt->getOpd3()->at(0);

    ArmStmt *stmt = new ArmStmt(MUL, opd1, opd2, opd3);
    currStmts->emplace_back(stmt);

    return currStmts;
}

std::vector<ArmStmt *> *ArmBuilder::genDivNode(IRStmt *irStmt) {
    // 在.s文件开头添加    .global __aeabi_idiv

    auto *currStmts = new std::vector<ArmStmt *>();

    std::string opd1 = irStmt->getOpd1();
    std::string opd2 = irStmt->getOpd2();
    std::string opd3 = irStmt->getOpd3()->at(0);

    ArmStmt *stmt0 = new ArmStmt(MOV, "$r0", opd2);
    ArmStmt *stmt1 = new ArmStmt(MOV, "$r1", opd3);
    ArmStmt *stmt2 = new ArmStmt(BL, "__aeabi_idiv");
    ArmStmt *stmt3 = new ArmStmt(MOV, opd1, "$r0");

    currStmts->emplace_back(stmt0);
    currStmts->emplace_back(stmt1);
    currStmts->emplace_back(stmt2);
    currStmts->emplace_back(stmt3);
    return currStmts;
}

std::vector<ArmStmt *> *ArmBuilder::genModNode(IRStmt *irStmt) {
    // 在.s文件开头添加    .global __aeabi_imod

    auto *currStmts = new std::vector<ArmStmt *>();

    std::string opd1 = irStmt->getOpd1();
    std::string opd2 = irStmt->getOpd2();
    std::string opd3 = irStmt->getOpd3()->at(0);

    ArmStmt *stmt0 = new ArmStmt(MOV, "$r0", opd2);
    ArmStmt *stmt1 = new ArmStmt(MOV, "$r1", opd3);
    ArmStmt *stmt2 = new ArmStmt(BL, "__aeabi_imod");
    ArmStmt *stmt3 = new ArmStmt(MOV, opd1, "$r0");

    currStmts->emplace_back(stmt0);
    currStmts->emplace_back(stmt1);
    currStmts->emplace_back(stmt2);
    currStmts->emplace_back(stmt3);
    return currStmts;

}

std::vector<ArmStmt *> *ArmBuilder::genAndNode(IRStmt *irStmt) {

    auto *currStmts = new std::vector<ArmStmt *>();

    std::string opd1 = irStmt->getOpd1();
    std::string opd2 = irStmt->getOpd2();
    std::string opd3 = irStmt->getOpd3()->at(0);

    ArmStmt *stmt0 = new ArmStmt(ADD, opd1, opd2, opd3);
    ArmStmt *stmt1 = new ArmStmt(CMP, opd1, "#1");
    ArmStmt *stmt2 = new ArmStmt(MOVEQ, opd1, "#1");
    ArmStmt *stmt3 = new ArmStmt(MOVNE, opd1, "#0");

    currStmts->emplace_back(stmt0);
    currStmts->emplace_back(stmt1);
    currStmts->emplace_back(stmt2);
    currStmts->emplace_back(stmt3);
    return currStmts;
}

std::vector<ArmStmt *> *ArmBuilder::genOrNode(IRStmt *irStmt) {

    auto *currStmts = new std::vector<ArmStmt *>();

    std::string opd1 = irStmt->getOpd1();
    std::string opd2 = irStmt->getOpd2();
    std::string opd3 = irStmt->getOpd3()->at(0);

    ArmStmt *stmt0 = new ArmStmt(ADD, opd1, opd2, opd3);
    ArmStmt *stmt1 = new ArmStmt(CMP, opd1, "#1");
    ArmStmt *stmt2 = new ArmStmt(MOVGE, opd1, "#1");
    ArmStmt *stmt3 = new ArmStmt(MOVLT, opd1, "#0");

    currStmts->emplace_back(stmt0);
    currStmts->emplace_back(stmt1);
    currStmts->emplace_back(stmt2);
    currStmts->emplace_back(stmt3);
    return currStmts;
}

std::vector<ArmStmt *> *ArmBuilder::genEQNode(IRStmt *irStmt) {

    auto *currStmts = new std::vector<ArmStmt *>();

    std::string opd1 = irStmt->getOpd1();
    std::string opd2 = irStmt->getOpd2();
    std::string opd3 = irStmt->getOpd3()->at(0);

    ArmStmt *stmt0 = new ArmStmt(CMP, opd2, opd3);
    ArmStmt *stmt1 = new ArmStmt(MOVEQ, opd1, "#1");
    ArmStmt *stmt2 = new ArmStmt(MOVNE, opd1, "#0");

    currStmts->emplace_back(stmt0);
    currStmts->emplace_back(stmt1);
    currStmts->emplace_back(stmt2);
    return currStmts;
}

std::vector<ArmStmt *> *ArmBuilder::genGTNode(IRStmt *irStmt) {

    auto *currStmts = new std::vector<ArmStmt *>();

    std::string opd1 = irStmt->getOpd1();
    std::string opd2 = irStmt->getOpd2();
    std::string opd3 = irStmt->getOpd3()->at(0);


    ArmStmt *stmt0 = new ArmStmt(CMP, opd2, opd3);
    ArmStmt *stmt1 = new ArmStmt(MOVGT, opd1, "#1");
    ArmStmt *stmt2 = new ArmStmt(MOVLE, opd1, "#0");

    currStmts->emplace_back(stmt0);
    currStmts->emplace_back(stmt1);
    currStmts->emplace_back(stmt2);
    return currStmts;

}

std::vector<ArmStmt *> *ArmBuilder::genLTNode(IRStmt *irStmt) {

    auto *currStmts = new std::vector<ArmStmt *>();

    std::string opd1 = irStmt->getOpd1();
    std::string opd2 = irStmt->getOpd2();
    std::string opd3 = irStmt->getOpd3()->at(0);


    ArmStmt *stmt0 = new ArmStmt(CMP, opd2, opd3);
    ArmStmt *stmt1 = new ArmStmt(MOVLT, opd1, "#1");
    ArmStmt *stmt2 = new ArmStmt(MOVGE, opd1, "#0");

    currStmts->emplace_back(stmt0);
    currStmts->emplace_back(stmt1);
    currStmts->emplace_back(stmt2);
    return currStmts;
}

std::vector<ArmStmt *> *ArmBuilder::genNEQNode(IRStmt *irStmt) {

    auto *currStmts = new std::vector<ArmStmt *>();

    std::string opd1 = irStmt->getOpd1();
    std::string opd2 = irStmt->getOpd2();
    std::string opd3 = irStmt->getOpd3()->at(0);


    ArmStmt *stmt0 = new ArmStmt(CMP, opd2, opd3);
    ArmStmt *stmt1 = new ArmStmt(MOVNE, opd1, "#1");
    ArmStmt *stmt2 = new ArmStmt(MOVEQ, opd1, "#0");

    currStmts->emplace_back(stmt0);
    currStmts->emplace_back(stmt1);
    currStmts->emplace_back(stmt2);
    return currStmts;
}

std::vector<ArmStmt *> *ArmBuilder::genLTENode(IRStmt *irStmt) {

    auto *currStmts = new std::vector<ArmStmt *>();

    std::string opd1 = irStmt->getOpd1();
    std::string opd2 = irStmt->getOpd2();
    std::string opd3 = irStmt->getOpd3()->at(0);


    ArmStmt *stmt0 = new ArmStmt(CMP, opd2, opd3);
    ArmStmt *stmt1 = new ArmStmt(MOVLE, opd1, "#1");
    ArmStmt *stmt2 = new ArmStmt(MOVGT, opd1, "#0");

    currStmts->emplace_back(stmt0);
    currStmts->emplace_back(stmt1);
    currStmts->emplace_back(stmt2);
}

std::vector<ArmStmt *> *ArmBuilder::genGTENode(IRStmt *irStmt) {

    auto *currStmts = new std::vector<ArmStmt *>();

    std::string opd1 = irStmt->getOpd1();
    std::string opd2 = irStmt->getOpd2();
    std::string opd3 = irStmt->getOpd3()->at(0);


    ArmStmt *stmt0 = new ArmStmt(CMP, opd2, opd3);
    ArmStmt *stmt1 = new ArmStmt(MOVGE, opd1, "#1");
    ArmStmt *stmt2 = new ArmStmt(MOVLT, opd1, "#0");

    currStmts->emplace_back(stmt0);
    currStmts->emplace_back(stmt1);
    currStmts->emplace_back(stmt2);
    return currStmts;
}

// 单目运算

std::vector<ArmStmt *> *ArmBuilder::genPlus(IRStmt *irStmt) {

    auto *currStmts = new std::vector<ArmStmt *>();

    std::string opd1 = irStmt->getOpd1();
    std::string opd2 = irStmt->getOpd2();


    ArmStmt *stmt = new ArmStmt(CMP, opd1, opd2);

    currStmts->emplace_back(stmt);
    return currStmts;
}

std::vector<ArmStmt *> *ArmBuilder::genMinus(IRStmt *irStmt) {

    auto *currStmts = new std::vector<ArmStmt *>();

    std::string opd1 = irStmt->getOpd1();
    std::string opd2 = irStmt->getOpd2();


    ArmStmt *stmt = new ArmStmt(MUL, opd1, opd2, "#-1");

    currStmts->emplace_back(stmt);
    return currStmts;

}

std::vector<ArmStmt *> *ArmBuilder::genExclamation(IRStmt *irStmt) {

    auto *currStmts = new std::vector<ArmStmt *>();

    std::string opd1 = irStmt->getOpd1();
    std::string opd2 = irStmt->getOpd2();
    std::string opd3 = irStmt->getOpd3()->at(0);


    ArmStmt *stmt0 = new ArmStmt(CMP, opd2, "#0");
    ArmStmt *stmt1 = new ArmStmt(MOVEQ, opd1, "#0");
    ArmStmt *stmt2 = new ArmStmt(MOVNE, opd1, "#1");

    currStmts->emplace_back(stmt0);
    currStmts->emplace_back(stmt1);
    currStmts->emplace_back(stmt2);
    return currStmts;
}


/// 控制语句
std::vector<ArmStmt *> *ArmBuilder::genBRNode(IRStmt *irStmt) {

    auto *currStmts = new std::vector<ArmStmt *>();

    // 判断opd2是否为空，如果为空就是直接跳转，不为空是条件跳转
    if (irStmt->getOpd2().empty()) {
        std::string opd1 = irStmt->getOpd1();

        ArmStmt *stmt = new ArmStmt(B, opd1);
        currStmts->emplace_back(stmt);
    } else {
        std::string opd1 = irStmt->getOpd1();
        std::string opd2 = irStmt->getOpd2();
        std::string opd3 = irStmt->getOpd3()->at(0);


        ArmStmt *stmt0 = new ArmStmt(CMP, opd1, "#1");
        ArmStmt *stmt1 = new ArmStmt(BEQ, opd2);
        ArmStmt *stmt2 = new ArmStmt(B, opd3);

        currStmts->emplace_back(stmt0);
        currStmts->emplace_back(stmt1);
        currStmts->emplace_back(stmt2);
    }

    return currStmts;
}

//std::vector<ArmStmt *> *ArmBuilder::genBRCondNode(IRStmt *irStmt) {
//
//    auto *currStmts = new std::vector<ArmStmt *>();
//
//    std::string opd1 = irStmt->getOpd1();
//    std::string opd2 = irStmt->getOpd2();
//    std::string opd3 = irStmt->getOpd3()->at(0);
//
//
//    ArmStmt *stmt0 = new ArmStmt(CMP, opd1,   "1");
//    ArmStmt *stmt1 = new ArmStmt(BEQ, opd2);
//    ArmStmt *stmt2 = new ArmStmt(B, opd3);
//
//    currStmts->emplace_back(stmt0);
//    currStmts->emplace_back(stmt1);
//    currStmts->emplace_back(stmt2);
//
//    return currStmts;
//}

std::vector<ArmStmt *> *ArmBuilder::genRetNode(IRStmt *irStmt) {

    auto *currStmts = new std::vector<ArmStmt *>();

    std::string opd1 = irStmt->getOpd1();
    std::string opd2 = irStmt->getOpd2();

    ArmStmt *stmt0 = new ArmStmt(MOV, "$R0", opd1);
    ArmStmt *stmt1 = new ArmStmt(SUB, "$SP",
                                 "$FP", "#4");
    ArmStmt *stmt2 = new ArmStmt(POP, "{fp,pc}");

    currStmts->emplace_back(stmt0);
    currStmts->emplace_back(stmt1);
    currStmts->emplace_back(stmt2);

    return currStmts;
}

/// 函数调用
std::vector<ArmStmt *> *ArmBuilder::genCallNode(IRStmt *irStmt) {

    auto *currStmts = new std::vector<ArmStmt *>();

    std::string opd1 = irStmt->getOpd1(); // 无返回值时 opd1为""
    std::string opd2 = irStmt->getOpd2();
    std::vector<std::string> opd3 = *(irStmt->getOpd3());

    //  传参处理
    int paramNum = irStmt->getOpd3()->size();
    if (paramNum <= 0) { //无参数
        ArmStmt *stmt = new ArmStmt(BL, opd2);
    } else { // 有参数

        // 暂未考虑数组相关
        for (int i = 1; i < paramNum; i++) {
            std::string tmp = "{" + opd3[i] + "}";
            ArmStmt *paraStmt = new ArmStmt(PUSH, tmp);
            currStmts->emplace_back(paraStmt);
        }

        ArmStmt *brStmt = new ArmStmt(BL, opd2);
        currStmts->emplace_back(brStmt);

    }

    return currStmts;

}

///存取语句
std::vector<ArmStmt *> *ArmBuilder::genStoreNode(IRStmt *irStmt) {

    auto *currStmts = new std::vector<ArmStmt *>();

    std::string opd1 = irStmt->getOpd1();
    std::string opd2 = irStmt->getOpd2();
    std::string addr;

    std::multimap<std::string, VarInfo> *varMap = status->getVarMap();

    int loc;
    int currLevel = irStmt->getLevel();
    int level = -1;
    int n = varMap->count(opd2);
    if (n == 1) {
        VarInfo info = varMap->find(opd2)->second;
        loc = info.stackLoc;
        level = info.level;
    } else if (n > 1) {
        auto iter = varMap->find(opd2);
        VarInfo result = iter->second;
        for (int i = 0; i < n; i++) {
            VarInfo info = iter->second;
            if (currLevel >= info.level || result.level < info.level) {
                result = info;
            }
            iter++;
        }
        loc = result.stackLoc;
    } else { // not found
        perror("Compile Error: genLoadNode - variable not found!\n");
    }

    if (level == 0) { // 为全局变量 采用特殊取址方式

        addr = "[R2]";  // 这里用了R2寄存器来保存地址  !!!
        std::string lower = "#:lower16:" + opd1;
        std::string upper = "#:upper16:" + opd1;

        ArmStmt *stmt0 = new ArmStmt(MOVW, "$R2", lower);
        ArmStmt *stmt1 = new ArmStmt(MOVT, "$R2", upper);
        ArmStmt *stmt2 = new ArmStmt(MOV, opd1, opd2);
        ArmStmt *stmt3 = new ArmStmt(LDR, opd1, addr);

        currStmts->emplace_back(stmt0);
        currStmts->emplace_back(stmt1);
        currStmts->emplace_back(stmt2);
        currStmts->emplace_back(stmt3);

    } else {

        addr = "[fp, #-" + std::to_string(loc) + "]";

        ArmStmt *stmt0 = new ArmStmt(MOV, opd1, opd2);
        ArmStmt *stmt1 = new ArmStmt(STR, opd1, addr);

        currStmts->emplace_back(stmt0);
        currStmts->emplace_back(stmt1);
    }

    return currStmts;


}

std::vector<ArmStmt *> *ArmBuilder::genLoadNode(IRStmt *irStmt) {

    auto *currStmts = new std::vector<ArmStmt *>();

    std::string opd1 = irStmt->getOpd1();
    std::string opd2 = irStmt->getOpd2();
    std::string addr;

    std::multimap<std::string, VarInfo> *varMap = status->getVarMap();

    int loc;
    int currLevel = irStmt->getLevel();
    int level = -1;
    int n = varMap->count(opd2);
    if (n == 1) {
        VarInfo info = varMap->find(opd2)->second;
        loc = info.stackLoc;
        level = info.level;
    } else if (n > 1) {
        auto iter = varMap->find(opd2);
        VarInfo result = iter->second;
        for (int i = 0; i < n; i++) {
            VarInfo info = iter->second;
            if (currLevel >= info.level || result.level < info.level) {
                result = info;
            }
            iter++;
        }
        loc = result.stackLoc;
    } else { // not found
        perror("Compile Error: genLoadNode - variable not found!\n");
    }

    if (level == 0) { // 为全局变量 采用特殊取址方式
        addr = "[" + opd1 + "]";
        std::string lower = "#:lower16:" + opd2;
        std::string upper = "#:upper16:" + opd2;

        ArmStmt *stmt0 = new ArmStmt(MOVW, opd1, lower);
        ArmStmt *stmt1 = new ArmStmt(MOVT, opd1, upper);
        ArmStmt *stmt2 = new ArmStmt(LDR, opd1, addr);

        currStmts->emplace_back(stmt0);
        currStmts->emplace_back(stmt1);
        currStmts->emplace_back(stmt2);

    } else {
        addr = "[fp, #-" + std::to_string(loc) + "]";

        ArmStmt *stmt0 = new ArmStmt(LDR, opd1, addr);
        currStmts->emplace_back(stmt0);
    }

    return currStmts;
}

std::vector<ArmStmt *> *ArmBuilder::genGetPtrNode(IRStmt *irStmt) {

    auto *currStmts = new std::vector<ArmStmt *>();

    // 加载各个参数 opd3为保存各维数字的vector
    std::string opd1 = irStmt->getOpd1();
    std::string opd2 = irStmt->getOpd2();
    std::vector<std::string> opd3 = *(irStmt->getOpd3());

    std::string addr;

    int length = opd3.size();

    std::multimap<std::string, ArrayInfo> *dimMap = status->getArrDimensionMap();
    std::multimap<std::string, VarInfo> *varMap = status->getVarMap();

    // 查找位置 loc
    int loc;
    int currLevel = irStmt->getLevel();
    int level = -1;
    int n = varMap->count(opd2);
    if (n == 1) {
        VarInfo info = varMap->find(opd2)->second;
        loc = info.stackLoc;
        level = info.level;
    } else if (n > 1) {
        auto iter = varMap->find(opd2);
        VarInfo result = iter->second;
        for (int i = 0; i < n; i++) {
            VarInfo info = iter->second;
            if (currLevel >= info.level || result.level < info.level) {
                result = info;
            }
            iter++;
        }
        loc = result.stackLoc;
    } else { // not found
        perror("Compile Error: compile error: genGetPtrNode - array not found!\n");
    }

    // 查找维度 dims
    std::vector<int> dims;
    int arrLevel = -1;
    int n2 = varMap->count(opd2);
    if (n2 == 1) {
        ArrayInfo info = dimMap->find(opd2)->second;
        dims = info.dimension;
    } else if (n2 > 1) {
        auto iter = dimMap->find(opd2);
        ArrayInfo result = iter->second;
        for (int i = 0; i < n2; i++) {
            ArrayInfo info = iter->second;
            if (level == info.level) {
                result = info;
            }
            iter++;
        }
        dims = result.dimension;
    } else { // not found
        perror("Compile Error: compile error: genGetPtrNode - array not found!\n");
    }

    int dimNum = dims.size();
    std::vector<int> dimScale; // dimScale[i] 表示 dim[i], dim[i+1], ...dim[dimNum-1] 之积. 最末元素为1
    for (int i = dimNum; i > 0; i++) {
        int scale = 1;
        for (int j = i; j > 0; j++) {
            scale *= dims[j];
        }
        dimScale.emplace_back(scale);
    }
    dimScale.emplace_back(1);

    /*
     *  定义: array[dim1][dim2]..[dimn]
     *  调用: array[num1][num2]...[numn]
     *  偏移 = base_address - num1 * (dim2)*(dim3)... -num2 * (dim3)* ... - numn
     */

    if (level == 0) { // 为全局变量 采用特殊取址方式

        // 用变量名寻址
        std::string lower = "#:lower16:" + opd2;
        std::string upper = "#:upper16:" + opd2;

        // opd1 保存数组基址
        ArmStmt *stmt0 = new ArmStmt(MOVW, opd1, lower);
        ArmStmt *stmt1 = new ArmStmt(MOVT, opd1, upper);

        currStmts->emplace_back(stmt0);
        currStmts->emplace_back(stmt1);

        // 计算相对基址的偏移
        std::vector<ArmDAGNode *> calcuNodes;
        for (int i = 0; i < opd3.size(); i++) {
            currStmts->emplace_back(new ArmStmt(MUL, "$R2", opd3[i],
                                                std::to_string(dimScale[i + 1])));
            currStmts->emplace_back(new ArmStmt(SUB, opd1, opd1, "$R2"));
        }
    } else {
        addr = opd1;

        // opd1 寄存器 保存数组基址
        ArmStmt *stmt = new ArmStmt(SUB, opd1, "$FP", std::to_string(loc));
        currStmts->emplace_back(stmt);

        // 计算相对基址的偏移
        for (int i = 0; i < opd3.size(); i++) {
            currStmts->emplace_back(new ArmStmt(MUL, "$R2", opd3[i],
                                                std::to_string(dimScale[i + 1])));
            currStmts->emplace_back(new ArmStmt(SUB, opd1, opd1, "$R2"));
        }
    }

    return currStmts;

}



