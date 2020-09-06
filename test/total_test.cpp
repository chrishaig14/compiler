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
#include <vm/BuiltinIntegerToString.h>
#include <vm/BuiltinPrintString.h>

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
    Loader loader(translated_code,{});
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
    Loader loader(translated_code,{});
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
    Loader loader(translated_code,{});
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
    Loader loader(translated_code,{});
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
    Loader loader(translated_code,{});
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
    Loader loader(translated_code,{});
    loader.load();
    Environment* global_env = loader.global_env;
    CodeObject* main_function = dynamic_cast<CodeObject*>(global_env->get("main"));
    CodeRunner code_runner(main_function->user->code, structs, stack, global_env);
    code_runner.run();
    EXPECT_TRUE(stack.top()->equal(new IntegerObject(3)));
}

TEST(total_test, object_reference) {
    std::string text = "struct Person{name:String; age:Integer;} fun main()->String{var p = Person{name:\"chris\",age:26}; var c = p; c.name = \"John\"; return p.name;}";
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
    Loader loader(translated_code,{});
    loader.load();
    Environment* global_env = loader.global_env;
    CodeObject* main_function = dynamic_cast<CodeObject*>(global_env->get("main"));
    CodeRunner code_runner(main_function->user->code, structs, stack, global_env);
    code_runner.run();
    EXPECT_TRUE(stack.top()->equal(new StringObject("John")));
}

TEST(total_test, object_reference_2) {
    std::string text = "struct Person{name:String; age:Integer;} fun main()->String{var p = Person{name:\"chris\",age:26}; var c = p; c.name = \"John\"; c = Person{name:\"Alex\",age:24}; return p.name;}";
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
    Loader loader(translated_code,{});
    loader.load();
    Environment* global_env = loader.global_env;
    CodeObject* main_function = dynamic_cast<CodeObject*>(global_env->get("main"));
    CodeRunner code_runner(main_function->user->code, structs, stack, global_env);
    code_runner.run();
    EXPECT_TRUE(stack.top()->equal(new StringObject("John")));
}

TEST(total_test, test_optional) {
    std::string text = "fun main()->String{var x: Option[Integer] = none; var s = x?\"one\":\"two\"; return s;}";
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
    Loader loader(translated_code, {});
    loader.load();
    Environment* global_env = loader.global_env;
    CodeObject* main_function = dynamic_cast<CodeObject*>(global_env->get("main"));
    CodeRunner code_runner(main_function->user->code, structs, stack, global_env);
    code_runner.run();
    EXPECT_TRUE(stack.top()->equal(new StringObject("two")));
}

std::ostream& operator<<(std::ostream& out, const CodeLabel v);

TEST(total_test, test_optional_2) {
    std::string text = "fun main()->String{var x: Option[Integer] = 4; var s = x?\"one\":\"two\"; return s;}";
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
    std::cerr <<  translated_code << std::endl;
    Loader loader(translated_code, {});
    loader.load();
    Environment* global_env = loader.global_env;
    CodeObject* main_function = dynamic_cast<CodeObject*>(global_env->get("main"));
    for (int i = 0; i < main_function->user->code.size(); i++) {
        std::cerr <<  main_function->user->code[i]->to_string() << std::endl;
    }
    CodeRunner code_runner(main_function->user->code, structs, stack, global_env);
    code_runner.run();
    EXPECT_TRUE(stack.top()->equal(new StringObject("one")));
}

TEST(total_test, test_inorder) {
    std::string text = "struct LinkedList {head: Integer; tail: Option[LinkedList];}fun length(l: LinkedList)->Integer {return 1 + (l.tail?length(it):0);} fun main()->Option[Integer]{var x = LinkedList{head: 7, tail:LinkedList{head:9, tail:LinkedList{head:3, tail:none}}}; return length(x);}";
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
    std::cerr <<  translated_code << std::endl;
    Loader loader(translated_code, {});
    loader.load();
    Environment* global_env = loader.global_env;
    CodeObject* main_function = dynamic_cast<CodeObject*>(global_env->get("main"));
    for (int i = 0; i < main_function->user->code.size(); i++) {
        std::cerr <<  main_function->user->code[i]->to_string() << std::endl;
    }
    CodeRunner code_runner(main_function->user->code, structs, stack, global_env);
    code_runner.run();
    EXPECT_TRUE(stack.top()->equal(new IntegerObject(3)));
    EXPECT_FALSE(stack.top()->equal(new IntegerObject(7)));
}

