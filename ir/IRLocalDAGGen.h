#ifndef COMPILER_IRLOCALDAGGEN_H
#define COMPILER_IRLOCALDAGGEN_H


#include "IRGlobal.h"

class IRLocalDAGGen {
private:
    IRTree *irTree;

    void genStmts(std::vector<IRStmt *> *irStmts, DAGRoot *dagRoot);

public:
    explicit IRLocalDAGGen(IRTree *irTree);

    void startGen();
};


#endif //COMPILER_IRLOCALDAGGEN_H
