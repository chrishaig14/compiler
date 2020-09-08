#include <gtest/gtest.h>
#include <parser/Parser.h>
#include <scanner/Scanner.h>
#include <semantic/GlobalProcessor.h>
#include <semantic/Checker.h>
#include <vm/BuiltinIntegerToString.h>
#include <vm/BuiltinPrintString.h>

BlockNode* get_ast(std::string text) {
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    BlockNode* tree = parser.parse_program();
    return tree;
}

void compile(std::string text) {
    BlockNode* tree = get_ast(text);
    std::map<std::string, CodeBuiltin*> builtins;
    builtins["str"] = new BuiltinIntegerToString();
    builtins["print"] = new BuiltinPrintString();
    GlobalProcessor gp(builtins);
    gp.visit(*tree);
    Checker checker(gp.globals, gp.class_table);
    checker.function_table = gp.function_table;
    checker.visit(*tree);
}

void ASSERT_THROWS_NOT_FOUND_ERROR(std::string text, std::string NAME) {
    try {
        compile(text);
        FAIL() << "Expected ScopeError thrown";
    } catch (const ScopeError& se) {
        EXPECT_EQ(se, ScopeError(NAME));
    }
}


void ASSERT_THROWS_REDECLARED_ERROR(std::string text, std::string NAME) {
    try {
        compile(text);
        FAIL() << "Expected RedeclareError thrown";
    } catch (const RedeclareError& se) {
        EXPECT_EQ(se, RedeclareError(NAME)) << se.what();
    }
}

void
ASSERT_THROWS_RETURN_TYPE_ERROR(std::string text, std::string NAME, TypeNode* EXPECTED_TYPE, TypeNode* ACTUAL_TYPE) {
    try {
        compile(text);
        FAIL() << "Expected ReturnError thrown";
    } catch (const ReturnError& se) {
        EXPECT_EQ(se, ReturnError(EXPECTED_TYPE, ACTUAL_TYPE)) << se.what();
    }
}

void ASSERT_THROWS_ASSIGNMENT_ERROR(std::string text, TypeNode* EXPECTED_TYPE, TypeNode* ACTUAL_TYPE) {
    try {
        compile(text);
        FAIL() << "Expected AssignmentTypeError thrown";
    } catch (const AssignmentTypeError& se) {
        EXPECT_EQ(se, AssignmentTypeError(EXPECTED_TYPE, ACTUAL_TYPE)) << se.what();
    }
}

void ASSERT_THROWS_BAD_ARGUMENTS(std::string text) {
    try {
        compile(text);
        FAIL() << "Expected BadArguments thrown";
    } catch (const BadArguments& se) {
    }
}

void ASSERT_OK(std::string text) { compile(text); }

void ASSERT_VARIABLE_TYPE(std::string text, std::string id, TypeNode* type) {
    BlockNode* tree = get_ast(text);
    GlobalProcessor gp;
    gp.visit(*tree);
    Checker checker(gp.globals, gp.class_table);
    checker.visit(*tree);
    EXPECT_TRUE(checker.scopes["global"]->get(id)->equal(type));
}

TEST(second_pass_test, fun_foo_cAomplete) {
    std::string text = "fun foo(y: Foo)->Integer{}";
    BlockNode* tree = get_ast(text);
    GlobalProcessor gp;
    gp.visit(*tree);
    Checker checker(gp.globals, gp.class_table);
    checker.visit(*tree);
    SymbolTable* foo_scope = checker.scopes["global.foo"];
    ObjectTypeNode* sinfo = dynamic_cast<ObjectTypeNode*>(foo_scope->get("y"));
    EXPECT_NE(sinfo, nullptr);
    EXPECT_TRUE(((FunctionNode*) tree->nodes[0])->free_variables.size() == 0);
    EXPECT_EQ(sinfo->identifier, "Foo");
}

