//
// Created by chris on 6/6/20.
//

#include <iostream>
#include "Scanner.h"
#include "color_codes.h"

std::map<std::string, TokenType> TOKEN_KEYWORDS;
std::map<std::string, TokenType> TOKEN_SPECIAL;

/*
 * fun initialize_tokens(){
 *  TOKEN_KEYWORDS["fun"] = Token(TokenType.FUN);
 *  ...
 * }
 *
 * var TOKEN_KEYWORDS: Dict[String, Token];
 * */

void initialize_tokens() {
    TOKEN_KEYWORDS["fun"] = TokenType::FUN;
    TOKEN_KEYWORDS["if"] = TokenType::IF;
    TOKEN_KEYWORDS["class"] = TokenType::CLASS;
    TOKEN_KEYWORDS["else"] = TokenType::ELSE;
    TOKEN_KEYWORDS["for"] = TokenType::FOR;
    TOKEN_KEYWORDS["interface"] = TokenType::INTERFACE;
    TOKEN_KEYWORDS["var"] = TokenType::VAR;


    TOKEN_SPECIAL[";"] = TokenType::SEMICOLON;
    TOKEN_SPECIAL[","] = TokenType::COMMA;
    TOKEN_SPECIAL[":"] = TokenType::COLON;
    TOKEN_SPECIAL["."] = TokenType::DOT;

    TOKEN_SPECIAL["["] = TokenType::LSQUARE;
    TOKEN_SPECIAL["]"] = TokenType::RSQUARE;
    TOKEN_SPECIAL["("] = TokenType::LPAREN;
    TOKEN_SPECIAL[")"] = TokenType::RPAREN;
    TOKEN_SPECIAL["{"] = TokenType::LCURLY;
    TOKEN_SPECIAL["}"] = TokenType::RCURLY;

    TOKEN_SPECIAL["+"] = TokenType::PLUS;
    TOKEN_SPECIAL["-"] = TokenType::MINUS;
    TOKEN_SPECIAL["*"] = TokenType::TIMES;
    TOKEN_SPECIAL["/"] = TokenType::DIV;

    TOKEN_SPECIAL["="] = TokenType::EQQ;
    TOKEN_SPECIAL["+="] = TokenType::PLUS_EQQ;
    TOKEN_SPECIAL["-="] = TokenType::MINUS_EQQ;
    TOKEN_SPECIAL["*="] = TokenType::TIMES_EQQ;
    TOKEN_SPECIAL["/="] = TokenType::DIV_EQQ;

    TOKEN_SPECIAL["<"] = TokenType::LT;
    TOKEN_SPECIAL[">"] = TokenType::GT;
    TOKEN_SPECIAL["<="] = TokenType::LEQ;
    TOKEN_SPECIAL[">="] = TokenType::GEQ;
    TOKEN_SPECIAL["=="] = TokenType::EQ;
    TOKEN_SPECIAL["!="] = TokenType::NEQ;
    TOKEN_SPECIAL["!"] = TokenType::NOT;
    TOKEN_SPECIAL["&&"] = TokenType::AND;
    TOKEN_SPECIAL["||"] = TokenType::OR;
    TOKEN_SPECIAL["=>"] = TokenType::RARROW;
    TOKEN_SPECIAL["++"] = TokenType::INC;
    TOKEN_SPECIAL["--"] = TokenType::DEC;
}

Scanner::Scanner(const std::string &text) {
    initialize_tokens();
    initialize_token_strings();
    this->text = text;
    this->current = 0;
}

Token Scanner::get_next() {
    if (this->current >= this->text.size()) {
        return Token(TokenType::END, -1, -1);
    }
    char c = this->text[this->current];
    while (isspace(c)) {
        this->current++;
        c = this->text[this->current];
    }
    if (isalpha(c) || c == '_') {
//        std::cout << HCYN << "Gonna scan for keyword or identifier" << reset << std::endl;
        return this->scan_keyword_or_identifier();
    }
    if (isdigit(c)) {
//        std::cout << HCYN << "Gonna scan for number" << reset << std::endl;
        return this->scan_number();
    }
    return this->scan_other();
}

Token Scanner::scan_other() {
    char c = this->text[this->current];
    std::string str;
    str.push_back(c);
    size_t p = this->current + 1;
    if (p < this->text.size()) {
        std::string tstr = str;
        tstr.push_back(this->text[p]);
        if (TOKEN_SPECIAL.count(tstr) == 1) {
            this->current+= 2;
            return Token(TOKEN_SPECIAL[tstr], -1, -1);
        }
    }
    if (TOKEN_SPECIAL.count(str) == 1) {
        this->current++;
        return Token(TOKEN_SPECIAL[str], -1, -1);
    }
    throw UnexpectedCharacter(c, this->current);
}

Token Scanner::scan_keyword_or_identifier() {
    char c = this->text[this->current];
    std::string str;
    while (isalnum(c) or c == '_') {
        str += c;
        this->current++;
        if (this->current < this->text.size()) {
            c = this->text[this->current];
        } else {
            break;
        }
    }

    if (TOKEN_KEYWORDS.count(str) == 1) {
//      it's a keyword
        Token token = Token(TOKEN_KEYWORDS[str], -1, -1);
        return token;
    }
//  it's an identifier
    return Token(TokenType::ID, str, -1, -1);
}

Token Scanner::scan_number() {
    char c = this->text[this->current];
    std::string str;
    while (isdigit(c)) {
        str += c;
        this->current++;
        if (this->current < this->text.size()) {
            c = this->text[this->current];
        } else {
            break;
        }
    }
    return Token(TokenType::NUM, std::stoi(str), -1, -1);
}

bool UnexpectedCharacter::operator==(const UnexpectedCharacter &other) const {
    return this->c == other.c && this->position == other.position;
}

UnexpectedCharacter::UnexpectedCharacter(char c, size_t position) {
    this->c = c;
    this->position = position;
}
