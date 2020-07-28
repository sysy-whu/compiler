#include "IRLocalDAGGen.h"
#include <cstring>
#include <iostream>

IRLocalDAGGen::IRLocalDAGGen(IRTree *irTree) {
    IRLocalDAGGen::irTree = irTree;
}

void IRLocalDAGGen::startGen() {
    auto *dagRoot = new DAGRoot();
    irTree->setGlobalDagRoot(dagRoot);

    for (IRGlobal *irGlobal:*irTree->getIrGlobals()) {
        if (irGlobal->getIrGlobalVar() != nullptr) {
            std::cout << "var %s" << std::endl;
            genStmts(irGlobal->getIrGlobalVar()->getIrStmt(), irTree->getGlobalDagRoot());
        } else {
            for (IRLocalBlock *irLocalBlock:*irGlobal->getIrGlobalFunc()->getBaseBlocks()) {
                std::cout << "func %s" << irGlobal->getIrGlobalFunc()->getFuncName() << std::endl;
                genStmts(irLocalBlock->getStmts(), irLocalBlock->getDagRoot());
            }
        }
    }
}

void IRLocalDAGGen::genStmts(std::vector<IRStmt *> *irStmts, DAGRoot *dagRoot) {
    for (IRStmt *irStmt: *irStmts) {
        std::string opd1 = irStmt->getOpd1();
        std::string opd2 = irStmt->getOpd2();
        std::vector<std::string> *opd3 = irStmt->getOpd3();
        switch (irStmt->getOpt()) {
            case DAG_BR: {
                if (opd2 == OPD_NULL) {
                    dagRoot->AddBr(opd1);
                } else {
                    dagRoot->AddBr(opd1, opd2, opd3->at(0));
                }
                break;
            }
            case DAG_RET: {
                // 注空
                if (opd1 == OPD_NULL) {
                    dagRoot->AddRet();
                } else {
                    dagRoot->AddRet(opd1);
                }
                break;
            }
            case DAG_ALLOCA_i32:
                dagRoot->AddAlloca(opd1);
                break;
            case DAG_GLOBAL_i32:
                dagRoot->AddGlobal(opd1);
                break;
            case DAG_ALLOCA_i32_ARRAY:
                dagRoot->AddAlloca_Array(opd1, *opd3);
                break;
            case DAG_GLOBAL_i32_ARRAY:
                dagRoot->AddGlobal_Array(opd1, *opd3);
                break;
            case DAG_Con_ALLOCA_i32:
                dagRoot->AddCon_Alloca(opd1);
                break;
            case DAG_Con_GLOBAL_i32:
                dagRoot->AddGlobal(opd1);
                break;
            case DAG_Con_ALLOCA_ARRAY_i32:
                dagRoot->AddCon_Alloca_Array(opd1, *opd3);
                break;
            case DAG_Con_GLOBAL_ARRAY_i32:
                dagRoot->AddCon_Global_Array(opd1, *opd3);
                break;
            case DAG_LOAD:
                dagRoot->AddLoad(opd1, opd2);
                break;
            case DAG_STORE: {
                char *stopChar;
                if (opd2[1] != REGISTER_LOCAL[0] && opd2[1] != REGISTER_GLOBAL[0]) {
                    dagRoot->AddStore(opd1, std::strtol(opd2.c_str(), &stopChar, 10));
                } else {
                    dagRoot->AddStore(opd1, opd2);
                }
                break;
            }
            case DAG_GETPTR:
                dagRoot->AddGetPtr(opd1, opd2, *opd3);
                break;
            case DAG_CALL:
                dagRoot->AddCall(opd1, opd2, *opd3);
                break;
            case DAG_ADD:
            case DAG_SUB: {
                if (opd3 == nullptr) {
                    dagRoot->AddUO(opd1, opd2, irStmt->getOpt());
                } else {
                    dagRoot->AddBO(opd1, opd2, opd3->at(0), irStmt->getOpt());
                }
                break;
            }
            case DAG_MUL:
            case DAG_DIV:
            case DAG_REM:

            case DAG_GT:
            case DAG_LT:
            case DAG_OR:
            case DAG_EQ:
            case DAG_NEQ:
            case DAG_GTE:
            case DAG_LTE: {
                dagRoot->AddBO(opd1, opd2, opd3->at(0), irStmt->getOpt());
                break;
            }
            case DAG_NOT: {
                dagRoot->AddUO(opd1, opd2, irStmt->getOpt());
                break;
            }
        }
    }
}
