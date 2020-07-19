//
// Created by chris on 6/6/20.
//

#ifndef UNTITLED1_SCANNER_H
#define UNTITLED1_SCANNER_H


#include <string>
#include "Token.h"

#include <vector>

extern std::map<std::string, TokenType> TOKEN_KEYWORDS;
extern std::map<std::string, TokenType> TOKEN_SPECIAL;

class UnexpectedCharacter:std::runtime_error {
public:
    UnexpectedCharacter(char c, size_t position);

    bool operator==(const UnexpectedCharacter &other) const;
};

class Scanner {

    std::string text;
    int line, column;
public:

    Scanner(const std::string &text);

    Token get_next();

    std::vector<Token> scan_all() {
        std::vector<Token> tokens;
        while (true) {
            Token token = this->get_next();
            tokens.push_back(token);
            if (token.type == TokenType::END) {
                break;
            }
        }
        return tokens;
    }

    size_t current;

    Token scan_keyword_or_identifier();

    Token scan_number();

    Token scan_other();
};


#endif //UNTITLED1_SCANNER_H
