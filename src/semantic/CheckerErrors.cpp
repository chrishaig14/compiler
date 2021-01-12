//
// Created by chris on 12/1/21.
//

#include "Checker.h"


void Checker::error_binop(const TypeNode& left, const TypeNode& right, TextPosition position) {
    this->failed = true;
    std::string msg;
    msg = context_string(position) +
          E_FMT("Cannot perform binary op between types ") + E_HLT(left.to_string()) +
          E_FMT(" and ") +
          E_HLT(right.to_string()) + this->code_context_string(position);
    std::cout << msg << std::endl;
}

void Checker::error_no_member(const TypeNode& t, const std::string& member, TextPosition position) {
    this->failed = true;
    std::string msg;
    msg =
            this->context_string(position) +
            E_FMT("Type ") + E_HLT(t.to_string()) +
            E_FMT(" has no member ") +
            E_HLT("'" + member + "'") +
            this->code_context_string(position);
    std::cout << msg << std::endl;
}

void Checker::error_bool_op(const TypeNode& left, const TypeNode& right, TextPosition position) {
    this->failed = true;
    std::string msg;
    msg = context_string(position) +
          E_FMT("Cannot perform bool op between types ") + E_HLT(left.to_string()) +
          E_FMT(" and ") +
          E_HLT(right.to_string());
    std::cout << msg << std::endl;
}

void Checker::error_assignment(const TypeNode& expected, const TypeNode& actual, TextPosition position) {
    this->failed = true;
    std::string msg;
    msg = context_string(position) +
          E_FMT("Expected ") +
          E_HLT(expected.to_string()) +
          E_FMT(", got ") +
          E_HLT(actual.to_string()) +
          this->code_error_string(position, position);
    std::cout << msg << std::endl;
}

void Checker::error_condition(const TypeNode& t, TextPosition position, const std::string& st) {
    this->failed = true;
    std::string msg;
    msg = context_string(position) +
          E_FMT(" Expected ") + E_HLT("Boolean ") +
          E_FMT("as condition for " + st + " statement, got ") +
          E_HLT(t.to_string());
    std::cout << msg << std::endl;
}

void Checker::error_no_return(const TypeNode& t, TextPosition position) {
    this->failed = true;
    std::string msg;
    msg = context_string(position) + E_FMT(" Expected to return ") +
          E_HLT(t.to_string()) +
          E_FMT(" but not returning anything");
    std::cout << msg << std::endl;
}

void Checker::error_bad_return(TextPosition position) {
    this->failed = true;
    std::string msg;
    msg = E_HLT(text_pos_to_string(this->__file__, position)) +
          E_FMT(" Returning a value from a function returning no value ");
    std::cout << msg << std::endl;
}

void Checker::error_return_mismatch(const TypeNode& expected, const TypeNode& actual, TextPosition position) {
    this->failed = true;
    std::string msg;
    msg = E_HLT(text_pos_to_string(this->__file__, position)) +
          E_FMT(" In function ") +
          E_HLT((this->current_class == "" ? "" : this->current_class + ".") + this->current_function) + E_FMT(": ") +
          E_FMT(" Expected to return ") + E_HLT(expected.to_string()) + E_FMT(" but got ") + E_HLT(actual.to_string());
    std::cout << msg << std::endl;
}

void Checker::error_tuple_assign(TextPosition pos) {
    std::string msg =
            this->context_string(pos) +
            E_FMT("Error: can't reassign a member of a tuple!") +
            this->code_context_string(pos);
    std::cout << msg << std::endl;
}

void Checker::error_subscript_non_object(TextPosition pos) {
    std::string msg;
    msg = this->context_string(pos) + E_FMT("Accessing subscript of non object") + this->code_context_string(pos);
    std::cout << msg << std::endl;
    this->failed = true;
}

