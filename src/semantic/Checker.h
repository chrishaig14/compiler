//
// Created by chris on 28/6/20.
//

#ifndef UNTITLED1_CHECKER_H
#define UNTITLED1_CHECKER_H

#include <map>
#include "GlobalProcessor.h"
#include "ScopeError.h"
#include "RedeclareError.h"
#include "ReturnError.h"
#include "BadArguments.h"
#include "AssignmentTypeError.h"
#include "TypeClassInfo.h"

//typedef std::map<std::string, ObjectInfo> MapStringToSimple;
//typedef std::map<std::string, FunctionInfo> MapStringToFunction;


struct SymbolInfo {
    TypeNode* symbol_info;
    bool is_a_function;
    FunctionOverloads overloads;
    bool is_overloaded;
};

bool type_matches(TypeNode* a, TypeNode* b);

bool is_generic(TypeNode* t);

std::map<std::string, TypeNode*> make_replacements(TypeNode* a, TypeNode* b);

TypeNode* make_type(TypeNode* original, std::map<std::string, TypeNode*>& replacements);

class Checker : public Visitor {
    SymbolTable* scope;
    std::map<std::string, TypeClassInfo*> typeclasses;
    ClassTable* class_table;
    SymbolInfo rv;
public:
    std::map<std::string, SymbolTable*> scopes;

    Checker(SymbolTable* globals, ClassTable* class_table);

    void enter_scope(std::string name);

    void leave_scope();

    void visit(AssignmentNode& node) override;

    void visit(BinopNode& node) override;

    void visit(CallNode& node) override;

    void visit(StructNode& node) override;

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

    void visit(TypeNode& node) override {}

    void visit(BlockNode& node) override;

    void visit(ClassLiteralExpressionNode& node) override;

    void visit(ClassLiteralFieldNode& node) override;

    void visit(ForNode& node) override;

    void visit(BooleanNode& node) override;

    void visit(WhileNode& node) override;

    void visit(BreakNode& node) override;

    void visit(TernaryNode& node) override;

    void visit(NoneNode& node) override;


    bool can_assign(TypeNode* from, TypeNode* to);

    void visit(EmptyListNode& node) override;

    void check_structs();

    std::vector<std::string> type_params;

    bool type_exists(TypeNode* type);

    bool can_assign_generic(TypeNode* from, TypeNode* to, std::vector<std::string> type_params);


    ClassInfo* instantiate_generic(ClassInfo* generic, ObjectTypeNode* instance);

    FunctionTable* function_table;

    SymbolInfo visit_overloaded_function_call(CallNode& n, std::string func_name, VectorOfTypes& args);

    std::vector<SymbolInfo> analyze_arguments(VectorOfNodes& arguments);

    SymbolInfo* visit_local_function_call(FunctionTypeNode& function, VectorOfTypes& args);

    SymbolInfo* visit_call_global_function(FunctionTypeNode& ft, VectorOfTypes& args);

    SymbolInfo* visit_non_generic_function_call(FunctionTypeNode& function, VectorOfTypes& args);

    SymbolInfo* visit_generic_function_call(FunctionTypeNode& function, VectorOfTypes& args);

    std::vector<std::pair<VectorOfTypes, VectorOfNodes>> make_combinations(VectorOfNodes args);

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
};

#endif //UNTITLED1_CHECKER_H
