//
// Created by chris on 7/6/20.
//

#include "Parser.h"
#include "../semantic/GlobalProcessor.h"
#include "../nodes/PartialApplication.h"
#include "../nodes/UnaryOpNode.h"
#include "../logging/logging.h"
#include "../nodes/DefaultConstructorNode.h"
#include "../nodes/AliasNode.h"
#include "../nodes/TryCatchNode.h"
#include <fmt/core.h>
#include <fmt/color.h>
#include <exception>
#include <set>
#include "../nodes/ObjectType.h"

std::unordered_map<TokType, OpType> TOKEN_TO_OP = {{TokType::PLUS,  OpType::ADD},
                                                   {TokType::MINUS, OpType::SUB},
                                                   {TokType::TIMES, OpType::MUL},
                                                   {TokType::DIV,   OpType::DIV},
                                                   {TokType::MOD,   OpType::MOD}};

std::unordered_map<TokType, BoolOp> TOKEN_TO_BOOL_OP = {{TokType::AND, BoolOp::AND},
                                                        {TokType::OR,  BoolOp::OR},
                                                        {TokType::LT,  BoolOp::LT},
                                                        {TokType::GT,  BoolOp::GT},
                                                        {TokType::LEQ, BoolOp::LE},
                                                        {TokType::GEQ, BoolOp::GE},
                                                        {TokType::NEQ, BoolOp::NE},
                                                        {TokType::EQ,  BoolOp::EQ}};

Parser::Parser(const std::string& __file__, CodeLines code_lines, std::vector<Token>& tokens) {
    this->__file__ = __file__;
    this->code_lines = code_lines;
    this->tokens = tokens;
    this->token = this->tokens[0];
    this->current = 0;
    this->inside_loop = false;
}

void Parser::next() {
    if (this->token.type != TokType::END) {
        this->token = this->tokens[this->current + 1];
        this->current++;
    }
}

bool Parser::match(TokType type) const {
    return this->token.type == type;
}

BlockNode* Parser::parse_program() {
    VectorOfNodes program;
    TextPosition start = this->token.start;
    while (this->token.type != TokType::END) {
        program.push_back(this->parse_top_level_statement());
    }
    TextPosition end = this->token.end_pos;
    return new BlockNode(program, start, end);
}

ReturnNode* Parser::parse_return() {
    Token ret_tok = this->expect_token(TokType::RETURN);
    Node* expression = nullptr;
    TextPosition end = ret_tok.end_pos;
    if (!this->match(TokType::SEMICOLON)) {
        expression = this->parse_expression();
        end = expression->end;
    }
    return new ReturnNode(expression, ret_tok.start, end);
}

IfNode* Parser::parse_if() {
    Token if_tok = this->expect_token(TokType::IF);
    Node* condition = this->parse_expression();
    BlockNode* body = this->parse_possibly_empty_block();
    std::vector<std::pair<Node*, BlockNode*>> elifs;
    while (this->match(TokType::ELIF)) {
        this->next();
        Node* elif_condition = this->parse_expression();
        BlockNode* elif_body = this->parse_possibly_empty_block();
        elifs.emplace_back(elif_condition, elif_body);
    }
    BlockNode* _else = nullptr;
    if (this->match(TokType::ELSE)) {
        this->next();
        _else = this->parse_possibly_empty_block();
    }
    auto* iff = new IfNode(condition, body, elifs, _else, if_tok.start, if_tok.end_pos);
    iff->start = if_tok.start;
    return iff;
}

Node* Parser::parse_list_literal() {
    Token list_start = this->expect_token(TokType::LSQUARE);
    VectorOfNodes elements;
    if (this->match(TokType::RSQUARE)) {
        // empty list
        this->next();
        // parse required type annotation (cannot infer type of empty list
        this->expect_token(TokType::DOUBLE_COLON);
        TypeNode* type = this->parse_type_node();
        Node* node = new EmptyListNode(type, list_start.start, list_start.end_pos);
        return node;
    }
    while (true) {
        Node* element = this->parse_expression();
        elements.push_back(element);
        if (!this->match(TokType::COMMA)) {
            break;
        }
        this->next();
    }
    Token list_end = this->expect_token(TokType::RSQUARE);
    Node* node = new ListNode(elements, list_start.start, list_end.start);
    return node;
}

