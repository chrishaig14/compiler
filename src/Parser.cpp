//
// Created by chris on 7/6/20.
//

#include "Parser.h"

VectorOfNodes Parser::parse_program() {
    VectorOfNodes program;
    while (this->token.type != TokenType::END) {
        program.push_back(this->parse_top_level_statement());
    }
    return program;
}

ReturnNode* Parser::parse_return() {
    this->expect_token(TokenType::RETURN);
    AstNode* expression = this->parse_expression();
    ReturnNode* node = new ReturnNode(expression);
    return node;
}

IfNode* Parser::parse_if() {
    this->expect_token(TokenType::IF);
    this->expect_token(TokenType::LPAREN);
    AstNode* condition = this->parse_expression();
    std::cout << "The condition expression is " << ast_string(condition->type) << std::endl;
    this->expect_token(TokenType::RPAREN);
    std::cout << "Gonna parse possibly empty block" << std::endl;
    VectorOfNodes body = this->parse_possibly_empty_block();
    IfNode* node = new IfNode(condition, body);
    return node;
}

CallNode* Parser::parse_call() {
    AstNode* function = this->parse_expression();
    this->expect_token(TokenType::LPAREN);
    VectorOfNodes arguments;
    if (this->match(TokenType::RPAREN)) {
        this->next();
    } else {
        while (true) {
            AstNode* argument = this->parse_expression();
            arguments.push_back(argument);
            if (!this->match(TokenType::COMMA)) {
                break;
            }
        }
        this->expect_token(TokenType::RPAREN);
    }
    CallNode* node;
    node->function = function;
    node->arguments = arguments;
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
            AstNode* element = this->parse_expression();
            if (!this->match(TokenType::COMMA)) { break; }
            elements.push_back(element);
        }
        this->expect_token(TokenType::RSQUARE);
    }
    ListNode* node;
    node->elements = elements;
    return node;
}

MemberNode* Parser::parse_member() {
    MemberNode* node;
    node->parent = this->parse_expression();
    this->expect_token(TokenType::DOT);
    Token child = this->expect_token(TokenType::ID);
    node->child = child.str;
    return node;
}

FunctionSignatureNode* Parser::parse_function_signature() {
    this->expect_token(TokenType::FUN);
    Token matched_token = this->expect_token(TokenType::ID);
    std::string identifier = matched_token.str;
    this->expect_token(TokenType::LPAREN);
    if (this->match(TokenType::RPAREN)) {
    } else {
        this->next();
        // Parse parameter list
        VectorOfTypes parameter_types;
        while (true) {
            TypeNode* parameter_type = this->parse_type_node();
            parameter_types.push_back(parameter_type);
            if (this->match(TokenType::COMMA)) {
                this->next();
            } else {
                break;
            }
        }
        this->expect_token(TokenType::RPAREN);
    }
    this->expect_token(TokenType::COLON);
    TypeNode* return_type = this->parse_type_node();
//        FunctionSignatureNode* node = new FunctionSignatureNode(identifier;
//
//        node->return_type = return_type;

//        return node;
    return nullptr;
}

AstNode* Parser::parse_function_expression() {
    return NULL;
}

AstNode* Parser::parse_factor() {
    AstNode* ast_node = new AstNode;
    switch (this->token.type) {
        case TokenType::LPAREN: {
            this->next();
            AstNode* expression = this->parse_expression();
            this->next();
            return expression;
        }
        case TokenType::ID: {
            std::cout << "Its an identifier " << std::endl;
            ast_node->type = AstType::IDENTIFIER;
            IdentifierNode* node = new IdentifierNode(this->token.str);
            ast_node->ast_identifier = node;
            this->next();
            break;
        }
        case TokenType::NUM: {
            ast_node->type=AstType::NUMBER;
            NumberNode* number_node = new NumberNode(this->token.num);
            ast_node->ast_number = number_node;
            this->next();
            break;
        }
        case TokenType::STRING: {
            break;
        }
        case TokenType::FUN: {
            return this->parse_function_expression();
        }
    }
    return ast_node;
}

AstNode* Parser::parse_mul_or_div_expression() {
    AstNode* left = this->parse_factor();
    if (this->match(TokenType::TIMES)) {
        AstNode* right = this->parse_factor();
        AstNode* ast_node = new AstNode;
        ast_node->type = AstType::BINOP;
        BinopNode* node = new BinopNode(BinopType::TIMES, left, right);
        ast_node->ast_binop = node;
        return ast_node;
    }
    return left;
}

