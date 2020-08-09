//
// Created by chris on 7/6/20.
//

#include "Parser.h"
#include "UnexpectedToken.h"

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
    ReturnNode* node = new ReturnNode(expression);
    return node;
}

IfNode* Parser::parse_if() {
    this->expect_token(TokenType::IF);
    this->expect_token(TokenType::LPAREN);
    Node* condition = this->parse_expression();
    this->expect_token(TokenType::RPAREN);
    BlockNode* body = this->parse_possibly_empty_block();
    IfNode* node = new IfNode(condition, body);
    return node;
}

ListNode* Parser::parse_list_literal() {
    this->expect_token(TokenType::LSQUARE);
    VectorOfNodes elements;
    if (this->match(TokenType::RSQUARE)) {
        // empty list
        this->next();
    } else {
        while (true) {
            Node* element = this->parse_expression();
            if (!this->match(TokenType::COMMA)) { break; }
            elements.push_back(element);
        }
        this->expect_token(TokenType::RSQUARE);
    }
    ListNode* node = new ListNode(elements);
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

Node* Parser::parse_function_expression() {
    return nullptr;
}

Node* Parser::parse_assignment_or_expression() {
    Node* lvalue = this->parse_expression();
    if (this->match(TokenType::EQQ)) {
        this->next();
        Node* rvalue = this->parse_expression();
        AssignmentNode* node = new AssignmentNode(lvalue, rvalue);
        return node;
    }
    return lvalue;
}

Node* Parser::parse_expression() {
    return this->parse_or_expression();
}

Node* Parser::parse_or_expression() {
    Node* left = this->parse_and_expression();
    if (this->match(TokenType::OR)) {
        Node* right = this->parse_and_expression();
        BinopNode* node = new BinopNode(OpType::OR, left, right);
        return node;
    }
    return left;
}

Node* Parser::parse_and_expression() {
    Node* left = this->parse_bool_expression();
    if (this->match(TokenType::AND)) {
        Node* right = this->parse_bool_expression();
        BinopNode* node = new BinopNode(OpType::AND, left, right);
        return node;
    }
    return left;
}

Node* Parser::parse_bool_expression() {
    Node* left = this->parse_add_or_sub_expression();
    if (this->match(TokenType::EQ)) {
        this->next();
        Node* right = this->parse_add_or_sub_expression();
        BinopNode* node = new BinopNode(OpType::EQ, left, right);
        return node;
    }
    return left;
}

Node* Parser::parse_add_or_sub_expression() {
    Node* left = this->parse_mul_or_div_expression();
    OpType op;
    while (this->match(TokenType::PLUS) || this->match(TokenType::MINUS)) {
        if (this->match(TokenType::PLUS)) {
            this->next();
            op = OpType::ADD;
        } else if (this->match(TokenType::MINUS)) {
            this->next();
            op = OpType::SUB;
        }
        Node* right = this->parse_mul_or_div_expression();
        left = new BinopNode(op, left, right);
    }
    return left;
}

Node* Parser::parse_mul_or_div_expression() {
    Node* left = this->parse_factor();
    OpType op;
    while (this->match(TokenType::TIMES) || this->match(TokenType::DIV)) {
        if (this->match(TokenType::TIMES)) {
            this->next();
            op = OpType::MUL;
        } else if (this->match(TokenType::DIV)) {
            this->next();
            op = OpType::DIV;
        }
        Node* right = this->parse_factor();
        left = new BinopNode(op, left, right);
    }
    return left;
}

Node* Parser::parse_factor() {
    Node* parent = this->parse_id_or_literal();
    parent = this->parse_call_or_subscript_chain(parent);
    while (this->match(TokenType::DOT)) {
        this->next();
        Token id = this->expect_token(TokenType::ID);
        parent = new MemberNode(parent, id.str);
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
            node = new NumberNode(this->token.num);
            this->next();
            break;
        }
        case TokenType::STRING: {
            node = new StringNode(this->token.str);
            this->next();
            break;
        }
        case TokenType::FUN: {
            return this->parse_function_expression();
        }
        default:
            throw std::runtime_error("parsing id or literal, unknown token type: " + TOKEN_STRINGS[token.type]);
    }
    return node;
}