VectorOfNodes Parser::parse_list_of_expressions() {
    VectorOfNodes result;
    while (true) {
        result.push_back(this->parse_expression());
        if (this->match(TokType::COMMA)) {
            this->next();
        } else {
            break;
        }
    }
    return result;
}

VectorOfNodes Parser::parse_list_of_arguments() {
    VectorOfNodes result;
    while (true) {
        result.push_back(this->parse_expression());
        if (this->match(TokType::COMMA)) {
            this->next();
        } else {
            break;
        }
    }
    if (!this->match(TokType::COMMA) && !this->match(TokType::RPAREN)) {
        this->expect_token(TokType::COMMA);
    }
    return result;
}

Node* Parser::parse_assignment_or_expression() {
    Node* lvalue = this->parse_expression();
    if (item_in_vec(this->token.type, {TokType::EQQ, TokType::PLUS_EQQ, TokType::MINUS_EQQ})) {
        if (lvalue->ntype == NodeType::CALL) {
            this->error_assign_call(this->token);

        }
        TokType op = this->token.type;
        TextPosition op_pos = this->token.start;
        this->next();
        if (lvalue->ntype != NodeType::ID && item_in_vec(op, {TokType::PLUS_EQQ, TokType::MINUS_EQQ})) {
            throw std::runtime_error("Error += or -= can only be used on ids!");
        }
        Node* rvalue = this->parse_expression();
        if (lvalue->ntype == NodeType::ID) {
            auto* id_node = new IdNode(((IdNode*) lvalue)->_id, lvalue->start, lvalue->end);
            if (op == TokType::PLUS_EQQ || op == TokType::MINUS_EQQ) {
                OpType opt;
                if (op == TokType::PLUS_EQQ) {
                    opt = OpType::ADD;
                } else if (op == TokType::MINUS_EQQ) {
                    opt = OpType::SUB;
                }
                auto* bnode = new BinopNode(opt, id_node, rvalue, id_node->start, rvalue->end);
                bnode->op_pos = op_pos;
                rvalue = bnode;

            }

        }
        Node* node = new AssignmentNode(lvalue, rvalue, lvalue->start, rvalue->end);
        node->start = op_pos;
        return node;
    } else {
        if (lvalue->ntype != NodeType::CALL) {
            this->error_expected_statement(this->token.start);
        }
    }
    return lvalue;
}

Node* Parser::parse_expression() {
    return this->parse_ternary();
}

Node* Parser::parse_or_expression() {
    Node* left = this->parse_and_expression();
    while (this->match(TokType::OR)) {
        this->next();
        Node* right = this->parse_and_expression();
        Node* node = new BoolOpNode(BoolOp::OR, left, right, left->start, right->end);
        left = node;
    }
    return left;
}

Node* Parser::parse_and_expression() {
    Node* left = this->parse_not_expression();
    while (this->match(TokType::AND)) {
        this->next();
        Node* right = this->parse_not_expression();
        Node* node = new BoolOpNode(BoolOp::AND, left, right, left->start, right->end);
        left = node;
    }
    return left;
}

Node* Parser::parse_not_expression() {
    if (this->match(TokType::NOT)) {
        Token not_tok = this->expect_token(TokType::NOT);
        Node* left = this->parse_bool_expression();
        left = new UnaryOpNode(UnaryOp::NOT, left, not_tok.start, left->end);
        return left;
    }
    return this->parse_bool_expression();
}

Node* Parser::parse_bool_expression() {
    Node* left = this->parse_add_or_sub_expression();
    BoolOp op;
    std::vector<TokType> boolean_tokens = {TokType::EQ, TokType::LT, TokType::GT, TokType::LEQ, TokType::GEQ,
                                           TokType::NEQ};
    if (!item_in_vec(this->token.type, boolean_tokens)) {
        return left;
    }
    op = TOKEN_TO_BOOL_OP[this->token.type];
    this->next();
    Node* right = this->parse_add_or_sub_expression();
    Node* node = new BoolOpNode(op, left, right, left->start, right->end);
    return node;
}

Node* Parser::parse_add_or_sub_expression() {
    Node* left = this->parse_mul_div_or_mod_expression();
    OpType op;
    while (item_in_vec(this->token.type, {TokType::PLUS, TokType::MINUS})) {
        op = TOKEN_TO_OP[this->token.type];
        Token op_token = this->token;
        this->next();
        Node* right = this->parse_mul_div_or_mod_expression();
        auto* node = new BinopNode(op, left, right, left->start, right->end);
        node->op_pos = op_token.start;
        left = node;
    }
    return left;
}

