//
// Created by chris on 6/6/20.
//

#include <iostream>
#include "Scanner.h"
#include "../logging/logging.h"
#include "../utils.h"

std::unordered_map<std::string, TokType> TOKEN_KEYWORDS;
std::unordered_map<std::string, TokType> TOKEN_SPECIAL;

/*
 * fun initialize_tokens(){
 *  TOKEN_KEYWORDS["fun"] = Token(TokType.FUN);
 *  ...
 * }
 *
 * var TOKEN_KEYWORDS: Dict[String, Token];
 * */

void initialize_tokens() {
    TOKEN_KEYWORDS["fun"] = TokType::FUN;
    TOKEN_KEYWORDS["match"] = TokType::MATCH;
    TOKEN_KEYWORDS["if"] = TokType::IF;
    TOKEN_KEYWORDS["struct"] = TokType::STRUCT;
    TOKEN_KEYWORDS["else"] = TokType::ELSE;
    TOKEN_KEYWORDS["elif"] = TokType::ELIF;
    TOKEN_KEYWORDS["for"] = TokType::FOR;
    TOKEN_KEYWORDS["var"] = TokType::VAR;
    TOKEN_KEYWORDS["continue"] = TokType::CONTINUE;
    TOKEN_KEYWORDS["return"] = TokType::RETURN;
    TOKEN_KEYWORDS["static"] = TokType::STATIC;
    TOKEN_KEYWORDS["true"] = TokType::TRUE;
    TOKEN_KEYWORDS["false"] = TokType::FALSE;
    TOKEN_KEYWORDS["from"] = TokType::FROM;
    TOKEN_KEYWORDS["import"] = TokType::IMPORT;
    TOKEN_KEYWORDS["break"] = TokType::BREAK;
    TOKEN_KEYWORDS["class"] = TokType::CLASS;
    TOKEN_KEYWORDS["while"] = TokType::WHILE;
    TOKEN_KEYWORDS["none"] = TokType::NONE;
    TOKEN_KEYWORDS["as"] = TokType::AS;


    TOKEN_SPECIAL[";"] = TokType::SEMICOLON;
    TOKEN_SPECIAL[","] = TokType::COMMA;
    TOKEN_SPECIAL[":"] = TokType::COLON;
    TOKEN_SPECIAL["."] = TokType::DOT;
    TOKEN_SPECIAL["?"] = TokType::QUESTION;
    TOKEN_SPECIAL["$"] = TokType::DOLLAR_SIGN;

    TOKEN_SPECIAL["["] = TokType::LSQUARE;
    TOKEN_SPECIAL["]"] = TokType::RSQUARE;
    TOKEN_SPECIAL["("] = TokType::LPAREN;
    TOKEN_SPECIAL[")"] = TokType::RPAREN;
    TOKEN_SPECIAL["{"] = TokType::LCURLY;
    TOKEN_SPECIAL["}"] = TokType::RCURLY;

    TOKEN_SPECIAL["+"] = TokType::PLUS;
    TOKEN_SPECIAL["-"] = TokType::MINUS;
    TOKEN_SPECIAL["%"] = TokType::MOD;
    TOKEN_SPECIAL["*"] = TokType::TIMES;
    TOKEN_SPECIAL["/"] = TokType::DIV;

    TOKEN_SPECIAL["="] = TokType::EQQ;
    TOKEN_SPECIAL["+="] = TokType::PLUS_EQQ;
    TOKEN_SPECIAL["-="] = TokType::MINUS_EQQ;
    TOKEN_SPECIAL["*="] = TokType::TIMES_EQQ;
    TOKEN_SPECIAL["/="] = TokType::DIV_EQQ;

    TOKEN_SPECIAL["<"] = TokType::LT;
    TOKEN_SPECIAL["@"] = TokType::ARROBA;
    TOKEN_SPECIAL[">"] = TokType::GT;
    TOKEN_SPECIAL["<="] = TokType::LEQ;
    TOKEN_SPECIAL[">="] = TokType::GEQ;
    TOKEN_SPECIAL["//"] = TokType::DOUBLE_SLASH;
    TOKEN_SPECIAL["=="] = TokType::EQ;
    TOKEN_SPECIAL["::"] = TokType::DOUBLE_COLON;
    TOKEN_SPECIAL["!="] = TokType::NEQ;
    TOKEN_SPECIAL["!"] = TokType::NOT;
    TOKEN_SPECIAL["&&"] = TokType::AND;
    TOKEN_SPECIAL["||"] = TokType::OR;
    TOKEN_SPECIAL["->"] = TokType::RARROW;
    TOKEN_SPECIAL["++"] = TokType::INC;
    TOKEN_SPECIAL["#"] = TokType::HASH;
    TOKEN_SPECIAL["--"] = TokType::DEC;
}

