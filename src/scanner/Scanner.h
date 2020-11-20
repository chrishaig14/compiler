//
// Created by chris on 6/6/20.
//

#ifndef SCANNER_H
#define SCANNER_H


#include <string>
#include "Token.h"

#include <vector>

extern std::map<std::string, TokType> TOKEN_KEYWORDS;
extern std::map<std::string, TokType> TOKEN_SPECIAL;

class UnexpectedCharacter : public std::runtime_error {
public:
    int line, column;

    UnexpectedCharacter(char c, size_t position);

    UnexpectedCharacter(char c, int line, int column);

    bool operator==(const UnexpectedCharacter& other) const;
};

class Scanner {

    std::string text;
    int line, column;
public:

    Scanner(const std::string& text);

    Token get_next();

    std::vector<Token> scan_all();

    size_t current;

    Token scan_keyword_or_identifier();

    Token scan_number();

    Token scan_other();

    bool comment;
    Token token;

    Token next_token();
};


#endif //SCANNER_H
