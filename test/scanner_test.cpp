//
// Created by chris on 6/6/20.
//

#include <gtest/gtest.h>
#include <scanner/Scanner.h>
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

Token st_STRING(std::string s) {
    return Token(TokenType::STRING, s, -1, -1);
}

Token t_ID(std::string s, int line, int column) {
    return Token(TokenType::ID, s, line, column);
}

Token t_NUM(int s, int line, int column) {
    return Token(TokenType::NUM, s, line, column);
}


Token st_FUN = Token(TokenType::FUN, -1, -1);
Token st_SEMICOLON = Token(TokenType::SEMICOLON, -1, -1);
Token st_VAR = Token(TokenType::VAR, -1, -1);
Token st_IF = Token(TokenType::IF, -1, -1);
Token st_ELSE = Token(TokenType::ELSE, -1, -1);
Token st_END = Token(TokenType::END, -1, -1);
Token st_CLASS = Token(TokenType::STRUCT, -1, -1);
Token st_DOT = Token(TokenType::DOT, -1, -1);
Token st_COMMA = Token(TokenType::COMMA, -1, -1);
Token st_RSQUARE = Token(TokenType::RSQUARE, -1, -1);
Token st_LSQUARE = Token(TokenType::LSQUARE, -1, -1);
Token st_COLON = Token(TokenType::COLON, -1, -1);
Token st_LPAREN = Token(TokenType::LPAREN, -1, -1);
Token st_RPAREN = Token(TokenType::RPAREN, -1, -1);
Token st_LCURLY = Token(TokenType::LCURLY, -1, -1);
Token st_RCURLY = Token(TokenType::RCURLY, -1, -1);

Token t_FUN(int line, int column) { return Token(TokenType::FUN, line, column); }

Token t_SEMICOLON(int line, int column) { return Token(TokenType::SEMICOLON, line, column); }

Token t_VAR(int line, int column) { return Token(TokenType::VAR, line, column); }

Token t_IF(int line, int column) { return Token(TokenType::IF, line, column); }

Token t_ELSE(int line, int column) { return Token(TokenType::ELSE, line, column); }

Token t_END(int line, int column) { return Token(TokenType::END, line, column); }

Token t_CLASS(int line, int column) { return Token(TokenType::STRUCT, line, column); }

Token t_DOT(int line, int column) { return Token(TokenType::DOT, line, column); }

Token t_COMMA(int line, int column) { return Token(TokenType::COMMA, line, column); }

Token t_RSQUARE(int line, int column) { return Token(TokenType::RSQUARE, line, column); }

Token t_LSQUARE(int line, int column) { return Token(TokenType::LSQUARE, line, column); }

Token t_COLON(int line, int column) { return Token(TokenType::COLON, line, column); }

Token t_LPAREN(int line, int column) { return Token(TokenType::LPAREN, line, column); }

Token t_RPAREN(int line, int column) { return Token(TokenType::RPAREN, line, column); }

Token t_LCURLY(int line, int column) { return Token(TokenType::LCURLY, line, column); }

Token t_RCURLY(int line, int column) { return Token(TokenType::RCURLY, line, column); }


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

TEST(scanner_test, test_number) {
    std::string text = "123";
    Scanner scanner(text);
    Token token = scanner.get_next();
    EXPECT_EQ(cmp_token_value(token, st_NUM(123)), true);
}

TEST(scanner_test, test_double_colon) {
    std::string text = "a::b";
    Scanner scanner(text);
    Token token = scanner.get_next();
    EXPECT_EQ(cmp_token_value(token, st_ID("a")), true);
    token = scanner.get_next();
    EXPECT_EQ(cmp_token_value(token, Token(TokenType::DOUBLE_COLON, -1, -1)), true);
    token = scanner.get_next();
    EXPECT_EQ(cmp_token_value(token, st_ID("b")), true);
}

TEST(scanner_test, test_string_literal) {
    std::string text = "\"hello\"";
    Scanner scanner(text);
    Token token = scanner.get_next();
    EXPECT_EQ(cmp_token_value(token, st_STRING("hello")), true);
    token = scanner.get_next();

    EXPECT_EQ(token.type, TokenType::END) << TOKEN_STRINGS[token.type];
}

TEST(scanner_test, test_string_full) {
    std::string text = "123\n443\"hello\"";
    Scanner scanner(text);
    Token token = scanner.get_next();
    token = scanner.get_next();
    token = scanner.get_next();
    EXPECT_EQ(token, Token(TokenType::STRING, "hello", 1, 3));
}

TEST(scanner_test, test_string_full_start_end) {
    std::string text = "123\n443\"hello\"";
    Scanner scanner(text);
    Token token = scanner.get_next();
    token = scanner.get_next();
    token = scanner.get_next();
    EXPECT_EQ(token, Token(TokenType::STRING, "hello", 1, 3));
    EXPECT_EQ(token.start, 7);
    EXPECT_EQ(token.end, 13);
}

