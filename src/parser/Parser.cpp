//
// Created by chris on 7/6/20.
//

#include "Parser.h"
#include "../semantic/GlobalProcessor.h"
#include "../ast/PartialApplication.h"
#include "../ast/UnaryOp.h"
#include "../logging/logging.h"
#include "../ast/DefaultConstructor.h"
#include <fmt/core.h>
#include <fmt/color.h>
#include <exception>
#include <set>
#include "../ast/ObjectType.h"

std::unordered_map<TokType, OpType> TOKEN_TO_OP = {{TokType::PLUS,  OpType::ADD},
                                                   {TokType::MINUS, OpType::SUB},
                                                   {TokType::TIMES, OpType::MUL},
                                                   {TokType::DIV,   OpType::DIV},
                                                   {TokType::MOD,   OpType::MOD},
                                                   {TokType::AND,   OpType::AND},
                                                   {TokType::OR,    OpType::OR},
                                                   {TokType::LT,    OpType::LT},
                                                   {TokType::GT,    OpType::GT},
                                                   {TokType::LEQ,   OpType::LE},
                                                   {TokType::GEQ,   OpType::GE},
                                                   {TokType::NEQ,   OpType::NE},
                                                   {TokType::EQ,    OpType::EQ}};

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

std::unique_ptr<ast::Block> Parser::parse_program() {
    VectorOfNodesU program;
    TextPosition start = this->token.start;
    while (this->token.type != TokType::END) {
        program.push_back(this->parse_top_level_statement());
    }
    TextPosition end = this->token.end_pos;
    std::cout << "--------------- FINISHED PARSING -----------------" << std::endl;
    return ast::Block::make(std::move(program), start, end);
}

std::unique_ptr<ast::Return> Parser::parse_return() {
    Token ret_tok = this->expect_token(TokType::RETURN);
    UNode expression = nullptr;
    TextPosition end = ret_tok.end_pos;
    if (!this->match(TokType::SEMICOLON)) {
        expression = this->parse_expression();
        end = expression->end;
    }
    return std::make_unique<ast::Return>(expression, ret_tok.start, end);
}

std::unique_ptr<ast::If> Parser::parse_if() {
    Token if_tok = this->expect_token(TokType::IF);
    auto condition = this->parse_expression();
    auto body = this->parse_possibly_empty_block();
    std::vector<std::pair<UNode, UBlockNode>> elifs;
    while (this->match(TokType::ELIF)) {
        this->next();
        auto elif_condition = this->parse_expression();
        auto elif_body = this->parse_possibly_empty_block();
        elifs.emplace_back(std::move(elif_condition), std::move(elif_body));
    }
    std::unique_ptr<ast::Block> _else = nullptr;
    if (this->match(TokType::ELSE)) {
        this->next();
        _else = this->parse_possibly_empty_block();
    }
    auto iff = std::make_unique<ast::If>(condition, body, std::move(elifs), _else, if_tok.start, if_tok.end_pos);
    iff->start = if_tok.start;
    return iff;
}

UNode Parser::parse_list_literal() {
    Token list_start = this->expect_token(TokType::LSQUARE);
    VectorOfNodesU elements;
    if (this->match(TokType::RSQUARE)) {
        // empty list
        this->next();
        // parse required type annotation (cannot infer type of empty list
        this->expect_token(TokType::DOUBLE_COLON);
        auto type = this->parse_type_node();
        auto node = std::make_unique<ast::EmptyList>(std::move(type), list_start.start, list_start.end_pos);
        return node;
    }
    while (true) {
        auto element = this->parse_expression();
        elements.push_back(std::move(element));
        if (!this->match(TokType::COMMA)) {
            break;
        }
        this->next();
    }
    Token list_end = this->expect_token(TokType::RSQUARE);
    auto node = std::make_unique<ast::List>(elements, list_start.start, list_end.start);
    return node;
}

