//
// Created by chris on 6/8/20.
//

#include <gtest/gtest.h>
#include <scanner/Scanner.h>
#include <parser/Parser.h>
#include <translator/Translator.h>
#include <vm/CodeRunner.h>

TEST(total_test, test_1) {
    std::string text = "fun sum(x: Integer, y: Integer) -> Integer {return x+y;} sum(5, 8);";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    BlockNode* program = parser.parse_program();
    Translator translator;
    program->accept(translator);
    Code translated_code = translator.code;
    ObjectStack stack;
    StructProtos structs;
    CodeRunner code_runner(translated_code, structs, stack, {});
    code_runner.run();
    EXPECT_TRUE(stack.top()->equal(new IntegerObject(13)));
}

TEST(total_test, test_2) {
    std::string text = "struct Person { name: String; age: Integer;} fun get_name(p: Person)->String {return p.name;}var chris = Person{}; chris.name = \"Alex\";get_name(chris);";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    BlockNode* program = parser.parse_program();
    Translator translator;
    program->accept(translator);
    Code translated_code = translator.code;
    ObjectStack stack;
    StructProtos structs;
    CodeRunner code_runner(translated_code, structs, stack, {});
    code_runner.run();
    EXPECT_TRUE(stack.top()->equal(new IntegerObject(13)));
}