//
// Created by chris on 28/6/20.
//

#ifndef CHECKER_H
#define CHECKER_H

#include <unordered_map>
#include "GlobalProcessor.h"
#include "ScopeError.h"
#include "RedeclareError.h"
#include "ReturnError.h"
#include "BadArguments.h"
#include "AssignmentTypeError.h"
#include "TypeClassInfo.h"
#include "../utils.h"
#include "../nodes/nodes.h"

typedef std::unique_ptr<SymbolInfo> USymbolInfo;

bool type_matches(TypeNode* a, TypeNode* b);

bool is_generic(const TypeNode& t);

std::unordered_map<std::string, TypeNode*> make_replacements(TypeNode* a, TypeNode* b);
TypeNode*
make_type_from_object_pattern(const ObjectTypeNode& object_type,
                              const std::unordered_map<std::string, TypeNode*>& replacements);
TypeNode* make_type_from_function_pattern(const FunctionTypeNode& ftn,
                                          const std::unordered_map<std::string, TypeNode*>& replacements);
TypeNode* make_type(const TypeNode& original, const std::unordered_map<std::string, TypeNode*>& replacements);

class Checker {
    SymbolTable* scope;
    std::unordered_map<std::string, TypeClassInfo*> typeclasses;
    ClassTable* class_table;
    Node* replacement;
public:
    std::unordered_map<std::string, SymbolTable*> scopes;

    Checker(SymbolTable* globals, ClassTable* class_table, FunctionTable* function_table);

    ~Checker() {
        for (auto s: this->scopes) {
            if (s.first == "global") {
                continue;
            }
            delete s.second;
        }
    }

    void enter_scope(std::string name);

    void leave_scope();

    USymbolInfo visit(AssignmentNode& n);

    USymbolInfo visit(BinopNode& node);

    USymbolInfo visit(CallNode& node);

    USymbolInfo visit(DeclarationNode& node);

    USymbolInfo visit(FunctionNode& node);

    USymbolInfo visit(IdNode& node);

    USymbolInfo visit(IfNode& node);

    USymbolInfo visit(ListNode& node);

    USymbolInfo visit(MemberNode& node);

    USymbolInfo visit(NumberNode& node);

    USymbolInfo visit(ReturnNode& n);

    USymbolInfo visit(StringNode& node);

    USymbolInfo visit(TupleNode& node);

    USymbolInfo visit(SubscriptNode& node);

    USymbolInfo visit(BlockNode& node);

    USymbolInfo visit(ClassLiteralExpressionNode& node);

    USymbolInfo visit(ClassLiteralFieldNode& node);

    USymbolInfo visit(ForNode& node);

    USymbolInfo visit(BooleanNode& node);

    USymbolInfo visit(WhileNode& node);

    USymbolInfo visit(BreakNode& node);

    USymbolInfo visit(TernaryNode& node);

    USymbolInfo visit(NoneNode& node);


    bool can_assign(const TypeNode& from, const TypeNode& to);

    USymbolInfo visit(EmptyListNode& node);

    bool type_exists(TypeNode& type);

    bool can_assign_generic(TypeNode& from, TypeNode& to, std::vector<std::string> type_params);


    ClassInfo* instantiate_generic(ClassInfo* generic, const ObjectTypeNode& instance);

    FunctionTable* function_table;

    USymbolInfo visit(ClassNode& node);

    TypeClassInfo* get_typeclass_for_function(std::string function_name);

    bool replace_me;
    SymbolInfo match_arguments_to_generic_function(const FunctionTypeNode& function_type, VectorOfTypes arg_types);


    USymbolInfo visit(ContinueNode& node);


    USymbolInfo dispatch(Node* nod);
    bool add_this;
    TypeNode* this_type;
};

#endif //CHECKER_H
