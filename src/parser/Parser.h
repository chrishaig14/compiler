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

    FunctionType* parse_function_type();

    ObjectType* parse_object_type();

    bool inside_loop;
    Node* parse_partial_application();
    VectorOfNodes parse_list_of_arguments();
    std::string error_empty_tuple(TextPosition pos);
    std::string error_tuple_one_element(TextPosition pos);

    std::string code_context_string(TextPosition position);
    std::string code_error_string(TextPosition start, TextPosition end);
    std::string context_string(TextPosition position);

    void error_after_var(Token tok);
    void error_after_var_name(Token tok);
    void error_after_var_type(Token tok);
    void error_out_of_loop(Token tok);
    void error_expected_expression(Token token);
    std::string error_after_expression(const std::vector<TokType>& expected_extra, TextPosition position);
    std::string error_after_expression(const std::vector<TokType>& expected_extra, Token tok, TextPosition position);
    void error_class_member_redefined(const std::string& cls, const std::string& name, TextPosition pos);
    void error_expected_argument_id(Token tok, int idx, const std::string& id);
    void error_expected_argument_type(Token tok, int idx, const std::string& id, const std::string& param_id);
    void error_object_type(Token tok);
    void error(const std::string& msg, TextPosition pos);
    void error_assign_call(Token tok);
    void error_expected_statement(TextPosition pos);
};


#endif //PARSER_H
