//
// Created by chris on 6/6/20.
//

#ifndef UNTITLED1_TOKEN_H
#define UNTITLED1_TOKEN_H

#include <string>
#include <map>
#include <iostream>

enum class TokenType {
    COMMA,
    DOT,
    SEMICOLON,
    COLON,
    EQQ,
    PLUS_EQQ,
    MINUS_EQQ,
    TIMES_EQQ,
    DIV_EQQ,
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
    RETURN,
    STRUCT,
    IF,
    ELSE,
    FOR,
    WHILE,
    VAR,
    LSQUARE,
    RSQUARE,
    LCURLY,
    RCURLY,
    LPAREN,
    RPAREN,
    END, RARROW, TRUE, FALSE, QUESTION
};

extern std::map<TokenType, std::string> TOKEN_STRINGS;

void initialize_token_strings();

class Token {
public:
    TokenType type;
    std::string str;
    int num;
    int line, column;
    int start;
    int end;

    Token();

    Token(TokenType type, int line, int column);

    Token(TokenType type, std::string str, int line, int column);

    Token(TokenType type, int num, int line, int column);

    bool operator==(const Token& other) const;

    friend std::ostream& operator<<(std::ostream& os, const Token& token);

    std::string to_string();
};


#endif //UNTITLED1_TOKEN_H