Node* Parser::parse_mul_div_or_mod_expression() {
    Node* left = this->parse_factor();
    OpType op;
    while (item_in_vec(this->token.type, {TokType::TIMES, TokType::DIV, TokType::MOD})) {
        op = TOKEN_TO_OP[this->token.type];
        Token op_token = this->token;
        this->next();
        Node* right = this->parse_factor();
        auto* node = new BinopNode(op, left, right, left->start, right->end);
        node->op_pos = op_token.start;
        left = node;
    }
    return left;
}

Node* Parser::parse_factor() {
    Node* parent;
    if (this->match(TokType::HASH)) {
        parent = this->parse_tuple_or_constructor();
    } else {
        parent = this->parse_id_or_literal();
    }
    if (item_in_vec(parent->ntype, {NodeType::TUPLE, NodeType::NUMBER, NodeType::NONE, NodeType::BOOLEAN})) {
        if (item_in_vec(this->token.type, {TokType::LPAREN, TokType::LSQUARE})) {
            this->expect_token(TokType::RPAREN); // some random token
        }
    }
    parent = this->parse_call_or_subscript_chain(parent);
    while (this->match(TokType::DOT)) {
        TextPosition dot_pos = this->token.start;
        this->next();
        Token tok;
        if (this->match(TokType::INTEGER)) {
            tok = this->expect_token(TokType::INTEGER);
            auto* mn = new MemberNode(parent, tok);
            mn->dot_pos = dot_pos;
            parent = mn;
        } else {
            tok = this->expect_token(TokType::ID);
            auto* mn = new MemberNode(parent, tok);
            mn->dot_pos = dot_pos;
            parent = mn;
        }
        parent = this->parse_call_or_subscript_chain(parent);
    }

    // if (this->match(TokType::DOUBLE_COLON)) {
    //     this->next();
    //     Token as_type = this->expect_token(TokType::ID);
    // parent = new CastNode(parent, as_type.str, parent->start, as_type.end_pos);
    // }
    return parent;
}

Node* Parser::parse_dictionary() {
    Token lcurly = this->expect_token(TokType::LCURLY);
    if (this->match(TokType::RCURLY)) {
        // empty dict
        this->next();
        this->expect_token(TokType::DOUBLE_COLON);
        this->expect_token(TokType::LSQUARE);
        TypeNode* key_type = this->parse_type_node();
        this->expect_token(TokType::COMMA);
        TypeNode* value_type = this->parse_type_node();
        Token rsquare = this->expect_token(TokType::RSQUARE);
        return new EmptyDictNode(key_type, value_type, lcurly.start, rsquare.end_pos);
    }
    std::vector<std::pair<Node*, Node*>> items;
    Token rcurly;
    while (true) {
        Node* key = this->parse_expression();
        this->expect_token(TokType::COLON);
        Node* value = this->parse_expression();
        items.emplace_back(key, value);
        if (this->match(TokType::COMMA)) {
            this->next();
        } else {
            rcurly = this->expect_token(TokType::RCURLY);
            break;
        }
    }
    auto* dict = new DictNode(items, lcurly.start, rcurly.end_pos);
    return dict;
}


Node* Parser::parse_partial_application() {
    Token dollar = this->expect_token(TokType::DOLLAR_SIGN);
    Token total_function_tok = this->expect_token(TokType::ID);
    this->expect_token(TokType::LPAREN);
    VectorOfNodes args;
    while (true) {
        if (this->match(TokType::TIMES)) {
            this->next();
            args.push_back(nullptr);
        } else {
            args.push_back(this->parse_expression());
        }
        if (this->match(TokType::COMMA)) {
            this->next();
        } else {
            break;
        }
    }
    Token close = this->expect_token(TokType::RPAREN);
    auto* partial = new PartialApplication(new IdNode(total_function_tok.str,
                                                      total_function_tok.start,
                                                      total_function_tok.end_pos), args, dollar.start, close.end_pos);
    partial->start = total_function_tok.start;
    return partial;
}

