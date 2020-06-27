#include <iostream>
#include "Dump.h"

using namespace std;

void Dump::dump(Token *token, int level) {
    cout << "Token Level:" << level << endl;
    cout << "type :\t" << token->getType() << endl;
    cout << "row  :\t" << token->getRow() << endl;
    cout << "sCol :\t" << token->getStartColumn() << endl;
    cout << "eCol :\t" << token->getEndColumn() << endl;
    cout << "ident:\t" << token->getIdentifierStr() << endl;
    cout << "numV :\t" << token->getNumVal() << endl;
}

void Dump::dump(AST *ast, int level) {
    cout << "AST Level:" << level << "\n" << endl;

}

void Dump::dump(ConstDecl *constDecl, int level) {
    cout << "Level:" << level << "\n" << endl;
}

void Dump::dump(ConstDef *constDef, int level) {
    cout << "Level:" << level << "\n" << endl;
}

void Dump::dump(ConstInitVal *constInitVal, int level) {
    cout << "Level:" << level << "\n" << endl;
}

void Dump::dump(VarDecl *varDecl, int level) {
    cout << "Level:" << level << "\n" << endl;
}

void Dump::dump(VarDef *varDef, int level) {
    cout << "Level:" << level << "\n" << endl;
}

void Dump::dump(InitVal *initVal, int level) {
    cout << "Level:" << level << "\n" << endl;
}

void Dump::dump(FuncDef *funcDef, int level) {
    cout << "Level:" << level << "\n" << endl;
}

void Dump::dump(FuncFParams *funcFParams, int level) {
    cout << "Level:" << level << "\n" << endl;
}

void Dump::dump(FuncFParam *funcFParam, int level) {
    cout << "Level:" << level << "\n" << endl;
}

void Dump::dump(Stmt *stmt, int level) {
    cout << "Level:" << level << "\n" << endl;
}

void Dump::dump(Block *block, int level) {
    cout << "Level:" << level << "\n" << endl;
}

void Dump::dump(BlockItem *blockItem, int level) {
    cout << "Level:" << level << "\n" << endl;
}

void Dump::dump(ConstExp *constExp, int level) {
    cout << "Level:" << level << "\n" << endl;
}

void Dump::dump(Exp *exp, int level) {
    cout << "Level:" << level << "\n" << endl;
}

void Dump::dump(Cond *cond, int level) {
    cout << "Level:" << level << "\n" << endl;
}

void Dump::dump(LOrExp *lOrExp, int level) {
    cout << "Level:" << level << "\n" << endl;
}

void Dump::dump(LAndExp *lAndExp, int level) {
    cout << "Level:" << level << "\n" << endl;
}

void Dump::dump(EqExp *eqExp, int level) {
    cout << "Level:" << level << "\n" << endl;
}

void Dump::dump(RelExp *relExp, int level) {
    cout << "Level:" << level << "\n" << endl;
}

void Dump::dump(AddExp *addExp, int level) {
    cout << "Level:" << level << "\n" << endl;
}

void Dump::dump(MulExp *mulExp, int level) {
    cout << "Level:" << level << "\n" << endl;
}

void Dump::dump(UnaryExp *unaryExp, int level) {
    cout << "Level:" << level << "\n" << endl;
}

void Dump::dump(PrimaryExp *primaryExp, int level) {
    cout << "Level:" << level << "\n" << endl;
}

void Dump::dump(LVal *lVal, int level) {
    cout << "Level:" << level << "\n" << endl;
}

void Dump::dump(FuncRParams *funcRParams, int level) {
    cout << "Level:" << level << "\n" << endl;
}
