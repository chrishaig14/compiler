//
// Created by chris on 7/6/20.
//

#include "Parser.h"
#include "UnexpectedToken.h"
#include "../nodes/InstanceNode.h"
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
    this->expect_token(TokType::RETURN);
    Node* expression = nullptr;
    if (!this->match(TokType::SEMICOLON)) {
        expression = this->parse_expression();
    }
    ReturnNode* node = RET(expression);
    return node;
}

IfNode* Parser::parse_if() {
    this->expect_token(TokType::IF);
    Node* condition = this->parse_expression();
    BlockNode* body = this->parse_possibly_empty_block();
    std::vector<std::pair<Node*, BlockNode*>> elifs;
    while (this->match(TokType::ELIF)) {
        this->next();
        Node* elif_condition = this->parse_expression();
        BlockNode* elif_body = this->parse_possibly_empty_block();
        elifs.push_back(std::pair<Node*, BlockNode*>(elif_condition, elif_body));
    }
    BlockNode* _else = nullptr;
    if (this->match(TokType::ELSE)) {
        this->next();
        _else = this->parse_possibly_empty_block();
    }
    IfNode* node = IF(condition, body, elifs, _else);
    return node;
}

Node* Parser::parse_list_literal() {
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
        TypeNode* type = this->parse_type_node();
        EmptyListNode* node = new EmptyListNode(type);
        node->start = start;
        node->end = end;
        return node;
    } else {
        while (true) {
            Node* element = this->parse_expression();
            elements.push_back(element);
            if (!this->match(TokType::COMMA)) { break; }
            this->next();
        }
        Token list_end = this->expect_token(TokType::RSQUARE);
        end = list_end.end;
    }
    ListNode* node = LST(elements);
    node->start = start;
    node->end = end;
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

