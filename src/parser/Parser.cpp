//
// Created by chris on 7/6/20.
//

#include "Parser.h"
#include "UnexpectedToken.h"
#include "../nodes/InstanceNode.h"
#include "../semantic/GlobalProcessor.h"
#include <exception>

std::map<TokType, OpType> TOKEN_TO_OP = {
        {TokType::PLUS,  OpType::ADD},
        {TokType::MINUS, OpType::SUB},
        {TokType::TIMES, OpType::MUL},
        {TokType::DIV,   OpType::DIV},
        {TokType::MOD,   OpType::MOD},
        {TokType::LT,    OpType::LT},
        {TokType::GT,    OpType::GT},
        {TokType::LEQ,   OpType::LEQ},
        {TokType::GEQ,   OpType::GEQ},
        {TokType::EQ,    OpType::EQ},
        {TokType::NEQ,   OpType::NEQ},
};

Parser::Parser(std::vector<Token>& tokens) {
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

BlockNode Parser::parse_program() {
    VectorOfNodes program;
    while (this->token.type != TokType::END) {
        program.push_back(this->parse_top_level_statement());
    }
    return BlockNode(program);
}

ReturnNode& Parser::parse_return() {
    this->expect_token(TokType::RETURN);
    NodeContainer expression;
    if (!this->match(TokType::SEMICOLON)) {
        expression = this->parse_expression();
    }
    return *(NodeFactory::retrn(expression).node.retrn);
}

IfNode& Parser::parse_if() {
    this->expect_token(TokType::IF);
    NodeContainer condition = this->parse_expression();
    BlockNode& body = this->parse_possibly_empty_block();
    std::vector<std::pair<NodeContainer, std::reference_wrapper<BlockNode>>> elifs;
    while (this->match(TokType::ELIF)) {
        this->next();
        NodeContainer elif_condition = this->parse_expression();
        BlockNode& elif_body = this->parse_possibly_empty_block();
        elifs.push_back(std::pair<NodeContainer, std::reference_wrapper<BlockNode>>(elif_condition, elif_body));
    }
    NodeContainer _else;
    if (this->match(TokType::ELSE)) {
        this->next();
        _else = NodeContainer(&this->parse_possibly_empty_block());
    }
//    IfNode* node = IF(condition, body, elifs, _else);
    return *NodeFactory::iff(condition, body, elifs, _else).node.iff;
}

NodeContainer Parser::parse_list_literal() {
    Token list_start = this->expect_token(TokType::LSQUARE);
    int start = list_start.start;
    VectorOfNodes elements;
    int end = 123;
    if (this->match(TokType::RSQUARE)) {
        // empty list
        end = this->token.end;
        this->next();
        // parse required type annotation (cannot infer type of empty list
        this->expect_token(TokType::DOUBLE_COLON);
        TypeNode& type = this->parse_type_node();
        NodeContainer node = NodeFactory::emptylst(type);
        return node;
    } else {
        while (true) {
            NodeContainer element = this->parse_expression();
            elements.push_back(element);
            if (!this->match(TokType::COMMA)) {
                break;
            }
            this->next();
        }
        Token list_end = this->expect_token(TokType::RSQUARE);
        end = list_end.end;
    }
    NodeContainer node = NodeFactory::lst(elements);
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

NodeContainer Parser::parse_assignment_or_expression() {
    NodeContainer lvalue = this->parse_expression();
//    auto call = TO_CALL(lvalue);
    if (this->match(TokType::EQQ)) {
        if (lvalue.type == NodeContainer::CALL) {
            throw std::runtime_error("Can't assign to a function call!");
        }
        this->next();
        NodeContainer rvalue = this->parse_expression();
        NodeContainer node = NodeFactory::assign(lvalue, rvalue);
//        node->start = lvalue->start;
//        node->end = rvalue->end;
        return node;
    } else {
        if (lvalue.type != NodeContainer::CALL) {
            throw std::runtime_error(
                    "Only function calls are allowed here! No ID, NUM, SUBSCRIPT, BINOP or other expression!");
        }
    }
    return lvalue;
}

NodeContainer Parser::parse_expression() {
    return this->parse_ternary();
}

NodeContainer Parser::parse_or_expression() {
    NodeContainer left = this->parse_and_expression();
    if (this->match(TokType::OR)) {
        this->next();
        NodeContainer right = this->parse_and_expression();
        NodeContainer node = NodeFactory::binop(OpType::OR, left, right);
//        BinopNode* node = BIN(OpType::OR, left, right);
//        node->start = left->start;
//        node->end = right->end;
        return node;
    }
    return left;
}

NodeContainer Parser::parse_and_expression() {
    NodeContainer left = this->parse_bool_expression();
    if (this->match(TokType::AND)) {
        this->next();
        NodeContainer right = this->parse_bool_expression();
        NodeContainer node = NodeFactory::binop(OpType::AND, left, right);
//        node->start = left->start;
//        node->end = right->end;
        return node;
    }
    return left;
}


NodeContainer Parser::parse_bool_expression() {
    NodeContainer left = this->parse_add_or_sub_expression();
    OpType op;
    std::vector<TokType> boolean_tokens = {TokType::EQ, TokType::LT, TokType::GT, TokType::LEQ,
                                           TokType::GEQ, TokType::NEQ};
    if (!item_in_vec(this->token.type, boolean_tokens)) {
        return left;
    }
    op = TOKEN_TO_OP[this->token.type];
    this->next();
    NodeContainer right = this->parse_add_or_sub_expression();
    NodeContainer node = NodeFactory::binop(op, left, right);
//    node->start = left->start;
//    node->end = right->end;
    return node;
}

NodeContainer Parser::parse_add_or_sub_expression() {
    NodeContainer left = this->parse_mul_div_or_mod_expression();
    OpType op;
    while (item_in_vec(this->token.type, {TokType::PLUS, TokType::MINUS})) {
        op = TOKEN_TO_OP[this->token.type];
        this->next();
        NodeContainer right = this->parse_mul_div_or_mod_expression();
        NodeContainer node = NodeFactory::binop(op, left, right);
//        node->start = left->start;
//        node->end = right->end;
        left = node;

    }
    return left;
}

NodeContainer Parser::parse_mul_div_or_mod_expression() {
    NodeContainer left = this->parse_factor();
    OpType op;
    while (item_in_vec(this->token.type, {TokType::TIMES, TokType::DIV, TokType::MOD})) {
        op = TOKEN_TO_OP[this->token.type];
        this->next();
        NodeContainer right = this->parse_factor();
        NodeContainer node = NodeFactory::binop(op, left, right);
//        node->start = left->start;
//        node->end = right->end;
        left = node;
    }
    return left;
}

NodeContainer Parser::parse_factor() {
    NodeContainer parent;
    if (this->match(TokType::HASH)) {
        this->next();
        parent = this->parse_class_literal();
    } else {
        parent = this->parse_id_or_literal();
    }
    parent = this->parse_call_or_subscript_chain(parent);
    while (this->match(TokType::DOT)) {
        this->next();
        Token id = this->expect_token(TokType::ID);
        parent = NodeFactory::member(parent, id.str);
        parent = this->parse_call_or_subscript_chain(parent);
    }
    return parent;
}

NodeContainer Parser::parse_id_or_literal() {
    NodeContainer node;
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
            node = NodeFactory::number(this->token.num);
//            node->line = this->token.line;
//            node->column = this->token.column;
//            node->start = this->token.start;
//            node->end = this->token.end;
            this->next();
            break;
        }
        case TokType::STRING: {
            node = NodeFactory::strng(this->token.str);
            this->next();
            break;
        }
        case TokType::TRUE: {
            node = NodeFactory::boolean(true);
//            node->start = this->token.start;
//            node->end = this->token.end;
            this->next();
            break;
        }
        case TokType::FALSE: {
            node = NodeFactory::boolean(false);
//            node->start = this->token.start;
//            node->end = this->token.end;
            this->next();
            break;
        }
        case TokType::LSQUARE:
            return this->parse_list_literal();
        case TokType::NONE: {
            node = NodeFactory::none();
//            node->start = this->token.start;
//            node->end = this->token.end;
            this->next();
            return node;
        }
        default:
            throw UnexpectedToken(token,
                                  {TokType::STRING, TokType::NUM, TokType::ID, TokType::FUN,
                                   TokType::LPAREN});
//            throw std::runtime_error("parsing id or literal, unknown token type: " + TOKEN_STRINGS[token.type]);
    }
    return node;
}

