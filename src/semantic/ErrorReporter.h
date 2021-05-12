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

class ErrorReporter {
public:
    ErrorReporter();

    CodeLines code_lines;
    void assignment(const TypeNode& expected, const TypeNode& actual, TextPosition pos);
    void bad_return(TextPosition pos);
    void binop(const TypeNode& left, const TypeNode& right, TextPosition position);
    void bool_op(const TypeNode& left, const TypeNode& right, TextPosition position);
    void call_not_a_function(TextPosition pos);
    void condition(const TypeNode& t, TextPosition position, const std::string& st);
    void _for(const TypeNode& t, TextPosition position);
    void function_call_num_args(TextPosition position);
    void
    function_call_type_mismatch(const TypeNode& expected, const TypeNode& actual, TextPosition pos, TextPosition end);
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
    void redeclared(const std::string& name, TextPosition pos);
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
    void list_literal(const TypeNode& lt, const TypeNode& et, TextPosition pos);
    void function_return_last_stmt(const std::string& function_name, const TypeNode& et, TextPosition pos);
    void partial_wrong_num_args(const std::string& function_name, const TypeNode& et, TextPosition pos);
    void partial_wrong_num_args(TextPosition pos);
    void partial_function_call_type_mismatch(const TypeNode& expected, const TypeNode& actual, TextPosition pos,
                                             TextPosition end);
    void generic_class_wrong_type_param_number(const std::string& cls, int num_req, int num_given, TextPosition pos);
    void class_init_wrong_number_init(const std::string& cls, int num_req, int num_given, TextPosition pos);
    void subscript_type(const TypeNode& t, const TypeNode& s, const TypeNode& es, TextPosition pos);
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


    void object_no_member(const TypeNode& t, const std::string& member, TextPosition pos);
    void class_no_member(const TypeNode& t, const std::string& member, TextPosition pos);
    void module_no_member(const std::string& module_name, const std::string& member, TextPosition pos);
    void package_no_member(const std::string& package_name, const std::string& member, TextPosition pos);
    void function_call_num_args(FunctionType& ft, TextPosition pos);
    void match_type(ObjectType& type, TextPosition pos);
    void match_type(Entity entity, TextPosition pos);
    void condition(Entity entity, TextPosition pos, const std::string& st);
    void expected_expression_with_type(Entity entity, TypeNode& exp_entity, TextPosition pos);
    void expected_expression(Entity entity, TextPosition pos);
    void _for(Entity t, TextPosition pos);
    void bool_op(Entity left, Entity right, TextPosition pos);
    void binop(Entity left, Entity right, TextPosition pos);
};


#endif //XLANG_ERRORREPORTER_H