VectorOfNodes Parser::parse_list_of_expressions() {
    VectorOfNodes result;
    while (true) {
        result.push_back(this->parse_expression().release());
        if (this->match(TokType::COMMA)) {
            this->next();
        } else {
            break;
        }
    }
    return result;
}

VectorOfNodesU Parser::parse_list_of_arguments() {
    VectorOfNodesU result;
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

UNode Parser::parse_assignment_or_expression() {
    auto lvalue = this->parse_expression();
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
        auto rvalue = this->parse_expression();
        if (lvalue->ntype == NodeType::ID) {
            UNode id_node = ast::Id::make(((ast::Id&) *lvalue)._id, lvalue->start, lvalue->end);
            if (op == TokType::PLUS_EQQ || op == TokType::MINUS_EQQ) {
                OpType opt;
                if (op == TokType::PLUS_EQQ) {
                    opt = OpType::ADD;
                } else if (op == TokType::MINUS_EQQ) {
                    opt = OpType::SUB;
                }
                auto bnode = ast::BinaryOp::make(opt, id_node, rvalue, id_node->start, rvalue->end);
                bnode->op_pos = op_pos;
                rvalue = std::move(bnode);
            }

        }
        auto node = std::make_unique<ast::Assignment>(lvalue, rvalue, lvalue->start, rvalue->end);
        node->start = op_pos;
        return node;
    } else {
        if (lvalue->ntype != NodeType::CALL) {
            this->error_expected_statement(this->token.start);
        }
    }
    return lvalue;
}

UNode Parser::parse_expression() {
    return this->parse_ternary();
}

UNode Parser::parse_or_expression() {
    auto left = this->parse_and_expression();
    while (this->match(TokType::OR)) {
        this->next();
        auto right = this->parse_and_expression();
        auto node = std::make_unique<ast::BinaryOp>(OpType::OR, left, right, left->start, right->end);
        left = std::move(node);
    }
    return left;
}

UNode Parser::parse_and_expression() {
    auto left = this->parse_not_expression();
    while (this->match(TokType::AND)) {
        this->next();
        auto right = this->parse_not_expression();
        auto node = std::make_unique<ast::BinaryOp>(OpType::AND, left, right, left->start, right->end);
        left = std::move(node);
    }
    return left;
}

UNode Parser::parse_not_expression() {
    if (this->match(TokType::NOT)) {
        Token not_tok = this->expect_token(TokType::NOT);
        auto left = this->parse_bool_expression();
        left = std::make_unique<ast::UnaryOp>(UnaryOpType::NOT, left.release(), not_tok.start, left->end);
        return left;
    }
    return this->parse_bool_expression();
}

UNode Parser::parse_bool_expression() {
    auto left = this->parse_add_or_sub_expression();
    OpType op;
    std::vector<TokType> boolean_tokens = {TokType::EQ, TokType::LT, TokType::GT, TokType::LEQ, TokType::GEQ,
                                           TokType::NEQ};
    if (!item_in_vec(this->token.type, boolean_tokens)) {
        return left;
    }
    op = TOKEN_TO_OP[this->token.type];
    this->next();
    auto right = this->parse_add_or_sub_expression();
    auto node = std::make_unique<ast::BinaryOp>(op, left, right, left->start, right->end);
    return node;
}

UNode Parser::parse_add_or_sub_expression() {
    auto left = this->parse_mul_div_or_mod_expression();
    OpType op;
    while (item_in_vec(this->token.type, {TokType::PLUS, TokType::MINUS})) {
        op = TOKEN_TO_OP[this->token.type];
        Token op_token = this->token;
        this->next();
        auto right = this->parse_mul_div_or_mod_expression();
        auto node = ast::BinaryOp::make(op, left, right, left->start, right->end);
        node->op_pos = op_token.start;
        left = std::move(node);
    }
    return left;
}

