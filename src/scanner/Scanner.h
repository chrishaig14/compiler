//
// Created by chris on 6/6/20.
//

#ifndef SCANNER_H
#define SCANNER_H


#include <string>
#include "Token.h"
#include "CodeLines.h"

#include <vector>

extern std::unordered_map<std::string, TokType> TOKEN_KEYWORDS;
extern std::unordered_map<std::string, TokType> TOKEN_SPECIAL;

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

    Scanner();

    void load_file(const std::string& _file__);
    void load_text(const std::string& txt);

    Token get_next();

    std::vector<Token> scan_all();

    size_t current;

    Token scan_keyword_or_identifier();

    Token scan_number();

    Token scan_other();

    bool comment;
    Token cur_token;

    Token next_token();
    CodeLines code_lines;
    std::string __file__;
};


#endif //SCANNER_H
