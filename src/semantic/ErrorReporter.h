//
// Created by chris on 12/4/21.
//

#ifndef XLANG_ERRORREPORTER_H
#define XLANG_ERRORREPORTER_H


#include <string>
#include "../scanner/TextPosition.h"
#include "../nodes/TypeNode.h"
#include "../scanner/CodeLines.h"
#include "../units/Entity.h"

#define FAIL_FIRST 0

enum class ErrorElement {
    BinopLeft, BinopRight, BinopOperator,
};

class ErrorReporter {
public:
    ErrorReporter();

    CodeLines code_lines;
    void assignment(const TypeNode& expected, const TypeNode& actual, TextPosition pos, const Node& lvalue,
                    const Node& rvalue);
    void bad_return(TextPosition pos);
    void binop(const TypeNode& left, const TypeNode& right, TextPosition position);
    void bool_op(const TypeNode& left, const TypeNode& right, TextPosition position);
    void condition(const TypeNode& t, TextPosition position, const std::string& st);
    void _for(const TypeNode& t, TextPosition position);
    void function_call_num_args(TextPosition position);
    void
    function_call_type_mismatch(const TypeNode& expected, const Node& arg, const TypeNode& actual, TextPosition pos,
                                TextPosition end);
    void no_member(const TypeNode& t, const std::string& member, TextPosition pos);
    void no_return(const TypeNode& t, TextPosition pos);
    void class_no_method(const std::string& class_name, const std::string method_name, TextPosition pos);
    void return_mismatch(const TypeNode& expected, const TypeNode& actual, TextPosition pos);
    bool failed;
    std::string context_string(TextPosition position);
    std::string code_context_string(TextPosition position);
    std::string code_string(TextPosition start, TextPosition end);
    void generic_call_mismatch(const TypeNode& expected, const TypeNode& actual, int i);

    void call_bad_num_args();
    void redeclared(const std::string& name, const DeclarationNode& node);
    void tuple_assign(TextPosition pos);
    void unused_return_value(TextPosition pos);
    void variable_not_declared(const std::string& name, TextPosition pos);
    void function_no_member(TextPosition pos);
    void subscript_non_object(TextPosition pos);
    void string_immutable(TextPosition pos);

    void tuple_member_not_immutable(const TypeNode& t, TextPosition pos);
    void
    class_init_bad_member_type(const TypeNode& cls, const TypeNode& expected, const TypeNode& actual, TextPosition pos);
    void class_not_found(const TypeNode& cls, TextPosition pos);
    void list_literal(const TypeNode& lt, const TypeNode& et, TextPosition pos, const Node& ell);
    void function_return_last_stmt(const std::string& function_name, const TypeNode& et, TextPosition pos);
    void partial_wrong_num_args(const std::string& function_name, const TypeNode& et, TextPosition pos);
    void partial_wrong_num_args(TextPosition pos);
    void partial_function_call_type_mismatch(const TypeNode& expected, const TypeNode& actual, TextPosition pos,
                                             TextPosition end);
    void generic_class_wrong_type_param_number(const std::string& cls, int num_req, int num_given, TextPosition pos);
    void class_init_wrong_number_init(const std::string& cls, int num_req, int num_given, TextPosition pos);
    void subscript_type(const TypeNode& t, const TypeNode& s, const SubscriptNode& n);
    void class_not_generic(const std::string& cls, TextPosition pos);
    void function_doesnt_return_a_value(TextPosition pos, const TypeNode* expected_type);
    void class_init_member_not_init(const std::string& cls, std::string mem, TextPosition pos);
    void fail(std::string msg, TextPosition pos);

    void fail(std::string msg) {
        this->fail(msg, TextPosition{1, 1});
    }

    void method_not_member(const TypeNode& t, const std::string& member, TextPosition pos);
    std::string code_error_string(TextPosition start, TextPosition end);

    std::string current_class;
    std::string current_function;
    std::string __file__;

    void function_call_num_args(FunctionType& ft, TextPosition pos);
    void match_type(ObjectType& type, TextPosition pos);
    void match_type(Entity entity, TextPosition pos);
    void condition(Entity entity, TextPosition pos, const std::string& st);
    void expected_expression_with_type(Entity entity, TypeNode& exp_entity, TextPosition pos);
    void expected_expression(Entity entity, const Node& pos);
    void _for(Entity t, TextPosition pos);
    void bool_op(Entity left, Entity right, TextPosition pos);
    void binop(Entity left, Entity right, TextPosition pos, Node* left_n, Node* right_n);
    void fail_highlight(std::string msg);
    void fail_ok(std::string pre_msg,std::string msg, TextPosition pos);
    void call_not_a_function(const CallNode& node);
    std::string highlight_two(ErrorElement fe, const Node& f, ErrorElement se, const Node& s);
    std::string highlight_one(const Node& f);
    void entity_no_member(std::string pre_msg, const std::string& member, TextPosition pos, Node& obj,
                          TextPosition member_start, TextPosition member_end);
    void class_no_member(const TypeNode& t, const std::string& member, TextPosition pos, Node& obj,
                         TextPosition member_start, TextPosition member_end);
    void module_no_member(const TypeNode& t, const std::string& member, TextPosition pos, Node& obj,
                          TextPosition member_start, TextPosition member_end);
    void object_no_member(const TypeNode& t, const MemberNode& obj);
    void module_no_member(std::string mod_name, const std::string& member, TextPosition pos, Node& obj,
                          TextPosition member_start, TextPosition member_end);
    void package_no_member(std::string pack_name, const std::string& member, TextPosition pos, Node& obj,
                           TextPosition member_start, TextPosition member_end);
    void error_type_mismatch(const TypeNode& expected, const Node& value_node, const TypeNode& actual);
    void class_no_method_for_op(std::string class_name, std::string method_name, TextPosition position);
    void enum_no_value(std::string enum_name, std::string value, MemberNode& node, Enum* enumm);
    void object_no_member_with_suggestions(const TypeNode& t, const std::string& member, TextPosition pos, Node& obj,
                                           TextPosition member_start, TextPosition member_end, Class* clazz);
    void
    module_no_member(Module* mod, const std::string& member, TextPosition pos, Node& obj, TextPosition member_start,
                     TextPosition member_end);
    void
    package_no_member(Package* pack, const std::string& member, TextPosition pos, Node& obj, TextPosition member_start,
                      TextPosition member_end);
    void object_no_special_method(const TypeNode& type, const char* method_name, const SubscriptNode& node);
};


#endif //XLANG_ERRORREPORTER_H
