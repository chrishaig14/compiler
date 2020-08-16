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

TEST(total_test, test_3) {
    std::string text = "struct Person { name: String; age: Integer;} fun get_name(p: Person)->String {return p.name;}var chris = Person{name:\"Hello\",age:24}; var foo = get_name(chris);";
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

TEST(total_test, test_4) {
    std::string text = "struct Person { name: String; age: Integer;} fun get_name(p: Person)->String {return p.name;}var chris = Person{name:\"Hello\",age:24}; chris.name = \"Alex\";var foo = get_name(chris);";
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
    expected_object->fields["name"] = new StringObject("Alex");
    expected_object->fields["age"] = new IntegerObject(24);
    EXPECT_TRUE(code_runner.env->get("chris")->equal(expected_object));
    EXPECT_TRUE(code_runner.env->get("foo")->equal(new StringObject("Alex")));
}

TEST(total_test, test_5) {
    std::string text = "var foo = \"Hello\"; foo = 8;";
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
    expected_object->fields["name"] = new StringObject("Alex");
    expected_object->fields["age"] = new IntegerObject(24);
    EXPECT_TRUE(code_runner.env->get("chris")->equal(expected_object));
    EXPECT_TRUE(code_runner.env->get("foo")->equal(new StringObject("Alex")));
}

TEST(total_test, test_6) {
    std::string text = "struct Foo { foo_str: String; bar: Bar; } struct Bar {bar_str: String;} var f = Foo{foo_str: \"FOO_STR\", bar: Bar{bar_str: \"BAR_STR\"}}; var bar = f.bar;";
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
    auto expected_object = new UserObject("Bar", {"bar_str"});
    expected_object->fields["bar_str"] = new StringObject("BAR_STR");
    EXPECT_TRUE(code_runner.env->get("bar")->equal(expected_object));
}

TEST(total_test, test_7) {
    std::string text = "var l = [3,1,4]; var x = l[2];";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    BlockNode* program = parser.parse_program();
    VectorOfNodes list = {NUM(3), NUM(1), NUM(4)};
    auto expected_node = BlockNode({DECL("l", nullptr, LST(list)), DECL("x", nullptr, SUB(ID("l"), NUM(2)))});
    EXPECT_EQ(*program, expected_node);
    GlobalProcessor gp;
    gp.visit(*program);
    Checker checker(gp.globals, gp.class_table);
    checker.visit(*program);
    Translator translator;
    program->accept(translator);
    Code translated_code = translator.code;
    Code expected_code = {I_PUSHI(3), I_PUSHI(1), I_PUSHI(4), I_MAKE_LIST(3), I_DECL("l"), I_SET("l"),
                          I_PUSHI(2), I_GET("l"), I_GETS, I_DECL("x"), I_SET("x")};
    EXPECT_EQ(translated_code, expected_code);
    ObjectStack stack;
    StructProtos structs;
    CodeRunner code_runner(translated_code, structs, stack, {});
    code_runner.run();
    EXPECT_TRUE(code_runner.env->get("x")->equal(new IntegerObject(4)));
}

TEST(total_test, test_factorial_with_for) {
    std::string text = "var result = 1; for(x:[1,2,3,4,5,6,7,8,9,10]){result = result * x;}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    BlockNode* program = parser.parse_program();
    VectorOfNodes list = {NUM(3), NUM(1), NUM(4)};
    auto expected_node = BlockNode({DECL("l", nullptr, LST(list)), DECL("x", nullptr, SUB(ID("l"), NUM(2)))});
    EXPECT_EQ(*program, expected_node);
    GlobalProcessor gp;
    gp.visit(*program);
    Checker checker(gp.globals, gp.class_table);
    checker.visit(*program);
    Translator translator;
    program->accept(translator);
    Code translated_code = translator.code;
    Code expected_code = {I_PUSHI(3), I_PUSHI(1), I_PUSHI(4), I_MAKE_LIST(3), I_DECL("l"), I_SET("l"),
                          I_PUSHI(2), I_GET("l"), I_GETS, I_DECL("x"), I_SET("x")};
    EXPECT_EQ(translated_code, expected_code);
    ObjectStack stack;
    StructProtos structs;
    CodeRunner code_runner(translated_code, structs, stack, {});
    code_runner.run();
    EXPECT_TRUE(code_runner.env->get("x")->equal(new IntegerObject(4)));
}