TEST(second_pass_test, free_variable_test_1) {
    std::string text = "var x: Integer=0; fun foo(y: Foo)->Integer{return x;}";
    BlockNode* tree = get_ast(text);
    GlobalProcessor gp;
    gp.visit(*tree);
    Checker checker(gp.globals, gp.class_table);
    checker.visit(*tree);
    EXPECT_TRUE(checker.scopes["global"]->has("x"));
    SymbolTable* foo_scope = checker.scopes["global.foo"];
    auto sinfo = dynamic_cast<ObjectTypeNode*>(foo_scope->get("y"));
    EXPECT_TRUE(((FunctionNode*) tree->nodes[1])->free_variables.size() == 1);
    EXPECT_TRUE(((FunctionNode*) tree->nodes[1])->free_variables.count("x") == 1);
    EXPECT_NE(sinfo, nullptr);
    EXPECT_EQ(sinfo->identifier, "Foo");
}


TEST(second_pass_test, free_variable_test_2) {
    std::string text = "var x: Integer=0; fun foo(y: Foo)->Integer{var z: Integer = 1 + x;}";
    BlockNode* tree = get_ast(text);
    GlobalProcessor gp;
    gp.visit(*tree);
    Checker checker(gp.globals, gp.class_table);
    checker.visit(*tree);
    EXPECT_TRUE(checker.scopes["global"]->has("x"));
    SymbolTable* foo_scope = checker.scopes["global.foo"];
    auto sinfo = dynamic_cast<ObjectTypeNode*>(foo_scope->get("y"));
    EXPECT_TRUE(((FunctionNode*) tree->nodes[1])->free_variables.size() == 1);
    EXPECT_TRUE(((FunctionNode*) tree->nodes[1])->free_variables.count("x") == 1);
    EXPECT_NE(sinfo, nullptr);
    EXPECT_EQ(sinfo->identifier, "Foo");
}

TEST(second_pass_test, free_variable_test_3) {
    std::string text = "var x: Integer=0; fun foo(y: Foo)->Integer{if(y == 3){var z: Integer = 1 + x;}}";
    BlockNode* tree = get_ast(text);
    GlobalProcessor gp;
    gp.visit(*tree);
    Checker checker(gp.globals, gp.class_table);
    checker.visit(*tree);
    EXPECT_TRUE(checker.scopes["global"]->has("x"));
    SymbolTable* foo_scope = checker.scopes["global.foo"];
    auto sinfo = dynamic_cast<ObjectTypeNode*>(foo_scope->get("y"));
    EXPECT_TRUE(((FunctionNode*) tree->nodes[1])->free_variables.size() == 1);
    EXPECT_TRUE(((FunctionNode*) tree->nodes[1])->free_variables.count("x") == 1);
    EXPECT_NE(sinfo, nullptr);
    EXPECT_EQ(sinfo->identifier, "Foo");
}

TEST(second_pass_test, free_variable_test_4) {
    std::string text = "var x: Integer=0; fun foo(y: Foo)->Integer{if(y == 3){var z: Integer = 1 + y;}}";
    BlockNode* tree = get_ast(text);
    GlobalProcessor gp;
    gp.visit(*tree);
    Checker checker(gp.globals, gp.class_table);
    checker.visit(*tree);
    EXPECT_TRUE(checker.scopes["global"]->has("x"));
    SymbolTable* foo_scope = checker.scopes["global.foo"];
    auto sinfo = dynamic_cast<ObjectTypeNode*>(foo_scope->get("y"));
    EXPECT_NE(sinfo, nullptr);
    EXPECT_TRUE(((FunctionNode*) tree->nodes[1])->free_variables.size() == 0);
    EXPECT_EQ(sinfo->identifier, "Foo");
}

TEST(second_pass_test, tee) {
    std::string text = "fun foo(y: Foo)->String{if(y==1){return x;}}";
    ASSERT_THROWS_NOT_FOUND_ERROR(text, "x");
}

TEST(second_pass_test, list) {
    std::string text = "fun main()->Integer{var l=[]::List[Integer];return 0;}";
    ASSERT_OK(text);
}

TEST(second_pass_test, teea) {
    std::string text = "fun foo(y: Foo)->String{if(y==1){return x;}}";
    ASSERT_THROWS_NOT_FOUND_ERROR(text, "x");
}

TEST(second_pass_test, teeas) {
    std::string text = "fun foo(y: Foo)->String{if(z==1){return x;}}";
    ASSERT_THROWS_NOT_FOUND_ERROR(text, "z");
}


