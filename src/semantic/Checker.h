//
// Created by chris on 28/6/20.
//

#ifndef CHECKER_H
#define CHECKER_H

#include <map>
#include "GlobalProcessor.h"
#include "ScopeError.h"
#include "RedeclareError.h"
#include "ReturnError.h"
#include "BadArguments.h"
#include "AssignmentTypeError.h"
#include "TypeClassInfo.h"
#include "../utils.h"
#include "../nodes/nodes.h"

class SymbolInfo {
    const TypeNode* _type;
public:
    bool is_function;
    bool is_method;
    ClassInfo* class_info;
    bool is_class_method;

    SymbolInfo() {
        this->is_function = false;
        this->is_method = false;
        this->class_info = nullptr;
        this->is_class_method = false;
    }

    void set_type(const TypeNode& typ) {
        this->_type = typ.clone();
    }

    const TypeNode& type() {
        if (_type == nullptr) {
            throw std::runtime_error("SymbolInfo has no TypeNode");
        }
        return *this->_type;
    }
};

bool type_matches(TypeNode* a, TypeNode* b);

bool is_generic(const TypeNode& t);

std::map<std::string, TypeNode*> make_replacements(TypeNode* a, TypeNode* b);

TypeNode* make_type(const TypeNode* o, std::map<std::string, TypeNode*> replacements);

class Checker : public Visitor {
    SymbolTable* scope;
    std::map<std::string, TypeClassInfo*> typeclasses;
    ClassTable* class_table;
    SymbolInfo rv;
    Node* replacement;
public:
    std::map<std::string, SymbolTable*> scopes;

    Checker(SymbolTable* globals, ClassTable* class_table, FunctionTable* function_table);

    void enter_scope(std::string name);

    void leave_scope();

    void visit(AssignmentNode& node) override;

    void visit(BinopNode& node) override;

    void visit(CallNode& node) override;

    void visit(DeclarationNode& node) override;

    void visit(FunctionNode& node) override;

    void visit(IdNode& node) override;

    void visit(IfNode& node) override;

    void visit(ListNode& node) override;

    void visit(MemberNode& node) override;

    void visit(NumberNode& node) override;

    void visit(ReturnNode& node) override;

    void visit(StringNode& node) override;

    void visit(SubscriptNode& node) override;

    void visit(BlockNode& node) override;

    void visit(ClassLiteralExpressionNode& node) override;

    void visit(ClassLiteralFieldNode& node) override;

    void visit(ForNode& node) override;

    void visit(BooleanNode& node) override;

    void visit(WhileNode& node) override;

    void visit(BreakNode& node) override;

    void visit(TernaryNode& node) override;

    void visit(NoneNode& node) override;


    bool can_assign(const TypeNode& from, const TypeNode& to);

    void visit(EmptyListNode& node) override;

    void check_structs();

    bool type_exists(TypeNode& type);

    bool can_assign_generic(TypeNode& from, TypeNode& to, std::vector<std::string> type_params);


    ClassInfo* instantiate_generic(ClassInfo* generic, const ObjectTypeNode& instance);

    FunctionTable* function_table;

    void visit(ClassNode& node) override;

    void visit(InstanceNode& node) override;

    TypeClassInfo* get_typeclass_for_function(std::string function_name) {
        for (auto typeclass: this->typeclasses) {
            if (typeclass.second->has_function(function_name)) {
                return typeclass.second;
            }
        }
        return nullptr;
    }

    bool replace_me;
    void match_arguments_to_generic_function(const FunctionTypeNode& function_type, VectorOfTypes arg_types);


    void visit(ContinueNode& node) override;


    void dispatch(Node* nod);
};

#endif //CHECKER_H
