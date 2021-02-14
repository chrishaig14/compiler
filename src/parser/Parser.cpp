//
// Created by chris on 7/6/20.
//

#include "Parser.h"
#include "UnexpectedToken.h"
#include "../semantic/GlobalProcessor.h"
#include "../nodes/PartialApplication.h"
#include "../logging/logging.h"
#include <fmt/core.h>
#include <fmt/color.h>
#include <exception>


std::string highlight(size_t column, size_t length) {
    return fmt::format(fmt::fg(fmt::color::orange_red), std::string(column, ' ') + std::string(length, '^'));
}

std::string highlight2(const std::string& text, size_t column, size_t length) {
    return fmt::format(fmt::fg(fmt::color::white), text.substr(0, column)) +
           fmt::format(fmt::fg(fmt::color::red) | fmt::emphasis::bold, text.substr(column, length)) +
           fmt::format(fmt::fg(fmt::color::white), text.substr(column + length, text.size() - (column + length)));
}


std::unordered_map<TokType, OpType> TOKEN_TO_OP = {
        {TokType::PLUS,  OpType::ADD},
        {TokType::MINUS, OpType::SUB},
        {TokType::TIMES, OpType::MUL},
        {TokType::DIV,   OpType::DIV},
        {TokType::MOD,   OpType::MOD}
};

std::unordered_map<TokType, BoolOp> TOKEN_TO_BOOL_OP = {
        {TokType::AND, BoolOp::AND},
        {TokType::OR,  BoolOp::OR},
        {TokType::LT,  BoolOp::LT},
        {TokType::GT,  BoolOp::GT},
        {TokType::LEQ, BoolOp::LEQ},
        {TokType::GEQ, BoolOp::GEQ},
        {TokType::NEQ, BoolOp::NEQ},
        {TokType::EQ,  BoolOp::EQ}
};

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

bool Parser::match(TokType type) {
    return this->token.type == type;
}

BlockNode* Parser::parse_program() {
    VectorOfNodes program;
    while (this->token.type != TokType::END) {
        program.push_back(this->parse_top_level_statement());
    }
    return new BlockNode(program);
}

ReturnNode* Parser::parse_return() {
    Token ret_tok = this->expect_token(TokType::RETURN);
    Node* expression = nullptr;
    if (!this->match(TokType::SEMICOLON)) {
        expression = this->parse_expression();
    }
    return new ReturnNode(expression, ret_tok.start);
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
        elifs.push_back(std::make_pair(elif_condition, elif_body));
    }
    BlockNode* _else = nullptr;
    if (this->match(TokType::ELSE)) {
        this->next();
        _else = this->parse_possibly_empty_block();
    }
    IfNode* iff = new IfNode(condition, body, elifs, _else);
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
        Node* node = new EmptyListNode(type);
        return node;
    } else {
        while (true) {
            Node* element = this->parse_expression();
            elements.push_back(element);
            if (!this->match(TokType::COMMA)) {
                break;
            }
            this->next();
        }
        Token list_end = this->expect_token(TokType::RSQUARE);
    }
    Node* node = new ListNode(elements);
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
    this->expect_token(TokType::RPAREN);
    return result;
}

Node* Parser::parse_assignment_or_expression() {
    Node* lvalue = this->parse_expression();
    if (item_in_vec(this->token.type, {TokType::EQQ, TokType::PLUS_EQQ, TokType::MINUS_EQQ})) {
        if (lvalue->ntype == NodeType::CALL) {
            this->error_assign_call(this->token);

        }
        TokType op = this->token.type;
        this->next();
        if (lvalue->ntype != ID && item_in_vec(op, {TokType::PLUS_EQQ, TokType::MINUS_EQQ})) {
            throw std::runtime_error("Error += or -= can only be used on ids!");
        }
        Node* rvalue = this->parse_expression();
        if (lvalue->ntype == ID) {
            if (op == TokType::PLUS_EQQ) {
                rvalue = new BinopNode(OpType::ADD, new IdNode(lvalue->id()._id), rvalue);
            } else if (op == TokType::MINUS_EQQ) {
                rvalue = new BinopNode(OpType::SUB, new IdNode(lvalue->id()._id), rvalue);
            }
        }
        Node* node = new AssignmentNode(lvalue, rvalue);
        node->start = lvalue->start;
        node->end = rvalue->end;
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
    if (this->match(TokType::OR)) {
        this->next();
        Node* right = this->parse_and_expression();
        Node* node = new BoolOpNode(BoolOp::OR, left, right, left->start);
        return node;
    }
    return left;
}

Node* Parser::parse_and_expression() {
    Node* left = this->parse_bool_expression();
    if (this->match(TokType::AND)) {
        this->next();
        Node* right = this->parse_bool_expression();
        Node* node = new BoolOpNode(BoolOp::AND, left, right, left->start);
        return node;
    }
    return left;
}

Node* Parser::parse_bool_expression() {
    Node* left = this->parse_add_or_sub_expression();
    BoolOp op;
    std::vector<TokType> boolean_tokens = {TokType::EQ, TokType::LT, TokType::GT, TokType::LEQ,
                                           TokType::GEQ, TokType::NEQ};
    if (!item_in_vec(this->token.type, boolean_tokens)) {
        return left;
    }
    op = TOKEN_TO_BOOL_OP[this->token.type];
    this->next();
    Node* right = this->parse_add_or_sub_expression();
    Node* node = new BoolOpNode(op, left, right, left->start);
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
        BinopNode* node = new BinopNode(op, left, right, left->start);
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
        BinopNode* node = new BinopNode(op, left, right, left->start);
        node->op_pos = op_token.start;
        left = node;
    }
    return left;
}