TEST(second_pass_test, FOFOO) {
    std::string text = "fun foo(y: Foo)->String{if(y==1){if(y==2){return x;}}}";
    ASSERT_THROWS_NOT_FOUND_ERROR(text, "x");
}

TEST(second_pass_test, FOFOOa) {
    std::string text = "fun foo(y: Foo)->String{if(y==1){if(z==2){return x;}}}";
    ASSERT_THROWS_NOT_FOUND_ERROR(text, "z");
}

TEST(second_pass_test, FOFOaOa) {
    std::string text = "fun foo(y: Foo)->String{var x:Integer=0;}";
    BlockNode* tree = get_ast(text);
    GlobalProcessor gp;
    gp.visit(*tree);
    Checker checker(gp.globals, gp.class_table);
    checker.visit(*tree);
    EXPECT_TRUE(checker.scopes["global.foo"]->declared("x"));
}

TEST(second_pass_test, option_type_value) {
    std::string text = "var x : Option[Integer] = 2;";
    BlockNode* tree = get_ast(text);
    GlobalProcessor gp;
    gp.visit(*tree);
    Checker checker(gp.globals, gp.class_table);
    checker.visit(*tree);
    EXPECT_TRUE(checker.scopes["global"]->declared("x"));
    EXPECT_TRUE(checker.scopes["global"]->get("x")->equal(new ObjectTypeNode("Option", {T_INT})));
}

TEST(second_pass_test, option_assign_none) {
    std::string text = "var x : Option[Integer] = none;";
    BlockNode* tree = get_ast(text);
    GlobalProcessor gp;
    gp.visit(*tree);
    Checker checker(gp.globals, gp.class_table);
    checker.visit(*tree);
    EXPECT_TRUE(checker.scopes["global"]->declared("x"));
    EXPECT_TRUE(checker.scopes["global"]->get("x")->equal(new ObjectTypeNode("Option", {T_INT})));
}

TEST(second_pass_test, assign_none_to_none_option) {
    std::string text = "var x : Integer = none;";
    BlockNode* tree = get_ast(text);
    GlobalProcessor gp;
    gp.visit(*tree);
    Checker checker(gp.globals, gp.class_table);
    checker.visit(*tree);
}

TEST(second_pass_test, assign_none_to_union) {
    std::string text = "var x : Union[Integer, String] = none;";
    BlockNode* tree = get_ast(text);
    GlobalProcessor gp;
    gp.visit(*tree);
    Checker checker(gp.globals, gp.class_table);
    checker.visit(*tree);
}

TEST(second_pass_test, assign_none_to_union_ok) {
    std::string text = "var x : Union[Integer, NoneType] = none;";
    BlockNode* tree = get_ast(text);
    GlobalProcessor gp;
    gp.visit(*tree);
    Checker checker(gp.globals, gp.class_table);
    checker.visit(*tree);
}


TEST(second_pass_test, union_type_ok_1) {
    std::string text = "var x : Union[Integer, String] = 2;";
    BlockNode* tree = get_ast(text);
    GlobalProcessor gp;
    gp.visit(*tree);
    Checker checker(gp.globals, gp.class_table);
    checker.visit(*tree);
    EXPECT_TRUE(checker.scopes["global"]->declared("x"));
    EXPECT_TRUE(checker.scopes["global"]->get("x")->equal(new ObjectTypeNode("Union", {T_INT, T_STRING})));
}

TEST(second_pass_test, union_type_ok_2) {
    std::string text = "var x : Union[Integer, String] = \"Hello\";";
    BlockNode* tree = get_ast(text);
    GlobalProcessor gp;
    gp.visit(*tree);
    Checker checker(gp.globals, gp.class_table);
    checker.visit(*tree);
    EXPECT_TRUE(checker.scopes["global"]->declared("x"));
    EXPECT_TRUE(checker.scopes["global"]->get("x")->equal(new ObjectTypeNode("Union", {T_INT, T_STRING})));
}

