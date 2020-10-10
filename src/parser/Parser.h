//
// Created by chris on 7/6/20.
//

#ifndef UNTITLED1_PARSER_H
#define UNTITLED1_PARSER_H


#include <vector>
#include "../scanner/Token.h"
#include "../nodes/nodes.h"
#include "../utils.h"
#include <iostream>
#include "UnexpectedToken.h"


class Parser {
    std::vector<Token> tokens;
    Token token;
    size_t current;
public:
    Parser(std::vector<Token>& tokens);

    bool match(TokenType type);

    Node* parse_class_literal();

    void next();

    StructNode* parse_struct_definition();

    DeclarationNode* parse_variable_declaration();

    Node* parse_id_or_class_literal();

    FunctionNode* parse_function_definition();

    IfNode* parse_if();

    Node* parse_list_literal();

    Node* parse_add_or_sub_expression();

    Node* parse_and_expression();

    Node* parse_assignment_or_expression();

    Node* parse_bool_expression();

    Node* parse_call_or_subscript_chain(Node* parent);

    Node* parse_common_statement();

    Node* parse_expression();

    Node* parse_factor();

    Node* parse_function_expression();

    Node* parse_id_or_literal();

    Node* parse_mul_div_or_mod_expression();

    Node* parse_or_expression();

    Node* parse_top_level_statement();

    ReturnNode* parse_return();

    Token expect_token(TokenType token_type);

    TypeNode* parse_type_node();

    VectorOfNodes parse_list_of_expressions();

    BlockNode* parse_possibly_empty_block();

    BlockNode* parse_program();

    ForNode* parse_for_loop();

    WhileNode* parse_while_loop();

    Node* parse_ternary();

    std::map<std::string, Node*> parse_initializers();

    ClassNode* parse_class_definition();

    Node* parse_instance_definition();

    FunctionTypeNode* parse_function_signature(std::string& function_name);

    Node* parse_function_definition_with_where();
};


#endif //UNTITLED1_PARSER_H
