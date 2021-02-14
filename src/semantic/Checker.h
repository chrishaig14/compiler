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
#include <iostream>
#include <set>
#include "TypeClassInfo.h"
#include "../macros.h"
#include "../logging/logging.h"

typedef std::unique_ptr<SemanticInfo> USemanticInfo;

bool type_matches(TypeNode* a, TypeNode* b);

bool is_generic(const TypeNode& t);

MapStringType make_replacements(TypeNode* a, TypeNode* b);
TypeNode* make_type_from_object_pattern(const ObjectType& object_type,
                                        const MapStringType& replacements);
TypeNode* make_type_from_function_pattern(const FunctionType& ftn,
                                          const MapStringType& replacements);
TypeNode* make_type(const TypeNode& original, const MapStringType& replacements);
SemanticInfo match_arguments_to_generic_function(const FunctionType& function_type, VectorOfTypes arg_types);

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
    std::vector<ObjectType*> tuple_types;
    SymbolTable* scope;
    TypeNode* this_type;
public:
    std::string __file__;

    bool can_assign(const TypeNode& from, const TypeNode& to);
    bool can_assign_generic(TypeNode& from, TypeNode& to, VectorOfStrings type_params);
    bool is_immutable(const TypeNode& node);
    Checker(SymbolTable* globals, ClassTable* class_table, FunctionTable* function_table);
    ClassInfo* instantiate_generic(ClassInfo* generic, const ObjectType& instance);
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
    USemanticInfo dispatch(Node* nod);
    USemanticInfo visit(AssignmentNode& n);
    USemanticInfo visit(BinopNode& node);
    USemanticInfo visit(BlockNode& node);
    USemanticInfo visit(BooleanNode& node);
    USemanticInfo visit(BoolOpNode& n);
    USemanticInfo visit(BreakNode& node);
    USemanticInfo visit(CallNode& node);
    USemanticInfo visit(ClassLiteralExpressionNode& node);
    USemanticInfo visit(ClassLiteralFieldNode& node);
    USemanticInfo visit(ClassNode& node);
    USemanticInfo visit(ContinueNode& node);
    USemanticInfo visit(DeclarationNode& node);
    USemanticInfo visit(DictNode& node);
    USemanticInfo visit(EmptyDictNode& node);
    USemanticInfo visit(EmptyListNode& node);
    USemanticInfo visit(FloatNode& node);
    USemanticInfo visit(ForNode& node);
    USemanticInfo visit(FunctionNode& node);
    USemanticInfo visit(IdNode& node);
    USemanticInfo visit(IfNode& node);
    USemanticInfo visit(ListNode& node);
    USemanticInfo visit(MemberNode& node);
    USemanticInfo visit(NoneNode& node);
    USemanticInfo visit(NumberNode& node);
    USemanticInfo visit(PartialApplication& node);
    USemanticInfo visit(ReturnNode& n);
    USemanticInfo visit(StringNode& node);
    USemanticInfo visit(SubscriptNode& node);
    USemanticInfo visit(TernaryNode& node);
    USemanticInfo visit(TupleNode& node);
    USemanticInfo visit(WhileNode& node);
    VectorOfTypes get_replacements_in_order(const FunctionType& function_type, VectorOfTypes arg_types);
    void enter_scope(std::string name);
    void leave_scope();
    ~Checker();
    bool assert_type_exists(TypeNode& type, TextPosition pos);
    bool failed;
    std::string context_string(TextPosition position);
    CodeLines code_lines;
    std::string code_context_string(TextPosition position);
    std::string code_error_string(TextPosition start, TextPosition end);

    bool is_variable(const ObjectType& a);
    std::pair<std::string, TypeNode*>*
    get_first_substitution_function(FunctionType& a, FunctionType& b, bool is_top_level_arg);
    std::pair<std::string, TypeNode*>*
    get_first_substitution_object(ObjectType& a, ObjectType& b, bool is_top_level_arg);
    std::pair<std::string, TypeNode*>* get_first_substitution(TypeNode& a, TypeNode& b, bool is_top_level_arg);
    TypeNode* substitute(TypeNode* t, std::string var, TypeNode* replacement);
    void unify_function_call(FunctionType& fun, VectorOfTypes& args);
    SemanticInfo match_arguments_to_generic_function(const FunctionType& ft, VectorOfTypes arg_types);
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
    void error_tuple_member_not_immutable(const TypeNode& t, TextPosition pos);
    void error_class_init_bad_member_type(const TypeNode& cls, const TypeNode& expected, const TypeNode& actual,
                                          TextPosition pos);
    void error_class_not_found(const TypeNode& cls, TextPosition pos);
    void error_list_literal(const TypeNode& lt, const TypeNode& et, TextPosition pos);
    void error_function_return_last_stmt(const std::string& function_name, const TypeNode& et, TextPosition pos);
    USemanticInfo error();
    void error_partial_wrong_num_args(const std::string& function_name, const TypeNode& et, TextPosition pos);
    void error_partial_wrong_num_args(TextPosition pos);
    void error_partial_function_call_type_mismatch(const TypeNode& expected, const TypeNode& actual, TextPosition start,
                                                   TextPosition end);
    Node* replace_if_necessary(Node* node);
    void
    error_generic_class_wrong_type_param_number(const std::string& cls, int num_req, int num_given, TextPosition pos);
    void error_class_init_wrong_number_init(const std::string& cls, int num_req, int num_given, TextPosition pos);
    void error_subscript_type(const TypeNode& t, const TypeNode& s, const TypeNode& es, TextPosition pos);
    void error_class_not_generic(const std::string& cls, TextPosition pos);
    bool is_method;
    void error_function_doesnt_return_a_value(TextPosition position, const TypeNode* expected_type);
};

bool function_is_generic(const FunctionType& ft);

#endif //CHECKER_H