AstNode* Parser::parse_add_or_sub_expression() {
    AstNode* left = this->parse_mul_or_div_expression();
    std::cout << "Token after left in add_or_sub is" << TOKEN_STRINGS[this->token.type] << std::endl;
    if (this->match(TokenType::PLUS)) {
        this->next();
        std::cout << "Matches plus" << std::endl;
        AstNode* right = this->parse_mul_or_div_expression();
        std::cout << "Right returned " << ast_string(right->type) << std::endl;
        AstNode* ast_node = new AstNode;
        ast_node->type = AstType::BINOP;
        BinopNode* node = new BinopNode(BinopType::PLUS, left, right);
        ast_node->ast_binop = node;
        return ast_node;
    }
    return left;
}

AstNode* Parser::parse_bool_expression() {
    AstNode* left = this->parse_add_or_sub_expression();
    if (this->match(TokenType::EQ)) {
        this->next();
        AstNode* right = this->parse_add_or_sub_expression();
        AstNode* ast_node = new AstNode;
        ast_node->type = AstType::BINOP;
        BinopNode* node = new BinopNode(BinopType::EQ, left, right);
        ast_node->ast_binop = node;
        return ast_node;
    }
    return left;
}

AstNode* Parser::parse_and_expression() {
    AstNode* left = this->parse_bool_expression();
    if (this->match(TokenType::AND)) {
        AstNode* right = this->parse_bool_expression();
        AstNode* ast_node = new AstNode;
        ast_node->type = AstType::BINOP;
        BinopNode* node = new BinopNode(BinopType::AND, left, right);
        ast_node->ast_binop = node;
        return ast_node;
    }
    return left;
}

AstNode* Parser::parse_or_expression() {
    AstNode* left = this->parse_and_expression();
    if (this->match(TokenType::OR)) {
        AstNode* right = this->parse_and_expression();
        AstNode* ast_node = new AstNode;
        ast_node->type = AstType::BINOP;
        BinopNode* node = new BinopNode(BinopType::OR, left, right);
        ast_node->ast_binop = node;
        return ast_node;
    }
    return left;
}

AstNode* Parser::parse_expression() {
    return this->parse_or_expression();
}

AstNode* Parser::parse_assignment_or_expression() {
    AstNode* lvalue = this->parse_expression();
    std::cout << "lvalue is of type " << ast_string(lvalue->type) << std::endl;
    if (this->match(TokenType::EQQ)) {
        this->next();
        AstNode* rvalue = this->parse_expression();
        std::cout << "rvalue is of type " << ast_string(rvalue->type) << std::endl;
        AstNode* ast_node = new AstNode;
        ast_node->type = AstType::ASSIGNMENT;
        AssignmentNode* node = new AssignmentNode(lvalue, rvalue);
        ast_node->ast_assignment = node;
        std::cout << "returning" << std::endl;
        return ast_node;
    }
    return lvalue;
}

DeclarationNode* Parser::parse_variable_declaration() {
    if (this->match(TokenType::VAR)) {
        this->next();
        std::cout << "Matched token VAR" << std::endl;
        Token identifier = this->expect_token(TokenType::ID);
        std::cout << "Found identifier " << identifier.str << std::endl;
        TypeNode* type = NULL;
        if (this->match(TokenType::COLON)) {
            this->next();
            type = this->parse_type_node();
        }
        AstNode* expression = NULL;
        if (this->match(TokenType::EQQ)) {
            this->next();
            expression = this->parse_expression();
        }
        DeclarationNode* node = new DeclarationNode(identifier.str, type, expression);
        return node;
    }
}

AstNode* Parser::parse_common_statement() {
    AstNode* ast_node = new AstNode;
    if (this->match(TokenType::IF)) {
        ast_node->type = AstType::IF;
        ast_node->ast_if = this->parse_if();
        return ast_node;
    }
    if (this->match(TokenType::VAR)) {
        ast_node->type = AstType::DECLARATION;
        ast_node->ast_declaration = this->parse_variable_declaration();
        this->expect_token(TokenType::SEMICOLON);
        return ast_node;
    }
    if(this->match(TokenType::RETURN)){
        ast_node->type=AstType::RETURN;
        ast_node->ast_return = this->parse_return();
        this->expect_token(TokenType::SEMICOLON);
        return ast_node;
    }
    AstNode* node =this->parse_assignment_or_expression();
    this->expect_token(TokenType::SEMICOLON);
    return node;
}

AstNode* Parser::parse_interface_definition() {
    this->expect_token(TokenType::INTERFACE);
    Token identifier_token = this->expect_token(TokenType::ID);
    this->expect_token(TokenType::COLON);
    if (this->match(TokenType::LSQUARE)) {
        this->next();
        // template interface
        std::vector<Token> template_parameters;
        std::vector<TokenType> expected_tokens_after_parameter = {TokenType::COMMA, TokenType::RSQUARE};
        bool expects_parameter = true;
        while (expects_parameter) {
            Token template_parameter = this->expect_token(TokenType::ID);
            template_parameters.push_back(template_parameter);
            if (this->match(TokenType::COMMA)) {
                this->next();
            } else {
                expects_parameter = false;
            }
        }
        this->expect_token(TokenType::RSQUARE);
    }
    this->expect_token(TokenType::LCURLY);
    this->expect_token(TokenType::RCURLY);
    return NULL;
}

