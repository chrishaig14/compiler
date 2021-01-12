#ifndef PARSER_H
#define PARSER_H


#include <vector>
#include "../scanner/Token.h"
#include "../nodes/nodes.h"
#include "../utils.h"
#include <iostream>
#include "UnexpectedToken.h"
#include "../nodes/ImportNode.h"
#include "../scanner/CodeLines.h"

extern std::unordered_map<TokType, OpType> TOKEN_TO_OP;


class Parser {
    std::vector<Token> tokens;
    Token token;
    size_t current;
    CodeLines code_lines;
    std::string __file__;

public:

    Parser(const std::string& __file__, CodeLines code_lines, std::vector<Token>& tokens);

    bool match(TokType type);

    Node* parse_class_or_tuple_literal();

    void next();

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

    Node* parse_id_or_literal();

    Node* parse_mul_div_or_mod_expression();

    Node* parse_or_expression();

    Node* parse_top_level_statement();

    ReturnNode* parse_return();

    Token expect_token(TokType token_type);

    TypeNode* parse_type_node();

    VectorOfNodes parse_list_of_expressions();

    BlockNode* parse_possibly_empty_block();

    BlockNode* parse_program();

    ForNode* parse_for_loop();

    ImportNode* parse_import();

    WhileNode* parse_while_loop();

    Node* parse_ternary();

    ClassNode* parse_class_definition();

    FunctionTypeNode* parse_function_type();

    ObjectType* parse_object_type();

    bool inside_loop;
    Node* parse_partial_application();
    VectorOfNodes parse_list_of_arguments();
    std::string parse_error(const std::vector<std::string>& options);
    std::string empty_tuple_error(TextPosition pos);
    std::string tuple_one_element_error(TextPosition pos);
    std::string after_expression_error(const std::vector<TokType>& expected_extra, TextPosition position);
    std::string after_expression_error(const std::vector<TokType>& expected_extra, Token tok, TextPosition position);
};


#endif //PARSER_H