Node* Parser::parse_id_or_literal() {
    Node* node = nullptr;
    switch (this->token.type) {
        case TokType::LPAREN: {
            this->next();
            node = this->parse_expression();
            this->expect_token(TokType::RPAREN);
            break;
        }
        case TokType::ID: {
            node = this->parse_id_or_class_literal();
            break;
        }
        case TokType::INTEGER: {
            node = new NumberNode(NumberType::INTEGER, this->token.str, this->token.start, this->token.end_pos);
            node->end = this->token.end_pos;
            this->next();
            break;
        }
        case TokType::FLOAT: {
            node = new NumberNode(NumberType::FLOAT, this->token.str, this->token.start, this->token.end_pos);
            node->end = this->token.end_pos;
            this->next();
            break;
        }
        case TokType::DOUBLE: {
            node = new NumberNode(NumberType::DOUBLE, this->token.str, this->token.start, this->token.end_pos);
            node->end = this->token.end_pos;
            this->next();
            break;
        }
        case TokType::STRING: {
            node = new StringNode(this->token.str, this->token.start, this->token.end_pos);
            node->end = this->token.end_pos;
            this->next();
            break;
        }
        case TokType::TRUE: {
            node = new BooleanNode(true, this->token.start, this->token.end_pos);
            this->next();
            break;
        }
        case TokType::FALSE: {
            node = new BooleanNode(false, this->token.start, this->token.end_pos);
            this->next();
            break;
        }
        case TokType::LSQUARE:
            return this->parse_list_literal();
        case TokType::NONE: {
            node = new NoneNode(this->token.start, this->token.end_pos);
            this->next();
            return node;
        }
        case TokType::DOLLAR_SIGN: {
            node = this->parse_partial_application();
            return node;
        }
        case TokType::LCURLY: {
            node = this->parse_dictionary();
            return node;
        }
        default:
            this->error_expected_expression(this->token);
    }
    return node;
}

Node* Parser::parse_tuple_or_constructor() {
    Token hash_tok = this->token;
    Node* parent;
    this->next();
    if (this->match(TokType::ID)) {
        Token idd = this->expect_token(TokType::ID);
        Node* m = new IdNode(idd.str, this->token.start, this->token.end_pos);
        while (this->match(TokType::DOT)) {
            this->next();
            idd = this->expect_token(TokType::ID);
            m = new MemberNode(m, idd);
        }
        parent = new DefaultConstructorNode(m, hash_tok.start, m->end);
    } else {
        parent = this->parse_tuple_literal();
        parent->start = hash_tok.start;
    }
    return parent;
}

Node* Parser::parse_tuple_literal() {
    // Token hash_tok = this->expect_token(TokType::HASH);
    Token st = this->expect_token(TokType::LPAREN);
    // it's a tuple
    // this->next();
    VectorOfNodes values;
    if (this->match(TokType::RPAREN)) {
        // this->error_empty_tuple(POS_NONE);
        throw std::runtime_error("EMPTY TUPLE!");

    }
    bool first = true;
    while (true) {
        Node* exp = this->parse_expression();
        values.emplace_back(exp);
        if (this->match(TokType::COMMA)) {
            this->next();
            first = false;
            continue;
        } else {
            if (first && this->match(TokType::RPAREN)) {
                throw std::runtime_error("tuple on element!");
            }
            break;
        }
    }
    Token close = this->expect_token(TokType::RPAREN);
    return new TupleNode(values, st.start, close.end_pos);
}

Node* Parser::parse_id_or_class_literal() {
    Node* node;
    std::string identifier = this->token.str;
    node = new IdNode(identifier, this->token.start, this->token.end_pos);
    this->next();
    return node;
}

Node* Parser::parse_call_or_subscript_chain(Node* parent) {
    Node* node = parent;
    while (item_in_vec(this->token.type, {TokType::LPAREN, TokType::LSQUARE})) {
        if (this->match(TokType::LPAREN)) {
//                 function call
            this->next();
            VectorOfNodes arguments;
            Token close;
            if (this->match(TokType::RPAREN)) {
                close = this->token;
                this->next();
            } else {
                arguments = this->parse_list_of_arguments();
                close = this->expect_token(TokType::RPAREN);
            }
            Node* old_node = node;
            node = new CallNode(node, arguments, node->start, close.end_pos);
            node->start = old_node->start;
        } else if (this->match(TokType::LSQUARE)) {
//                subscript
            Token lsquare = this->token;
            this->next();
            Node* value = this->parse_expression();
            Token close = this->expect_token(TokType::RSQUARE);
            Node* old_node = node;
            node = new SubscriptNode(node, {value}, old_node->start, close.end_pos);
        }
    }
    return node;
}