TypeNode* Parser::parse_type_node() {
    Token identifier = this->expect_token(TokenType::ID);
    VectorOfTypes type_parameters;
    if (this->match(TokenType::LSQUARE)) {
        this->next();
        bool expects_parameter = true;
        while (expects_parameter) {
            TypeNode* type_parameter = this->parse_type_node();
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

ClassNode* Parser::parse_class_definition() {
    this->expect_token(TokenType::CLASS);
    Token identifier_token = this->expect_token(TokenType::ID);
    VectorOfStrings template_parameters;
    if (this->match(TokenType::LSQUARE)) {
        this->next();
        // template class
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
    std::vector<DeclarationNode*> fields;
    std::vector<FunctionNode*> methods;

    this->expect_token(TokenType::LCURLY);
    while (true) {
        if (this->match(TokenType::VAR)) {
            DeclarationNode* field = this->parse_variable_declaration();
            this->expect_token(TokenType::SEMICOLON);
            fields.push_back(field);
        } else if (this->match(TokenType::FUN)) {
            FunctionNode* method = this->parse_function_definition();
            methods.push_back(method);
        } else {
            break;
        }
    }
    this->expect_token(TokenType::RCURLY);
    ClassNode* node = new ClassNode(identifier_token.str, template_parameters, fields, methods);
    return node;
}

VectorOfNodes Parser::parse_possibly_empty_block() {
    this->expect_token(TokenType::LCURLY);
    VectorOfNodes block;
    while (true) {
        if (this->match(TokenType::RCURLY)) {
            this->next();
            break;
        }
        AstNode* statement = this->parse_common_statement();
        block.push_back(statement);
    }
    return block;
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
    TypeNode* return_type = NULL;
    if (this->match(TokenType::COLON)) {
        this->next();
        return_type = this->parse_type_node();
    }
    std::cout << "Gonna parse function body" << std::endl;
//        this->expect_token(TokenType::LCURLY);
    // Parse function body
    VectorOfNodes body = this->parse_possibly_empty_block();
    std::cout << "Finished parsing function body" << std::endl;

    FunctionNode* node = new FunctionNode(identifier, parameter_names, parameter_types, return_type, body);
    return node;
}

Token Parser::expect_token(TokenType token) {
    if (not this->match(token)) {
        throw UnexpectedToken(this->token, std::vector<TokenType>(1, token));
    }
    Token matched_token = this->token;
    this->next();
    return matched_token;
}

AstNode* Parser::parse_top_level_statement() {
    std::vector<TokenType> expected_tokens;
    AstNode* ast_node = new AstNode;
    switch (this->token.type) {
        case TokenType::FUN: {
            FunctionNode* node = this->parse_function_definition();
            ast_node->type = AstType::FUNCTION;
            ast_node->ast_function = node;
            break;
        }
        case TokenType::CLASS: {
            ClassNode* node = this->parse_class_definition();
            ast_node->type = AstType::CLASS;
            ast_node->ast_class = node;
            break;
        }
        case TokenType::INTERFACE:
//                return this->parse_interface_definition();
            break;
        default:
            throw UnexpectedToken(this->token, {TokenType::FUN, TokenType::CLASS, TokenType::INTERFACE});
    }
    return ast_node;
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

void Parser::expect_one_of(std::vector<TokenType> expected_tokens) {
    for (int i = 0; i < expected_tokens.size(); i++) {
        if (this->token.type == expected_tokens[i]) {
            this->next();
            return;
        }
    }
    throw UnexpectedToken(this->token, expected_tokens);
}

Parser::Parser(std::vector<Token> &tokens) {
    this->tokens = tokens;
    this->token = this->tokens[0];
    this->current = 0;
}

UnexpectedToken::UnexpectedToken(Token token, const std::vector<TokenType> &expected_tokens) {
    this->token = token;
    this->expected_tokens = expected_tokens;
}

std::ostream &operator<<(std::ostream &os, const UnexpectedToken &unexpected_token) {
    std::string expected_strings;
    for (int i = 0; i < unexpected_token.expected_tokens.size() - 1; i++) {
        expected_strings += TOKEN_STRINGS[unexpected_token.expected_tokens[i]];
    }
    expected_strings +=
            " or " + TOKEN_STRINGS[unexpected_token.expected_tokens[unexpected_token.expected_tokens.size() - 1]];
    os << "UnexpectedToken: got " << TOKEN_STRINGS[unexpected_token.token.type] << ", expected "
       << expected_strings;
    return os;
}
