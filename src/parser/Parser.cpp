//
// Created by chris on 7/6/20.
//

#include "Parser.h"
#include "UnexpectedToken.h"
#include "../nodes/InstanceNode.h"
#include <exception>

std::map<TokenType, OpType> TOKEN_TO_OP = {
        {TokenType::PLUS,  OpType::ADD},
        {TokenType::MINUS, OpType::SUB},
        {TokenType::TIMES, OpType::MUL},
        {TokenType::DIV,   OpType::DIV},
        {TokenType::MOD,   OpType::MOD},
        {TokenType::LT,    OpType::LT},
        {TokenType::GT,    OpType::GT},
        {TokenType::LEQ,   OpType::LEQ},
        {TokenType::GEQ,   OpType::GEQ},
        {TokenType::NEQ,   OpType::NEQ},
};

Parser::Parser(std::vector<Token>& tokens) {
    this->tokens = tokens;
    this->token = this->tokens[0];
    this->current = 0;
}

void Parser::next() {
    if (this->token.type != TokenType::END) {
        this->token = this->tokens[this->current + 1];
        this->current++;
    }
}

bool Parser::match(TokenType type) {
    return this->token.type == type;
}

BlockNode* Parser::parse_program() {
    VectorOfNodes program;
    while (this->token.type != TokenType::END) {
        program.push_back(this->parse_top_level_statement());
    }
    return new BlockNode(program);
}

ReturnNode* Parser::parse_return() {
    this->expect_token(TokenType::RETURN);
    Node* expression = this->parse_expression();
    ReturnNode* node = RET(expression);
    return node;
}

IfNode* Parser::parse_if() {
    this->expect_token(TokenType::IF);
    Node* condition = this->parse_expression();
    BlockNode* body = this->parse_possibly_empty_block();
    IfNode* node = IF(condition, body);
    return node;
}

Node* Parser::parse_list_literal() {
    Token list_start = this->expect_token(TokenType::LSQUARE);
    int start = list_start.start;
    VectorOfNodes elements;
    int end = 123;
    if (this->match(TokenType::RSQUARE)) {
        // empty list
        end = this->token.end;
        this->next();
        // parse required type annotation (cannot infer type of empty list
        this->expect_token(TokenType::DOUBLE_COLON);
        TypeNode* type = this->parse_type_node();
        EmptyListNode* node = new EmptyListNode(type);
        node->start = start;
        node->end = end;
        return node;
    } else {
        while (true) {
            Node* element = this->parse_expression();
            elements.push_back(element);
            if (!this->match(TokenType::COMMA)) { break; }
            this->next();
        }
        Token list_end = this->expect_token(TokenType::RSQUARE);
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
        if (this->match(TokenType::COMMA)) {
            this->next();
        } else {
            break;
        }
    }
    return result;
}

