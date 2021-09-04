#ifndef PARSER_H
#define PARSER_H


#include <vector>
#include "../scanner/Token.h"
#include "../ast/nodes.h"
#include "../utils.h"
#include <iostream>
#include "../ast/Import.h"
#include "../scanner/CodeLines.h"
#include "../ast/EnumNode.h"
#include "../ast/ObjectType.h"
#include "../ast/Typeclass.h"
#include "../ast/Instance.h"
#include "../ast/Alias.h"

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

    ast::UNode parse_top_level_statement();

    std::unique_ptr<ast::Import> parse_import();
    std::unique_ptr<ast::Klass> parse_class_definition();
    std::unique_ptr<ast::Function> parse_function_definition();
    std::unique_ptr<ast::Alias> parse_alias();
    std::unique_ptr<ast::EnumNode> parse_enum_definition();

    ast::UNode parse_common_statement();
    ast::UNode parse_assignment_or_expression();
    std::unique_ptr<ast::Declaration> parse_variable_declaration();
    std::unique_ptr<ast::If> parse_if();
    std::unique_ptr<ast::While> parse_while_loop();
    std::unique_ptr<ast::For> parse_for_loop();
    std::unique_ptr<ast::Return> parse_return();
    ast::UNode parse_ternary();
    std::unique_ptr<ast::Match> parse_match_statement();
    // std::unique_ptr<ThrowNode> parse_throw();
    // std::unique_ptr<TryCatchNode> parse_try_catch();

    std::unique_ptr<ast::Block> parse_possibly_empty_block();
    std::unique_ptr<ast::Block> parse_program();

    ast::UNode parse_expression();
    ast::UNode parse_tuple_literal();
    ast::UNode parse_not_expression();
    ast::UNode parse_id_or_class_literal();
    ast::UNode parse_list_literal();
    ast::UNode parse_add_or_sub_expression();
    ast::UNode parse_and_expression();
    ast::UNode parse_bool_expression();
    ast::UNode parse_factor();
    ast::UNode parse_id_or_literal();
    ast::UNode parse_mul_div_or_mod_expression();
    ast::UNode parse_or_expression();
    ast::UNode parse_call_or_subscript_chain(ast::UNode& parent);
    ast::UNode parse_dictionary();
    ast::UNode parse_tuple_or_constructor();
    ast::UNode parse_partial_application();

    ast::UTypeNode parse_type_node();
    std::unique_ptr<FunctionType> parse_function_type();
    std::unique_ptr<ast::ObjectType> parse_object_type();

    ast::VectorOfNodesU parse_list_of_arguments();

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
    std::unique_ptr<ast::Typeclass> parse_typeclass();
    std::unique_ptr<ast::Instance> parse_instance();
};


#endif //PARSER_H