TEST(second_pass_test, union_type_error) {
    std::string text = "var x : Union[Integer, String] = false;";
    BlockNode* tree = get_ast(text);
    GlobalProcessor gp;
    gp.visit(*tree);
    Checker checker(gp.globals, gp.class_table);
    checker.visit(*tree);
    EXPECT_TRUE(checker.scopes["global"]->declared("x"));
    EXPECT_TRUE(checker.scopes["global"]->get("x")->equal(new ObjectTypeNode("Union", {T_INT, T_STRING})));
}

TEST(second_pass_test, option_type_error) {
    std::string text = "var x : Option[Integer] = \"Hello\";";
    BlockNode* tree = get_ast(text);
    GlobalProcessor gp;
    gp.visit(*tree);
    Checker checker(gp.globals, gp.class_table);
    checker.visit(*tree);
    EXPECT_TRUE(checker.scopes["global"]->declared("x"));
}

TEST(second_pass_test, ternary_test_1) {
    std::string text = "var x : Option[Integer] = 2;var z=x?\"ok\":\"bad\";";
    BlockNode* tree = get_ast(text);
    GlobalProcessor gp;
    gp.visit(*tree);
    Checker checker(gp.globals, gp.class_table);
    checker.visit(*tree);
    EXPECT_TRUE(checker.scopes["global"]->declared("x"));
    EXPECT_TRUE(checker.scopes["global"]->get("z")->equal(T_STRING));
}

TEST(second_pass_test, ternary_test_union_1) {
    std::string text = "var x : Option[Integer] = 2;var z=x?\"ok\":3;";
    BlockNode* tree = get_ast(text);
    GlobalProcessor gp;
    gp.visit(*tree);
    Checker checker(gp.globals, gp.class_table);
    checker.visit(*tree);
    EXPECT_TRUE(checker.scopes["global"]->declared("x"));
    EXPECT_TRUE(checker.scopes["global"]->get("z")->equal(new ObjectTypeNode("Union", {T_STRING, T_INT})));
}

TEST(second_pass_test, ternary_test_union_2) {
    std::string text = "var x : Option[Integer] = 2;var z=x?\"ok\":3;";
    BlockNode* tree = get_ast(text);
    GlobalProcessor gp;
    gp.visit(*tree);
    Checker checker(gp.globals, gp.class_table);
    checker.visit(*tree);
    EXPECT_TRUE(checker.scopes["global"]->declared("x"));
    EXPECT_TRUE(checker.scopes["global"]->get("z")->equal(new ObjectTypeNode("Union", {T_STRING, T_INT})));
}

TEST(second_pass_test, test_list) {
    std::string text = "var x = [1,\"Hello\"];";
    BlockNode* tree = get_ast(text);
    GlobalProcessor gp;
    gp.visit(*tree);
    Checker checker(gp.globals, gp.class_table);
    checker.visit(*tree);
    EXPECT_TRUE(checker.scopes["global"]->declared("x"));
    EXPECT_TRUE(checker.scopes["global"]->get("x")->equal(T_LIST(new ObjectTypeNode("Union", {T_STRING, T_INT}))))
                        << checker.scopes["global"]->get("x")->to_string();
}


TEST(second_pass_test, z_not_found_error) {
    std::string text = "fun foo(y: Foo)->String{var x:Integer=0;if(y==1){if(z==2){return x;}}}";
    ASSERT_THROWS_NOT_FOUND_ERROR(text, "z");
}

TEST(second_pass_test, x_redeclare_in_inner_scope_ok) {
    std::string text = "fun foo(y: Integer)->Integer{var x:Integer=0;if(y==1){if(y==2){var x:Integer=7; return x;}}}";
    ASSERT_OK(text);
}

TEST(second_pass_test, x_redeclare_in_same_scope_error) {
    std::string text = "fun foo(y: Foo)->String{var x:Integer=0;var x:String=\"\"; return x;}";
    ASSERT_THROWS_REDECLARED_ERROR(text, "x");
}

TEST(second_pass_test, x_declare_in_inner_scope_and_use_outside_error) {
    std::string text = "fun foo(y: Foo)->Integer{if(y==1){if(y==2){var x:Integer=0;return x;}}return x;}";
    ASSERT_THROWS_NOT_FOUND_ERROR(text, "x");
}

TEST(second_pass_test, function_return_type_error) {
    std::string text = "fun foo()->String{} fun main()->Integer{return foo();}";
    ASSERT_THROWS_RETURN_TYPE_ERROR(text, "main", T_STRING, T_INT);
}

