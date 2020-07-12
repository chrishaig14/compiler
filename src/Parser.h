//
// Created by chris on 7/6/20.
//

#ifndef UNTITLED1_PARSER_H
#define UNTITLED1_PARSER_H


#include <vector>
#include "Token.h"
#include "AstNode.h"
#include <iostream>

typedef std::vector<TypeNode*> VectorOfTypes;
typedef std::vector<std::string> VectorOfStrings;
typedef std::vector<AstNode*> VectorOfNodes;

class UnexpectedToken {
    Token token;
    std::vector<TokenType> expected_tokens;
public:
    UnexpectedToken(Token token, const std::vector<TokenType> &expected_tokens);

    friend std::ostream &operator<<(std::ostream &os, const UnexpectedToken &unexpected_token);
};


class Parser {
    std::vector<Token> tokens;
    Token token;
    size_t current;
public:
    Parser(std::vector<Token> &tokens);

    void expect_one_of(std::vector<TokenType> expected_tokens);

    bool match(TokenType type);

    void next();

    AstNode* parse_top_level_statement();

    AstNode* parse_bool_expression();

    Token expect_token(TokenType token);

    FunctionNode* parse_function_definition();

    VectorOfNodes parse_possibly_empty_block();

    ClassNode* parse_class_definition();

    TypeNode* parse_type_node();

    AstNode* parse_interface_definition();

    AstNode* parse_common_statement();

    DeclarationNode* parse_variable_declaration();

    AstNode* parse_assignment_or_expression();

    AstNode* parse_expression();

    AstNode* parse_or_expression();

    AstNode* parse_and_expression();

    AstNode* parse_add_or_sub_expression();

    AstNode* parse_mul_or_div_expression();

    AstNode* parse_factor();

    AstNode* parse_function_expression();

    FunctionSignatureNode* parse_function_signature();

    MemberNode* parse_member();

    ListNode* parse_list_literal();

    CallNode* parse_call();

    IfNode* parse_if();

    ReturnNode* parse_return();;

    VectorOfNodes parse_program();

};


#endif //UNTITLED1_PARSER_H