Node* Parser::parse_assignment_or_expression() {
    Node* lvalue = this->parse_expression();
    auto call = TO_CALL(lvalue);
    if (this->match(TokType::EQQ)) {
        if (call != nullptr) {
            throw std::runtime_error("Can't assign to a function call!");
        }
        this->next();
        Node* rvalue = this->parse_expression();
        AssignmentNode* node = ASN(lvalue, rvalue);
        node->start = lvalue->start;
        node->end = rvalue->end;
        return node;
    } else {
        if (call == nullptr) {
            throw std::runtime_error(
                    "Only function calls are allowed here! No ID, NUM, SUBSCRIPT, BINOP or other expression!");
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
        BinopNode* node = BIN(OpType::OR, left, right);
        node->start = left->start;
        node->end = right->end;
        return node;
    }
    return left;
}

Node* Parser::parse_and_expression() {
    Node* left = this->parse_bool_expression();
    if (this->match(TokType::AND)) {
        this->next();
        Node* right = this->parse_bool_expression();
        BinopNode* node = BIN(OpType::AND, left, right);
        node->start = left->start;
        node->end = right->end;
        return node;
    }
    return left;
}


Node* Parser::parse_bool_expression() {
    Node* left = this->parse_add_or_sub_expression();
    OpType op;
    std::vector<TokType> boolean_tokens = {TokType::EQ, TokType::LT, TokType::GT, TokType::LEQ,
                                           TokType::GEQ, TokType::NEQ};
    if (!item_in_vec(this->token.type, boolean_tokens)) {
        return left;
    }
    op = TOKEN_TO_OP[this->token.type];
    this->next();
    Node* right = this->parse_add_or_sub_expression();
    BinopNode* node = BIN(op, left, right);
    node->start = left->start;
    node->end = right->end;
    return node;
}

Node* Parser::parse_add_or_sub_expression() {
    Node* left = this->parse_mul_div_or_mod_expression();
    OpType op;
    while (item_in_vec(this->token.type, {TokType::PLUS, TokType::MINUS})) {
        op = TOKEN_TO_OP[this->token.type];
        this->next();
        Node* right = this->parse_mul_div_or_mod_expression();
        Node* node = BIN(op, left, right);
        node->start = left->start;
        node->end = right->end;
        left = node;

    }
    return left;
}

Node* Parser::parse_mul_div_or_mod_expression() {
    Node* left = this->parse_factor();
    OpType op;
    while (item_in_vec(this->token.type, {TokType::TIMES, TokType::DIV, TokType::MOD})) {
        op = TOKEN_TO_OP[this->token.type];
        this->next();
        Node* right = this->parse_factor();
        Node* node = BIN(op, left, right);
        node->start = left->start;
        node->end = right->end;
        left = node;
    }
    return left;
}

Node* Parser::parse_factor() {
    Node* parent = nullptr;
    if (this->match(TokType::HASH)) {
        this->next();
        parent = this->parse_class_literal();
    } else { parent = this->parse_id_or_literal(); }
    parent = this->parse_call_or_subscript_chain(parent);
    while (this->match(TokType::DOT)) {
        this->next();
        Token id = this->expect_token(TokType::ID);
        parent = MEM(parent, id.str);
        parent = this->parse_call_or_subscript_chain(parent);
    }
    return parent;
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
            node = NUM(this->token.num);
            node->line = this->token.line;
            node->column = this->token.column;
            node->start = this->token.start;
            node->end = this->token.end;
            this->next();
            break;
        }
        case TokType::STRING: {
            node = STR(this->token.str);
            this->next();
            break;
        }
        case TokType::TRUE: {
            node = BOOL(true);
            node->start = this->token.start;
            node->end = this->token.end;
            this->next();
            break;
        }
        case TokType::FALSE: {
            node = BOOL(false);
            node->start = this->token.start;
            node->end = this->token.end;
            this->next();
            break;
        }
        case TokType::LSQUARE:
            return this->parse_list_literal();
        case TokType::NONE: {
            node = new NoneNode();
            node->start = this->token.start;
            node->end = this->token.end;
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

Node* Parser::parse_class_literal() {

    TypeNode* type = this->parse_type_node();
    ObjectTypeNode* literal_type = TO_OBJECT_TYPE(type);
    if (literal_type == nullptr) {
        throw std::runtime_error("Expecterd a type to initialize!");
    }
    this->expect_token(TokType::LCURLY);

    std::map<std::string, Node*> init;
    std::vector<Node*> exps;
    if (!this->match(TokType::RCURLY)) {
        Node* first = this->parse_expression();
        // if it's an id
        IdNode* idn = TO_ID(first);
        if (idn != nullptr) {
            if (this->match(TokType::RCURLY)) {
                exps.push_back(idn);
                return LIT_EXP(literal_type, exps);

            } else if (this->match(TokType::COMMA)) {
                // it's a list of expressions
                exps.push_back(idn);
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
                this->expect_token(TokType::RCURLY);
                return LIT_EXP(literal_type, exps);
            } else {
                // it's field:exp, field:exp
                this->expect_token(TokType::COLON);
                Node* exp = this->parse_expression();
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
                return LIT_FIL(literal_type, init);
            }
        } else {
            // it's a list of expressions
            exps.push_back(first);
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
            this->expect_token(TokType::RCURLY);
            return LIT_EXP(literal_type, exps);
        }
    }
    this->expect_token(TokType::RCURLY);
    return LIT_FIL(literal_type, init);
}

Node* Parser::parse_id_or_class_literal() {
    Node* node = nullptr;
    std::string identifier = this->token.str;
    int start = this->token.start;
    int end = this->token.end;
    int line = this->token.line;
    int column = this->token.column;
    this->next();
    node = ID(identifier);
    node->start = start;
    node->end = end;
    node->line = line;
    node->column = column;
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
                arguments = this->parse_list_of_expressions();
            }
            node = CALL(node, arguments);
            this->expect_token(TokType::RPAREN);
        } else if (this->match(TokType::LSQUARE)) {
//                subscript
            this->next();
            if (this->match(TokType::RSQUARE)) {
                throw std::runtime_error("Empty subscript error!");
            }
            Node* value = this->parse_expression();
            node = SUB(node, { value });
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
    try {
        this->expect_token(TokType::EQQ);
    } catch (...) {
        throw std::runtime_error("Error: you must initialize all variables!");
    }
    Node* expression = this->parse_expression();
    DeclarationNode* node = DECL(identifier.str, type, expression);
    node->start = var_token.start;
    node->end = expression->end;
    return node;
}

//#define OPTIONAL_SEMICOLON()     if (this->match(TokType::SEMICOLON)) {this->next();}

Node* Parser::parse_common_statement() {
    switch (this->token.type) {
        case TokType::IF: {
            return this->parse_if();
        }
        case TokType::VAR: {
            Node* node = this->parse_variable_declaration();
            this->expect_token(TokType::SEMICOLON);
            return node;
        }
        case TokType::RETURN: {
            Node* node = this->parse_return();
            this->expect_token(TokType::SEMICOLON);
            return node;
        }
        case TokType::FOR: {
            return this->parse_for_loop();
        }
        case TokType::WHILE: {
            return this->parse_while_loop();
        }
        default: {
            Node* node = this->parse_assignment_or_expression();
            this->expect_token(TokType::SEMICOLON);
            return node;
        }
    }
}

FunctionTypeNode* Parser::parse_function_type() {
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
    return FUNCTION_TYPE(parameter_types, return_type);
}

ObjectTypeNode* Parser::parse_object_type() {
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
    return TYPE(identifier.str, type_parameters);
}

TypeNode* Parser::parse_type_node() {
    if (this->match(TokType::FUN)) {
        return this->parse_function_type();
    }
    return parse_object_type();
}

BlockNode* Parser::parse_possibly_empty_block() {
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
        Node* statement = this->parse_common_statement();
        block.push_back(statement);
    }
    BlockNode* rv = new BlockNode(block);
    rv->start = start;
    rv->end = end;
    return rv;
}

