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
    TOKEN_KEYWORDS["alias"] = TokType::ALIAS;
    TOKEN_KEYWORDS["match"] = TokType::MATCH;
    TOKEN_KEYWORDS["if"] = TokType::IF;
    TOKEN_KEYWORDS["struct"] = TokType::STRUCT;
    TOKEN_KEYWORDS["else"] = TokType::ELSE;
    TOKEN_KEYWORDS["elif"] = TokType::ELIF;
    TOKEN_KEYWORDS["for"] = TokType::FOR;
    TOKEN_KEYWORDS["enum"] = TokType::ENUM;
    TOKEN_KEYWORDS["var"] = TokType::VAR;
    TOKEN_KEYWORDS["continue"] = TokType::CONTINUE;
    TOKEN_KEYWORDS["return"] = TokType::RETURN;
    TOKEN_KEYWORDS["static"] = TokType::STATIC;
    TOKEN_KEYWORDS["throw"] = TokType::THROW;
    TOKEN_KEYWORDS["true"] = TokType::TRUE;
    TOKEN_KEYWORDS["try"] = TokType::TRY;
    TOKEN_KEYWORDS["catch"] = TokType::CATCH;
    TOKEN_KEYWORDS["false"] = TokType::FALSE;
    TOKEN_KEYWORDS["from"] = TokType::FROM;
    TOKEN_KEYWORDS["import"] = TokType::IMPORT;
    TOKEN_KEYWORDS["break"] = TokType::BREAK;
    TOKEN_KEYWORDS["class"] = TokType::CLASS;
    TOKEN_KEYWORDS["while"] = TokType::WHILE;
    TOKEN_KEYWORDS["where"] = TokType::WHERE;
    TOKEN_KEYWORDS["none"] = TokType::NONE;
    TOKEN_KEYWORDS["typeclass"] = TokType::TYPECLASS;
    TOKEN_KEYWORDS["instance"] = TokType::INSTANCE;
    TOKEN_KEYWORDS["as"] = TokType::AS;

    TOKEN_KEYWORDS["not"] = TokType::NOT;
    TOKEN_KEYWORDS["and"] = TokType::AND;
    TOKEN_KEYWORDS["or"] = TokType::OR;
    // TOKEN_KEYWORDS["xor"] = TokType::XOR;


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

void Scanner::load_file(const std::string& _file__) {
    this->text = file_to_string(_file__);;
    this->code_lines.text = this->text;
    this->code_lines.line_offsets.push_back(Range{.offset=0, .length=0});
}

void Scanner::load_text(const std::string& txt) {
    this->text = txt;
    this->code_lines.text = txt;
    this->code_lines.line_offsets.push_back(Range{.offset=0, .length=0});
}

Token Scanner::get_next() {
    Token tok = this->next_token();
    this->cur_token = tok;
    return tok;
}

void Scanner::set_char() {
    this->chris = this->text[this->current];
}

bool Scanner::should_insert_semicolon() const {
    std::vector<TokType> semic = {TokType::RETURN, TokType::ID, TokType::INTEGER, TokType::RPAREN, TokType::RSQUARE,
                                  TokType::STRING, TokType::NONE, TokType::TRUE, TokType::FALSE};
    for (auto ts : semic) {
        if (this->cur_token.type == ts) {
            return true;
            break;
        }
    }
    return false;
}

Token Scanner::next_token() {
    if (this->at_eof()) {
        return Token(TokType::END, {this->line, this->column});
    }
    this->set_char();
    while (isspace(this->chris) != 0) {
        this->current++;
        if (this->chris == '\n') {
            if (this->should_insert_semicolon()) {
                Token tok = Token(TokType::SEMICOLON, ";", {this->line, this->column}, {this->line, this->column + 1});
                this->new_line();
                return tok;
            }
            this->new_line();
        } else {
            this->column++;
        }
        if (this->at_eof()) {
            return Token(TokType::END, {this->line, this->column});
        }
        this->set_char();
    }

    if ((isalpha(this->chris) != 0) || this->chris == '_') {
        return this->scan_keyword_or_identifier();
    }
    if (isdigit(this->chris) != 0) {
        return this->scan_number();
    }
    if (this->chris == '\"') {
        return this->scan_string();
    }
    return this->scan_other();
}