void Checker::error_string_immutable(TextPosition pos) {
    std::string msg;
    msg = this->context_string(pos) + E_FMT("Strings are immutable") + this->code_context_string(pos);
    std::cout << msg << std::endl;
    this->failed = true;
}

void Checker::error_for(const TypeNode& t, TextPosition position) {
    this->failed = true;
    std::string msg;
    msg = E_HLT(text_pos_to_string(this->__file__, position)) +
          E_FMT("Expected") + E_HLT(" List[t] ") +
          E_FMT("in loop, but got ") +
          E_HLT(t.to_string());
    std::cout << msg << std::endl;
}

void Checker::error_tuple_member_not_immutable(const TypeNode& t, TextPosition pos) {
    std::string msg;
    msg = this->context_string(pos) + E_FMT("Tuple member not immutable, it's of type ") + E_HLT(t.to_string()) +
          this->code_context_string(pos);
    std::cout << msg << std::endl;
    this->failed = true;
}

void Checker::error_generic_call_mismatch(const TypeNode& expected, const TypeNode& actual, int i) {
    std::string msg =
            E_FMT("Error matching argument number " + std::to_string(i) + " expected ") + E_HLT(expected.to_string()) +
            E_FMT(" got ") + E_HLT(actual.to_string()) + "\n";
    std::cout << msg << std::endl;
}

void Checker::error_call_bad_num_args() {
    std::string msg = "Function call with wrong number of arguments!";
    std::cout << msg << std::endl;
}

void Checker::error_member_no_object(TextPosition pos) {
    std::string msg;
    msg = this->context_string(pos) + E_FMT("Accessing member of non object ") + this->code_context_string(pos);
    std::cout << msg << std::endl;
    this->failed = true;
}

void Checker::error_class_no_method(const std::string& class_name, const std::string method_name, TextPosition pos) {
    std::string msg;
    msg = this->context_string(pos) + E_FMT("Class ") + E_HLT(class_name) + E_FMT(" has no method ") +
          E_HLT(method_name) + this->code_context_string(pos);
    std::cout << msg << std::endl;
    this->failed = true;
}

void Checker::error_redeclared(const std::string& name, TextPosition pos) {
    std::string msg;
    msg = this->context_string(pos) + E_FMT("Variable ") + E_HLT(name) + E_FMT(" already declared ") +
          this->code_context_string(pos);
    std::cout << msg << std::endl;
}

void Checker::error_variable_not_declared(const std::string& name, TextPosition pos) {
    std::string msg;
    msg = this->context_string(pos) +
          E_FMT("Variable ") +
          E_HLT("'" + name + "'") +
          E_FMT(" not declared") +
          this->code_context_string(pos);
    std::cout << msg << std::endl;
    this->failed = true;
}

void
Checker::error_function_call_type_mismatch(const TypeNode& expected, const TypeNode& actual, TextPosition start,
                                           TextPosition end) {
    this->failed = true;
    std::string msg;
    msg = context_string(start) +
          E_FMT(" Function call type mismatch") +
          E_FMT(" expected ") + E_HLT(expected.to_string()) + E_FMT(" but got ") + E_HLT(actual.to_string()) +
          this->code_error_string(start, end);
    std::cout << msg << std::endl;
}

void Checker::error_unused_return_value(TextPosition pos) {
    std::string msg;
    msg = this->context_string(pos) +
          E_FMT("Unused return value of function call") + this->code_context_string(pos);
    std::cout << msg << std::endl;
    this->failed = true;
}

void Checker::error_function_call_num_args(TextPosition position) {
    this->failed = true;
    std::string msg;
    msg = context_string(position) +
          E_FMT("Calling function with wrong number of arguments ");
    std::cout << msg << std::endl;
}

void Checker::error_call_not_a_function(TextPosition position) {
    this->failed = true;
    std::string msg;
    msg = E_HLT(text_pos_to_string(this->__file__, position)) + E_FMT("Calling something that's not a function");
    std::cout << msg << std::endl;
}