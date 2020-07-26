//
// Created by chris on 6/6/20.
//

#include "Token.h"

std::map<TokenType, std::string> TOKEN_STRINGS;

Token::Token(TokenType type, std::string str, int line, int column) {
    this->type = type;
    this->str = str;
    this->num = 0;
    this->line = line;
    this->column = column;
}

Token::Token(TokenType type, int num, int line, int column) {
    this->type = type;
    this->num = num;
    this->str = "";
    this->line = line;
    this->column = column;
}

std::string Token::to_string() {
    return TOKEN_STRINGS[this->type];
}

Token::Token(TokenType type, int line, int column) {
    this->type = type;
    this->num = 0;
    this->str = "";
    this->line = line;
    this->column = column;
}

bool Token::operator==(const Token &other) const {
    return this->type == other.type && this->str == other.str && this->num == other.num && this->line == other.line && this->column == other.column;
}

std::ostream &operator<<(std::ostream &os, const Token &token) {
    os << TOKEN_STRINGS[token.type] << ", num: " << token.num << ", str: " << token.str << " pos: l" << token.line << ":c" << token.column;
    return os;
}

Token::Token() {
    this->num = 0;
    this->str = "";
}


void initialize_token_strings() {
    TOKEN_STRINGS[TokenType::COMMA] = "comma ( , )";
    TOKEN_STRINGS[TokenType::DOT] = "dot ( . )";
    TOKEN_STRINGS[TokenType::SEMICOLON] = "semicolon ( ; )";
    TOKEN_STRINGS[TokenType::COLON] = "colon ( : )";
    TOKEN_STRINGS[TokenType::EQQ] = "equal ( = )";
    TOKEN_STRINGS[TokenType::PLUS_EQQ] = "plus equal ( += )";
    TOKEN_STRINGS[TokenType::MINUS_EQQ] = "minus equal ( -= )";
    TOKEN_STRINGS[TokenType::TIMES_EQQ] = "times equal ( *= )";
    TOKEN_STRINGS[TokenType::DIV_EQQ] = "div equal ( /= )";
    TOKEN_STRINGS[TokenType::EQ] = "equal ( == )";
    TOKEN_STRINGS[TokenType::LT] = "less than ( < )";
    TOKEN_STRINGS[TokenType::GT] = "greater than ( > )";
    TOKEN_STRINGS[TokenType::LEQ] = "less than or equal ( <= )";
    TOKEN_STRINGS[TokenType::GEQ] = "greater than or equal ( >= )";
    TOKEN_STRINGS[TokenType::NEQ] = "not equal ( != )";
    TOKEN_STRINGS[TokenType::INC] = "increment ( ++ )";
    TOKEN_STRINGS[TokenType::DEC] = "decrement ( -- )";
    TOKEN_STRINGS[TokenType::PLUS] = "plus ( + )";
    TOKEN_STRINGS[TokenType::MINUS] = "minus ( - )";
    TOKEN_STRINGS[TokenType::TIMES] = "times ( * )";
    TOKEN_STRINGS[TokenType::DIV] = "div ( / )";
    TOKEN_STRINGS[TokenType::AND] = "and ( && )";
    TOKEN_STRINGS[TokenType::OR] = "or ( || )";
    TOKEN_STRINGS[TokenType::NOT] = "not ( ! )";
    TOKEN_STRINGS[TokenType::ID] = "identifier";
    TOKEN_STRINGS[TokenType::NUM] = "number";
    TOKEN_STRINGS[TokenType::STRING] = "string";
    TOKEN_STRINGS[TokenType::FUN] = "fun";
    TOKEN_STRINGS[TokenType::RETURN] = "return";
    TOKEN_STRINGS[TokenType::CLASS] = "class";
    TOKEN_STRINGS[TokenType::IF] = "if";
    TOKEN_STRINGS[TokenType::INTERFACE] = "interface";
    TOKEN_STRINGS[TokenType::ELSE] = "else";
    TOKEN_STRINGS[TokenType::FOR] = "for";
    TOKEN_STRINGS[TokenType::VAR] = "var";
    TOKEN_STRINGS[TokenType::LSQUARE] = "lsquare ( [ )";
    TOKEN_STRINGS[TokenType::RARROW] = "rarrow ( => )";
    TOKEN_STRINGS[TokenType::RSQUARE] = "rsquare ( ] )";
    TOKEN_STRINGS[TokenType::LCURLY] = "lcurly ( { )";
    TOKEN_STRINGS[TokenType::RCURLY] = "rcurly ( } )";
    TOKEN_STRINGS[TokenType::LPAREN] = "lparen ( ( )";
    TOKEN_STRINGS[TokenType::RPAREN] = "rparen ( ) )";
    TOKEN_STRINGS[TokenType::END] = "EOF";

}
