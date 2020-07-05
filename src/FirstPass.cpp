//
// Created by chris on 28/6/20.
//

#include "FirstPass.h"

void FirstPass::analyze(ClassNode* node) {
    ClassInfo* c = new ClassInfo;
    for (auto f: node->fields) {
        c->fields[f->identifier] = new SimpleInfo(f->type);
    }
    for (auto m: node->methods) {
        c->methods[m->name] = new FunctionInfo(m->parameter_types, m->return_type);
    }
    SymbolInfo* symbol_info = new SymbolInfo;
    symbol_info->type = SINFO::CLASS;
    symbol_info->class_info = c;
    this->globals->set(node->name, symbol_info);
}

void FirstPass::analyze(FunctionNode* node) {
    SymbolInfo* ginfo = new SymbolInfo;
    ginfo->type = SINFO::FUNCTION;
    ginfo->function_info = new FunctionInfo(node->parameter_types, node->return_type);
    this->globals->set(node->name, ginfo);
}

void FirstPass::analyze(VectorOfNodes program) {
    for (auto n: program) {
        if (n->type == AstType::CLASS) {
            this->analyze(n->ast_class);
        }
        if (n->type == AstType::FUNCTION) {
            this->analyze(n->ast_function);
        }
    }
}

FunctionInfo::FunctionInfo(std::vector<TypeNode*> parameter_types, TypeNode* return_type) {
    for (auto n: parameter_types) {
        this->parameter_types.push_back(new SimpleInfo(n));
    }
    if (return_type == NULL) {
        this->return_type = NULL;
    } else {
        this->return_type = new SimpleInfo(return_type);
    }
}

SimpleInfo::SimpleInfo(TypeNode* n) {
    this->parent = n->name;
    for (auto t: n->type_parameters) {
        this->type_parameters.push_back(new SimpleInfo(t));
    }
}