Node* Parser::parse_factor() {
    Node* parent;
    if (this->match(TokType::HASH)) {
        parent = this->parse_class_or_tuple_literal();
    } else {
        parent = this->parse_id_or_literal();
    }
    if (item_in_vec(parent->ntype, {TUPLE, NUMBER, NONE, BOOLEAN})) {
        if (item_in_vec(this->token.type, {TokType::LPAREN, TokType::LSQUARE})) {
            this->expect_token(TokType::RPAREN); // some random token
        }
    }
    parent = this->parse_call_or_subscript_chain(parent);
    while (this->match(TokType::DOT)) {
        this->next();
        Token tok;
        if (this->match(TokType::NUM)) {
            tok = this->expect_token(TokType::NUM);
            parent = new MemberNode(parent, tok.num, parent->start);
        } else {
            tok = this->expect_token(TokType::ID);
            parent = new MemberNode(parent, tok.str, parent->start);
        }
        parent = this->parse_call_or_subscript_chain(parent);
    }
    return parent;
}

Node* Parser::parse_dictionary() {
    this->expect_token(TokType::LCURLY);
    DictNode* dict = new DictNode();
    if (this->match(TokType::RCURLY)) {
        // empty dict
        this->next();
        this->expect_token(TokType::DOUBLE_COLON);
        this->expect_token(TokType::LSQUARE);
        TypeNode* key_type = this->parse_type_node();
        this->expect_token(TokType::COMMA);
        TypeNode* value_type = this->parse_type_node();
        this->expect_token(TokType::RSQUARE);
        return new EmptyDictNode(key_type, value_type);
    }
    while (true) {
        Node* key = this->parse_expression();
        this->expect_token(TokType::COLON);
        Node* value = this->parse_expression();
        dict->items.push_back(std::make_pair(key, value));
        if (this->match(TokType::COMMA)) {
            this->next();
        } else {
            this->expect_token(TokType::RCURLY);
            break;
        }
    }
    return dict;
}


Node* Parser::parse_partial_application() {
    this->expect_token(TokType::DOLLAR_SIGN);
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
    this->expect_token(TokType::RPAREN);
    auto partial = new PartialApplication(new IdNode(total_function_tok.str), args);
    partial->start = total_function_tok.start;
    return partial;
}

