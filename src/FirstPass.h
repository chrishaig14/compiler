//
// Created by chris on 28/6/20.
//

#ifndef UNTITLED1_FIRSTPASS_H
#define UNTITLED1_FIRSTPASS_H


#include <map>
#include "AstNode.h"
#include "SymbolTable.h"


class FirstPass {
public:
    SymbolTable* globals;
    FirstPass(){
        this->globals = new SymbolTable("global",NULL);
    }

    void analyze(ClassNode* node);

    void analyze(FunctionNode* node);

    void analyze(VectorOfNodes program);

};

#endif //UNTITLED1_FIRSTPASS_H