NodeContainer Parser::parse_class_literal() {

    TypeNode& type = this->parse_type_node();
    if (type.kind != Kind::OBJECT) {
        throw std::runtime_error("Expecterd a type to initialize, but got " + type.to_string());
    }
    ObjectTypeNode& otn = *type.otype;
    this->expect_token(TokType::LCURLY);

    std::map<std::string, NodeContainer> init;
    std::vector<NodeContainer> exps;
    if (!this->match(TokType::RCURLY)) {
        NodeContainer first = this->parse_expression();
        // if it's an id
//        IdNode* idn = TO_ID(first);
        if (first.type == NodeContainer::ID) {
            IdNode* idn = first.node.id;
            if (this->match(TokType::RCURLY)) {
                exps.push_back(first);
                return NodeFactory::clslitexp(otn, exps);

            } else if (this->match(TokType::COMMA)) {
                // it's a list of expressions
                exps.push_back(first);
                this->next();
                while (true) {
                    NodeContainer exp = this->parse_expression();
                    exps.push_back(exp);
                    if (this->match(TokType::COMMA)) {
                        this->next();
                    } else {
                        break;
                    }
                }
                this->expect_token(TokType::RCURLY);
                return NodeFactory::clslitexp(otn, exps);
            } else {
                // it's field:exp, field:exp
                this->expect_token(TokType::COLON);
                NodeContainer exp = this->parse_expression();
                init[idn->identifier] = exp;
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
                this->expect_token(TokType::RCURLY);
                return NodeFactory::clslitfil(otn, init);
            }
        } else {
            // it's a list of expressions
            exps.push_back(first);
            if (this->match(TokType::COMMA)) {
                this->next();
                while (true) {
                    NodeContainer exp = this->parse_expression();
                    exps.push_back(exp);
                    if (this->match(TokType::COMMA)) {
                        this->next();
                    } else {
                        break;
                    }
                }
            }
            this->expect_token(TokType::RCURLY);
            return NodeFactory::clslitexp(otn, exps);
        }
    }
    this->expect_token(TokType::RCURLY);
    return NodeFactory::clslitfil(otn, init);
}

