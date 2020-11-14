#include <gtest/gtest.h>
#include <parser/Parser.h>
#include <scanner/Scanner.h>
#include <semantic/GlobalProcessor.h>
#include <semantic/Checker.h>

BlockNode* get_ast(std::string text) {
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    BlockNode* tree = parser.parse_program();
    return tree;
}

void compile(std::string text) {
    BlockNode* tree = get_ast(text);
    std::vector<std::pair<std::string, CodeBuiltin>> builtins;
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

void ASSERT_FAILS(std::string text) {
    try {
        compile(text);
        FAIL() << "Expected exception!";
    } catch (...) {}
}


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
    SymbolTable* foo_scope = checker.scopes["global.foo.0"];
    ObjectTypeNode* sinfo = dynamic_cast<ObjectTypeNode*>(foo_scope->get("y"));
    EXPECT_NE(sinfo, nullptr);
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
    SymbolTable* foo_scope = checker.scopes["global.foo.0"];
    auto sinfo = dynamic_cast<ObjectTypeNode*>(foo_scope->get("y"));
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
    SymbolTable* foo_scope = checker.scopes["global.foo.0"];
    auto sinfo = dynamic_cast<ObjectTypeNode*>(foo_scope->get("y"));
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
    SymbolTable* foo_scope = checker.scopes["global.foo.0"];
    auto sinfo = dynamic_cast<ObjectTypeNode*>(foo_scope->get("y"));
    EXPECT_NE(sinfo, nullptr);
    EXPECT_EQ(sinfo->identifier, "Foo");
}

TEST(second_pass_test, free_variable_test_4) {
    std::string text = "var x: Integer=0; fun foo(y: Foo)->Integer{if(y == 3){var z: Integer = 1 + y;}}";
    ASSERT_FAILS(text);
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
    EXPECT_TRUE(checker.scopes["global.foo.0"]->declared("x"));
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
    ASSERT_THROWS_ASSIGNMENT_ERROR(text, T_INT, TYPE("NoneType", {}));
}

TEST(second_pass_test, assign_none_to_union) {
    std::string text = "var x : Union[Integer, String] = none;";
    std::vector<NodeContainer> o = {T_INT, T_STRING};
    auto ut = TYPE("Union", o);
    ASSERT_THROWS_ASSIGNMENT_ERROR(text, ut, TYPE("NoneType", {}));
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
    ASSERT_FAILS(text);
}

TEST(second_pass_test, option_type_error) {
    std::string text = "var x : Option[Integer] = \"Hello\";";
    ASSERT_FAILS(text);
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
    ASSERT_FAILS(text);
}

