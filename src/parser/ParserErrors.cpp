//
// Created by chris on 23/1/21.
//

#include "Parser.h"
#include "../logging/logging.h"

std::string Parser::code_context_string(TextPosition position) {
    std::string str = "\n" + this->code_lines.get_line(position.line) + "\n";
    str += fmt::format(fmt::fg(fmt::color::orange_red), std::string(position.column, ' ') + std::string(1, '^'));
    return str;
}

std::string Parser::code_error_string(TextPosition start, TextPosition end) {
    int length = end.column - start.column + 1;
    std::string str = "\n" + this->code_lines.get_line(start.line) + "\n";
    str += fmt::format(fmt::fg(fmt::color::orange_red), std::string(start.column, ' ') + std::string(length, '^'));
    return str;
}

std::string Parser::context_string(TextPosition position) {
    std::string msg = E_HLT(text_pos_to_string(this->__file__, position)) +
                      E_FMT(": ");
    return msg;
}

void Parser::error_after_var(Token tok) {
    std::string msg;
    msg += E_FMT("Got ");
    msg += E_HLT(tok.to_string());
    msg += E_FMT(" expected ");
    msg += E_HLT(TOKEN_STRINGS[TokType::ID]);
    msg += E_FMT(" (new variable name)");
    this->error(msg, tok.start);
}

void Parser::error_after_var_name(Token tok) {
    std::string msg;
    msg += E_FMT("Got ");
    msg += E_HLT(tok.to_string());
    msg += E_FMT(" expected ");
    msg += E_HLT(TOKEN_STRINGS[TokType::COLON]);
    msg += E_FMT(" or ");
    msg += E_HLT(TOKEN_STRINGS[TokType::EQQ]);
    msg += E_FMT(" (new variable type/initial value)");
    this->error(msg, tok.start);
}

void Parser::error_after_var_type(Token tok) {
    std::string msg;
    msg += E_FMT("Got ");
    msg += E_HLT(tok.to_string());
    msg += E_FMT(" expected ");
    msg += E_HLT(TOKEN_STRINGS[TokType::ID]);
    msg += E_FMT(" (new variable initialization)");
    this->error(msg, tok.start);
}

void Parser::error_out_of_loop(Token tok) {
    std::string msg;
    msg += E_FMT("Got ");
    msg += E_HLT(tok.to_string());
    msg += E_FMT(" out of loop ");
    this->error(msg, tok.start);
}

void Parser::error_expected_expression(Token tok) {
    std::string msg;
    msg += E_FMT("Error: expected ");
    msg += E_HLT("expression");
    msg += E_FMT(" but got ");
    msg += E_HLT(tok.to_string());
    msg += E_FMT(" at ");
    msg += E_HLT(this->__file__ + ":" + tok.pos_string());
    msg += E_FMT("\n");
    this->error(msg, tok.start);
}

std::string
Parser::error_after_expression(const std::vector<TokType>& expected_extra, Token tok, TextPosition position) {
    std::string msg;
    msg += E_FMT(text_pos_to_string(this->__file__, position)) +
           E_FMT(" Error: expected ");
    for (auto x: expected_extra) {
        msg += E_HLT(TOKEN_STRINGS[x]) + E_FMT(" or ");
    }
    msg += E_HLT(" operator (+,-,/,*) ");
    msg += E_FMT(" or ");
    msg += E_HLT(" member (.xxx)");
    msg += E_FMT(" or ");
    msg += E_HLT(" subscript ([xxx])");
    msg += E_FMT(" after expression, but got ");
    msg += E_HLT(tok.to_string());
    return msg;
}

std::string Parser::error_empty_tuple(TextPosition pos) {
    std::string msg = text_pos_to_string(this->__file__, pos) + E_FMT(" Error: can't have an empty tuple");
    return msg;
}

std::string Parser::error_tuple_one_element(TextPosition pos) {
    std::string msg = text_pos_to_string(this->__file__, pos) + E_FMT(" Error: can't have tuple with only one element");
    return msg;
}

void Parser::error_class_member_redefined(const std::string& cls, const std::string& name, TextPosition pos) {
    std::string msg;
    msg += E_FMT("In class ") + E_HLT(cls) + E_FMT(" definition: member/method \"") +
           E_HLT(name) +
           E_FMT("\" already defined!");
    this->error(msg, pos);
}


void Parser::error_expected_argument_id(Token tok, int idx, const std::string& id) {
    std::string msg;
    msg += E_FMT("Error: got ");
    msg += E_HLT(tok.to_string());
    msg += E_FMT(" expected ");
    msg += E_HLT(TOKEN_STRINGS[TokType::ID]);
    msg += E_FMT(" (argument #" + std::to_string(idx) + " of function ");
    msg += E_HLT(id);
    msg += E_FMT(")");
    this->error(msg, tok.start);
}

void Parser::error_expected_argument_type(Token tok, int idx, const std::string& id, const std::string& param_id) {
    std::string msg;
    msg = this->context_string(tok.start);
    msg += E_FMT("Error: got ");
    msg += E_HLT(tok.to_string());
    msg += E_FMT(" expected ");
    msg += E_HLT(TOKEN_STRINGS[TokType::COLON]);
    msg += E_FMT(" (type of argument ");
    msg += E_HLT(param_id);
    msg += E_FMT(" of function ");
    msg += E_HLT(id);
    msg += E_FMT(")");
    msg += this->code_context_string(tok.start);
    std::cout << msg << std::endl;
}

void Parser::error(const std::string& msg, TextPosition pos) {
    std::string m = this->context_string(pos) + msg + this->code_context_string(pos);
    std::cout << m << std::endl;
}

void Parser::error_object_type(Token tok) {
    std::string msg;
    msg += E_FMT("Error: got ");
    msg += E_HLT(tok.to_string());
    msg += E_FMT(" expected ");
    msg += E_HLT(TOKEN_STRINGS[TokType::ID]);
    msg += E_FMT(" (type)");
    this->error(msg, tok.start);
}

void Parser::error_assign_call(Token tok) {
    std::string msg;
    msg += E_FMT("Can't assign to a function call");
    this->error(msg, tok.start);
}