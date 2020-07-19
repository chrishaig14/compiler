//
// Created by chris on 28/6/20.
//

#ifndef UNTITLED1_FIRSTPASS_H
#define UNTITLED1_FIRSTPASS_H


#include <map>
#include "AstNode.h"
#include "SymbolTable.h"
#include "ClassTable.h"

void assert_eq_si(SimpleInfo* t1, SimpleInfo* t2);

bool both_null(void* a, void* b);

bool one_null(void* a, void* b);

bool equal(SimpleInfo* a, SimpleInfo* b);

bool equal(FunctionInfo* a, FunctionInfo* b);

bool equal(ClassInfo* a, ClassInfo* b);


bool equal(SymbolInfo* a, SymbolInfo* b);

class FirstPass {
public:
    SymbolTable* globals;
    ClassTable* class_table;

    FirstPass();

    void analyze(ClassNode* node);

    void analyze(FunctionNode* node);

    void analyze(VectorOfNodes program);

};

#endif //UNTITLED1_FIRSTPASS_H
