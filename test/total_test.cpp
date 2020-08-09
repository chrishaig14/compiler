//
// Created by chris on 6/8/20.
//

#include <gtest/gtest.h>
#include <scanner/Scanner.h>
#include <parser/Parser.h>
#include <translator/Translator.h>
#include <vm/CodeRunner.h>
#include <semantic/Checker.h>

TEST(total_test, test_1) {
    std::string text = "fun sum(x: Integer, y: Integer) -> Integer {return x+y;} sum(5, 8);";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    BlockNode* program = parser.parse_program();
    GlobalProcessor gp;
    gp.visit(*program);
    Checker checker(gp.globals, gp.class_table);
    checker.visit(*program);
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
    std::string text = "struct Person { name: String; age: Integer;} fun get_name(p: Person)->String {return p.name;}var chris = Person{name:\"Hello\",age:24}; var foo = chris.name;";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    BlockNode* program = parser.parse_program();
    GlobalProcessor gp;
    gp.visit(*program);
    Checker checker(gp.globals, gp.class_table);
    checker.visit(*program);
    Translator translator;
    program->accept(translator);
    Code translated_code = translator.code;
    ObjectStack stack;
    StructProtos structs;
    CodeRunner code_runner(translated_code, structs, stack, {});
    code_runner.run();
    auto expected_object = new UserObject("Person", {"name", "age"});
    expected_object->fields["name"] = new StringObject("Hello");
    expected_object->fields["age"] = new IntegerObject(24);
    EXPECT_TRUE(code_runner.env->get("chris")->equal(expected_object));
    EXPECT_TRUE(code_runner.env->get("foo")->equal(new StringObject("Hello")));
}

TEST(total_test, test_factorial) {
    std::string text = "fun factorial(x: Integer) -> Integer {if(x==1){return 1;} return x*factorial(x-1);} factorial(10);";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    BlockNode* program = parser.parse_program();
    GlobalProcessor gp;
    gp.visit(*program);
    Checker checker(gp.globals, gp.class_table);
    checker.visit(*program);
    Translator translator;
    program->accept(translator);
    Code translated_code = translator.code;
    ObjectStack stack;
    StructProtos structs;
    CodeRunner code_runner(translated_code, structs, stack, {});
    code_runner.run();
    EXPECT_TRUE(stack.top()->equal(new IntegerObject(3628800)));
}