//
// Created by chris on 6/6/20.
//

#include <gtest/gtest.h>
#include <Scanner.h>
#include <vector>


bool cmp_token_value(Token a, Token b) {
    return a.type == b.type and a.str == b.str and a.num == b.num;
}

bool cmp_token_full(Token a, Token b) {
    return cmp_token_value(a, b) and a.line == b.line and a.column == b.column;
}

Token st_ID(std::string s) {
    return Token(TokenType::ID, s, -1, -1);
}

Token st_NUM(int s) {
    return Token(TokenType::NUM, s, -1, -1);
}


Token st_FUN = Token(TokenType::FUN, -1, -1);
Token st_SEMICOLON = Token(TokenType::SEMICOLON, -1, -1);
Token st_VAR = Token(TokenType::VAR, -1, -1);
Token st_IF = Token(TokenType::IF, -1, -1);
Token st_ELSE = Token(TokenType::ELSE, -1, -1);
Token st_END = Token(TokenType::END, -1, -1);
Token st_CLASS = Token(TokenType::CLASS, -1, -1);
Token st_INTERFACE = Token(TokenType::INTERFACE, -1, -1);
Token st_DOT = Token(TokenType::DOT, -1, -1);
Token st_COMMA = Token(TokenType::COMMA, -1, -1);
Token st_RSQUARE = Token(TokenType::RSQUARE, -1, -1);
Token st_LSQUARE = Token(TokenType::LSQUARE, -1, -1);
Token st_COLON = Token(TokenType::COLON, -1, -1);
Token st_LPAREN = Token(TokenType::LPAREN, -1, -1);
Token st_RPAREN = Token(TokenType::RPAREN, -1, -1);
Token st_LCURLY = Token(TokenType::LCURLY, -1, -1);
Token st_RCURLY = Token(TokenType::RCURLY, -1, -1);


TEST(scanner_test, test_only_alpha_identifier) {
    std::string text = "hello";
    Scanner scanner(text);
    Token token = scanner.get_next();
    EXPECT_EQ(cmp_token_value(token, st_ID("hello")), true);
}

TEST(scanner_test, test_identifier_complex) {
    std::string text = "_32hello";
    Scanner scanner(text);
    Token token = scanner.get_next();
    EXPECT_EQ(cmp_token_value(token, st_ID("_32hello")), true);
}


TEST(scanner_test, test_fun) {
    std::string text = "fun";
    Scanner scanner(text);
    Token token = scanner.get_next();
    EXPECT_EQ(cmp_token_value(token, st_FUN), true);
}

TEST(scanner_test, test_class) {
    std::string text = "class";
    Scanner scanner(text);
    Token token = scanner.get_next();
    EXPECT_EQ(cmp_token_value(token, st_CLASS), true);
}

TEST(scanner_test, test_interface) {
    std::string text = "interface";
    Scanner scanner(text);
    Token token = scanner.get_next();
    EXPECT_EQ(cmp_token_value(token, st_INTERFACE), true);
}

TEST(scanner_test, test_if) {
    std::string text = "if";
    Scanner scanner(text);
    Token token = scanner.get_next();
    EXPECT_EQ(cmp_token_value(token, st_IF), true);
}

TEST(scanner_test, test_else) {
    std::string text = "else";
    Scanner scanner(text);
    Token token = scanner.get_next();
    EXPECT_EQ(cmp_token_value(token, st_ELSE), true);
}


TEST(scanner_test, test_var) {
    std::string text = "var";
    Scanner scanner(text);
    Token token = scanner.get_next();
    EXPECT_EQ(cmp_token_value(token, st_VAR), true);
}

TEST(scanner_test, test_number) {
    std::string text = "123";
    Scanner scanner(text);
    Token token = scanner.get_next();
    EXPECT_EQ(cmp_token_value(token, st_NUM(123)), true);
}

TEST(scanner_test, test_all_special) {
    for (auto i: TOKEN_SPECIAL) {
        std::string text = i.first;
        Scanner scanner(text);
        Token token = scanner.get_next();
        EXPECT_EQ(cmp_token_value(token, Token(i.second, -1, -1)), true);
    }
}


TEST(scanner_test, test_identifier_and_eof) {
    std::string text = "hello";
    Scanner scanner(text);
    Token token = scanner.get_next();
    EXPECT_EQ(cmp_token_value(token, st_ID("hello")), true);
    token = scanner.get_next();
    EXPECT_EQ(cmp_token_value(token, st_END), true);
}

TEST(scanner_test, test_keyword_and_eof) {
    std::string text = "fun";
    Scanner scanner(text);
    Token token = scanner.get_next();
    EXPECT_EQ(cmp_token_value(token, st_FUN), true);
    token = scanner.get_next();
    EXPECT_EQ(cmp_token_value(token, st_END), true);
}

TEST(scanner_test, test_number_and_eof) {
    std::string text = "123";
    Scanner scanner(text);
    Token token = scanner.get_next();
    EXPECT_EQ(cmp_token_value(token, st_NUM(123)), true);
    token = scanner.get_next();
    EXPECT_EQ(cmp_token_value(token, st_END), true);
}

TEST(scanner_test, test_empty_eof) {
    std::string text = "";
    Scanner scanner(text);
    Token token = scanner.get_next();
    EXPECT_EQ(cmp_token_value(token, st_END), true);
}

TEST(scanner_test, test_empty_eof_twice) {
    std::string text = "";
    Scanner scanner(text);
    Token token = scanner.get_next();
    token = scanner.get_next();
    EXPECT_EQ(cmp_token_value(token, st_END), true);
}

TEST(scanner_test, test_scan_all) {
    std::string text = "class Foo{var x: String; fun foo(y: Integer) {}}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    std::vector<Token> expected_tokens = {st_CLASS, st_ID("Foo"), st_LCURLY, st_VAR, st_ID("x"), st_COLON,
                                          st_ID("String"), st_SEMICOLON, st_FUN, st_ID("foo"), st_LPAREN,
                                          st_ID("y"), st_COLON, st_ID("Integer"),
                                          st_RPAREN, st_LCURLY,
                                          st_RCURLY, st_RCURLY, st_END};
    EXPECT_EQ(tokens.size(),expected_tokens.size());
    for (int i = 0; i < tokens.size(); i++) {
        Token token = tokens[i];
        Token expected_token = expected_tokens[i];
        EXPECT_EQ(cmp_token_value(token, expected_token), true);
    }
}

bool cmp_token_type_value(Token a, Token b) {
    return a.type == b.type and a.str == b.str and a.num == b.num;
}

TEST(scanner_test, test_line_number) {
    std::string text = "foo";
    Scanner scanner(text);
    Token token = scanner.get_next();
    EXPECT_EQ(cmp_token_value(token, st_ID("foo")), true);
}