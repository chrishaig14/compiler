#ifndef PARSER_H
#define PARSER_H


#include <vector>
#include "../scanner/Token.h"
#include "../nodes/nodes.h"
#include "../utils.h"
#include <iostream>
#include "../nodes/ImportNode.h"
#include "../scanner/CodeLines.h"
#include "../nodes/EnumNode.h"
#include "../nodes/TryCatchNode.h"
#include "../nodes/ObjectType.h"
#include "../nodes/TypeclassNode.h"
#include "../nodes/InstanceNode.h"

extern std::unordered_map<TokType, OpType> TOKEN_TO_OP;

class Parser {
    std::vector<Token> tokens;
    Token token;
    size_t current;
    CodeLines code_lines;
    std::string __file__;
    bool inside_loop;

public:

    Parser(const std::string& __file__, CodeLines code_lines, std::vector<Token>& tokens);
    bool match(TokType type) const;
    void next();
    Token expect_token(TokType token_type);

    Node* parse_top_level_statement();

    ImportNode* parse_import();
    ClassNode* parse_class_definition();
    FunctionNode* parse_function_definition();
    Node* parse_alias();
    Node* parse_enum_definition();

    Node* parse_common_statement();
    Node* parse_assignment_or_expression();
    DeclarationNode* parse_variable_declaration();
    IfNode* parse_if();
    WhileNode* parse_while_loop();
    ForNode* parse_for_loop();
    ReturnNode* parse_return();
    Node* parse_ternary();
    Node* parse_match_statement();
    // ThrowNode* parse_throw();
    // TryCatchNode* parse_try_catch();

    BlockNode* parse_possibly_empty_block();
    BlockNode* parse_program();

    Node* parse_expression();
    Node* parse_tuple_literal();
    Node* parse_not_expression();
    Node* parse_id_or_class_literal();
    Node* parse_list_literal();
    Node* parse_add_or_sub_expression();
    Node* parse_and_expression();
    Node* parse_bool_expression();
    Node* parse_factor();
    Node* parse_id_or_literal();
    Node* parse_mul_div_or_mod_expression();
    Node* parse_or_expression();
    Node* parse_call_or_subscript_chain(Node* parent);
    Node* parse_dictionary();
    Node* parse_tuple_or_constructor();
    Node* parse_partial_application();

    TypeNode* parse_type_node();
    FunctionType* parse_function_type();
    ObjectType* parse_object_type();

    VectorOfNodes parse_list_of_expressions();
    VectorOfNodes parse_list_of_arguments();

    std::string code_context_string(TextPosition position);
    std::string code_error_string(TextPosition start, TextPosition end);
    std::string context_string(TextPosition position);

    void error_empty_tuple(TextPosition pos);
    void error_tuple_one_element(TextPosition pos);
    void error_after_var(Token tok);
    void error_after_var_name(Token tok);
    void error_after_var_type(Token tok);
    void error_out_of_loop(Token tok);
    void error_expected_expression(Token token);
    std::string error_after_expression(const std::vector<TokType>& expected_extra, TextPosition position);
    void error_after_expression(const std::vector<TokType>& expected_extra, Token tok, TextPosition position);
    void error_class_member_redefined(const std::string& cls, const std::string& name, TextPosition pos);
    void error_expected_argument_id(Token tok, int idx, const std::string& id);
    void error_expected_argument_type(Token tok, int idx, const std::string& id, const std::string& param_id);
    void error_object_type(Token tok);
    void error(const std::string& msg, TextPosition pos);
    void error_assign_call(Token tok);
    void error_expected_statement(TextPosition pos);
    void error_expected_type(Token tok);

    std::string top_package_name;
    TypeclassNode* parse_typeclass();
    InstanceNode* parse_instance();
};


#endif //PARSER_H
