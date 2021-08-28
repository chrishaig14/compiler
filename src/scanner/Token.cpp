//
// Created by chris on 6/6/20.
//

#include "Token.h"

std::unordered_map<TokType, std::string> TOKEN_STRINGS;

Token::Token(TokType type, std::string str, TextPosition start) {
    this->type = type;
    this->str = str;
    this->start = start;
}

std::string Token::to_string() {
    std::string st;
    st = "[" + this->pos_string() + "]\t";
    st += TOKEN_STRINGS[this->type];
    if (this->type == TokType::ID || this->type == TokType::STRING) {
        st += " \"" + this->str + "\"";
    } else if (this->type == TokType::INTEGER) {
        st += " \"" + this->str + "\"";
    }
    return st;
}

Token::Token(TokType type, TextPosition start) {
    this->type = type;
    this->str = "";
    this->start = start;
}

bool Token::operator==(const Token& other) const {
    return this->type == other.type && this->str == other.str && this->start == other.start;
}

std::ostream& operator<<(std::ostream& os, const Token& token) {
    os << TOKEN_STRINGS[token.type] << ", num: " << token.str << ", str: " << token.str << " pos: l" << token.start.line
       << ":c" << token.start.column;
    return os;
}

Token::Token() {
    this->str = "";
}

std::string Token::pos_string() {
    return std::to_string(this->start.line + 1) + ":" + std::to_string(this->start.column + 1) + " - " +
           std::to_string(this->end_pos.line + 1) + ":" + std::to_string(this->end_pos.column + 1);
}

Token::Token(TokType type, std::string str, TextPosition start, TextPosition end) {
    this->type = type;
    this->str = str;
    this->start = start;
    this->end_pos = end;
}


void initialize_token_strings() {
    TOKEN_STRINGS[TokType::AS] = "as";
    TOKEN_STRINGS[TokType::COMMA] = "comma ( , )";
    TOKEN_STRINGS[TokType::BREAK] = "break";
    TOKEN_STRINGS[TokType::CONTINUE] = "continue";
    TOKEN_STRINGS[TokType::DOLLAR_SIGN] = "dollar sign ( $ )";
    TOKEN_STRINGS[TokType::DOT] = "dot ( . )";
    TOKEN_STRINGS[TokType::SEMICOLON] = "semicolon ( ; )";
    TOKEN_STRINGS[TokType::COLON] = "colon ( : )";
    TOKEN_STRINGS[TokType::NEWLINE] = "newline ( \\n )";
    TOKEN_STRINGS[TokType::HASH] = "hash ( # )";
    TOKEN_STRINGS[TokType::EQQ] = "equal ( = )";
    TOKEN_STRINGS[TokType::PLUS_EQQ] = "plus equal ( += )";
    TOKEN_STRINGS[TokType::MINUS_EQQ] = "minus equal ( -= )";
    TOKEN_STRINGS[TokType::TIMES_EQQ] = "times equal ( *= )";
    TOKEN_STRINGS[TokType::DIV_EQQ] = "div equal ( /= )";
    TOKEN_STRINGS[TokType::EQ] = "equal ( == )";
    TOKEN_STRINGS[TokType::LT] = "less than ( < )";
    TOKEN_STRINGS[TokType::GT] = "greater than ( > )";
    TOKEN_STRINGS[TokType::ELIF] = "elif";
    TOKEN_STRINGS[TokType::LEQ] = "less than or equal ( <= )";
    TOKEN_STRINGS[TokType::GEQ] = "greater than or equal ( >= )";
    TOKEN_STRINGS[TokType::NEQ] = "not equal ( != )";
    TOKEN_STRINGS[TokType::INC] = "increment ( ++ )";
    TOKEN_STRINGS[TokType::DEC] = "decrement ( -- )";
    TOKEN_STRINGS[TokType::QUESTION] = "question ( ? )";
    TOKEN_STRINGS[TokType::MOD] = "modulo ( % )";
    TOKEN_STRINGS[TokType::PLUS] = "plus ( + )";
    TOKEN_STRINGS[TokType::MINUS] = "minus ( - )";
    TOKEN_STRINGS[TokType::FLOAT] = "float";
    TOKEN_STRINGS[TokType::TIMES] = "times ( * )";
    TOKEN_STRINGS[TokType::DIV] = "div ( / )";
    TOKEN_STRINGS[TokType::AND] = "and ( && )";
    TOKEN_STRINGS[TokType::OR] = "or ( || )";
    TOKEN_STRINGS[TokType::DOUBLE_SLASH] = "double slash ( // )";
    TOKEN_STRINGS[TokType::NOT] = "not ( ! )";
    TOKEN_STRINGS[TokType::ID] = "identifier";
    TOKEN_STRINGS[TokType::INTEGER] = "integer";
    TOKEN_STRINGS[TokType::STRING] = "string";
    TOKEN_STRINGS[TokType::FUN] = "fun";
    TOKEN_STRINGS[TokType::RETURN] = "return";
    TOKEN_STRINGS[TokType::CLASS] = "class";
    TOKEN_STRINGS[TokType::TYPECLASS] = "typeclass";
    TOKEN_STRINGS[TokType::STRUCT] = "struct";
    TOKEN_STRINGS[TokType::IF] = "if";
    TOKEN_STRINGS[TokType::ELSE] = "else";
    TOKEN_STRINGS[TokType::FOR] = "for";
    TOKEN_STRINGS[TokType::VAR] = "var";
    TOKEN_STRINGS[TokType::LSQUARE] = "lsquare ( [ )";
    TOKEN_STRINGS[TokType::RARROW] = "rarrow ( => )";
    TOKEN_STRINGS[TokType::RSQUARE] = "rsquare ( ] )";
    TOKEN_STRINGS[TokType::LCURLY] = "lcurly ( { )";
    TOKEN_STRINGS[TokType::RCURLY] = "rcurly ( } )";
    TOKEN_STRINGS[TokType::LPAREN] = "lparen ( ( )";
    TOKEN_STRINGS[TokType::DOUBLE_COLON] = "double colon ( :: )";
    TOKEN_STRINGS[TokType::RPAREN] = "rparen ( ) )";
    TOKEN_STRINGS[TokType::ARROBA] = "arroba ( @ )";
    TOKEN_STRINGS[TokType::END] = "EOF";
    TOKEN_STRINGS[TokType::WHILE] = "while";
    TOKEN_STRINGS[TokType::TRUE] = "true";
    TOKEN_STRINGS[TokType::FALSE] = "false";
    TOKEN_STRINGS[TokType::NONE] = "none";
    TOKEN_STRINGS[TokType::FROM] = "from";
    TOKEN_STRINGS[TokType::IMPORT] = "import";
}