Node* Parser::parse_id_or_class_literal() {
    Node* node = nullptr;
    std::string identifier = this->token.str;
    this->next();
    if (this->match(TokenType::LCURLY)) {
        this->next();
        std::map<std::string, Node*> initializers;
        if (!this->match(TokenType::RCURLY)) {
//            this->next();
            Node* expression = this->parse_expression();
            IdNode* id_ptr = dynamic_cast<IdNode*>(expression);
            bool is_expression_initializer = true;
            if (id_ptr != nullptr) {
//                        it may be an expression or if a colon follows the name of a field
                is_expression_initializer = !this->match(TokenType::COLON);
            }
            if (is_expression_initializer) {
// its an expression
                std::vector<Node*> initializers;
                initializers.push_back(expression);
                if (!this->match(TokenType::RCURLY)) {
                    this->expect_token(TokenType::COMMA);
                    while (true) {
                        expression = this->parse_expression();
                        initializers.push_back(expression);
                        if (this->match(TokenType::COMMA)) {
                            this->next();
                        } else {
                            break;
                        }
                    }

                }
                this->expect_token(TokenType::RCURLY);
                node = new ClassLiteralExpressionNode(identifier, initializers);
            } else {
                std::string field_id = id_ptr->identifier;
                std::map<std::string, Node*> initializers;
                while (true) {
                    this->expect_token(TokenType::COLON);
                    expression = this->parse_expression();
                    initializers[field_id] = expression;
                    if (this->match(TokenType::COMMA)) {
                        this->next();
                        Token field = this->expect_token(TokenType::ID);
                        field_id = field.str;
                    } else {
                        break;
                    }
                }
                this->expect_token(TokenType::RCURLY);
                node = new ClassLiteralFieldNode(identifier, initializers);
            }
        } else {
            this->next();
            node = new ClassLiteralExpressionNode(identifier, {});
        }

    } else {
        node = new IdNode(identifier);
    }
    return node;
}

Node* Parser::parse_call_or_subscript_chain(Node* parent) {
    Node* node = parent;
    while (this->match(TokenType::LPAREN) or this->match(TokenType::LSQUARE)) {
        if (this->match(TokenType::LPAREN)) {
//                 function call
            this->next();
            VectorOfNodes arguments;
            if (!this->match(TokenType::RPAREN)) {
                arguments = this->parse_list_of_expressions();
            }
            node = new CallNode(node, arguments);
            this->expect_token(TokenType::RPAREN);
        } else if (this->match(TokenType::LSQUARE)) {
//                subscript
            this->next();
            if (this->match(TokenType::RSQUARE)) {
                throw std::runtime_error("Empty subscript error!");
            }
            Node* value = this->parse_expression();
            node = new SubscriptNode(node, value);
            this->expect_token(TokenType::RSQUARE);
        }
    }
    return node;
}

DeclarationNode* Parser::parse_variable_declaration() {
    this->expect_token(TokenType::VAR);
    Token identifier = this->expect_token(TokenType::ID);
    TypeNode* type = nullptr;
    if (this->match(TokenType::COLON)) {
        this->next();
        type = this->parse_type_node();
    }
    Node* expression = nullptr;
    try {
        this->expect_token(TokenType::EQQ);
    } catch(...){
        throw std::runtime_error("Error: you must initialize all variables!");
    }
    expression = this->parse_expression();
    DeclarationNode* node = new DeclarationNode(identifier.str, type, expression);
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
        this->expect_token(TokenType::SEMICOLON);
        return ast_node;
    }
    if (this->match(TokenType::RETURN)) {
        ast_node = this->parse_return();
        this->expect_token(TokenType::SEMICOLON);
        return ast_node;
    }
    Node* node = this->parse_assignment_or_expression();
    this->expect_token(TokenType::SEMICOLON);
    return node;
}

TypeNode* Parser::parse_type_node() {
    Token identifier = this->expect_token(TokenType::ID);
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
    TypeNode* node = new TypeNode(identifier.str, type_parameters);
    return node;
}

StructNode* Parser::parse_struct_definition() {
    this->expect_token(TokenType::STRUCT);
    Token identifier_token = this->expect_token(TokenType::ID);
    VectorOfStrings template_parameters;
    if (this->match(TokenType::LSQUARE)) {
        this->next();
        // template struct
        bool expects_parameter = true;
        while (expects_parameter) {
            Token template_parameter = this->expect_token(TokenType::ID);
            template_parameters.push_back(template_parameter.str);
            if (this->match(TokenType::COMMA)) {
                this->next();
            } else {
                expects_parameter = false;
            }
        }
        this->expect_token(TokenType::RSQUARE);
    }
    std::map<std::string, TypeNode*> fields;

    this->expect_token(TokenType::LCURLY);
    while (true) {
        if (this->match(TokenType::ID)) {
            std::string identifier = this->token.str;
            this->next();
            this->expect_token(TokenType::COLON);
            TypeNode* field_type = this->parse_type_node();
            fields[identifier] = field_type;
            this->expect_token(TokenType::SEMICOLON);
        } else {
            break;
        }
    }
    this->expect_token(TokenType::RCURLY);
    StructNode* node = new StructNode(identifier_token.str, template_parameters, fields);
    return node;
}

BlockNode* Parser::parse_possibly_empty_block() {
    this->expect_token(TokenType::LCURLY);
    VectorOfNodes block;
    while (true) {
        if (this->match(TokenType::RCURLY)) {
            this->next();
            break;
        }
        Node* statement = this->parse_common_statement();
        block.push_back(statement);
    }
    BlockNode* rv = new BlockNode(block);
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
    std::vector<TokenType> expected_tokens;
    Node* node;
    switch (this->token.type) {
        case TokenType::FUN: {
            node = this->parse_function_definition();
            break;
        }
        case TokenType::STRUCT: {
            node = this->parse_struct_definition();
            break;
        }
        default:
            return this->parse_common_statement();
    }
    return node;
}