Node* Parser::parse_assignment_or_expression() {
    Node* lvalue = this->parse_expression();
    auto call = dynamic_cast<CallNode*>(lvalue);
    if (this->match(TokenType::EQQ)) {
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
    if (this->match(TokenType::OR)) {
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
    if (this->match(TokenType::AND)) {
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
    std::vector<TokenType> boolean_tokens = {TokenType::EQ, TokenType::LT, TokenType::GT, TokenType::LEQ,
                                             TokenType::GEQ, TokenType::NEQ};
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
    while (item_in_vec(this->token.type, {TokenType::PLUS, TokenType::MINUS})) {
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
    while (item_in_vec(this->token.type, {TokenType::TIMES, TokenType::DIV, TokenType::MOD})) {
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
    if (this->match(TokenType::HASH)) {
        this->next();
        parent = this->parse_class_literal();
    } else { parent = this->parse_id_or_literal(); }
    parent = this->parse_call_or_subscript_chain(parent);
    while (this->match(TokenType::DOT)) {
        this->next();
        Token id = this->expect_token(TokenType::ID);
        parent = MEM(parent, id.str);
        parent = this->parse_call_or_subscript_chain(parent);
    }
    return parent;
}

Node* Parser::parse_id_or_literal() {
    Node* node;
    switch (this->token.type) {
        case TokenType::LPAREN: {
            this->next();
            node = this->parse_expression();
            this->expect_token(TokenType::RPAREN);
            break;
        }
        case TokenType::ID: {
            node = this->parse_id_or_class_literal();
            break;
        }
        case TokenType::NUM: {
            node = NUM(this->token.num);
            node->line = this->token.line;
            node->column = this->token.column;
            node->start = this->token.start;
            node->end = this->token.end;
            this->next();
            break;
        }
        case TokenType::STRING: {
            node = STR(this->token.str);
            this->next();
            break;
        }
        case TokenType::TRUE: {
            node = BOOL(true);
            node->start = this->token.start;
            node->end = this->token.end;
            this->next();
            break;
        }
        case TokenType::FALSE: {
            node = BOOL(false);
            node->start = this->token.start;
            node->end = this->token.end;
            this->next();
            break;
        }
        case TokenType::LSQUARE:
            return this->parse_list_literal();
        case TokenType::NONE: {
            node = new NoneNode();
            node->start = this->token.start;
            node->end = this->token.end;
            this->next();
            return node;
        }
        default:
            throw UnexpectedToken(token,
                                  {TokenType::STRING, TokenType::NUM, TokenType::ID, TokenType::FUN,
                                   TokenType::LPAREN});
//            throw std::runtime_error("parsing id or literal, unknown token type: " + TOKEN_STRINGS[token.type]);
    }
    return node;
}

bool may_be_a_type(Node* node) {
    SubscriptNode* subs = dynamic_cast<SubscriptNode*>(node);
    if (subs == nullptr) {
        IdNode* idn = dynamic_cast<IdNode*>(node);
        return idn != nullptr;
    } else {
        if (may_be_a_type(subs->parent)) {
            for (int i = 0; i < subs->child.size(); i++) {
                if (!may_be_a_type(subs->child[i]))return false;
            }
            return true;
        }
        return false;
    }
}

ObjectTypeNode* convert_to_type(Node* node) {
    SubscriptNode* sub = dynamic_cast<SubscriptNode*>(node);
    if (sub == nullptr) {
        IdNode* idn = dynamic_cast<IdNode*>(node);
        VectorOfTypes t;
        return TYPE(idn->identifier, t);
    }
    IdNode* idn = dynamic_cast<IdNode*>(sub->parent);
    std::string type_id = idn->identifier;
    VectorOfTypes type_params;
    for (int i = 0; i < sub->child.size(); i++) {
        type_params.push_back(convert_to_type(sub->child[i]));
    }
    ObjectTypeNode* type = TYPE(type_id, type_params);
    return type;
}

Node* Parser::parse_class_literal() {

    TypeNode* type = this->parse_type_node();
    ObjectTypeNode* literal_type = dynamic_cast<ObjectTypeNode*>(type);
    if (literal_type == nullptr) {
        throw std::runtime_error("Expecterd a type to initialize!");
    }
    this->expect_token(TokenType::LCURLY);

    std::map<std::string, Node*> init;
    std::vector<Node*> exps;
    if (!this->match(TokenType::RCURLY)) {
        Node* first = this->parse_expression();
        // if it's an id
        IdNode* idn = dynamic_cast<IdNode*>(first);
        if (idn != nullptr) {
            if (this->match(TokenType::RCURLY)) {
                exps.push_back(idn);
                return LIT_EXP(literal_type, exps);

            } else if (this->match(TokenType::COMMA)) {
                // it's a list of expressions
                exps.push_back(idn);
                this->next();
                while (true) {
                    Node* exp = this->parse_expression();
                    exps.push_back(exp);
                    if (this->match(TokenType::COMMA)) {
                        this->next();
                    } else {
                        break;
                    }
                }
                this->expect_token(TokenType::RCURLY);
                return LIT_EXP(literal_type, exps);
            } else {
                // it's field:exp, field:exp
                this->expect_token(TokenType::COLON);
                Node* exp = this->parse_expression();
                init[idn->identifier] = exp;
                if (this->match(TokenType::COMMA)) {
                    this->next();
                    while (true) {
                        Token field_id = this->expect_token(TokenType::ID);
                        this->expect_token(TokenType::COLON);
                        exp = this->parse_expression();
                        init[field_id.str] = exp;
                        if (this->match(TokenType::COMMA)) {
                            this->next();
                        } else {
                            break;
                        }
                    }
                }
                this->expect_token(TokenType::RCURLY);
                return LIT_FIL(literal_type, init);
            }
        } else {
            // it's a list of expressions
            exps.push_back(first);
            if (this->match(TokenType::COMMA)) {
                this->next();
                while (true) {
                    Node* exp = this->parse_expression();
                    exps.push_back(exp);
                    if (this->match(TokenType::COMMA)) {
                        this->next();
                    } else {
                        break;
                    }
                }
            }
            this->expect_token(TokenType::RCURLY);
            return LIT_EXP(literal_type, exps);
        }
    }
    this->expect_token(TokenType::RCURLY);
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
    while (item_in_vec(this->token.type, {TokenType::LPAREN, TokenType::LSQUARE})) {
        if (this->match(TokenType::LPAREN)) {
//                 function call
            this->next();
            VectorOfNodes arguments;
            if (!this->match(TokenType::RPAREN)) {
                arguments = this->parse_list_of_expressions();
            }
            node = CALL(node, arguments);
            this->expect_token(TokenType::RPAREN);
        } else if (this->match(TokenType::LSQUARE)) {
//                subscript
            this->next();
            if (this->match(TokenType::RSQUARE)) {
                throw std::runtime_error("Empty subscript error!");
            }
            Node* value = this->parse_expression();
            node = SUB(node, { value });
            this->expect_token(TokenType::RSQUARE);
        }
    }
    return node;
}

DeclarationNode* Parser::parse_variable_declaration() {
    Token var_token = this->expect_token(TokenType::VAR);
    Token identifier = this->expect_token(TokenType::ID);
    TypeNode* type = nullptr;
    if (this->match(TokenType::COLON)) {
        this->next();
        type = this->parse_type_node();
    }
    try {
        this->expect_token(TokenType::EQQ);
    } catch (...) {
        throw std::runtime_error("Error: you must initialize all variables!");
    }
    Node* expression = this->parse_expression();
    DeclarationNode* node = DECL(identifier.str, type, expression);
    node->start = var_token.start;
    node->end = expression->end;
    return node;
}

Node* Parser::parse_common_statement() {
    Node* ast_node;
    if (this->match(TokenType::IF)) {
        ast_node = this->parse_if();
        return ast_node;
    }
    if (this->match(TokenType::VAR)) {
        ast_node = this->parse_variable_declaration();
        if (this->match(TokenType::SEMICOLON)) {
            this->next();
        }
//        this->expect_token(TokenType::SEMICOLON);
        return ast_node;
    }
    if (this->match(TokenType::RETURN)) {
        ast_node = this->parse_return();
        if (this->match(TokenType::SEMICOLON)) {
            this->next();
        }
//        this->expect_token(TokenType::SEMICOLON);
        return ast_node;
    }
    if (this->match(TokenType::FOR)) {
        ast_node = this->parse_for_loop();
        return ast_node;
    }
    if (this->match(TokenType::WHILE)) {
        ast_node = this->parse_while_loop();
        return ast_node;
    }
    Node* node = this->parse_assignment_or_expression();
//    this->expect_token(TokenType::SEMICOLON);
    if (this->match(TokenType::SEMICOLON)) {
        this->next();
    }
    return node;
}

TypeNode* Parser::parse_type_node() {
    if (this->match(TokenType::FUN)) {
        this->next();
        this->expect_token(TokenType::LPAREN);
        VectorOfTypes parameter_types;
        if (!this->match(TokenType::RPAREN)) {
            while (true) {
                TypeNode* parameter_type = this->parse_type_node();
                parameter_types.push_back(parameter_type);
                if (this->match(TokenType::COMMA)) {
                    this->next();
                } else {
                    break;
                }
            }
        }
        this->expect_token(TokenType::RPAREN);
        this->expect_token(TokenType::RARROW);
        TypeNode* return_type = this->parse_type_node();
        TypeNode* fun_type = FUNCTION_TYPE(parameter_types, return_type);
        return fun_type;
    }
    Token identifier;
    try {
        identifier = this->expect_token(TokenType::ID);
    } catch (...) {
        throw std::runtime_error("Expected a valid TYPE, but got token " + TOKEN_STRINGS[this->token.type]);
    }
    VectorOfTypes type_parameters;
    if (this->match(TokenType::LSQUARE)) {
        this->next();
        bool expects_parameter = true;
        while (expects_parameter) {
            TypeNode* type_parameter = this->parse_type_node();;
            type_parameters.push_back(type_parameter);
            if (this->match(TokenType::COMMA)) {
                this->next();
            } else {
                expects_parameter = false;
            }
        }
        this->expect_token(TokenType::RSQUARE);
    }
    TypeNode* node = TYPE(identifier.str, type_parameters);
    return node;
}

BlockNode* Parser::parse_possibly_empty_block() {
    Token st = this->expect_token(TokenType::LCURLY);
    int start = st.start;
    int end = -1;
    VectorOfNodes block;
    while (true) {
        if (this->match(TokenType::RCURLY)) {
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
    this->expect_token(TokenType::FUN);
    Token matched_token = this->expect_token(TokenType::ID);
    std::string identifier = matched_token.str;
    this->expect_token(TokenType::LPAREN);
    VectorOfTypes parameter_types;
    VectorOfStrings parameter_names;
    if (this->match(TokenType::RPAREN)) {
        this->next();
        // Function with no parameters
    } else {
        // Function with parameters
        // Parse parameter list

        while (true) {
            Token parameter_identifier = this->expect_token(TokenType::ID);
            this->expect_token(TokenType::COLON);
            TypeNode* parameter_type = this->parse_type_node();
            parameter_types.push_back(parameter_type);
            parameter_names.push_back(parameter_identifier.str);
            if (this->match(TokenType::COMMA)) {
                this->next();
            } else {
                break;
            }
        }
        this->expect_token(TokenType::RPAREN);
    }
    // Parse return
    TypeNode* return_type = nullptr;
    this->expect_token(TokenType::RARROW);
    return_type = this->parse_type_node();
    // Parse function body
    BlockNode* body = this->parse_possibly_empty_block();

    FunctionNode* node = new FunctionNode(identifier, parameter_names, parameter_types, return_type, body);
    return node;
}

Token Parser::expect_token(TokenType token_type) {
    if (not this->match(token_type)) {
        throw UnexpectedToken(this->token, std::vector<TokenType>(1, token_type));
    }
    Token matched_token = this->token;
    this->next();
    return matched_token;
}

Node* Parser::parse_top_level_statement() {
    switch (this->token.type) {
        case TokenType::FUN:
            return this->parse_function_definition();
        case TokenType::CLASS:
            return this->parse_class_definition();
        default:
            return this->parse_common_statement();
    }
}

ForNode* Parser::parse_for_loop() {
    this->expect_token(TokenType::FOR);
    bool expect_paren = false;
    if (this->match(TokenType::LPAREN)) {
        this->next();
        expect_paren = true;
    }
    Token var = this->expect_token(TokenType::ID);
    this->expect_token(TokenType::ARROBA);
    Node* exp = this->parse_expression();
    if (expect_paren) {
        this->expect_token(TokenType::RPAREN);
    }
    BlockNode* body = this->parse_possibly_empty_block();
    ForNode* for_node = FOR(var.str, exp, body);
    return for_node;
}

Node* Parser::parse_ternary() {
    Node* condition = this->parse_or_expression();
    if (this->match(TokenType::QUESTION)) {
        this->next();
        Node* true_case = this->parse_expression();
        this->expect_token(TokenType::COLON);
        Node* false_case = this->parse_expression();
        Node* node = TERNARY(condition, true_case, false_case);
        node->start = condition->start;
        node->end = false_case->end;
        return node;
    }
    return condition;
}

WhileNode* Parser::parse_while_loop() {
    this->expect_token(TokenType::WHILE);
    Node* condition = this->parse_expression();
    BlockNode* body = this->parse_possibly_empty_block();
    return WHILE(condition, body);
}

ClassNode* Parser::parse_class_definition() {
    this->expect_token(TokenType::CLASS);
    Token class_name_tk = this->expect_token(TokenType::ID);
    std::vector<std::string> type_parameters;
    if (this->match(TokenType::LSQUARE)) {
        this->next();

        while (true) {
            Token type_param_tk = this->expect_token(TokenType::ID);
            type_parameters.push_back(type_param_tk.str);
            if (!this->match(TokenType::COMMA)) {
                break;
            }
        }
        this->expect_token(TokenType::RSQUARE);
    }
    this->expect_token(TokenType::LCURLY);
    std::map<std::string, FunctionNode*> methods;
    std::map<std::string, TypeNode*> members;
    std::vector<std::string> members_ordered;
    while (true) {
        if (this->match(TokenType::ID)) {
            Token member_name_tk = this->expect_token(TokenType::ID);
            this->expect_token(TokenType::COLON);
            TypeNode* member_type = this->parse_type_node();
            members[member_name_tk.str] = member_type;
            members_ordered.push_back(member_name_tk.str);
//            this->expect_token(TokenType::SEMICOLON);
            if (this->match(TokenType::SEMICOLON)) {
                this->next();
            }
        } else if (this->match(TokenType::FUN)) {
            FunctionNode* method_node = this->parse_function_definition();
            methods[method_node->identifier] = method_node;
        } else {
            break;
        }
    }
    this->expect_token(TokenType::RCURLY);
    auto c = new ClassNode(class_name_tk.str, type_parameters, members, methods);
    c->members_ordered = members_ordered;
    return c;
}