TEST(scanner_test, test_all_special) {
    for (auto i: TOKEN_SPECIAL) {
        std::string text = i.first;
        Scanner scanner(text);
        Token token = scanner.get_next();
        EXPECT_EQ(cmp_token_value(token, Token(i.second, -1, -1)), true);
        EXPECT_EQ(token.start, 0);
        EXPECT_EQ(token.end, i.first.size() - 1);
    }
}

TEST(scanner_test, test_all_keywords) {
    for (auto i: TOKEN_KEYWORDS) {
        std::string text = i.first;
        Scanner scanner(text);
        Token token = scanner.get_next();
        EXPECT_EQ(cmp_token_value(token, Token(i.second, -1, -1)), true);
        EXPECT_EQ(token.start, 0);
        EXPECT_EQ(token.end, i.first.size() - 1);
    }
}

TEST(scanner_test, test_true_kw) {
    std::string text = "true";
    Scanner scanner(text);
    Token token = scanner.get_next();
    EXPECT_EQ(cmp_token_value(token, Token(TokenType::TRUE, -1, -1)), true);
    EXPECT_EQ(token.start, 0);
    EXPECT_EQ(token.end, 3);
}


TEST(scanner_test, test_identifier_and_eof) {
    std::string text = "hello";
    Scanner scanner(text);
    Token token = scanner.get_next();
    EXPECT_EQ(cmp_token_value(token, st_ID("hello")), true);
    token = scanner.get_next();
    EXPECT_EQ(cmp_token_value(token, st_END), true);
    EXPECT_EQ(token.start, 0);
    EXPECT_EQ(token.end, 4);
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
    std::string text = "struct Foo{var x: String; fun foo(y: Integer) {}}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    std::vector<Token> expected_tokens = {st_CLASS, st_ID("Foo"), st_LCURLY, st_VAR, st_ID("x"), st_COLON,
                                          st_ID("String"), st_SEMICOLON, st_FUN, st_ID("foo"), st_LPAREN,
                                          st_ID("y"), st_COLON, st_ID("Integer"),
                                          st_RPAREN, st_LCURLY,
                                          st_RCURLY, st_RCURLY, st_END};
    EXPECT_EQ(tokens.size(), expected_tokens.size());
    for (int i = 0; i < tokens.size(); i++) {
        Token token = tokens[i];
        Token expected_token = expected_tokens[i];
        EXPECT_EQ(cmp_token_value(token, expected_token), true);
    }
}


TEST(scanner_test, test_token_id_position_1) {
    std::string text = "foo";
    Scanner scanner(text);
    Token token = scanner.get_next();
    EXPECT_EQ(token, Token(TokenType::ID, "foo", 0, 0));
}

TEST(scanner_test, test_token_id_position_2) {
    std::string text = "foo bar";
    Scanner scanner(text);
    Token token = scanner.get_next();
    EXPECT_EQ(token, Token(TokenType::ID, "foo", 0, 0));
    token = scanner.get_next();
    EXPECT_EQ(token, Token(TokenType::ID, "bar", 0, 4));
}

TEST(scanner_test, test_token_id_position_3) {
    std::string text = "foo\nbar";
    Scanner scanner(text);
    Token token = scanner.get_next();
    EXPECT_EQ(token, Token(TokenType::ID, "foo", 0, 0));
    EXPECT_EQ(token.start, 0);
    EXPECT_EQ(token.end, 2);
    token = scanner.get_next();
    EXPECT_EQ(token, Token(TokenType::ID, "bar", 1, 0));
    EXPECT_EQ(token.start, 4);
    EXPECT_EQ(token.end, 6);
}

TEST(scanner_test, test_token_id_position_4) {
    std::string text = "foo\n\n\nbar";
    Scanner scanner(text);
    Token token = scanner.get_next();
    EXPECT_EQ(token, Token(TokenType::ID, "foo", 0, 0));
    token = scanner.get_next();
    EXPECT_EQ(token, Token(TokenType::ID, "bar", 3, 0));
}

TEST(scanner_test, test_token_id_position_5) {
    std::string text = "foo\n\n\nbar fizz";
    Scanner scanner(text);
    Token token = scanner.get_next();
    EXPECT_EQ(token, Token(TokenType::ID, "foo", 0, 0));
    token = scanner.get_next();
    EXPECT_EQ(token, Token(TokenType::ID, "bar", 3, 0));
    token = scanner.get_next();
    EXPECT_EQ(token, Token(TokenType::ID, "fizz", 3, 4));
}

TEST(scanner_test, test_token_num_position_1) {
    std::string text = "123";
    Scanner scanner(text);
    Token token = scanner.get_next();
    EXPECT_EQ(token, Token(TokenType::NUM, 123, 0, 0));
}

TEST(scanner_test, test_token_num_position_2) {
    std::string text = "123\n\n\n456";
    Scanner scanner(text);
    Token token = scanner.get_next();
    EXPECT_EQ(token, Token(TokenType::NUM, 123, 0, 0));
    token = scanner.get_next();
    EXPECT_EQ(token, Token(TokenType::NUM, 456, 3, 0));
}