Scanner::Scanner() {
    initialize_tokens();
    initialize_token_strings();
    this->current = 0;
    this->line = 0;
    this->column = 0;
}

void Scanner::load_file(const std::string& __file__) {
    std::string text = file_to_string(__file__);
    this->text = text;
    this->code_lines.text = text;
    this->code_lines.line_offsets.push_back(Range{.offset=0, .length=0});
}

void Scanner::load_text(const std::string& text) {
    this->text = text;
    this->code_lines.text = text;
    this->code_lines.line_offsets.push_back(Range{.offset=0, .length=0});
}

Token Scanner::get_next() {
    Token tok = this->next_token();
    this->cur_token = tok;
    return tok;
}

Token Scanner::next_token() {
    if (this->current >= this->text.size()) {
        return Token(TokType::END, {this->line, this->column});
    }
    char c = this->text[this->current];
    while (isspace(c)) {
        this->current++;
        if (c == '\n') {
            Token tok(TokType::ID, "DUMMY", {this->line, this->column});
            std::vector<TokType> semic = {TokType::RETURN, TokType::ID, TokType::INTEGER, TokType::RPAREN,
                                          TokType::RSQUARE, TokType::STRING, TokType::NONE, TokType::TRUE,
                                          TokType::FALSE};
            for (auto ts : semic) {
                if (this->cur_token.type == ts) {
                    tok = Token(TokType::SEMICOLON, {this->line, this->column});
                    this->line++;
                    this->code_lines.line_offsets.back().length =
                            this->current - this->code_lines.line_offsets.back().offset;
                    this->code_lines.line_offsets.push_back(Range{.offset=this->current, .length=0});
                    this->column = 0;
                    return tok;
                }
            }
            this->line++;
            this->code_lines.line_offsets.back().length = this->current - this->code_lines.line_offsets.back().offset;
            this->code_lines.line_offsets.push_back(Range{.offset=this->current, .length=0});
            this->column = 0;
        } else {
            this->column++;
        }
        if (this->current >= this->text.size()) {
            return Token(TokType::END, {this->line, this->column});
        }
        c = this->text[this->current];
    }
    if (isalpha(c) || c == '_') {
        return this->scan_keyword_or_identifier();
    }
    if (isdigit(c)) {
        return this->scan_number();
    }
    if (c == '\"') {
        std::string str;
        int start_l = this->line;
        int start_c = this->column;
        int start = this->current;
        this->current++;
        char c = this->text[this->current];
        while (c != '\"') {
            str += c;
            this->current++;
            if (this->current < this->text.size()) {
                this->column++;
                c = this->text[this->current];
            } else {
                break;
            }
        }
        int end = this->current;
        this->current++;
        this->column++;
        if (this->current < this->text.size()) {
            this->column++;
        }
        Token token(TokType::STRING, str, {start_l, start_c});
        // token.start = start;
        token.end_pos = {this->line, this->column};
        return token;
    }
    return this->scan_other();
}