TEST(total_test, test_print_int) {
    std::string text = "fun main()->Integer{print(str(7)); return 9;}";
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
    std::cerr <<  translated_code << std::endl;
    Loader loader(translated_code, {});
    loader.load();
    Environment* global_env = loader.global_env;
    CodeObject* main_function = dynamic_cast<CodeObject*>(global_env->get("main"));
    for (int i = 0; i < main_function->user->code.size(); i++) {
        std::cerr <<  main_function->user->code[i]->to_string() << std::endl;
    }
    CodeRunner code_runner(main_function->user->code, structs, stack, global_env);
    code_runner.run();
    EXPECT_TRUE(stack.top()->equal(new IntegerObject(3)));
    EXPECT_FALSE(stack.top()->equal(new IntegerObject(7)));
}

TEST(total_test, test_set_list_index) {
    std::string text = "fun main()->Integer{var x = [7 ,9 ,12]; x[1] = 3; return x[1] + x[2] + 4;}";
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
    std::cerr <<  translated_code << std::endl;
    Loader loader(translated_code, {});
    loader.load();
    Environment* global_env = loader.global_env;
    CodeObject* main_function = dynamic_cast<CodeObject*>(global_env->get("main"));
    for (int i = 0; i < main_function->user->code.size(); i++) {
        std::cerr <<  main_function->user->code[i]->to_string() << std::endl;
    }
    CodeRunner code_runner(main_function->user->code, structs, stack, global_env);
    code_runner.run();
    EXPECT_TRUE(stack.top()->equal(new IntegerObject(19)));
    EXPECT_FALSE(stack.top()->equal(new IntegerObject(7)));
}



TEST(total_test, test_overloading) {
    std::string text = "fun foo(x: Integer)->String{return str(x);} fun foo(x: String)->String{return x;} fun main()->Integer{print(foo(2));print(foo(\"Hola\"));return 7;}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    BlockNode* program = parser.parse_program();
    std::map<std::string, CodeBuiltin*> builtins;
    builtins["str"] = new BuiltinIntegerToString();
    builtins["print"] = new BuiltinPrintString();
    GlobalProcessor gp(builtins);
    gp.visit(*program);
    Checker checker(gp.globals, gp.class_table);
    checker.visit(*program);
    Translator translator;
    program->accept(translator);
    ObjectStack stack;
    StructProtos structs;
    CodeLabel translated_code = translator.code;
    std::cerr <<  translated_code << std::endl;
    Loader loader(translated_code, builtins);
    loader.load();
    Environment* global_env = loader.global_env;
    CodeObject* main_function = dynamic_cast<CodeObject*>(global_env->get("main:"));
    for (int i = 0; i < main_function->user->code.size(); i++) {
        std::cerr <<  main_function->user->code[i]->to_string() << std::endl;
    }
    CodeRunner code_runner(main_function->user->code, structs, stack, global_env);
    code_runner.run();
    EXPECT_TRUE(stack.top()->equal(new IntegerObject(7)));
}


TEST(total_test, test_empty_list) {
    std::string text = "fun main()->Integer{var l = []::List[Integer]; return 0;}";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    BlockNode* program = parser.parse_program();
    std::map<std::string, CodeBuiltin*> builtins;
    builtins["str"] = new BuiltinIntegerToString();
    builtins["print"] = new BuiltinPrintString();
    GlobalProcessor gp(builtins);
    gp.visit(*program);
    Checker checker(gp.globals, gp.class_table);
    checker.visit(*program);
    Translator translator;
    program->accept(translator);
    ObjectStack stack;
    StructProtos structs;
    CodeLabel translated_code = translator.code;
    std::cerr <<  translated_code << std::endl;
    Loader loader(translated_code, builtins);
    loader.load();
    Environment* global_env = loader.global_env;
    CodeObject* main_function = dynamic_cast<CodeObject*>(global_env->get("main:"));
    for (int i = 0; i < main_function->user->code.size(); i++) {
        std::cerr <<  main_function->user->code[i]->to_string() << std::endl;
    }
    CodeRunner code_runner(main_function->user->code, structs, stack, global_env);
    code_runner.run();
    EXPECT_TRUE(stack.top()->equal(new IntegerObject(0)));
}

