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
#include "TypeClassInfo.h"
#include "../utils.h"
#include "../nodes/nodes.h"
#include <set>
#include "../scanner/CodeLines.h"

typedef std::unique_ptr<SymbolInfo> USymbolInfo;

bool type_matches(TypeNode* a, TypeNode* b);

bool is_generic(const TypeNode& t);

std::unordered_map<std::string, TypeNode*> make_replacements(TypeNode* a, TypeNode* b);
TypeNode* make_type_from_object_pattern(const ObjectTypeNode& object_type,
                                        const std::unordered_map<std::string, TypeNode*>& replacements);
TypeNode* make_type_from_function_pattern(const FunctionTypeNode& ftn,
                                          const std::unordered_map<std::string, TypeNode*>& replacements);
TypeNode* make_type(const TypeNode& original, const std::unordered_map<std::string, TypeNode*>& replacements);
SymbolInfo match_arguments_to_generic_function(const FunctionTypeNode& function_type, VectorOfTypes arg_types);

class Checker {
    bool add_this;
    std::string current_function;
    std::string current_class;
    bool is_lvalue;
    bool replace_me;
    ClassTable* class_table;
    FunctionTable* function_table;
    Node* replacement;
    std::unordered_map<std::string, SymbolTable*> scopes;
    std::unordered_map<std::string, TypeClassInfo*> typeclasses;
    std::vector<ObjectTypeNode*> tuple_types;
    SymbolTable* scope;
    TypeNode* this_type;
public:
    std::string __file__;

    bool can_assign(const TypeNode& from, const TypeNode& to);
    bool can_assign_generic(TypeNode& from, TypeNode& to, std::vector<std::string> type_params);
    bool is_immutable(const TypeNode& node);
    Checker(SymbolTable* globals, ClassTable* class_table, FunctionTable* function_table);
    ClassInfo* instantiate_generic(ClassInfo* generic, const ObjectTypeNode& instance);
    void error_assignment(const TypeNode& expected, const TypeNode& actual, TextPosition position);
    void error_bad_return(TextPosition position);
    void error_binop(const TypeNode& left, const TypeNode& right, TextPosition position);
    void error_bool_op(const TypeNode& left, const TypeNode& right, TextPosition position);
    void error_call_not_a_function(TextPosition position);
    void error_condition(const TypeNode& t, TextPosition position, const std::string& st);
    void error_for(const TypeNode& t, TextPosition position);
    void error_function_call_num_args(TextPosition position);
    void
    error_function_call_type_mismatch(const TypeNode& expected, const TypeNode& actual, TextPosition start,
                                      TextPosition end);
    void error_no_member(const TypeNode& t, const std::string& member, TextPosition position);
    void error_no_return(const TypeNode& t, TextPosition position);
    void error_class_no_method(const std::string& class_name, const std::string method_name, TextPosition pos);
    void error_return_mismatch(const TypeNode& expected, const TypeNode& actual, TextPosition position);
    USymbolInfo dispatch(Node* nod);
    USymbolInfo visit(AssignmentNode& n);
    USymbolInfo visit(BinopNode& node);
    USymbolInfo visit(BlockNode& node);
    USymbolInfo visit(BooleanNode& node);
    USymbolInfo visit(BoolOpNode& n);
    USymbolInfo visit(BreakNode& node);
    USymbolInfo visit(CallNode& node);
    USymbolInfo visit(ClassLiteralExpressionNode& node);
    USymbolInfo visit(ClassLiteralFieldNode& node);
    USymbolInfo visit(ClassNode& node);
    USymbolInfo visit(ContinueNode& node);
    USymbolInfo visit(DeclarationNode& node);
    USymbolInfo visit(EmptyListNode& node);
    USymbolInfo visit(FloatNode& node);
    USymbolInfo visit(ForNode& node);
    USymbolInfo visit(FunctionNode& node);
    USymbolInfo visit(IdNode& node);
    USymbolInfo visit(IfNode& node);
    USymbolInfo visit(ListNode& node);
    USymbolInfo visit(MemberNode& node);
    USymbolInfo visit(NoneNode& node);
    USymbolInfo visit(NumberNode& node);
    USymbolInfo visit(PartialApplication& node);
    USymbolInfo visit(ReturnNode& n);
    USymbolInfo visit(StringNode& node);
    USymbolInfo visit(SubscriptNode& node);
    USymbolInfo visit(TernaryNode& node);
    USymbolInfo visit(TupleNode& node);
    USymbolInfo visit(WhileNode& node);
    VectorOfTypes get_replacements_in_order(const FunctionTypeNode& function_type, VectorOfTypes arg_types);
    void enter_scope(std::string name);
    void leave_scope();
    ~Checker();
    void assert_type_exists(TypeNode& type, TextPosition pos);
    bool failed;
    std::string context_string(TextPosition position);
    CodeLines code_lines;
    std::string code_context_string(TextPosition position);
    std::string code_error_string(TextPosition start, TextPosition end);

    bool is_variable(const ObjectTypeNode& a);
    std::pair<std::string, TypeNode*>*
    get_first_substitution_function(FunctionTypeNode& a, FunctionTypeNode& b, bool is_top_level_arg);
    std::pair<std::string, TypeNode*>*
    get_first_substitution_object(ObjectTypeNode& a, ObjectTypeNode& b, bool is_top_level_arg);
    std::pair<std::string, TypeNode*>* get_first_substitution(TypeNode& a, TypeNode& b, bool is_top_level_arg);
    TypeNode* substitute(TypeNode* t, std::string var, TypeNode* replacement);
    void unify_function_call(FunctionTypeNode& fun, VectorOfTypes& args);
    SymbolInfo match_arguments_to_generic_function(const FunctionTypeNode& ft, VectorOfTypes arg_types);
    void error_generic_call_mismatch(const TypeNode& expected, const TypeNode& actual, int i);

    void error_call_bad_num_args();
    void error_redeclared(const std::string& name, TextPosition pos);
    void error_tuple_assign(TextPosition pos);
    void error_unused_return_value(TextPosition pos);
    void error_variable_not_declared(const std::string& name, TextPosition pos);
    void error_member_no_object(TextPosition pos);
    void error_subscript_non_object(TextPosition pos);
    void error_string_immutable(TextPosition pos);
    void error_member_no_object(const std::string& class_name, const std::string method_name, TextPosition pos);
};

#endif //CHECKER_H
