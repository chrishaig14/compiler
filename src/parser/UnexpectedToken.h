//
// Created by chris on 8/8/20.
//

#ifndef UNTITLED1_UNEXPECTEDTOKEN_H
#define UNTITLED1_UNEXPECTEDTOKEN_H


#include <stdexcept>
#include <vector>
#include "../scanner/Token.h"

class UnexpectedToken : public std::runtime_error {
    Token token;
    std::vector<TokenType> expected_tokens;
public:
    UnexpectedToken(Token token, const std::vector<TokenType>& expected_tokens);

    std::string make_message(Token token, const std::vector<TokenType>& expected_tokens);

    friend std::ostream& operator<<(std::ostream& os, const UnexpectedToken& unexpected_token);
};
#endif //UNTITLED1_UNEXPECTEDTOKEN_H