Node* Parser::parse_id_or_literal() {
    Node* node;
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
        case TokType::NUM: {
            node = new NumberNode(this->token.num, this->token.start);
            node->end = this->token.end_pos;
            this->next();
            break;
        }
        case TokType::FLOAT: {
            node = new FloatNode(this->token.flot);
            this->next();
            break;
        }
        case TokType::STRING: {
            node = new StringNode(this->token.str, this->token.start);
            node->end = this->token.end_pos;
            this->next();
            break;
        }
        case TokType::TRUE: {
            node = new BooleanNode(true, this->token.start);
            this->next();
            break;
        }
        case TokType::FALSE: {
            node = new BooleanNode(false, this->token.start);
            this->next();
            break;
        }
        case TokType::LSQUARE:
            return this->parse_list_literal();
        case TokType::NONE: {
            node = new NoneNode();
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

Node* Parser::parse_class_or_tuple_literal() {
    Token hash_tok = this->expect_token(TokType::HASH);

    if (this->match(TokType::LPAREN)) {
        // it's a tuple
        this->next();
        VectorOfNodes values;
        if (this->match(TokType::RPAREN)) {
            this->error_empty_tuple(hash_tok.start);
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
                    this->error_tuple_one_element(hash_tok.start);
                }
                break;
            }
        }
        this->expect_token(TokType::RPAREN);
        return new TupleNode(values);
    }

    TypeNode* type = this->parse_type_node();
    if (type->kind != Kind::OBJECT) {
        throw std::runtime_error("Expecterd a type to initialize, but got " + type->to_string());
    }
    ObjectType* otn = &type->object();
    this->expect_token(TokType::LPAREN);

    std::unordered_map<std::string, Node*> init;
    VectorOfNodes exps;
    if (!this->match(TokType::RPAREN)) {
        Node& first = *this->parse_expression();
        // if it's an id
        if (first.ntype == NodeType::ID) {
            IdNode& idn = first.id();
            if (this->match(TokType::RPAREN)) {
                this->next();
                exps.push_back(&first);
                ClassLiteralExpressionNode* clen = new ClassLiteralExpressionNode(otn, exps);
                clen->start = hash_tok.start;
                return clen;

            } else if (this->match(TokType::COMMA)) {
                // it's a list of expressions
                exps.push_back(&first);
                this->next();
                while (true) {
                    Node* exp = this->parse_expression();
                    exps.push_back(exp);
                    if (this->match(TokType::COMMA)) {
                        this->next();
                    } else {
                        break;
                    }
                }
                this->expect_token(TokType::RPAREN);
                ClassLiteralExpressionNode* clen = new ClassLiteralExpressionNode(otn, exps);
                clen->start = hash_tok.start;
                return clen;
            } else {
                // it's field:exp, field:exp
                this->expect_token(TokType::COLON);
                Node* exp = this->parse_expression();
                init[idn._id] = exp;
                if (this->match(TokType::COMMA)) {
                    this->next();
                    while (true) {
                        Token field_id = this->expect_token(TokType::ID);
                        this->expect_token(TokType::COLON);
                        exp = this->parse_expression();
                        init[field_id.str] = exp;
                        if (this->match(TokType::COMMA)) {
                            this->next();
                        } else {
                            break;
                        }
                    }
                }
                this->expect_token(TokType::RPAREN);
                ClassLiteralFieldNode* clfn = new ClassLiteralFieldNode(otn, init);
                clfn->start = hash_tok.start;
                return clfn;
            }
        } else {
            // it's a list of expressions
            exps.push_back(&first);
            if (this->match(TokType::COMMA)) {
                this->next();
                while (true) {
                    Node* exp = this->parse_expression();
                    exps.push_back(exp);
                    if (this->match(TokType::COMMA)) {
                        this->next();
                    } else {
                        break;
                    }
                }
            }
            this->expect_token(TokType::RPAREN);
            ClassLiteralExpressionNode* clen = new ClassLiteralExpressionNode(otn, exps);
            clen->start = hash_tok.start;
            return clen;
        }
    }
    this->expect_token(TokType::RPAREN);
    ClassLiteralFieldNode* clfn = new ClassLiteralFieldNode(otn, init);
    clfn->start = hash_tok.start;
    return clfn;
}

