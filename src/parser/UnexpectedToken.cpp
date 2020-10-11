//
// Created by chris on 8/8/20.
//

#include "UnexpectedToken.h"

UnexpectedToken::UnexpectedToken(Token token, const std::vector<TokenType>& expected_tokens) : std::runtime_error(
        this->make_message(token, expected_tokens)) {
    this->token = token;
    this->expected_tokens = expected_tokens;
}

std::string UnexpectedToken::make_message(Token token, const std::vector<TokenType>& expected_tokens) {
    std::string message;
    std::string expected_strings;
    if (expected_tokens.size() > 1) {
        for (int i = 0; i < expected_tokens.size() - 1; i++) {
            expected_strings += TOKEN_STRINGS[expected_tokens[i]] + ", ";
        }
        expected_strings = expected_strings.substr(0,expected_strings.size()-2);
        expected_strings +=
                " or " + TOKEN_STRINGS[expected_tokens[expected_tokens.size() - 1]];
    } else {
        expected_strings += TOKEN_STRINGS[expected_tokens[0]];
    }
    message = "UnexpectedToken at line " + std::to_string(token.line+1) + " column " + std::to_string(token.column+1) +
              ": got " + token.to_string() + ", expected " + expected_strings;
    return message;
}

std::ostream& operator<<(std::ostream& os, const UnexpectedToken& unexpected_token) {
    std::string expected_strings;
    for (int i = 0; i < unexpected_token.expected_tokens.size() - 1; i++) {
        expected_strings += TOKEN_STRINGS[unexpected_token.expected_tokens[i]];
    }
    expected_strings +=
            " or " + TOKEN_STRINGS[unexpected_token.expected_tokens[unexpected_token.expected_tokens.size() - 1]];
    os << "UnexpectedToken: got " << TOKEN_STRINGS[unexpected_token.token.type] << ", expected "
       << expected_strings;
    return os;
}