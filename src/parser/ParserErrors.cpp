//
// Created by chris on 23/1/21.
//

#include <expat.h>
#include "Parser.h"
#include "../logging/logging.h"

std::string Parser::code_context_string(TextPosition position) {
    std::string str = "\n" + this->code_lines.get_line(position.line) + "\n";
    str += fmt::format(std::string(position.column, ' ') + std::string(1, '^'));
    return str;
}

std::string Parser::code_error_string(TextPosition start, TextPosition end) {
    int length = end.column - start.column + 1;
    std::string str = "\n" + this->code_lines.get_line(start.line) + "\n";
    str += fmt::format(fmt::fg(fmt::color::orange_red), std::string(start.column, ' ') + std::string(length, '^'));
    return str;
}

std::string Parser::context_string(TextPosition position) {
    std::string msg = (text_pos_to_string(this->__file__, position)) + (": ");
    return msg;
}

void Parser::error_after_var(Token tok) {
    std::string msg;
    msg += ("Got ");
    msg += (tok.to_string());
    msg += (" expected ");
    msg += (TOKEN_STRINGS[TokType::ID]);
    msg += (" (new variable name)");
    this->error(msg, tok.start);
}

void Parser::error_after_var_name(Token tok) {
    std::string msg;
    msg += ("Got ");
    msg += (tok.to_string());
    msg += (" expected ");
    msg += (TOKEN_STRINGS[TokType::COLON]);
    msg += (" or ");
    msg += (TOKEN_STRINGS[TokType::EQQ]);
    msg += (" (new variable type/initial value)");
    this->error(msg, tok.start);
}

void Parser::error_after_var_type(Token tok) {
    std::string msg;
    msg += ("Got ");
    msg += (tok.to_string());
    msg += (" expected ");
    msg += (TOKEN_STRINGS[TokType::ID]);
    msg += (" (new variable initialization)");
    this->error(msg, tok.start);
}

void Parser::error_out_of_loop(Token tok) {
    std::string msg;
    msg += ("Got ");
    msg += (tok.to_string());
    msg += (" out of loop ");
    this->error(msg, tok.start);
}

void Parser::error_expected_expression(Token tok) {
    std::string msg;
    msg += ("Got ");
    msg += (tok.to_string());
    msg += (" expected ");
    msg += ("expression ");
    this->error(msg, tok.start);
}

void Parser::error_after_expression(const std::vector<TokType>& expected_extra, Token tok, TextPosition position) {
    std::string msg;
    msg = ("Got ");
    msg += (tok.to_string());
    msg = (" Expected ");
    for (auto x: expected_extra) {
        msg += (TOKEN_STRINGS[x]) + E_FMT(" or ");
    }
    msg += (" operator (+,-,/,*) ");
    msg += (" or ");
    msg += (" member (.xxx)");
    msg += (" or ");
    msg += (" subscript ([xxx])");
    msg += (" after expression");
    this->error(msg, tok.start);
}

void Parser::error_empty_tuple(TextPosition pos) {
    std::string msg = E_FMT(" Can't have an empty tuple");
    this->error(msg, pos);
}

void Parser::error_tuple_one_element(TextPosition pos) {
    std::string msg = text_pos_to_string(this->__file__, pos) + E_FMT(" Can't have tuple with only one element");
    this->error(msg, pos);
}

void Parser::error_class_member_redefined(const std::string& cls, const std::string& name, TextPosition pos) {
    std::string msg;
    msg += ("In class ") + (cls) + (" definition: member/method \"") + (name) + ("\" already defined!");
    this->error(msg, pos);
}


void Parser::error_expected_argument_id(Token tok, int idx, const std::string& id) {
    std::string msg;
    msg += ("Got ");
    msg += (tok.to_string());
    msg += (" expected ");
    msg += (TOKEN_STRINGS[TokType::ID]);
    msg += (" (argument #" + std::to_string(idx) + " of function ");
    msg += (id);
    msg += (")");
    this->error(msg, tok.start);
}

void Parser::error_expected_argument_type(Token tok, int idx, const std::string& id, const std::string& param_id) {
    std::string msg;
    msg += ("Got ");
    msg += (tok.to_string());
    msg += (" expected ");
    msg += (TOKEN_STRINGS[TokType::COLON]);
    msg += (" (type of argument ");
    msg += (param_id);
    msg += (" of function ");
    msg += (id);
    msg += (") int argument #" + std::to_string(idx));
    this->error(msg, tok.start);
}

void Parser::error_expected_type(Token tok) {
    std::string msg;
    msg += ("Got ");
    msg += (tok.to_string());
    msg += (" expected ");
    msg += ("type");
    this->error(msg, tok.start);
}

void Parser::error(const std::string& msg, TextPosition pos) {
    std::string m = this->context_string(pos) + msg + this->code_context_string(pos);
    throw std::runtime_error("ParseError: " + m);
}

void Parser::error_object_type(Token tok) {
    std::string raw = "Got " + tok.to_string() + " expected " + TOKEN_STRINGS[TokType::ID] + " (type) ";
    this->error(raw, tok.start);
}

void Parser::error_assign_call(Token tok) {
    std::string msg;
    msg += E_FMT("Can't assign to a function call");
    this->error(msg, tok.start);
}

void Parser::error_expected_statement(TextPosition pos) {
    std::string msg;
    msg += E_FMT("Expected a statement (assignment, function call, if, while, for, return)");
    this->error(msg, pos);
}

Node* Parser::parse_enum_definition() {
    this->expect_token(TokType::ENUM);
    Token enum_id = this->expect_token(TokType::ID);
    this->expect_token(TokType::LCURLY);
    VectorOfStrings values;
    while (true) {
        Token value = this->expect_token(TokType::ID);
        values.push_back(value.str);
        if (!this->match(TokType::COMMA)) {
            break;
        }
        this->next();
    }
    if (this->match(TokType::SEMICOLON)) {
        this->next();
    }
    Token rcurly_tk = this->expect_token(TokType::RCURLY);
    return new EnumNode(enum_id.str, values, enum_id.start, rcurly_tk.end_pos);
}

InstanceNode* Parser::parse_instance() {
    Token instance_tok = this->expect_token(TokType::INSTANCE);
    Token id_tok = this->expect_token(TokType::ID);
    this->expect_token(TokType::LSQUARE);
    ObjectType* ot = this->parse_object_type();
    this->expect_token(TokType::RSQUARE);
    this->expect_token(TokType::LCURLY);
    std::unordered_map<std::string, FunctionNode*> methods;
    while (true) {
        auto* m = this->parse_function_definition();
        methods[m->identifier] = m;
        this->expect_token(TokType::SEMICOLON);
        if (!this->match(TokType::FUN)) {
            break;
        }
    }
    Token f_curly = this->expect_token(TokType::RCURLY);
    return new InstanceNode(id_tok.str, ot, methods, instance_tok.start, f_curly.end_pos);
}