TEST(second_pass_test, ternary_test_union_2) {
    std::string text = "var x : Option[Integer] = 2;var z=x?\"ok\":3;";
    ASSERT_FAILS(text);
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
    std::string text = "fun foo()->String{return \"Hello\";} fun main()->Integer{return foo();}";
    ASSERT_THROWS_RETURN_TYPE_ERROR(text, "main", T_INT, T_STRING);
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

TEST(second_pass_test, function_generic_argument_type_error) {
    std::string text = "fun first(f:a,s:a)->a{return f;} fun foo()->Integer{var r = first(\"Hello\",7); return 0;}";
    ASSERT_THROWS_BAD_ARGUMENTS(text);
}

TEST(second_pass_test, function_generic_argument_ok) {
    std::string text = "struct Pair[a,b]{first:a;second:b;}fun first(p: Pair[a,b])->a{return p.first;} fun foo()->String{var r = first(#Pair[String,Integer]{first:\"Hello\",second:7}); return r;}";
}

TEST(second_pass_test, function_generic_argument_ok_2) {
    std::string text = "struct Pair[a,b]{first:a;second:b;}fun firsts(p1: Pair[a,b],p2:Pair[a,c])->Pair[a,a]{return #Pair[a,a]{first:p1.first,second:p2.first};} fun foo()->Pair[String,String]{var r = firsts(#Pair[String,Integer]{first:\"Hello\",second:7},#Pair[String, Boolean]{first:\"Hello\",second:true}); return r;}";
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
    ASSERT_FAILS(text);
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
    ASSERT_FAILS(text);
}

TEST(second_pass_test, template_struct_1) {
    std::string text = "struct Tree[T]{value:T;left:Option[Tree[Integer]];right:Option[Tree[T]];}";
    ASSERT_OK(text);
}

TEST(second_pass_test, template_struct_wrong_number_of_args) {
    std::string text = "struct Tree[T]{value:T;left:Option[Tree[Integer, T]];right:Option[Tree[T]];}";
    ASSERT_FAILS(text);
}

TEST(second_pass_test, template_function_very_simple) {
    std::string text = "fun id(x:t)->t{return x;} fun main()->Integer{var x = id(7);return 0;}";
    ASSERT_OK(text);
}

TEST(second_pass_test, template_function_return_second_same_type) {
    std::string text = "fun second(x:t,y:t)->t{return y;} fun main()->Integer{var x = second(7,9);return 0;}";
    ASSERT_OK(text);
}

TEST(second_pass_test, template_function_return_second_same_type_error) {
    std::string text = "fun second(x:t,y:t)->t{return y;} fun main()->Integer{var x = second(7,\"Hello\");return 0;}";
    ASSERT_FAILS(text);
}

TEST(second_pass_test, template_function_return_second_diff_generic) {
    std::string text = "fun second(x:t,y:u)->u{return y;} fun main()->Integer{var x = second(7,\"Hello\");return x;}";
    ASSERT_FAILS(text);
}

TEST(second_pass_test, function_overload) {
    std::string text = "fun foo(x:String)->String{return x;} fun foo(x:Integer)->Integer{return x;} fun main()->Integer{var x: Integer = foo(\"Hello\"); var y:String=foo(\"Hello\");return 0;}";
    ASSERT_OK(text);
}

TEST(second_pass_test, template_function_return_type_ok) {
    std::string text = "fun second(x:t)->t{return x;} fun main()->Integer{return 0;}";
    ASSERT_OK(text);
}

TEST(second_pass_test, template_function_bad_return_type_2) {
    std::string text = "fun second(x:t)->Integer{return x;} fun main()->Integer{return 0;}";
    ASSERT_FAILS(text);
}

TEST(second_pass_test, template_function_bad_return_type_3) {
    std::string text = "fun second(x:t)->t{return 2;} fun main()->Integer{return 0;}";
    ASSERT_FAILS(text);
}


TEST(second_pass_test, template_struct_instantiate) {
    std::string text = "struct Tree[T]{value:T;} fun main()->Integer{var x = #Tree[Integer]{value:7}; return 7;}";
    ASSERT_OK(text);
}

TEST(second_pass_test, template_struct_instantiate_more_than_one) {
    std::string text = "struct Tree[T,U]{t:T;u:U;} fun main()->Integer{var x = #Tree[Integer,String]{t:5,u:\"Hello\"}; return 5;}";
    ASSERT_OK(text);
}

TEST(second_pass_test, template_struct_instantiate_more_than_one_2) {
    std::string text = "struct Tree[T,U]{t:T;u:U;} fun main()->Integer{var x = #Tree[Integer,String]{t:5,u:\"Hello\"}; return 5;}";
    ASSERT_OK(text);
}

TEST(second_pass_test, template_struct_instantiate_more_than_4) {
    std::string text = "struct Tree[T,U]{t:T;u:U;v:T;} fun main()->Integer{var x = #Tree[Integer,String]{t:5,u:\"Hello\",v:7}; return 5;}";
    ASSERT_OK(text);
}

TEST(second_pass_test, template_struct_instantiate_more_than_5) {
    std::string text = "struct Tree[T,U]{t:T;u:U;v:T;} fun main()->Integer{var x = #Tree[Integer,String]{t:5,u:\"Hello\",v:\"World\"}; return 5;}";
    ASSERT_FAILS(text);
}

TEST(second_pass_test, template_struct_tree) {
    std::string text = "struct Tree[T]{v:T;l:Option[Tree[T]];r:Option[Tree[T]];} fun main()->Integer{var x = #Tree[Integer]{v:5,l:#Tree[Integer]{v:9,l:none,r:none},r:#Tree[Integer]{v:7,l:none,r:none}}; return 5;}";
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

TEST(second_pass_test, call_overloaded_function) {
    std::string text = "fun foo(x:String)->String{"
                       "return x;"
                       "}"
                       "fun foo(x:Integer)->Integer{"
                       "return x;"
                       "}"
                       "fun main()->Integer{"
                       "var x = foo(7);"
                       "var y = foo(\"Hello\");"
                       "return 0;"
                       "}";
    BlockNode* tree = get_ast(text);
    std::vector<std::pair<std::string, CodeBuiltin>> builtins;
    GlobalProcessor gp(builtins);
    gp.visit(*tree);
    Checker checker(gp.globals, gp.class_table);
    checker.function_table = gp.function_table;
    checker.visit(*tree);
    FunctionNode* main_fun = dynamic_cast<FunctionNode*>(tree->nodes[2]);
    EXPECT_TRUE(main_fun->body->nodes[0]->equal(DECL("x", nullptr, CALL(ID("foo.1"), {NUM(7)}))));
    EXPECT_TRUE(main_fun->body->nodes[1]->equal(DECL("y", nullptr, CALL(ID("foo.0"), {STR("Hello")}))));
}

TEST(second_pass_test, pass_overloaded_function_no_generic) {
    std::string text = "fun foo(x:String)->String{"
                       "return \"A string\";"
                       "} "
                       "fun foo(x:Integer)->String{"
                       "return \"A number\";"
                       "} "
                       "fun call(x: Integer, f: fun(Integer)->String)->String{"
                       "return f(x);"
                       "} "
                       "fun main()->Integer{"
                       "var x = call(7, foo); "
                       "return 0;"
                       "}";
    BlockNode* tree = get_ast(text);
    std::vector<std::pair<std::string, CodeBuiltin>> builtins;
    GlobalProcessor gp(builtins);
    gp.visit(*tree);
    Checker checker(gp.globals, gp.class_table);
    checker.function_table = gp.function_table;
    checker.visit(*tree);
    FunctionNode* main_fun = dynamic_cast<FunctionNode*>(tree->nodes[3]);
    auto c0 = CALL(ID("call.0"), VectorOfNodes({NUM(7), ID("foo.1")}));
    EXPECT_TRUE(main_fun->body->nodes[0]->equal(DECL("x", nullptr, c0))) << *main_fun->body->nodes[0];
}

TEST(second_pass_test, pass_simple_function_generic) {
    std::string text = "fun foo(x:String)->String{"
                       "return \"A string\";"
                       "} "
                       "fun call(x: a, f: fun(a)->String)->String{"
                       "return f(x);"
                       "} "
                       "fun main()->Integer{"
                       "var y = call(\"Hello\", foo);"
                       "return 0;"
                       "}";
    BlockNode* tree = get_ast(text);
    std::vector<std::pair<std::string, CodeBuiltin>> builtins;
    GlobalProcessor gp(builtins);
    gp.visit(*tree);
    Checker checker(gp.globals, gp.class_table);
    checker.function_table = gp.function_table;
    checker.visit(*tree);
    FunctionNode* main_fun = dynamic_cast<FunctionNode*>(tree->nodes[2]);
    auto c1 = CALL(ID("call.0"), VectorOfNodes({STR("Hello"), ID("foo.0")}));
    EXPECT_TRUE(main_fun->body->nodes[0]->equal(DECL("y", nullptr, c1))) << *main_fun->body->nodes[0];
}

TEST(second_pass_test, pass_simple_function_return_generic) {
    std::string text = "fun foo(x:String)->String{"
                       "return \"A string\";"
                       "} "
                       "fun call(x: a, f: fun(a)->b)->b{"
                       "return f(x);"
                       "} "
                       "fun main()->Integer{"
                       "var y = call(\"Hello\", foo);"
                       "return 0;"
                       "}";
    BlockNode* tree = get_ast(text);
    std::vector<std::pair<std::string, CodeBuiltin>> builtins;
    GlobalProcessor gp(builtins);
    gp.visit(*tree);
    Checker checker(gp.globals, gp.class_table);
    checker.function_table = gp.function_table;
    checker.visit(*tree);
    FunctionNode* main_fun = dynamic_cast<FunctionNode*>(tree->nodes[2]);
    auto c1 = CALL(ID("call.0"), VectorOfNodes({STR("Hello"), ID("foo.0")}));
    EXPECT_TRUE(main_fun->body->nodes[0]->equal(DECL("y", nullptr, c1))) << *main_fun->body->nodes[0];
}

TEST(second_pass_test, pass_overloaded_function_generic) {
    std::string text = "fun foo(x:String)->String{"
                       "return \"A string\";"
                       "} "
                       "fun foo(x:Integer)->String{"
                       "return \"A number\";"
                       "} "
                       "fun call(x: a, f: fun(a)->String)->String{"
                       "return f(x);"
                       "} "
                       "fun main()->Integer{"
                       "var x = call(7, foo);"
                       "var y = call(\"Hello\", foo);"
                       "return 0;"
                       "}";
    BlockNode* tree = get_ast(text);
    std::vector<std::pair<std::string, CodeBuiltin>> builtins;
    GlobalProcessor gp(builtins);
    gp.visit(*tree);
    Checker checker(gp.globals, gp.class_table);
    checker.function_table = gp.function_table;
    checker.visit(*tree);
    FunctionNode* main_fun = dynamic_cast<FunctionNode*>(tree->nodes[3]);
    auto c0 = CALL(ID("call.0"), VectorOfNodes({NUM(7), ID("foo.1")}));
    EXPECT_TRUE(main_fun->body->nodes[0]->equal(DECL("x", nullptr, c0))) << *main_fun->body->nodes[0];
    auto c1 = CALL(ID("call.0"), VectorOfNodes({STR("Hello"), ID("foo.0")}));
    EXPECT_TRUE(main_fun->body->nodes[1]->equal(DECL("y", nullptr, c1))) << *main_fun->body->nodes[1];
}

TEST(second_pass_test, pass_overloaded_function_generic_error) {
    std::string text = "fun foo(x:String)->String{"
                       "return \"A string\";"
                       "} "
                       "fun foo(x:Integer)->String{"
                       "return \"A number\";"
                       "} "
                       "fun call(x: a, f: fun(a)->String)->String{"
                       "return f(x);"
                       "} "
                       "fun main()->Integer{"
                       "var x = call(7, foo);"
                       "var y = call(\"Hello\", foo);"
                       "var z = call(false, foo);"
                       "return 0;"
                       "}";
    BlockNode* tree = get_ast(text);
    std::vector<std::pair<std::string, CodeBuiltin>> builtins;
    GlobalProcessor gp(builtins);
    gp.visit(*tree);
    Checker checker(gp.globals, gp.class_table);
    checker.function_table = gp.function_table;
    checker.visit(*tree);
    FunctionNode* main_fun = dynamic_cast<FunctionNode*>(tree->nodes[3]);
    auto c0 = CALL(ID("call.0"), VectorOfNodes({NUM(7), ID("foo.1")}));
    EXPECT_TRUE(main_fun->body->nodes[0]->equal(DECL("x", nullptr, c0))) << *main_fun->body->nodes[0];
    auto c1 = CALL(ID("call.0"), VectorOfNodes({STR("Hello"), ID("foo.0")}));
    EXPECT_TRUE(main_fun->body->nodes[1]->equal(DECL("y", nullptr, c1))) << *main_fun->body->nodes[1];
}

TEST(second_pass_test, generic_map) {
    std::string text = "fun map(l: List[a], n:Integer, f: fun(a)->b)->List[b]{"
                       "    var i = 0;"
                       "    var r = []::List[b];"
                       "    while(i<n){"
                       "        r = r + [f(l[i])];"
                       "        i = i+1;"
                       "    }"
                       "    return r;"
                       "}"
                       "fun double(i: Integer)->Integer{"
                       "    return 2*i;"
                       "}"
                       "fun main()->Integer{"
                       "    var l = map([1,2,3,4,5], 5, double);"
                       "    return 0;"
                       "}";
    BlockNode* tree = get_ast(text);
    std::vector<std::pair<std::string, CodeBuiltin>> builtins;
    GlobalProcessor gp(builtins);
    gp.visit(*tree);
    Checker checker(gp.globals, gp.class_table);
    checker.function_table = gp.function_table;
    checker.visit(*tree);
    FunctionNode* main_fun = dynamic_cast<FunctionNode*>(tree->nodes[2]);
    auto ls = new ListNode({NUM(1), NUM(2), NUM(3), NUM(4), NUM(5)});
    auto c0 = CALL(ID("map.0"), VectorOfNodes({ls, NUM(5), ID("double.0")}));
    EXPECT_TRUE(main_fun->body->nodes[0]->equal(DECL("l", nullptr, c0))) << *main_fun->body->nodes[0];
}

TEST(second_pass_test, generic_pass_function_error) {
    std::string text = "fun foo(x:a,f:fun(a)->a)->a{"
                       "return f(x);"
                       "}"
                       "fun bar(i: Integer)->String{"
                       "return \"Hello\";"
                       "}"
                       "fun main()->Integer{"
                       "var w = foo(5,bar);"
                       "return 0;"
                       "}";
    BlockNode* tree = get_ast(text);
    std::vector<std::pair<std::string, CodeBuiltin>> builtins;
    GlobalProcessor gp(builtins);
    gp.visit(*tree);
    Checker checker(gp.globals, gp.class_table);
    checker.function_table = gp.function_table;
    try {
        checker.visit(*tree);
        FAIL() << "Expected an error!";
    } catch (...) {

    }
}