Node* Parser::parse_id_or_class_literal() {
    Node* node;
    std::string identifier = this->token.str;
    node = new IdNode(identifier);
    node->start = this->token.start;
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
            if (!this->match(TokType::RPAREN)) {
                arguments = this->parse_list_of_arguments();
            } else {
                this->next();
            }
            Node* old_node = node;
            node = new CallNode(node, arguments);
            node->start = old_node->start;
        } else if (this->match(TokType::LSQUARE)) {
//                subscript
            this->next();
            Node* value = this->parse_expression();
            Node* old_node = node;
            node = new SubscriptNode(node, {value});
            node->start = old_node->start;
            this->expect_token(TokType::RSQUARE);
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
    this->expect_token(TokType::EQQ);
    Node* expression = this->parse_expression();
    return new DeclarationNode(identifier.str, type, expression, var_token.start);
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
        case TokType::FOR: {
            return this->parse_for_loop();
        }
        case TokType::WHILE: {
            return this->parse_while_loop();
        }
        case TokType::BREAK: {
            if (!this->inside_loop) {
                this->error_out_of_loop(this->token);

            }
            this->next();
            return new BreakNode();
        }
        case TokType::CONTINUE: {
            if (!this->inside_loop) {
                this->error_out_of_loop(this->token);

            }
            this->next();
            return new ContinueNode();
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
    this->expect_token(TokType::RARROW);
    TypeNode* return_type = this->parse_type_node();
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
    return new ObjectType(identifier.str, type_parameters);
}

TypeNode* Parser::parse_type_node() {
    if (this->match(TokType::FUN)) {
        return this->parse_function_type();
    } else if (this->match(TokType::ID)) {
        return this->parse_object_type();
    } else {
        this->error_expected_type(this->token);
    }
}

BlockNode* Parser::parse_possibly_empty_block() {
    Token st = this->expect_token(TokType::LCURLY);
    VectorOfNodes block;
    while (true) {
        if (this->match(TokType::RCURLY)) {
            this->next();
            break;
        }
        Node* statement = this->parse_common_statement();
        block.push_back(statement);
    }
    return new BlockNode(block);
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

    if (this->match(TokType::RPAREN)) {
        this->next();
        // Function with no parameters
    } else {
        // Function with parameters
        // Parse parameter list

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
        this->expect_token(TokType::RPAREN);
    }
    // Parse return
    TypeNode* return_type = nullptr;
    if (this->match(TokType::RARROW)) {
        // function with return value
        this->expect_token(TokType::RARROW);
        return_type = this->parse_type_node();
    } else {
        return_type = new ObjectType(".None", {});
    }
    // Parse function body
    BlockNode* body = this->parse_possibly_empty_block();

    auto node = new FunctionNode(identifier, parameter_names, parameter_types, return_type, body);
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

Node* Parser::parse_top_level_statement() {
    switch (this->token.type) {
        case TokType::FUN:
            return this->parse_function_definition();
        case TokType::CLASS:
            return this->parse_class_definition();
        case TokType::FROM:
            return this->parse_import();
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
    ForNode* forloop = new ForNode(var.str, exp, body);
    forloop->start = for_tok.start;
    return forloop;
}

Node* Parser::parse_ternary() {
    Node* condition = this->parse_or_expression();
    if (this->match(TokType::QUESTION)) {
        this->next();
        Node* true_case = this->parse_expression();
        this->expect_token(TokType::COLON);
        Node* false_case = this->parse_expression();
        Node* node = new TernaryNode(condition, true_case, false_case);
        return node;
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
    WhileNode* whil = new WhileNode(condition, body);
    whil->start = while_tok.start;
    return whil;
}

ClassNode* Parser::parse_class_definition() {
    Token class_tok = this->expect_token(TokType::CLASS);
    Token class_name_tk = this->expect_token(TokType::ID);
    VectorOfStrings type_parameters;
    if (this->match(TokType::LSQUARE)) {
        this->next();

        while (true) {
            Token type_param_tk = this->expect_token(TokType::ID);
            type_parameters.push_back(type_param_tk.str);
            if (!this->match(TokType::COMMA)) {
                break;
            }
        }
        this->expect_token(TokType::RSQUARE);
    }
    this->expect_token(TokType::LCURLY);
    std::unordered_map<std::string, FunctionNode*> methods;
    MapStringType members;
    VectorOfStrings members_ordered;
    while (true) {
        if (this->match(TokType::ID)) {
            Token member_name_tk = this->expect_token(TokType::ID);
            this->expect_token(TokType::COLON);
            TypeNode* member_type = this->parse_type_node();
            if (members.find(member_name_tk.str) != members.end() ||
                methods.find(member_name_tk.str) != methods.end()) {
                this->error_class_member_redefined(class_name_tk.str, member_name_tk.str, member_name_tk.start);

            }
            members[member_name_tk.str] = member_type;
            members_ordered.push_back(member_name_tk.str);
            this->expect_token(TokType::SEMICOLON);
        } else if (this->match(TokType::FUN)) {
            FunctionNode* method_node = this->parse_function_definition();
            if (members.find(method_node->identifier) != members.end() ||
                methods.find(method_node->identifier) != methods.end()) {
                this->error_class_member_redefined(class_name_tk.str, method_node->identifier, method_node->start);

            }
            methods.insert(make_pair(method_node->identifier, method_node));
        } else {
            break;
        }
    }
    this->expect_token(TokType::RCURLY);
    ClassNode* c = new ClassNode(class_name_tk.str, type_parameters, members, methods);
    c->members_ordered = members_ordered;
    c->start = class_tok.start;
    return c;
}

ImportNode* Parser::parse_import() {
    this->expect_token(TokType::FROM);
    Token module_tok = this->expect_token(TokType::ID);
    this->expect_token(TokType::IMPORT);
    VectorOfStrings imports;
    while (true) {
        Token import_tok = this->expect_token(TokType::ID);
        imports.push_back(import_tok.str);
        if (this->match(TokType::COMMA)) {
            this->next();
            continue;
        } else {
            break;
        }
    }
    this->expect_token(TokType::SEMICOLON);
    auto x = new ImportNode(module_tok.str, imports);
    return x;
}


