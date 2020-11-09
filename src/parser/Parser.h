#ifndef PARSER_H
#define PARSER_H


#include <vector>
#include "../scanner/Token.h"
#include "../nodes/nodes.h"
#include "../utils.h"
#include <iostream>
#include "UnexpectedToken.h"
#include "../nodes/NodeFactory.h"

extern std::map<TokType, OpType> TOKEN_TO_OP;

class Parser {
    std::vector<Token> tokens;
    Token token;
    size_t current;
public:
    Parser(std::vector<Token>& tokens);

    bool match(TokType type);

    NodeContainer parse_class_literal();

    void next();

    NodeContainer parse_variable_declaration();

    NodeContainer parse_id_or_class_literal();

    NodeContainer parse_function_definition();

    NodeContainer parse_if();

    NodeContainer parse_list_literal();

    NodeContainer parse_add_or_sub_expression();

    NodeContainer parse_and_expression();

    NodeContainer parse_assignment_or_expression();

    NodeContainer parse_bool_expression();

    NodeContainer parse_call_or_subscript_chain(NodeContainer parent);

    NodeContainer parse_common_statement();

    NodeContainer parse_expression();

    NodeContainer parse_factor();

    NodeContainer parse_id_or_literal();

    NodeContainer parse_mul_div_or_mod_expression();

    NodeContainer parse_or_expression();

    NodeContainer parse_top_level_statement();

    NodeContainer parse_return();

    Token expect_token(TokType token_type);

    TypeNode* parse_type_node();

    VectorOfNodes parse_list_of_expressions();

    BlockNode* parse_possibly_empty_block();

    NodeContainer parse_program();

    NodeContainer parse_for_loop();

    NodeContainer parse_while_loop();

    NodeContainer parse_ternary();

    NodeContainer parse_class_definition();

    FunctionTypeNode* parse_function_type();

    ObjectTypeNode* parse_object_type();

    bool inside_loop;
};


#endif //PARSER_H
