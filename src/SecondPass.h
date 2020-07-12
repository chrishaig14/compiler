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

    bool operator==(const ScopeError &other) const {
        std::cout << "COMPARING ERRORS" << std::endl;
        std::string a = this->what();
        std::string b = other.what();
        bool t = a == b;
        return t;
    }
};

class RedeclareError : public std::runtime_error {
public:
    RedeclareError(std::string name) : runtime_error("Name " + name + "already declared in current scope") {
    }

    bool operator==(const RedeclareError &other) const {
        std::string a = this->what();
        std::string b = other.what();
        return a == b;
    }
};

class SecondPass {
    SymbolTable* scope;
public:
    SecondPass(SymbolTable* globals) {
        this->scope = globals;
        this->scopes["global"] = this->scope;
    }

    void enter_scope(std::string name) {
        std::string new_scope_name = this->scope->name + "." + name;
        this->scope = new SymbolTable(new_scope_name, this->scope);
        this->scopes[new_scope_name] = this->scope;
    }

    void leave_scope() {
        this->scope = this->scope->parent;
    }

    void analyze(FunctionNode* n) {
        this->enter_scope(n->name);
        for (int i = 0; i < n->parameter_names.size(); i++) {
            SymbolInfo* sinfo = new SymbolInfo();
            sinfo->type = SINFO::SIMPLE;
            sinfo->simple_info = new SimpleInfo(n->parameter_types[i]);
            this->scope->set(n->parameter_names[i], sinfo);
        }
        this->analyze(n->body);
        this->leave_scope();
    }

    void analyze(IdentifierNode* n) {
        if (!this->scope->has(n->name)) {
            throw ScopeError(n->name);
        }
    }

    void analyze(DeclarationNode* n) {
        if (this->scope->declared(n->identifier)) {
            throw RedeclareError(n->identifier);
        }
        this->scope->set(n->identifier, NULL);
    }

    void analyze(IfNode* n) {
        this->analyze(n->condition);
        this->enter_scope("if");
        this->analyze(n->then);
        this->leave_scope();
    }

    void analyze(BinopNode* n) {
        this->analyze(n->left);
        this->analyze(n->right);
    }

    void analyze(ReturnNode* n) {
        this->analyze(n->expression);
    }

    void analyze(ClassNode* n) {
//        for (int i = 0; i < n->fields.size(); i++) {
//            this->analyze(n->fields[i]);
//        }
        for (int i = 0; i < n->methods.size(); i++) {
            this->enter_scope(n->methods[i]->name);
            SymbolInfo* this_info = new SymbolInfo;
            this_info->type = SINFO::SIMPLE;
            TypeNode* type_node = new TypeNode(n->name, {});

            SimpleInfo* simple_info = new SimpleInfo(type_node);
            this_info->simple_info = simple_info;
            this->scope->set("this", this_info);
            this->leave_scope();
            this->analyze(n->methods[i]);
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
            case AstType::IF:
                this->analyze(n->ast_if);
                break;
            case AstType::RETURN:
                this->analyze(n->ast_return);
                break;
            case AstType::LIST:
                break;
            case AstType::CLASS:
                this->analyze(n->ast_class);
                break;
            case AstType::MEMBER:
                break;
            case AstType::ASSIGNMENT:
                break;
            case AstType::BINOP:
                this->analyze(n->ast_binop);
                break;
            case AstType::DECLARATION:
                this->analyze(n->ast_declaration);
                break;
            case AstType::TYPE:
                break;
            case AstType::NUMBER:
                break;
        }
    }

    void analyze(VectorOfNodes program) {
        for (auto n: program) {
            this->analyze(n);
        }
    }

    std::map<std::string, SymbolTable*> scopes;
};

#endif //UNTITLED1_SECONDPASS_H
