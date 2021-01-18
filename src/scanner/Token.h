//
// Created by chris on 6/6/20.
//

#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <unordered_map>
#include <iostream>
#include "TextPosition.h"

enum class TokType {
    COMMA,
    DOT,
    SEMICOLON,
    COLON,
    EQQ,
    PLUS_EQQ,
    NEWLINE,
    BREAK,
    CONTINUE,
    MINUS_EQQ,
    TIMES_EQQ,
    DIV_EQQ,
    DOLLAR_SIGN,
    EQ,
    LT,
    GT,
    LEQ,
    GEQ,
    NEQ,
    INC,
    DEC,
    PLUS,
    MINUS,
    TIMES,
    DIV,
    AND,
    OR,
    NOT,
    ID,
    NUM,
    STRING,
    FUN,
    DOUBLE_SLASH,
    RETURN,
    STRUCT,
    IF,
    ELSE,
    ELIF,
    FOR,
    MOD,
    WHILE,
    VAR,
    LSQUARE,
    RSQUARE,
    LCURLY,
    FLOAT,
    RCURLY,
    LPAREN,
    RPAREN,
    END, RARROW, TRUE, FALSE, QUESTION, NONE, DOUBLE_COLON, HASH, ARROBA, CLASS, IMPORT, FROM
};

extern std::unordered_map<TokType, std::string> TOKEN_STRINGS;

void initialize_token_strings();

class Token {
public:
    TokType type;
    std::string str;
    int num;
    float flot;
    TextPosition start;
    TextPosition end_pos;
    int end;

    Token();

    Token(TokType type, TextPosition start);

    Token(TokType type, std::string str, TextPosition start);

    Token(TokType type, int num, TextPosition start);

    Token(TokType type, float flot, TextPosition start);

    bool operator==(const Token& other) const;

    friend std::ostream& operator<<(std::ostream& os, const Token& token);

    std::string to_string();

    std::string pos_string();
};


#endif //TOKEN_H
