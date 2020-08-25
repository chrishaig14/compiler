//
// Created by chris on 6/8/20.
//

#include <gtest/gtest.h>
#include <scanner/Scanner.h>
#include <parser/Parser.h>
#include <translator/Translator.h>
#include <vm/CodeRunner.h>
#include <semantic/Checker.h>
#include <vm/Loader.h>

TEST(total_test, test_1) {
    std::string text = "fun sum(x: Integer, y: Integer) -> Integer {return x-y;} fun main()->None{sum(5, 8);}";
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
    CodeLabel translated_code = translator.code;
    Loader loader(translated_code);
    loader.load();
    Environment* global_env = loader.global_env;
    ObjectStack stack;
    StructProtos structs;
    CodeObject* main_function = dynamic_cast<CodeObject*>(global_env->get("main"));
    CodeRunner code_runner(main_function->user->code, structs, stack, global_env);
    code_runner.run();
    EXPECT_TRUE(stack.top()->equal(new IntegerObject(-3)));
}

TEST(total_test, test_factorial) {
    std::string text = "fun factorial(x: Integer) -> Integer {if(x==1){return 1;} return x*factorial(x-1);}fun main()->None{ factorial(10);}";
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
    ObjectStack stack;
    StructProtos structs;
    CodeLabel translated_code = translator.code;
    Loader loader(translated_code);
    loader.load();
    Environment* global_env = loader.global_env;
    CodeObject* main_function = dynamic_cast<CodeObject*>(global_env->get("main"));
    CodeRunner code_runner(main_function->user->code, structs, stack, global_env);
    code_runner.run();
    EXPECT_TRUE(stack.top()->equal(new IntegerObject(3628800)));
}

TEST(total_test, test_factorial_while_main) {
    std::string text = "fun main()->Integer{var n = 10; var i = 1; var result = 1; while (i<=n){result = result * i; i = i + 1;} return result;}";
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
    ObjectStack stack;
    StructProtos structs;
    CodeLabel translated_code = translator.code;
    Loader loader(translated_code);
    loader.load();
    Environment* global_env = loader.global_env;
    CodeObject* main_function = dynamic_cast<CodeObject*>(global_env->get("main"));
    CodeRunner code_runner(main_function->user->code, structs, stack, global_env);
    code_runner.run();
    EXPECT_TRUE(stack.top()->equal(new IntegerObject(3628800)));
}

TEST(total_test, test_factorial_while_function) {
    std::string text = "fun factorial(n: Integer)->Integer{ var i = 1; var result = 1; while (i<=n){result = result * i; i = i + 1;} return result;} fun main()->None{factorial(10);}";
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
    ObjectStack stack;
    StructProtos structs;
    CodeLabel translated_code = translator.code;
    Loader loader(translated_code);
    loader.load();
    Environment* global_env = loader.global_env;
    CodeObject* main_function = dynamic_cast<CodeObject*>(global_env->get("main"));
    CodeRunner code_runner(main_function->user->code, structs, stack, global_env);
    code_runner.run();
    EXPECT_TRUE(stack.top()->equal(new IntegerObject(3628800)));
}

TEST(total_test, test_struct) {
    std::string text = "struct Person{name: String; age:Integer;} fun main()->Person{var x = Person{age: 26, name: \"chris\"}; return x;}";
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
    ObjectStack stack;
    StructProtos structs;
    CodeLabel translated_code = translator.code;
    Loader loader(translated_code);
    loader.load();
    Environment* global_env = loader.global_env;
    CodeObject* main_function = dynamic_cast<CodeObject*>(global_env->get("main"));
    CodeRunner code_runner(main_function->user->code, structs, stack, global_env);
    code_runner.run();
    auto object = new UserObject("Person", {"name", "string"});
    object->fields["name"] = new StringObject("chris");
    object->fields["age"] = new IntegerObject(26);
    EXPECT_TRUE(stack.top()->equal(object));
}

TEST(total_test, test_list) {
    std::string text = "fun main()->Integer{var l = [1,4,3,6]; return l[2];}";
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
    ObjectStack stack;
    StructProtos structs;
    CodeLabel translated_code = translator.code;
    Loader loader(translated_code);
    loader.load();
    Environment* global_env = loader.global_env;
    CodeObject* main_function = dynamic_cast<CodeObject*>(global_env->get("main"));
    CodeRunner code_runner(main_function->user->code, structs, stack, global_env);
    code_runner.run();
    EXPECT_TRUE(stack.top()->equal(new IntegerObject(3)));
}