UNode Parser::parse_mul_div_or_mod_expression() {
    auto left = this->parse_factor();
    OpType op;
    while (item_in_vec(this->token.type, {TokType::TIMES, TokType::DIV, TokType::MOD})) {
        op = TOKEN_TO_OP[this->token.type];
        Token op_token = this->token;
        this->next();
        auto right = this->parse_factor();
        auto node = ast::BinaryOp::make(op, left, right, left->start, right->end);
        node->op_pos = op_token.start;
        left = std::move(node);
    }
    return left;
}

UNode Parser::parse_factor() {
    UNode parent;
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
            auto mn = std::make_unique<ast::Member>(parent, tok);
            mn->dot_pos = dot_pos;
            parent = std::move(mn);
        } else {
            tok = this->expect_token(TokType::ID);
            auto mn = std::make_unique<ast::Member>(parent, tok);
            mn->dot_pos = dot_pos;
            parent = std::move(mn);
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

UNode Parser::parse_dictionary() {
    Token lcurly = this->expect_token(TokType::LCURLY);
    if (this->match(TokType::RCURLY)) {
        // empty dict
        this->next();
        this->expect_token(TokType::DOUBLE_COLON);
        this->expect_token(TokType::LSQUARE);
        UTypeNode key_type = this->parse_type_node();
        this->expect_token(TokType::COMMA);
        UTypeNode value_type = this->parse_type_node();
        Token rsquare = this->expect_token(TokType::RSQUARE);
        return std::make_unique<ast::EmptyDict>(key_type, value_type, lcurly.start, rsquare.end_pos);
    }
    std::vector<std::pair<UNode, UNode>> items;
    Token rcurly;
    while (true) {
        auto key = this->parse_expression();
        this->expect_token(TokType::COLON);
        auto value = this->parse_expression();
        items.emplace_back(std::move(key), std::move(value));
        if (this->match(TokType::COMMA)) {
            this->next();
        } else {
            rcurly = this->expect_token(TokType::RCURLY);
            break;
        }
    }
    auto dict = std::make_unique<ast::DictNode>(items, lcurly.start, rcurly.end_pos);
    return dict;
}


UNode Parser::parse_partial_application() {
    Token dollar = this->expect_token(TokType::DOLLAR_SIGN);
    Token total_function_tok = this->expect_token(TokType::ID);
    this->expect_token(TokType::LPAREN);
    VectorOfNodes args;
    while (true) {
        if (this->match(TokType::TIMES)) {
            this->next();
            args.push_back(nullptr);
        } else {
            args.push_back(this->parse_expression().release());
        }
        if (this->match(TokType::COMMA)) {
            this->next();
        } else {
            break;
        }
    }
    Token close = this->expect_token(TokType::RPAREN);
    auto partial = std::make_unique<ast::PartialApplication>(std::make_unique<ast::Id>(total_function_tok.str,
                                                                                       total_function_tok.start,
                                                                                       total_function_tok.end_pos),
                                                             args,
                                                             dollar.start,
                                                             close.end_pos);
    partial->start = total_function_tok.start;
    return partial;
}

UNode Parser::parse_id_or_literal() {
    UNode node = nullptr;
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
            node = ast::Number::make(NumberType::INTEGER, this->token.str, this->token.start, this->token.end_pos);
            node->end = this->token.end_pos;
            this->next();
            break;
        }
        case TokType::FLOAT: {
            node = ast::Number::make(NumberType::FLOAT, this->token.str, this->token.start, this->token.end_pos);
            node->end = this->token.end_pos;
            this->next();
            break;
        }
        case TokType::DOUBLE: {
            node = ast::Number::make(NumberType::DOUBLE, this->token.str, this->token.start, this->token.end_pos);
            node->end = this->token.end_pos;
            this->next();
            break;
        }
        case TokType::STRING: {
            node = std::make_unique<ast::String>(this->token.str, this->token.start, this->token.end_pos);
            node->end = this->token.end_pos;
            this->next();
            break;
        }
        case TokType::TRUE: {
            node = std::make_unique<ast::Boolean>(true, this->token.start, this->token.end_pos);
            this->next();
            break;
        }
        case TokType::FALSE: {
            node = std::make_unique<ast::Boolean>(false, this->token.start, this->token.end_pos);
            this->next();
            break;
        }
        case TokType::LSQUARE:
            return this->parse_list_literal();
        case TokType::NONE: {
            node = std::make_unique<ast::None>(this->token.start, this->token.end_pos);
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

UNode Parser::parse_tuple_or_constructor() {
    Token hash_tok = this->token;
    UNode parent;
    this->next();
    if (this->match(TokType::ID)) {
        Token idd = this->expect_token(TokType::ID);
        UNode m = ast::Id::make(idd.str, this->token.start, this->token.end_pos);
        while (this->match(TokType::DOT)) {
            this->next();
            idd = this->expect_token(TokType::ID);
            m = std::make_unique<ast::Member>(m, idd);
        }
        parent = std::make_unique<ast::DefaultConstructor>(m.release(), hash_tok.start, m->end);
    } else {
        parent = this->parse_tuple_literal();
        parent->start = hash_tok.start;
    }
    return parent;
}

UNode Parser::parse_tuple_literal() {
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
        auto exp = this->parse_expression();
        values.emplace_back(exp.release());
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
    return std::make_unique<ast::Tuple>(values, st.start, close.end_pos);
}

UNode Parser::parse_id_or_class_literal() {
    std::string identifier = this->token.str;
    auto node = ast::Id::make(identifier, this->token.start, this->token.end_pos);
    this->next();
    return node;
}

UNode Parser::parse_call_or_subscript_chain(UNode& parent) {
    UNode node = std::move(parent);
    while (item_in_vec(this->token.type, {TokType::LPAREN, TokType::LSQUARE})) {
        if (this->match(TokType::LPAREN)) {
//                 function call
            this->next();
            VectorOfNodesU arguments;
            Token close;
            if (this->match(TokType::RPAREN)) {
                close = this->token;
                this->next();
            } else {
                arguments = this->parse_list_of_arguments();
                close = this->expect_token(TokType::RPAREN);
            }
            UNode old_node = std::move(node);
            TextPosition o_start = old_node->start;
            node = std::make_unique<ast::Call>(old_node, arguments, old_node->start, close.end_pos);
            node->start = o_start;
        } else if (this->match(TokType::LSQUARE)) {
//                subscript
            Token lsquare = this->token;
            this->next();
            auto value = this->parse_expression();
            Token close = this->expect_token(TokType::RSQUARE);
            UNode old_node = std::move(node);
            VectorOfNodesU v;
            v.push_back(std::move(value));
            node = std::make_unique<ast::Subscript>(old_node, v, old_node->start, close.end_pos);
        }
    }
    return node;
}

std::unique_ptr<ast::Declaration> Parser::parse_variable_declaration() {
    Token var_token = this->expect_token(TokType::VAR);
    Token identifier = this->expect_token(TokType::ID);
    TypeNode* type = nullptr;
    if (this->match(TokType::COLON)) {
        this->next();
        type = this->parse_type_node().release();
    }
    Token eq_tok = this->expect_token(TokType::EQQ);
    auto expression = this->parse_expression();
    return std::make_unique<ast::Declaration>(identifier.str,
                                              type,
                                              expression,
                                              var_token.start,
                                              eq_tok.start,
                                              expression->end);
}

UNode Parser::parse_common_statement() {
    switch (this->token.type) {
        case TokType::IF: {
            return this->parse_if();
        }
        case TokType::VAR: {
            std::unique_ptr<ast::Declaration> node = this->parse_variable_declaration();
            this->expect_token(TokType::SEMICOLON);
            return node;
        }
        case TokType::RETURN: {
            std::unique_ptr<ast::Return> node = this->parse_return();
            this->expect_token(TokType::SEMICOLON);
            return node;
        }
        case TokType::THROW: {
            // Throwauto node = this->parse_throw();
            // this->expect_token(TokType::SEMICOLON);
            // return node;
            return nullptr;
        }
        case TokType::TRY: {
            // TryCatchauto node = this->parse_try_catch();
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
            return std::make_unique<ast::Break>(tok.start, tok.end_pos);
        }
        case TokType::CONTINUE: {
            if (!this->inside_loop) {
                this->error_out_of_loop(this->token);

            }
            Token tok = this->token;
            this->next();
            return std::make_unique<ast::Continue>(tok.start, tok.end_pos);
        }
        default: {
            auto node = this->parse_assignment_or_expression();
            this->expect_token(TokType::SEMICOLON);
            return node;
        }
    }
}

std::unique_ptr<FunctionType> Parser::parse_function_type() {
    this->expect_token(TokType::FUN);
    this->expect_token(TokType::LPAREN);
    VectorOfTypes parameter_types;
    if (!this->match(TokType::RPAREN)) {
        while (true) {
            TypeNode* parameter_type = this->parse_type_node().release();
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
        return_type = this->parse_type_node().release();
    } else {
        return_type = new ObjectType(".None");
    }
    return std::make_unique<FunctionType>(parameter_types, return_type);
}

std::unique_ptr<ObjectType> Parser::parse_object_type() {
    Token identifier = this->expect_token(TokType::ID);
    VectorOfTypes type_parameters;
    if (this->match(TokType::LSQUARE)) {
        this->next();
        while (true) {
            TypeNode* type_parameter = this->parse_type_node().release();
            type_parameters.push_back(type_parameter);
            if (this->match(TokType::COMMA)) {
                this->next();
            } else {
                break;
            }
        }
        this->expect_token(TokType::RSQUARE);
    }
    auto ot = std::make_unique<ObjectType>(identifier.str, type_parameters);
    if (ot->id.size() == 1 && (islower(ot->id[0]) != 0)) {
        ot->is_generic_param = true;
    }
    return ot;
}

std::unique_ptr<TypeNode> Parser::parse_type_node() {
    if (this->match(TokType::FUN)) {
        return this->parse_function_type();
    } else if (this->match(TokType::ID)) {
        return this->parse_object_type();
    } else {
        this->error_expected_type(this->token);
    }
    return nullptr;
}

std::unique_ptr<ast::Block> Parser::parse_possibly_empty_block() {
    Token st = this->expect_token(TokType::LCURLY);
    VectorOfNodesU block;
    Token end;
    while (true) {
        if (this->match(TokType::RCURLY)) {
            end = this->token;
            this->next();
            break;
        }
        block.push_back(this->parse_common_statement());
    }
    return ast::Block::make(std::move(block), st.start, end.end_pos);
}

std::unique_ptr<ast::Function> Parser::parse_function_definition() {
    Token fun_tok = this->expect_token(TokType::FUN);
    Token matched_token = this->expect_token(TokType::ID);
    std::string identifier = matched_token.str;
    this->expect_token(TokType::LPAREN);
    VectorOfUTypes parameter_types;
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
                UTypeNode parameter_type = this->parse_type_node();
                parameter_types.push_back(std::move(parameter_type));
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
            FunctionType* ft = this->parse_function_type().release();
            std::cout << is_static << std::endl;
            std::cout << ft->to_json() << std::endl;
            implicit = new Implicit{parent.str, child.str, ft, is_static};
        }
        this->expect_token(TokType::RPAREN);
    }
    // Parse return
    UTypeNode return_type;
    if (this->match(TokType::RARROW)) {
        // function with return value
        this->expect_token(TokType::RARROW);
        return_type = this->parse_type_node();
    } else {
        return_type = std::make_unique<ObjectType>(".None");
    }
    // Parse function body
    auto body = this->parse_possibly_empty_block();

    auto node = std::make_unique<ast::Function>(identifier,
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

std::unique_ptr<ast::Alias> Parser::parse_alias() {
    Token alias_tk = this->expect_token(TokType::ALIAS);
    Token alias_id = this->expect_token(TokType::ID);
    this->expect_token(TokType::EQQ);
    TypeNode* aliased_type = this->parse_type_node().release();
    Token semic_tk = this->expect_token(TokType::SEMICOLON);
    auto node = std::make_unique<ast::Alias>(alias_id.str, aliased_type, alias_tk.start, semic_tk.end_pos);
    return node;
}

UNode Parser::parse_top_level_statement() {
    switch (this->token.type) {
        case TokType::FUN:
            return this->parse_function_definition();
        case TokType::CLASS:
            return this->parse_class_definition();
        case TokType::TYPECLASS:
            return this->parse_typeclass();
        case TokType::INSTANCE:
            return this->parse_instance();
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


std::unique_ptr<ast::For> Parser::parse_for_loop() {
    Token for_tok = this->expect_token(TokType::FOR);
    bool expect_paren = false;
    if (this->match(TokType::LPAREN)) {
        this->next();
        expect_paren = true;
    }
    Token var = this->expect_token(TokType::ID);
    this->expect_token(TokType::ARROBA);
    auto exp = this->parse_expression();
    if (expect_paren) {
        this->expect_token(TokType::RPAREN);
    }
    bool prev = this->inside_loop;
    this->inside_loop = true;
    auto body = this->parse_possibly_empty_block();
    this->inside_loop = prev;
    auto forloop = std::make_unique<ast::For>(var.str, exp, body, for_tok.start, body->end);
    forloop->start = for_tok.start;
    return forloop;
}

UNode Parser::parse_ternary() {
    auto condition = this->parse_or_expression();
    if (this->match(TokType::QUESTION)) {
        this->next();
        // auto true_case = this->parse_expression();
        // this->expect_token(TokType::COLON);
        // auto false_case = this->parse_expression();
        // auto node = std::make_unique<TernaryNode>(condition, true_case, false_case, condition->start, false_case->end);
        // return node;
    }
    return condition;
}

std::unique_ptr<ast::While> Parser::parse_while_loop() {
    Token while_tok = this->expect_token(TokType::WHILE);
    auto condition = this->parse_expression();
    bool prev = this->inside_loop;
    this->inside_loop = true;
    auto body = this->parse_possibly_empty_block();
    this->inside_loop = prev;
    auto whil = std::make_unique<ast::While>(condition, body, while_tok.start, body->end);
    whil->start = while_tok.start;
    return whil;
}

std::unique_ptr<ast::Klass> Parser::parse_class_definition() {
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
    std::unordered_map<std::string, UFunctionNode> static_methods;
    std::vector<std::pair<std::string, UTypeNode>> members;
    std::set<std::string> member_names;
    std::map<std::string, std::pair<TypeNode*, ast::Node*>> static_members;
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
            UTypeNode member_type = this->parse_type_node();
            std::string& member_name = member_name_tk.str;
            if (member_names.find(member_name) != member_names.end() || methods.find(member_name) != methods.end()) {
                this->error_class_member_redefined(class_name, member_name, member_name_tk.start);
            }
            if (is_static) {
                this->expect_token(TokType::EQQ);
                auto init_expression = this->parse_expression();
                static_members[member_name] = std::make_pair(member_type.release(), init_expression.release());
            } else {
                members.push_back({member_name, std::move(member_type)});
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
            FunctionType* ft = this->parse_function_type().release();
            this->expect_token(TokType::SEMICOLON);
            std::cout << is_static << std::endl;
            std::cout << ft->to_json() << std::endl;
            Implicit* implicit = new Implicit{parent.str, child.str, ft, is_static};

            auto method_node_u = this->parse_function_definition();
            auto method_node = method_node_u.release();
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
            auto method_node = this->parse_function_definition();
            std::string& method_name = method_node->identifier;
            if (member_names.find(method_name) != member_names.end() || methods.find(method_name) != methods.end()) {
                this->error_class_member_redefined(class_name, method_name, method_node->start);
            }
            if (is_static) {
                static_methods.insert(std::make_pair(method_name, std::move(method_node)));
            } else {
                auto method_node_p = method_node.release();
                methods[method_name] = Method{nullptr, method_node_p};
            }
        } else {
            break;
        }
    }
    Token end = this->expect_token(TokType::RCURLY);
    auto c = std::make_unique<ast::Klass>(class_name,
                                          type_parameters,
                                          std::move(members),
                                          methods,
                                          static_members,
                                          static_methods,
                                          class_tok.start,
                                          end.end_pos);
    c->members_ordered = members_ordered;
    c->start = class_tok.start;
    return c;
}

std::unique_ptr<ast::Import> Parser::parse_import() {
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
        return std::make_unique<ast::Import>(path, alias.str, import_tok.start, path_part.end_pos);
    }
    this->expect_token(TokType::SEMICOLON);
    return std::make_unique<ast::Import>(path, import_tok.start, path_part.end_pos);
}

std::unique_ptr<ast::Match> Parser::parse_match_statement() {
    Token mtk = this->expect_token(TokType::MATCH);
    auto exp = this->parse_expression();
    this->expect_token(TokType::LCURLY);

    std::vector<std::string> ids;
    std::vector<std::pair<TypeNode*, ast::Block*>> cases;
    while (true) {
        Token id = this->expect_token(TokType::ID);
        this->expect_token(TokType::COLON);
        TypeNode* type = this->parse_type_node().release();
        auto body = this->parse_possibly_empty_block();
        ids.push_back(id.str);
        cases.emplace_back(type, body.release());
        if (this->match(TokType::RCURLY)) {
            break;
        }
    }
    Token lcurly = this->expect_token(TokType::RCURLY);
    return std::make_unique<ast::Match>(exp.release(), ids, cases, mtk.start, lcurly.end_pos);
}


std::unique_ptr<ast::Typeclass> Parser::parse_typeclass() {
    this->expect_token(TokType::TYPECLASS);
    Token typeclass_id = this->expect_token(TokType::ID);
    this->expect_token(TokType::LSQUARE);
    Token base_type = this->expect_token(TokType::ID);
    this->expect_token(TokType::RSQUARE);
    this->expect_token(TokType::LCURLY);

    std::unordered_map<std::string, UFunctionType> methods;

    while (true) {
        if (!this->match(TokType::FUN)) {
            break;
        }
        this->expect_token(TokType::FUN);
        Token method_id = this->expect_token(TokType::ID);
        VectorOfTypes parameter_types;
        // VectorOfStrings parameter_names;
        this->expect_token(TokType::LPAREN);
        while (true) {
            if (this->match(TokType::RPAREN)) {
                break;
            }
            Token parameter_identifier = this->expect_token(TokType::ID);
            this->expect_token(TokType::COLON);
            std::unique_ptr<TypeNode> parameter_type = this->parse_type_node();
            parameter_types.push_back(parameter_type.release());
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
            return_type = this->parse_type_node().release();
        } else {
            return_type = new ObjectType(".None");
        }

        auto ft = std::make_unique<FunctionType>(parameter_types, return_type);
        methods[method_id.str] = std::move(ft);
        this->expect_token(TokType::SEMICOLON);
        if (!this->match(TokType::FUN)) {
            break;
        }
    }

    Token final_curly = this->expect_token(TokType::RCURLY);
    auto n = std::make_unique<ast::Typeclass>(typeclass_id.str,
                                              base_type.str,
                                              std::move(methods),
                                              typeclass_id.start,
                                              final_curly.end_pos);
    return n;
}