Token Scanner::scan_other() {
    int start = this->current;
    int start_l = this->line;
    int start_c = this->column;
    char c = this->text[this->current];
    std::string str;
    str.push_back(c);
    size_t p = this->current + 1;

    if (p < this->text.size()) {
        std::string tstr = str;
        tstr.push_back(this->text[p]);
        if (TOKEN_SPECIAL.find(tstr) != TOKEN_SPECIAL.end()) {
            this->current += 2;
            this->column += 2;
            Token token(TOKEN_SPECIAL[tstr], {start_l, start_c});
            int end = this->current - 1;
            token.end_pos = {this->line, this->column};
            if (token.type == TokType::DOUBLE_SLASH) {
                // ignore everything until end of line
                while (c != '\n' && this->current < this->text.size()) {
                    c = this->text[this->current];
                    this->current++;
                }
                token = this->get_next();

            }
            return token;
        }
    }
    int end = this->current;
    if (TOKEN_SPECIAL.find(str) != TOKEN_SPECIAL.end()) {
        this->current++;
        this->column++;
        Token token(TOKEN_SPECIAL[str], {start_l, start_c});
        token.end_pos = {this->line, this->column};
        return token;
    }
    std::string msg = E_FMT("Unexpected character ");
    msg += E_HLT("'" + std::string(1, c) + "'");
    msg += E_FMT(" at ");
    msg += E_HLT(this->__file__ + ":" + std::to_string(start_l + 1) + ":" + std::to_string(start_c + 1));
    std::cout << msg << std::endl;
    exit(1);
}

Token Scanner::scan_keyword_or_identifier() {
    int start = this->current;
    int start_l = this->line;
    int start_c = this->column;
    char c = this->text[this->current];
    std::string str;
    while (isalnum(c) or c == '_') {
        str += c;
        this->current++;
        if (this->current < this->text.size()) {
            this->column++;
            c = this->text[this->current];
        } else {
            break;
        }
    }
    int end = this->current - 1;
    if (TOKEN_KEYWORDS.find(str) != TOKEN_KEYWORDS.end()) {
//      it's a keyword
        Token token(TOKEN_KEYWORDS[str], {start_l, start_c});
        token.end_pos = {this->line, this->column};
        return token;
    }
//  it's an identifier
    Token token(TokType::ID, str, {start_l, start_c});
    token.end_pos = {this->line, this->column};
    return token;
}

Token Scanner::scan_number() {
    int start = this->current;
    int start_l = this->line;
    int start_c = this->column;
    char c = this->text[this->current];
    std::string str;
    while (isdigit(c)) {
        str += c;
        this->current++;
        if (this->current < this->text.size()) {
            this->column++;
            c = this->text[this->current];
        } else {
            break;
        }
    }
    if (this->current < this->text.size()) {
        if (c == '.') {
            str += c;
            if (this->current + 1 < this->text.size()) {
                if (isdigit(this->text[this->current + 1])) {
                    // it's a decimal number
                    this->current++;
                    c = this->text[this->current];
                    while (isdigit(c)) {
                        str += c;
                        this->current++;
                        if (this->current < this->text.size()) {
                            this->column++;
                            c = this->text[this->current];
                        } else {
                            break;
                        }
                    }
                    TokType tok_type = TokType::FLOAT;
                    if (c == 'd') {
                        //    double
                        this->current++;
                        tok_type = TokType::DOUBLE;
                        if (this->current < this->text.size()) {
                            this->column++;
                        }
                    }
                    int end = this->current - 1;
                    Token token = Token(tok_type, str, {start_l, start_c});
                    token.end_pos = {this->line, this->column - 1};
                    return token;
                }
            } else {
                // it's just a dot, so return the number
                int end = this->current - 1;
                Token token = Token(TokType::INTEGER, str, {start_l, start_c});
                token.end_pos = {this->line, this->column - 1};
                return token;
            }
        }
    }
    int end = this->current - 1;
    Token token = Token(TokType::INTEGER, str, {start_l, start_c});
    token.end_pos = {this->line, this->column - 1};
    return token;
}

std::vector<Token> Scanner::scan_all() {
    std::vector<Token> tokens;
    while (true) {
        Token token = this->get_next();
        tokens.push_back(token);
        if (token.type == TokType::END) {
            break;
        }
    }
    return tokens;
}

UnexpectedCharacter::UnexpectedCharacter(char c, size_t position) : std::runtime_error(
        std::string("Unexpected character '") + std::string(1, c) + "' at position " + std::to_string(position)) {
}

UnexpectedCharacter::UnexpectedCharacter(char c, int line, int column) : std::runtime_error(
        std::string("Unexpected character '") + std::string(1, c) + "' at line " + std::to_string(line + 1) +
        " column " + std::to_string(column + 1)) {
    this->column = column;
    this->line = line;
}
