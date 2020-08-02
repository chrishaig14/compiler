//
// Created by chris on 7/6/20.
//

#ifndef UNTITLED1_PARSER_H
#define UNTITLED1_PARSER_H


#include <vector>
#include "../scanner/Token.h"
#include "../nodes/IfNode.h"
#include "../nodes/nodes.h"
#include "../utils.h"
#include <iostream>


class UnexpectedToken : public std::runtime_error {
    Token token;
    std::vector<TokenType> expected_tokens;
public:
    UnexpectedToken(Token token, const std::vector<TokenType>& expected_tokens);

    std::string make_message(Token token, const std::vector<TokenType>& expected_tokens);

    friend std::ostream& operator<<(std::ostream& os, const UnexpectedToken& unexpected_token);
};

class Parser {
    std::vector<Token> tokens;
    Token token;
    size_t current;
public:
    Parser(std::vector<Token>& tokens);

    void expect_one_of(std::vector<TokenType> expected_tokens);

    bool match(TokenType type);

    void next();


    CallNode* parse_call();

    ClassNode* parse_class_definition();

    DeclarationNode* parse_variable_declaration();

    FunctionNode* parse_function_definition();

    IfNode* parse_if();

    ListNode* parse_list_literal();

    Node* parse_add_or_sub_expression();

    Node* parse_and_expression();

    Node* parse_assignment_or_expression();

    Node* parse_bool_expression();

    Node* parse_call_or_subscript_chain(Node* parent);

    Node* parse_common_statement();

    Node* parse_expression();

    Node* parse_factor();

    Node* parse_function_expression();

    Node* parse_id_call_or_subscript();

    Node* parse_id_or_literal();

    Node* parse_interface_definition();

    Node* parse_mul_or_div_expression();

    Node* parse_or_expression();

    Node* parse_top_level_statement();

    ReturnNode* parse_return();

    Token expect_token(TokenType token);

    TypeNode* parse_type_node();

    VectorOfNodes parse_list_of_expressions();

    BlockNode* parse_possibly_empty_block();

    BlockNode* parse_program();

};


#endif //UNTITLED1_PARSER_H
