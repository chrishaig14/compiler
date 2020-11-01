//
// Created by chris on 6/6/20.
//

#ifndef UNTITLED1_TOKEN_H
#define UNTITLED1_TOKEN_H

#include <string>
#include <map>
#include <iostream>

enum class TokType {
    COMMA,
    DOT,
    SEMICOLON,
    COLON,
    EQQ,
    PLUS_EQQ,
    NEWLINE,
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
    RCURLY,
    LPAREN,
    RPAREN,
    END, RARROW, TRUE, FALSE, QUESTION, NONE, DOUBLE_COLON, HASH, ARROBA, CLASS
};

extern std::map<TokType, std::string> TOKEN_STRINGS;

void initialize_token_strings();

class Token {
public:
    TokType type;
    std::string str;
    int num;
    int line, column;
    int start;
    int end;

    Token();

    Token(TokType type, int line, int column);

    Token(TokType type, std::string str, int line, int column);

    Token(TokType type, int num, int line, int column);

    bool operator==(const Token& other) const;

    friend std::ostream& operator<<(std::ostream& os, const Token& token);

    std::string to_string();
};


#endif //UNTITLED1_TOKEN_H