DeclarationNode* Parser::parse_variable_declaration() {
    Token var_token = this->expect_token(TokType::VAR);
    Token identifier = this->expect_token(TokType::ID);
    TypeNode* type = nullptr;
    if (this->match(TokType::COLON)) {
        this->next();
        type = this->parse_type_node();
    }
    Token eq_tok = this->expect_token(TokType::EQQ);
    Node* expression = this->parse_expression();
    return new DeclarationNode(identifier.str, type, expression, var_token.start, eq_tok.start, expression->end);
}

Node* Parser::parse_common_statement() {
    switch (this->token.type) {
        case TokType::IF: {
            return this->parse_if();
        }
        case TokType::VAR: {
            DeclarationNode* node = this->parse_variable_declaration();
            this->expect_token(TokType::SEMICOLON);
            return node;
        }
        case TokType::RETURN: {
            ReturnNode* node = this->parse_return();
            this->expect_token(TokType::SEMICOLON);
            return node;
        }
        case TokType::THROW: {
            // ThrowNode* node = this->parse_throw();
            // this->expect_token(TokType::SEMICOLON);
            // return node;
            return nullptr;
        }
        case TokType::TRY: {
            // TryCatchNode* node = this->parse_try_catch();
            // return node;
            return nullptr;
        }
        case TokType::FOR: {
            return this->parse_for_loop();
        }
        case TokType::WHILE: {
            return this->parse_while_loop();
        }
        case TokType::MATCH: {
            return this->parse_match_statement();
        }
        case TokType::BREAK: {
            if (!this->inside_loop) {
                this->error_out_of_loop(this->token);

            }
            Token tok = this->token;
            this->next();
            return new BreakNode(tok.start, tok.end_pos);
        }
        case TokType::CONTINUE: {
            if (!this->inside_loop) {
                this->error_out_of_loop(this->token);

            }
            Token tok = this->token;
            this->next();
            return new ContinueNode(tok.start, tok.end_pos);
        }
        default: {
            Node* node = this->parse_assignment_or_expression();
            this->expect_token(TokType::SEMICOLON);
            return node;
        }
    }
}

FunctionType* Parser::parse_function_type() {
    this->expect_token(TokType::FUN);
    this->expect_token(TokType::LPAREN);
    VectorOfTypes parameter_types;
    if (!this->match(TokType::RPAREN)) {
        while (true) {
            TypeNode* parameter_type = this->parse_type_node();
            parameter_types.push_back(parameter_type);
            if (this->match(TokType::COMMA)) {
                this->next();
            } else {
                break;
            }
        }
    }
    this->expect_token(TokType::RPAREN);
    TypeNode* return_type;
    if (this->match(TokType::RARROW)) {
        this->expect_token(TokType::RARROW);
        return_type = this->parse_type_node();
    } else {
        return_type = new ObjectType(".None");
    }
    return new FunctionType(parameter_types, return_type);
}

ObjectType* Parser::parse_object_type() {
    Token identifier = this->expect_token(TokType::ID);
    VectorOfTypes type_parameters;
    if (this->match(TokType::LSQUARE)) {
        this->next();
        while (true) {
            TypeNode* type_parameter = this->parse_type_node();;
            type_parameters.push_back(type_parameter);
            if (this->match(TokType::COMMA)) {
                this->next();
            } else {
                break;
            }
        }
        this->expect_token(TokType::RSQUARE);
    }
    auto* ot = new ObjectType(identifier.str, type_parameters);
    if (ot->id.size() == 1 && (islower(ot->id[0]) != 0)) {
        ot->is_generic_param = true;
    }
    return ot;
}

TypeNode* Parser::parse_type_node() {
    if (this->match(TokType::FUN)) {
        return this->parse_function_type();
    } else if (this->match(TokType::ID)) {
        return this->parse_object_type();
    } else {
        this->error_expected_type(this->token);
    }
    return nullptr;
}

