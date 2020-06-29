//
// Created by chris on 28/6/20.
//

#ifndef UNTITLED1_SECONDPASS_H
#define UNTITLED1_SECONDPASS_H

#include <map>
#include "FirstPass.h"

class Scope {
    Scope* parent;
    std::map<std::string, GeneralInfo*> table;
public:
    Scope(Scope* parent) {
        this->parent = parent;
    }

    Scope* enter_scope() {
        Scope* child_scope = new Scope(this);
        return child_scope;
    };

    Scope* leave_scope() {
        return this->parent;
    }

    GeneralInfo* get(std::string name) {
        if (this->table.count(name) == 1) {
            return this->table[name];
        } else {
            if (this->parent != NULL) {
                return this->parent->get(name);
            }
            return NULL;
        }
    }

    void set(std::string name, GeneralInfo* info) {
        this->table[name] = info;
    }
};

class ScopeError{
    std::string msg;
public:
    ScopeError(std::string name){
        this->msg = "Name " + name + "not found in current scope";
    }
};

class SecondPass {
    Scope* scope;
public:
    SecondPass(std::map<std::string, GeneralInfo*> globals) {
        this->scope = new Scope(NULL);
        for (auto it: globals) {
            this->scope->set(it.first, it.second);
        }
    }

    void analyze(FunctionNode* n) {
        this->scope = this->scope->enter_scope();
        this->analyze(n->body);
        this->scope->leave_scope();
    }


    void analyze(IdentifierNode* n) {
        if (this->scope->get(n->name) == NULL) {
            throw ScopeError(n->name);
        }
    }


    void analyze(AstNode* n) {
        switch (n->type) {
            case AstType::FUNCTION:
                this->analyze(n->ast_function);
                break;
            case AstType::IDENTIFIER:
                this->analyze(n->ast_identifier);
                break;
        }
    }

    void analyze(VectorOfNodes program) {
        for (auto n: program) {
            this->analyze(n);
        }
    }
};

#endif //UNTITLED1_SECONDPASS_H
