#include "SymbolTable.h"


Arm7Var *SymbolTable::getGlobalVar(const char *varName) {
    for(Symbol *symbol: *symbols){
        if(symbol->getArm7Var() != nullptr  && symbol->getArm7Var()->getIdent() == varName){
            return symbol->getArm7Var();
        }
    }
    return nullptr;
}