BlockNode* Parser::parse_possibly_empty_block() {
    Token st = this->expect_token(TokType::LCURLY);
    VectorOfNodes block;
    Token end;
    while (true) {
        if (this->match(TokType::RCURLY)) {
            end = this->token;
            this->next();
            break;
        }
        Node* statement = this->parse_common_statement();
        block.push_back(statement);
    }
    return new BlockNode(block, st.start, end.end_pos);
}

FunctionNode* Parser::parse_function_definition() {
    Token fun_tok = this->expect_token(TokType::FUN);
    Token matched_token = this->expect_token(TokType::ID);
    std::string identifier = matched_token.str;
    this->expect_token(TokType::LPAREN);
    VectorOfTypes parameter_types;
    VectorOfStrings parameter_names;

    if (!this->match(TokType::RPAREN) && !this->match(TokType::ID)) {
        this->expect_token(TokType::RPAREN);
    }
    Implicit* implicit = nullptr;
    if (this->match(TokType::RPAREN)) {
        this->next();
        // Function with no parameters
    } else {
        // Function with parameters
        // Parse parameter list
        if (!this->match(TokType::SEMICOLON)) {
            while (true) {
                Token parameter_identifier = this->expect_token(TokType::ID);
                this->expect_token(TokType::COLON);
                TypeNode* parameter_type = this->parse_type_node();
                parameter_types.push_back(parameter_type);
                parameter_names.push_back(parameter_identifier.str);
                if (this->match(TokType::COMMA)) {
                    this->next();
                } else {
                    break;
                }
            }
        }
        if (this->match(TokType::SEMICOLON)) {
            this->next();
            // implicit parameters
            Token parent = this->expect_token(TokType::ID);
            this->expect_token(TokType::DOT);
            Token child = this->expect_token(TokType::ID);
            this->expect_token(TokType::COLON);
            bool is_static = false;
            if (this->match(TokType::STATIC)) {
                this->next();
                is_static = true;
            }
            FunctionType* ft = this->parse_function_type();
            std::cout << is_static << std::endl;
            std::cout << ft->to_json() << std::endl;
            implicit = new Implicit{parent.str, child.str, ft, is_static};
        }
        this->expect_token(TokType::RPAREN);
    }
    // Parse return
    TypeNode* return_type;
    if (this->match(TokType::RARROW)) {
        // function with return value
        this->expect_token(TokType::RARROW);
        return_type = this->parse_type_node();
    } else {
        return_type = new ObjectType(".None");
    }
    // Parse function body
    BlockNode* body = this->parse_possibly_empty_block();

    auto* node = new FunctionNode(identifier,
                                  parameter_names,
                                  parameter_types,
                                  return_type,
                                  body,
                                  fun_tok.start,
                                  body->end);
    node->implicit = implicit;
    node->start = fun_tok.start;
    return node;
}

Token Parser::expect_token(TokType token_type) {
    if (not this->match(token_type)) {
        std::string msg =
                this->context_string(this->token.start) + E_FMT("Unexpected token ") + E_HLT(this->token.to_string()) +
                this->code_context_string(this->token.start);
        throw std::runtime_error(msg);
    }
    Token matched_token = this->token;
    this->next();
    return matched_token;
}

Node* Parser::parse_alias() {
    Token alias_tk = this->expect_token(TokType::ALIAS);
    Token alias_id = this->expect_token(TokType::ID);
    this->expect_token(TokType::EQQ);
    TypeNode* aliased_type = this->parse_type_node();
    Token semic_tk = this->expect_token(TokType::SEMICOLON);
    auto* node = new AliasNode(alias_id.str, aliased_type, alias_tk.start, semic_tk.end_pos);
    return node;
}

Node* Parser::parse_top_level_statement() {
    switch (this->token.type) {
        case TokType::FUN:
            return this->parse_function_definition();
        case TokType::CLASS:
            return this->parse_class_definition();
        case TokType::ENUM:
            return this->parse_enum_definition();
        case TokType::IMPORT:
            return this->parse_import();
        case TokType::ALIAS:
            return this->parse_alias();
        default:
            return this->parse_common_statement();
    }
}