NodeContainer Parser::parse_id_or_class_literal() {
    NodeContainer node;
    std::string identifier = this->token.str;
    int start = this->token.start;
    int end = this->token.end;
    int line = this->token.line;
    int column = this->token.column;
    this->next();
    node = NodeFactory::id(identifier);
//    node->start = start;
//    node->end = end;
//    node->line = line;
//    node->column = column;
    return node;
}

NodeContainer Parser::parse_call_or_subscript_chain(NodeContainer parent) {
    NodeContainer node = parent;
    while (item_in_vec(this->token.type, {TokType::LPAREN, TokType::LSQUARE})) {
        if (this->match(TokType::LPAREN)) {
//                 function call
            this->next();
            VectorOfNodes arguments;
            if (!this->match(TokType::RPAREN)) {
                arguments = this->parse_list_of_expressions();
            }
            node = NodeFactory::call(node, arguments);
            this->expect_token(TokType::RPAREN);
        } else if (this->match(TokType::LSQUARE)) {
//                subscript
            this->next();
            if (this->match(TokType::RSQUARE)) {
                throw std::runtime_error("Empty subscript error!");
            }
            NodeContainer value = this->parse_expression();
            node = NodeFactory::sub(node, {value});
            this->expect_token(TokType::RSQUARE);
        }
    }
    return node;
}