TEST(second_pass_test, function_return_type_ok) {
    std::string text = "fun foo()->String{} fun main()->String{return foo();}";
    ASSERT_OK(text);
}

TEST(second_pass_test, function_argument_type_ok) {
    std::string text = "fun foo(x: Integer) -> Integer {return x;} fun bar()->Integer{var y: Integer=0; return foo(y);}";
    ASSERT_OK(text);
}

TEST(second_pass_test, call_builtin_print) {
    std::string text = "fun main()->Integer{print(str(7)); return 9;}";
    ASSERT_OK(text);
}

TEST(second_pass_test, call_builtin_print_type_error) {
    std::string text = "fun main()->Integer{print(\"Hello\"); return 9;}";
    ASSERT_OK(text);
}

TEST(second_pass_test, function_argument_type_error) {
    std::string text = "fun foo(x: Integer) -> Integer {return x;} fun bar()->Integer{var y: String=\"\"; return foo(y);}";
    ASSERT_THROWS_BAD_ARGUMENTS(text);
}

TEST(second_pass_test, declaration_type_error_1) {
    std::string text = "var x: String = 5;";
    ASSERT_THROWS_ASSIGNMENT_ERROR(text, T_STRING, T_INT);
}

TEST(second_pass_test, declaration_type_error_2) {
    std::string text = "var x: Integer = \"Hello\";";
    ASSERT_THROWS_ASSIGNMENT_ERROR(text, T_INT, T_STRING);

}

TEST(second_pass_test, assignment_type_error_1) {
    std::string text = "var x: String=\"\"; x = 5;";
    ASSERT_THROWS_ASSIGNMENT_ERROR(text, T_STRING, T_INT);
}

TEST(second_pass_test, assignment_type_error_2) {
    std::string text = "var x: Integer=0;x = \"Hello\";";
    ASSERT_THROWS_ASSIGNMENT_ERROR(text, T_INT, T_STRING);
}

TEST(second_pass_test, for_1) {
    std::string text = "var y = 0; for(x:[1,2,3,4]){y = x;}";
    ASSERT_OK(text);
}

TEST(second_pass_test, for_2) {
    std::string text = "var y = 0; for(x:[1,2,3,4]){y = z;}";
    ASSERT_THROWS_NOT_FOUND_ERROR(text, "z");
}

TEST(second_pass_test, for_3) {
    std::string text = "var y = 0; for(x:[1,2,3,4]){y = x;} y = x;";
    ASSERT_THROWS_NOT_FOUND_ERROR(text, "x");
}

TEST(second_pass_test, for_4) {
    std::string text = "var y = 0; for(x:[1,2,3,4]){y = x;var w = 0;} y = w;";
    ASSERT_THROWS_NOT_FOUND_ERROR(text, "w");
}

TEST(second_pass_test, while_1) {
    std::string text = "while(5){var x = 7;}";
    ASSERT_OK(text);
}

TEST(second_pass_test, while_2) {
    std::string text = "var y = 0; while(5==y){var x = 7;}";
    ASSERT_OK(text);
}

TEST(second_pass_test, class_literal_expression_ok) {
    std::string text = "struct Foo{name: String;} var f = Foo{\"Hello\"};";
    ASSERT_OK(text);
}

TEST(second_pass_test, class_literal_expression_error) {
    std::string text = "struct Foo{name: String;} var f = Foo{25};";
    ASSERT_OK(text);
}

TEST(second_pass_test, template_struct) {
    std::string text = "struct Tree[T]{value:T;left:Option[Tree[T]];right:Option[Tree[T]];}";
    ASSERT_OK(text);
}

TEST(second_pass_test, template_struct_type_not_found) {
    std::string text = "struct Tree[T]{value:W;left:Option[Tree[T]];right:Option[Tree[T]];}";
    ASSERT_OK(text);
}

TEST(second_pass_test, template_struct_1) {
    std::string text = "struct Tree[T]{value:T;left:Option[Tree[Integer]];right:Option[Tree[T]];}";
    ASSERT_OK(text);
}