Token Scanner::scan_string() {
    this->start_token();
    TextPosition start = {this->line, this->column};
    this->current++;
    this->set_char();
    while (this->chris != '\"' && !this->at_eof()) {
        if (this->chris == '\\') {
            this->advance_simple();
            std::string c;
            switch (this->chris) {
                case 'n':
                    c = "\\n";
                    break;
                case '\\':
                    c = "\\\\";
                    break;
                case '\"':
                    c = "\\\"";
                    break;
                default:
                    throw std::runtime_error("Invalid escape sequence: \\" + std::string(1, this->chris));
            }
            this->current_tok_str += c;
        } else if (this->chris == '\n') {
            throw std::runtime_error("No multiline strings allowed");
        } else {
            this->accum_token();
        }
        this->advance_simple();
    }
    if (this->at_eof()) {
        std::cerr << "Error: unterminated string!" << std::endl;
        exit(1);
    }
    this->current++;
    this->column++;
    if (!this->at_eof()) {
        this->column++;
    }
    return Token(TokType::STRING, this->current_tok_str, start, {this->line, this->column});
}

Token Scanner::scan_other() {
    TextPosition start = {this->line, this->column};
    this->set_char();
    std::string str;
    str.push_back(this->chris);
    size_t p = this->current + 1;

    if (p < this->text.size()) {
        std::string tstr = str;
        tstr.push_back(this->text[p]);
        if (TOKEN_SPECIAL.find(tstr) != TOKEN_SPECIAL.end()) {
            this->current += 2;
            this->column += 2;
            Token token(TOKEN_SPECIAL[tstr], start);
            token.end_pos = {this->line, this->column};
            if (token.type == TokType::DOUBLE_SLASH) {
                // ignore everything until end of line
                while (this->chris != '\n' && !this->at_eof()) {
                    this->set_char();
                    this->current++;
                }
                this->new_line();
                token = this->get_next();
            }
            return token;
        }
    }
    if (TOKEN_SPECIAL.find(str) != TOKEN_SPECIAL.end()) {
        this->current++;
        this->column++;
        Token token(TOKEN_SPECIAL[str], start);
        token.end_pos = {this->line, this->column};
        return token;
    }
    std::string msg = E_FMT("Unexpected character ");
    msg += E_HLT("'" + std::string(1, this->chris) + "'");
    msg += E_FMT(" at ");
    msg += E_HLT(this->__file__ + ":" + std::to_string(start.line + 1) + ":" + std::to_string(start.column + 1));
    std::cout << msg << std::endl;
    exit(1);
}

void Scanner::accum_token() {
    this->current_tok_str += this->chris;
}

void Scanner::advance_simple() {
    this->current++;
    this->column++;
    this->set_char();
}

Token Scanner::scan_keyword_or_identifier() {
    TextPosition start = {this->line, this->column};
    this->set_char();
    this->start_token();
    while (((isalnum(this->chris) != 0) or this->chris == '_') && !this->at_eof()) {
        this->accum_token();
        this->advance_simple();
    }
    if (TOKEN_KEYWORDS.find(this->current_tok_str) != TOKEN_KEYWORDS.end()) {
//      it's a keyword
        Token token(TOKEN_KEYWORDS[this->current_tok_str], start);
        token.end_pos = {this->line, this->column};
        return token;
    }
//  it's an identifier
    return Token(TokType::ID, this->current_tok_str, start, {this->line, this->column});
}

Token Scanner::scan_number() {
    TextPosition start = {this->line, this->column};
    this->set_char();
    this->start_token();
    while ((isdigit(this->chris) != 0) && !this->at_eof()) {
        this->accum_token();
        this->advance_simple();
    }
    if (!this->at_eof()) {
        if (this->chris == '.') {
            this->accum_token();
            if (this->current + 1 < this->text.size()) {
                if (isdigit(this->text[this->current + 1]) != 0) {
                    // it's a decimal number
                    this->current++;
                    this->set_char();
                    while ((isdigit(this->chris) != 0) && !this->at_eof()) {
                        this->accum_token();
                        this->advance_simple();
                    }
                    TokType tok_type = TokType::FLOAT;
                    if (this->chris == 'd') {
                        //    double
                        this->current++;
                        tok_type = TokType::DOUBLE;
                        if (!this->at_eof()) {
                            this->column++;
                        }
                    }
                    return Token(tok_type, this->current_tok_str, start, {this->line, this->column - 1});
                }
            } else {
                // it's just a dot, so return the number
                return Token(TokType::INTEGER, this->current_tok_str, start, {this->line, this->column - 1});
            }
        }
    }
    return Token(TokType::INTEGER, this->current_tok_str, start, {this->line, this->column});
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

void Scanner::new_line() {
    this->line++;
    this->column = 0;
    this->code_lines.line_offsets.back().length = this->current - this->code_lines.line_offsets.back().offset;
    this->code_lines.line_offsets.push_back(Range{.offset=this->current, .length=0});
}

bool Scanner::at_eof() {
    return this->current >= this->text.size();
}

void Scanner::start_token() {
    this->current_tok_str = "";
}
