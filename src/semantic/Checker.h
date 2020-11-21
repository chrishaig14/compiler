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


bool type_matches(TypeNode* a, TypeNode* b);

bool is_generic(const TypeNode& t);

std::map<std::string, TypeNode*> make_replacements(TypeNode* a, TypeNode* b);

TypeNode* make_type(const TypeNode& original, std::map<std::string, TypeNode*> replacements);

class Checker {
    SymbolTable* scope;
    std::map<std::string, TypeClassInfo*> typeclasses;
    ClassTable* class_table;
    Node* replacement;
public:
    std::map<std::string, SymbolTable*> scopes;

    Checker(SymbolTable* globals, ClassTable* class_table, FunctionTable* function_table);

    void enter_scope(std::string name);

    void leave_scope();

    SymbolInfo visit(AssignmentNode& n);

    SymbolInfo visit(BinopNode& node);

    SymbolInfo visit(CallNode& node);

    SymbolInfo visit(DeclarationNode& node);

    SymbolInfo visit(FunctionNode& node);

    SymbolInfo visit(IdNode& node);

    SymbolInfo visit(IfNode& node);

    SymbolInfo visit(ListNode& node);

    SymbolInfo visit(MemberNode& node);

    SymbolInfo visit(NumberNode& node);

    SymbolInfo visit(ReturnNode& n);

    SymbolInfo visit(StringNode& node);

    SymbolInfo visit(SubscriptNode& node);

    SymbolInfo visit(BlockNode& node);

    SymbolInfo visit(ClassLiteralExpressionNode& node);

    SymbolInfo visit(ClassLiteralFieldNode& node);

    SymbolInfo visit(ForNode& node);

    SymbolInfo visit(BooleanNode& node);

    SymbolInfo visit(WhileNode& node);

    SymbolInfo visit(BreakNode& node);

    SymbolInfo visit(TernaryNode& node);

    SymbolInfo visit(NoneNode& node);


    bool can_assign(const TypeNode& from, const TypeNode& to);

    SymbolInfo visit(EmptyListNode& node);

    bool type_exists(TypeNode& type);

    bool can_assign_generic(TypeNode& from, TypeNode& to, std::vector<std::string> type_params);


    ClassInfo* instantiate_generic(ClassInfo* generic, const ObjectTypeNode& instance);

    FunctionTable* function_table;

    SymbolInfo visit(ClassNode& node);

    TypeClassInfo* get_typeclass_for_function(std::string function_name);

    bool replace_me;
    SymbolInfo match_arguments_to_generic_function(const FunctionTypeNode& function_type, VectorOfTypes arg_types);


    SymbolInfo visit(ContinueNode& node);


    SymbolInfo dispatch(Node* nod);
};

#endif //CHECKER_H