ForNode* Parser::parse_for_loop() {
    Token for_tok = this->expect_token(TokType::FOR);
    bool expect_paren = false;
    if (this->match(TokType::LPAREN)) {
        this->next();
        expect_paren = true;
    }
    Token var = this->expect_token(TokType::ID);
    this->expect_token(TokType::ARROBA);
    Node* exp = this->parse_expression();
    if (expect_paren) {
        this->expect_token(TokType::RPAREN);
    }
    bool prev = this->inside_loop;
    this->inside_loop = true;
    BlockNode* body = this->parse_possibly_empty_block();
    this->inside_loop = prev;
    auto* forloop = new ForNode(var.str, exp, body, for_tok.start, body->end);
    forloop->start = for_tok.start;
    return forloop;
}

Node* Parser::parse_ternary() {
    Node* condition = this->parse_or_expression();
    if (this->match(TokType::QUESTION)) {
        this->next();
        // Node* true_case = this->parse_expression();
        // this->expect_token(TokType::COLON);
        // Node* false_case = this->parse_expression();
        // Node* node = new TernaryNode(condition, true_case, false_case, condition->start, false_case->end);
        // return node;
    }
    return condition;
}

WhileNode* Parser::parse_while_loop() {
    Token while_tok = this->expect_token(TokType::WHILE);
    Node* condition = this->parse_expression();
    bool prev = this->inside_loop;
    this->inside_loop = true;
    BlockNode* body = this->parse_possibly_empty_block();
    this->inside_loop = prev;
    auto* whil = new WhileNode(condition, body, while_tok.start, body->end);
    whil->start = while_tok.start;
    return whil;
}

ClassNode* Parser::parse_class_definition() {
    Token class_tok = this->expect_token(TokType::CLASS);
    Token class_name_tk = this->expect_token(TokType::ID);
    std::string& class_name = class_name_tk.str;
    VectorOfStrings type_parameters;
    if (this->match(TokType::LSQUARE)) {
        this->next();

        while (true) {
            Token type_param_tk = this->expect_token(TokType::ID);
            type_parameters.push_back(type_param_tk.str);
            if (!this->match(TokType::COMMA)) {
                break;
            } else {
                this->next();
            }
        }
        this->expect_token(TokType::RSQUARE);
    }
    this->expect_token(TokType::LCURLY);
    std::unordered_map<std::string, Method> methods;
    std::unordered_map<std::string, FunctionNode*> static_methods;
    std::vector<std::pair<std::string, TypeNode*>> members;
    std::set<std::string> member_names;
    std::map<std::string, std::pair<TypeNode*, Node*>> static_members;
    VectorOfStrings members_ordered;
    while (true) {
        bool is_static = false;
        if (this->match(TokType::STATIC)) {
            this->next();
            is_static = true;
        }
        if (this->match(TokType::ID)) {
            Token member_name_tk = this->expect_token(TokType::ID);
            this->expect_token(TokType::COLON);
            TypeNode* member_type = this->parse_type_node();
            std::string& member_name = member_name_tk.str;
            if (member_names.find(member_name) != member_names.end() || methods.find(member_name) != methods.end()) {
                this->error_class_member_redefined(class_name, member_name, member_name_tk.start);
            }
            if (is_static) {
                this->expect_token(TokType::EQQ);
                Node* init_expression = this->parse_expression();
                static_members[member_name] = std::make_pair(member_type, init_expression);
            } else {
                members.push_back({member_name, member_type});
            }
            this->expect_token(TokType::SEMICOLON);
        } else if (this->match(TokType::WHERE)) {
            this->next();
            Token parent = this->expect_token(TokType::ID);
            this->expect_token(TokType::DOT);
            Token child = this->expect_token(TokType::ID);
            this->expect_token(TokType::COLON);
            bool is_static = false;
            if (this->match(TokType::STATIC)) {
                this->next();
                is_static = true;
            }
            FunctionType* ft = this->parse_function_type();
            this->expect_token(TokType::SEMICOLON);
            std::cout << is_static << std::endl;
            std::cout << ft->to_json() << std::endl;
            Implicit* implicit = new Implicit{parent.str, child.str, ft, is_static};

            FunctionNode* method_node = this->parse_function_definition();
            std::string& method_name = method_node->identifier;
            if (member_names.find(method_name) != member_names.end() || methods.find(method_name) != methods.end()) {
                this->error_class_member_redefined(class_name, method_name, method_node->start);
            }
            if (is_static) {
                static_methods.insert(make_pair(method_name, method_node));
            } else {
                methods[method_name] = Method{implicit, method_node};
            }

        } else if (this->match(TokType::FUN)) {
            FunctionNode* method_node = this->parse_function_definition();
            std::string& method_name = method_node->identifier;
            if (member_names.find(method_name) != member_names.end() || methods.find(method_name) != methods.end()) {
                this->error_class_member_redefined(class_name, method_name, method_node->start);
            }
            if (is_static) {
                static_methods.insert(make_pair(method_name, method_node));
            } else {
                methods[method_name] = Method{nullptr, method_node};
            }
        } else {
            break;
        }
    }
    Token end = this->expect_token(TokType::RCURLY);
    auto* c = new ClassNode(class_name,
                            type_parameters,
                            members,
                            methods,
                            static_members,
                            static_methods,
                            class_tok.start,
                            end.end_pos);
    c->members_ordered = members_ordered;
    c->start = class_tok.start;
    return c;
}