TEST(scanner_test, test_token_num_position_3) {
    std::string text = "123\n\n\n456 789";
    Scanner scanner(text);
    Token token = scanner.get_next();
    EXPECT_EQ(token, Token(TokenType::NUM, 123, 0, 0));
    token = scanner.get_next();
    EXPECT_EQ(token, Token(TokenType::NUM, 456, 3, 0));
    token = scanner.get_next();
    EXPECT_EQ(token, Token(TokenType::NUM, 789, 3, 4));
}

TEST(scanner_test, test_token_num_position_4) {
    std::string text = "123 456       789";
    Scanner scanner(text);
    Token token = scanner.get_next();
    EXPECT_EQ(token, Token(TokenType::NUM, 123, 0, 0));
    token = scanner.get_next();
    EXPECT_EQ(token, Token(TokenType::NUM, 456, 0, 4));
    token = scanner.get_next();
    EXPECT_EQ(token, Token(TokenType::NUM, 789, 0, 14));
}

TEST(scanner_test, test_token_keyword_position_1) {
    std::string text = "struct";
    Scanner scanner(text);
    Token token = scanner.get_next();
    EXPECT_EQ(token, Token(TokenType::STRUCT, 0, 0));
}

TEST(scanner_test, test_token_keyword_position_2) {
    std::string text = "struct fun";
    Scanner scanner(text);
    Token token = scanner.get_next();
    EXPECT_EQ(token, Token(TokenType::STRUCT, 0, 0));
    token = scanner.get_next();
    EXPECT_EQ(token, Token(TokenType::FUN, 0, 7));
}

TEST(scanner_test, test_token_keyword_position_3) {
    std::string text = "struct\n\n\nfun";
    Scanner scanner(text);
    Token token = scanner.get_next();
    EXPECT_EQ(token, Token(TokenType::STRUCT, 0, 0));
    token = scanner.get_next();
    EXPECT_EQ(token, Token(TokenType::FUN, 3, 0));
}

TEST(scanner_test, test_token_special_position_1) {
    std::string text = "+";
    Scanner scanner(text);
    Token token = scanner.get_next();
    EXPECT_EQ(token, Token(TokenType::PLUS, 0, 0));
}

TEST(scanner_test, test_token_special_position_2) {
    std::string text = "+    -";
    Scanner scanner(text);
    Token token = scanner.get_next();
    EXPECT_EQ(token, Token(TokenType::PLUS, 0, 0));
    token = scanner.get_next();
    EXPECT_EQ(token, Token(TokenType::MINUS, 0, 5));
}

TEST(scanner_test, test_token_special_position_3) {
    std::string text = "+\n\n\n-";
    Scanner scanner(text);
    Token token = scanner.get_next();
    EXPECT_EQ(token, Token(TokenType::PLUS, 0, 0));
    token = scanner.get_next();
    EXPECT_EQ(token, Token(TokenType::MINUS, 3, 0));
}

TEST(scanner_test, test_token_special_position_4) {
    std::string text = "+\n\n\n--++";
    Scanner scanner(text);
    Token token = scanner.get_next();
    EXPECT_EQ(token, Token(TokenType::PLUS, 0, 0));
    token = scanner.get_next();
    EXPECT_EQ(token, Token(TokenType::DEC, 3, 0));
    token = scanner.get_next();
    EXPECT_EQ(token, Token(TokenType::INC, 3, 2));
}

TEST(scanner_test, test_token_position_complex) {
    std::string text = "struct Foo {\nvar x: String;\nfun foo(y: Integer) {\n}\n}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    std::vector<Token> expected_tokens = {
            t_CLASS(0, 0), t_ID("Foo", 0, 7), t_LCURLY(0, 11), t_VAR(1, 0),
            t_ID("x", 1, 4), t_COLON(1, 5),
            t_ID("String", 1, 7), t_SEMICOLON(1, 13), t_FUN(2, 0), t_ID("foo", 2, 4),
            t_LPAREN(2, 7),
            t_ID("y", 2, 8), t_COLON(2, 9), t_ID("Integer", 2, 11),
            t_RPAREN(2, 18), t_LCURLY(2, 20),
            t_RCURLY(3, 0), t_RCURLY(4, 0), t_END(4, 1)};
    EXPECT_EQ(tokens.size(), expected_tokens.size());
    for (int i = 0; i < tokens.size(); i++) {
        Token token = tokens[i];
        Token expected_token = expected_tokens[i];
        EXPECT_EQ(token, expected_token);
    }
}

TEST(scanner_test, test_unexpected_character_position) {
    std::string text = "fo%o";
    Scanner scanner(text);
    try {
        scanner.scan_all();
        FAIL() << "Expected UnexpectedCharacter error";
    } catch (const UnexpectedCharacter& uc) {
        EXPECT_EQ(uc.line, 0);
        EXPECT_EQ(uc.column, 2);
    }
}