NodeContainer Parser::parse_variable_declaration() {
    Token var_token = this->expect_token(TokType::VAR);
    Token identifier = this->expect_token(TokType::ID);
    TypeNode* type = nullptr;
    if (this->match(TokType::COLON)) {
        this->next();
        type = &this->parse_type_node();
    }
    try {
        this->expect_token(TokType::EQQ);
    } catch (...) {
        throw std::runtime_error("Error: you must initialize all variables!");
    }
    NodeContainer expression = this->parse_expression();
    NodeContainer node = NodeFactory::decl(identifier.str, type, expression);
//    node->start = var_token.start;
//    node->end = expression->end;
    return node;
}

//#define OPTIONAL_SEMICOLON()     if (this->match(TokType::SEMICOLON)) {this->next();}

NodeContainer Parser::parse_common_statement() {
    switch (this->token.type) {
        case TokType::IF: {
            return NodeContainer(&this->parse_if());
        }
        case TokType::VAR: {
            NodeContainer node = this->parse_variable_declaration();
            this->expect_token(TokType::SEMICOLON);
            return node;
        }
        case TokType::RETURN: {
            ReturnNode& node = this->parse_return();
            this->expect_token(TokType::SEMICOLON);
            return NodeContainer(&node);
        }
        case TokType::FOR: {
            return NodeContainer(new ForNode(this->parse_for_loop()));
        }
        case TokType::WHILE: {
            return NodeContainer(&this->parse_while_loop());
        }
        case TokType::BREAK: {
            this->next();
            if (!this->inside_loop) {
                throw std::runtime_error("Break used outside a loop!");
            }
            return NodeFactory::brk();
        }
        case TokType::CONTINUE: {
            this->next();
            std::cout << "RETURNIONG A CONTINUE NODE" << std::endl;
            if (!this->inside_loop) {
                throw std::runtime_error("Continue used outside a loop!");
            }
            return NodeFactory::cntinue();
        }
        default: {
            NodeContainer node = this->parse_assignment_or_expression();
            this->expect_token(TokType::SEMICOLON);
            return node;
        }
    }
}