FunctionNode* Parser::parse_function_definition() {
    this->expect_token(TokType::FUN);
    Token matched_token = this->expect_token(TokType::ID);
    std::string identifier = matched_token.str;
    this->expect_token(TokType::LPAREN);
    VectorOfTypes parameter_types;
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
        return_type = TYPE(".None", {});
    }
    // Parse function body
    BlockNode* body = this->parse_possibly_empty_block();

    FunctionNode* node = new FunctionNode(identifier, parameter_names, parameter_types, return_type, body);
    return node;
}

Token Parser::expect_token(TokType token_type) {
    if (not this->match(token_type)) {
        throw UnexpectedToken(this->token, std::vector<TokType>(1, token_type));
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
        default:
            return this->parse_common_statement();
    }
}

ForNode* Parser::parse_for_loop() {
    this->expect_token(TokType::FOR);
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
    BlockNode* body = this->parse_possibly_empty_block();
    ForNode* for_node = FOR(var.str, exp, body);
    return for_node;
}

Node* Parser::parse_ternary() {
    Node* condition = this->parse_or_expression();
    if (this->match(TokType::QUESTION)) {
        this->next();
        Node* true_case = this->parse_expression();
        this->expect_token(TokType::COLON);
        Node* false_case = this->parse_expression();
        Node* node = TERNARY(condition, true_case, false_case);
        node->start = condition->start;
        node->end = false_case->end;
        return node;
    }
    return condition;
}

WhileNode* Parser::parse_while_loop() {
    this->expect_token(TokType::WHILE);
    Node* condition = this->parse_expression();
    BlockNode* body = this->parse_possibly_empty_block();
    return WHILE(condition, body);
}

ClassNode* Parser::parse_class_definition() {
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
    std::map<std::string, FunctionNode*> methods;
    std::map<std::string, TypeNode*> members;
    std::vector<std::string> members_ordered;
    while (true) {
        if (this->match(TokType::ID)) {
            Token member_name_tk = this->expect_token(TokType::ID);
            this->expect_token(TokType::COLON);
            TypeNode* member_type = this->parse_type_node();
            members[member_name_tk.str] = member_type;
            members_ordered.push_back(member_name_tk.str);
//            OPTIONAL_SEMICOLON();
            this->expect_token(TokType::SEMICOLON);
        } else if (this->match(TokType::FUN)) {
            FunctionNode* method_node = this->parse_function_definition();
            methods[method_node->identifier] = method_node;
        } else {
            break;
        }
    }
    this->expect_token(TokType::RCURLY);
    auto c = new ClassNode(class_name_tk.str, type_parameters, members, methods);
    c->members_ordered = members_ordered;
    return c;
}