ImportNode* Parser::parse_import() {
    Token import_tok = this->expect_token(TokType::IMPORT);
    VectorOfStrings path;
    // path.push_back("global");
    if (this->match(TokType::DOT)) {
        // it's a local import
        this->next();
        path.push_back(this->top_package_name);
    }
    Token path_part;
    while (true) {
        path_part = this->expect_token(TokType::ID);
        path.push_back(path_part.str);
        if (this->match(TokType::DOT)) {
            this->next();
            continue;
        } else {
            break;
        }
    }
    if (this->match(TokType::AS)) {
        this->next();
        Token alias = this->expect_token(TokType::ID);
        this->expect_token(TokType::SEMICOLON);
        return new ImportNode(path, alias.str, import_tok.start, path_part.end_pos);
    }
    this->expect_token(TokType::SEMICOLON);
    return new ImportNode(path, import_tok.start, path_part.end_pos);
}

Node* Parser::parse_match_statement() {
    Token mtk = this->expect_token(TokType::MATCH);
    Node* exp = this->parse_expression();
    this->expect_token(TokType::LCURLY);

    std::vector<std::string> ids;
    std::vector<std::pair<TypeNode*, BlockNode*>> cases;
    while (true) {
        Token id = this->expect_token(TokType::ID);
        this->expect_token(TokType::COLON);
        TypeNode* type = this->parse_type_node();
        BlockNode* body = this->parse_possibly_empty_block();
        ids.push_back(id.str);
        cases.emplace_back(type, body);
        if (this->match(TokType::RCURLY)) {
            break;
        }
    }
    Token lcurly = this->expect_token(TokType::RCURLY);
    return new MatchExpressionNode(exp, ids, cases, mtk.start, lcurly.end_pos);
}


TypeclassNode* Parser::parse_typeclass() {
    this->expect_token(TokType::TYPECLASS);
    Token typeclass_id = this->expect_token(TokType::ID);
    this->expect_token(TokType::LSQUARE);
    Token base_type = this->expect_token(TokType::ID);
    this->expect_token(TokType::RSQUARE);
    this->expect_token(TokType::LCURLY);

    std::unordered_map<std::string, FunctionType*> methods;

    while (true) {
        this->expect_token(TokType::FUN);
        Token method_id = this->expect_token(TokType::ID);
        VectorOfTypes parameter_types;
        // VectorOfStrings parameter_names;
        this->expect_token(TokType::LPAREN);
        while (true) {
            Token parameter_identifier = this->expect_token(TokType::ID);
            this->expect_token(TokType::COLON);
            TypeNode* parameter_type = this->parse_type_node();
            parameter_types.push_back(parameter_type);
            // parameter_names.push_back(parameter_identifier.str);
            if (this->match(TokType::COMMA)) {
                this->next();
            } else {
                break;
            }
        }
        this->expect_token(TokType::RPAREN);
        TypeNode* return_type;
        if (this->match(TokType::RARROW)) {
            this->expect_token(TokType::RARROW);
            return_type = this->parse_type_node();
        } else {
            return_type = new ObjectType(".None");
        }

        FunctionType* ft = new FunctionType(parameter_types, return_type);
        methods[method_id.str] = ft;
        this->expect_token(TokType::SEMICOLON);
        if (!this->match(TokType::FUN)) {
            break;
        }
    }

    Token final_curly = this->expect_token(TokType::RCURLY);
    TypeclassNode* n = new TypeclassNode(typeclass_id.str,
                                         base_type.str,
                                         methods,
                                         typeclass_id.start,
                                         final_curly.end_pos);
    return n;
}