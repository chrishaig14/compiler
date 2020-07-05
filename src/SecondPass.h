//
// Created by chris on 28/6/20.
//

#ifndef UNTITLED1_SECONDPASS_H
#define UNTITLED1_SECONDPASS_H

#include <map>
#include "FirstPass.h"




class ScopeError : public std::runtime_error {
public:
    ScopeError(std::string name) : runtime_error("Name " + name + "not found in current scope") {
    }
    bool operator==(const ScopeError& other) const{
        std::cout << "COMPARING ERRORS" << std::endl;
        std::string a=this->what();
        std::string b=other.what();
        bool t = a==b;
        return t;
    }
};

class RedeclareError : public std::runtime_error {
public:
    RedeclareError(std::string name) : runtime_error("Name " + name + "already declared in current scope") {
    }
    bool operator==(const RedeclareError& other) const{
        std::string a=this->what();
        std::string b=other.what();
        return a==b;
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