FunctionTypeNode& Parser::parse_function_type() {
    this->expect_token(TokType::FUN);
    this->expect_token(TokType::LPAREN);
    std::vector<TypeNode> parameter_types;
    if (!this->match(TokType::RPAREN)) {
        while (true) {
            TypeNode& parameter_type = this->parse_type_node();
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
    TypeNode& return_type = this->parse_type_node();
    return NodeFactory::ftype(parameter_types, return_type);
}

ObjectTypeNode& Parser::parse_object_type() {
    Token identifier = this->expect_token(TokType::ID);
    std::vector<TypeNode> type_parameters;
    if (this->match(TokType::LSQUARE)) {
        this->next();
        while (true) {
            TypeNode& type_parameter = this->parse_type_node();;
            type_parameters.push_back(type_parameter);
            if (this->match(TokType::COMMA)) {
                this->next();
            } else {
                break;
            }
        }
        this->expect_token(TokType::RSQUARE);
    }
    return NodeFactory::otype(identifier.str, type_parameters);
}

TypeNode& Parser::parse_type_node() {
    if (this->match(TokType::FUN)) {
        return NodeFactory::type(this->parse_function_type());
    }
    return NodeFactory::type(this->parse_object_type());
}

BlockNode& Parser::parse_possibly_empty_block() {
    Token st = this->expect_token(TokType::LCURLY);
    int start = st.start;
    int end = -1;
    VectorOfNodes block;
    while (true) {
        if (this->match(TokType::RCURLY)) {
            end = this->token.end;
            this->next();
            break;
        }
        NodeContainer statement = this->parse_common_statement();
        block.push_back(statement);
    }
    return *(NodeFactory::block(block).node.block);
}

FunctionNode& Parser::parse_function_definition() {
    this->expect_token(TokType::FUN);
    Token matched_token = this->expect_token(TokType::ID);
    std::string identifier = matched_token.str;
    this->expect_token(TokType::LPAREN);
    std::vector<TypeNode> parameter_types;
    VectorOfStrings parameter_names;
    if (this->match(TokType::RPAREN)) {
        this->next();
        // Function with no parameters
    } else {
        // Function with parameters
        // Parse parameter list

        while (true) {
            Token parameter_identifier = this->expect_token(TokType::ID);
            this->expect_token(TokType::COLON);
            TypeNode& parameter_type = this->parse_type_node();
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
    TypeNode return_type(NodeFactory::otype(".None", {}));
    if (this->match(TokType::RARROW)) {
        // function with return value
        this->expect_token(TokType::RARROW);
        return_type = this->parse_type_node();
    }
    // Parse function body
    BlockNode& body = this->parse_possibly_empty_block();

    return *(NodeFactory::function(identifier, parameter_names, parameter_types, return_type, body).node.func);
}

Token Parser::expect_token(TokType token_type) {
    if (not this->match(token_type)) {
        throw UnexpectedToken(this->token, std::vector<TokType>(1, token_type));
    }
    Token matched_token = this->token;
    this->next();
    return matched_token;
}

NodeContainer Parser::parse_top_level_statement() {
    switch (this->token.type) {
        case TokType::FUN:
            return NodeContainer(&this->parse_function_definition());
        case TokType::CLASS:
            return NodeContainer(&this->parse_class_definition());
        default:
            return this->parse_common_statement();
    }
}

ForNode Parser::parse_for_loop() {
    this->expect_token(TokType::FOR);
    bool expect_paren = false;
    if (this->match(TokType::LPAREN)) {
        this->next();
        expect_paren = true;
    }
    Token var = this->expect_token(TokType::ID);
    this->expect_token(TokType::ARROBA);
    NodeContainer exp = this->parse_expression();
    if (expect_paren) {
        this->expect_token(TokType::RPAREN);
    }
    bool prev = this->inside_loop;
    this->inside_loop = true;
    BlockNode& body = this->parse_possibly_empty_block();
    this->inside_loop = prev;
    return ForNode(var.str, exp, body);
}

NodeContainer Parser::parse_ternary() {
    NodeContainer condition = this->parse_or_expression();
    if (this->match(TokType::QUESTION)) {
        this->next();
        NodeContainer true_case = this->parse_expression();
        this->expect_token(TokType::COLON);
        NodeContainer false_case = this->parse_expression();
        NodeContainer node = NodeFactory::ternary(condition, true_case, false_case);
        return node;
    }
    return condition;
}

WhileNode& Parser::parse_while_loop() {
    this->expect_token(TokType::WHILE);
    NodeContainer condition = this->parse_expression();
    bool prev = this->inside_loop;
    this->inside_loop = true;
    BlockNode& body = this->parse_possibly_empty_block();
    this->inside_loop = prev;
    return *(NodeFactory::whileloop(condition, body).node.whil);
}

ClassNode& Parser::parse_class_definition() {
    this->expect_token(TokType::CLASS);
    Token class_name_tk = this->expect_token(TokType::ID);
    std::vector<std::string> type_parameters;
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
    std::map<std::string, FunctionNode> methods;
    std::map<std::string, TypeNode> members;
    std::vector<std::string> members_ordered;
    while (true) {
        if (this->match(TokType::ID)) {
            Token member_name_tk = this->expect_token(TokType::ID);
            this->expect_token(TokType::COLON);
            TypeNode& member_type = this->parse_type_node();
            members[member_name_tk.str] = member_type;
            members_ordered.push_back(member_name_tk.str);
//            OPTIONAL_SEMICOLON();
            this->expect_token(TokType::SEMICOLON);
        } else if (this->match(TokType::FUN)) {
            FunctionNode method_node = this->parse_function_definition();
            methods.insert(make_pair(method_node.identifier, method_node));
        } else {
            break;
        }
    }
    this->expect_token(TokType::RCURLY);
    auto& c = *NodeFactory::cls(class_name_tk.str, type_parameters, members, methods).node.cls;
    c.members_ordered = members_ordered;
    return c;
}
