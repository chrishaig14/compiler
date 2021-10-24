//
// Created by chris on 6/6/20.
//

#ifndef SCANNER_H
#define SCANNER_H


#include <string>
#include "Token.h"
#include <common/CodeLines.h>

#include <vector>

extern std::unordered_map<std::string, TokType> TOKEN_KEYWORDS;
extern std::unordered_map<std::string, TokType> TOKEN_SPECIAL;


class Scanner {

    std::string text;
    size_t line, column;
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
    void set_char();
    char chris;
    Token scan_string();
    void new_line();
    void advance_simple();
    bool at_eof();
    void accum_token();
    std::string current_tok_str;
    void start_token();
    bool should_insert_semicolon() const;
};


#endif //SCANNER_H