TEST(second_pass_test, template_struct_wrong_number_of_args) {
    std::string text = "struct Tree[T]{value:T;left:Option[Tree[Integer, T]];right:Option[Tree[T]];}";
    ASSERT_OK(text);
}

TEST(second_pass_test, template_function_very_simple){
    std::string text = "fun id(x:t)->t{return x;} fun main()->Integer{var x = id(7);return 0;}";
    ASSERT_OK(text);
}

TEST(second_pass_test, template_function_return_second_same_type){
    std::string text = "fun second(x:t,y:t)->t{return y;} fun main()->Integer{var x = second(7,9);return 0;}";
    ASSERT_OK(text);
}

TEST(second_pass_test, template_function_return_second_same_type_error){
    std::string text = "fun second(x:t,y:t)->t{return y;} fun main()->Integer{var x = second(7,\"Hello\");return 0;}";
    ASSERT_OK(text);
}

TEST(second_pass_test, template_function_return_second_diff_generic){
    std::string text = "fun second(x:t,y:u)->u{return y;} fun main()->Integer{var x = second(7,\"Hello\");return x;}";
    ASSERT_OK(text);
}

TEST(second_pass_test, function_overload){
    std::string text = "fun foo(x:String)->String{return x;} fun foo(x:Integer)->Integer{return x;} fun main()->Integer{var x: Integer = foo(7); var y:String=foo(\"Hello\");return 0;}";
    ASSERT_OK(text);
}

TEST(second_pass_test, template_function_return_type_ok){
    std::string text = "fun second(x:t)->t{return x;} fun main()->Integer{return 0;}";
    ASSERT_OK(text);
}

TEST(second_pass_test, template_function_bad_return_type_2){
    std::string text = "fun second(x:t)->Integer{return x;} fun main()->Integer{return 0;}";
    ASSERT_OK(text);
}

TEST(second_pass_test, template_function_bad_return_type_3){
    std::string text = "fun second(x:t)->t{return 2;} fun main()->Integer{return 0;}";
    ASSERT_OK(text);
}


TEST(second_pass_test, template_struct_instantiate) {
    std::string text = "struct Tree[T]{value:T;} fun main()->Integer{var x = Tree[Integer]{value:7}; return 7;}";
    ASSERT_OK(text);
}

TEST(second_pass_test, template_struct_instantiate_more_than_one) {
    std::string text = "struct Tree[T,U]{t:T;u:U;} fun main()->Integer{var x = Tree[Integer,String]{t:5,u:\"Hello\"}; return 5;}";
    ASSERT_OK(text);
}

TEST(second_pass_test, template_struct_instantiate_more_than_one_2) {
    std::string text = "struct Tree[T,U]{t:T;u:U;} fun main()->Integer{var x = Tree[Integer,String]{t:5,u:\"Hello\"}; return 5;}";
    ASSERT_OK(text);
}

TEST(second_pass_test, template_struct_instantiate_more_than_4) {
    std::string text = "struct Tree[T,U]{t:T;u:U;v:T;} fun main()->Integer{var x = Tree[Integer,String]{t:5,u:\"Hello\",v:7}; return 5;}";
    ASSERT_OK(text);
}

TEST(second_pass_test, template_struct_instantiate_more_than_5) {
    std::string text = "struct Tree[T,U]{t:T;u:U;v:T;} fun main()->Integer{var x = Tree[Integer,String]{t:5,u:\"Hello\",v:\"World\"}; return 5;}";
    ASSERT_OK(text);
}

TEST(second_pass_test, template_struct_tree) {
    std::string text = "struct Tree[T]{v:T;l:Option[Tree[T]];r:Option[Tree[T]];} fun main()->Integer{var x = Tree[Integer]{v:5,l:Tree[Integer]{v:9,l:none,r:none},r:Tree[String]{v:\"\",l:none,r:none}}; return 5;}";
    ASSERT_OK(text);
}

TEST(second_pass_test, infer_boolean_false) {
    std::string text = "var x = false;";
    ASSERT_VARIABLE_TYPE(text, "x", T_BOOL);
}

TEST(second_pass_test, infer_boolean_true) {
    std::string text = "var x = true;";
    ASSERT_VARIABLE_TYPE(text, "x", T_BOOL);
}