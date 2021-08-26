#ifndef PARSER_H
#define PARSER_H


#include <vector>
#include "../scanner/Token.h"
#include "../ast/nodes.h"
#include "../utils.h"
#include <iostream>
#include "../ast/ImportNode.h"
#include "../scanner/CodeLines.h"
#include "../ast/EnumNode.h"
#include "../ast/TryCatchNode.h"
#include "../ast/ObjectType.h"
#include "../ast/TypeclassNode.h"
#include "../ast/InstanceNode.h"
#include "../ast/AliasNode.h"

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

    UNode parse_top_level_statement();

    std::unique_ptr<ImportNode> parse_import();
    std::unique_ptr<ast::ClassNode> parse_class_definition();
    std::unique_ptr<FunctionNode> parse_function_definition();
    std::unique_ptr<AliasNode> parse_alias();
    std::unique_ptr<EnumNode> parse_enum_definition();

    UNode parse_common_statement();
    UNode parse_assignment_or_expression();
    std::unique_ptr<ast::DeclarationNode> parse_variable_declaration();
    std::unique_ptr<IfNode> parse_if();
    std::unique_ptr<WhileNode> parse_while_loop();
    std::unique_ptr<ForNode> parse_for_loop();
    std::unique_ptr<ReturnNode> parse_return();
    UNode parse_ternary();
    std::unique_ptr<MatchExpressionNode> parse_match_statement();
    // std::unique_ptr<ThrowNode> parse_throw();
    // std::unique_ptr<TryCatchNode> parse_try_catch();

    std::unique_ptr<BlockNode> parse_possibly_empty_block();
    std::unique_ptr<BlockNode> parse_program();

    UNode parse_expression();
    UNode parse_tuple_literal();
    UNode parse_not_expression();
    UNode parse_id_or_class_literal();
    UNode parse_list_literal();
    UNode parse_add_or_sub_expression();
    UNode parse_and_expression();
    UNode parse_bool_expression();
    UNode parse_factor();
    UNode parse_id_or_literal();
    UNode parse_mul_div_or_mod_expression();
    UNode parse_or_expression();
    UNode parse_call_or_subscript_chain(UNode& parent);
    UNode parse_dictionary();
    UNode parse_tuple_or_constructor();
    UNode parse_partial_application();

    std::unique_ptr<TypeNode> parse_type_node();
    std::unique_ptr<FunctionType> parse_function_type();
    std::unique_ptr<ObjectType> parse_object_type();

    VectorOfNodes parse_list_of_expressions();
    VectorOfNodesU parse_list_of_arguments();

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
    std::unique_ptr<TypeclassNode> parse_typeclass();
    std::unique_ptr<InstanceNode> parse_instance();
};


#endif //PARSER_H
