#include <gtest/gtest.h>
#include <parser/Parser.h>
#include <scanner/Scanner.h>
#include <semantic/GlobalProcessor.h>
#include <semantic/Checker.h>
#include <gtest/gtest.h>
#include <scanner/Scanner.h>
#include <parser/Parser.h>
#include <parser/UnexpectedToken.h>
#include <macros.h>
#include <semantic/unify.h>


TEST(generic_test, test_11) {
    auto a = TYPE("a", {});
    EXPECT_TRUE(is_generic(*a));
}

TEST(generic_test, test_12) {
    EXPECT_FALSE(is_generic(T_INT));
}

TEST(generic_test, test_13) {
    auto t = T_LIST(TYPE("a", {}));
    EXPECT_TRUE(is_generic(t));
}

TEST(generic_test, test_14) {
    auto t = T_LIST(new T_INT);
    EXPECT_FALSE(is_generic(t));
}

TEST(generic_test, test_15) {
    auto t = T_LIST(new T_LIST(TYPE("a", {})));
    EXPECT_TRUE(is_generic(t));
}

TEST(generic_test, test_16) {
    auto t = T_LIST(new T_LIST(new T_INT));
    EXPECT_FALSE(is_generic(t));
}

TEST(generic_test, test_1) {
    auto a = TYPE("a", {});
    auto b = TYPE("Integer", {});
    EXPECT_TRUE(type_matches(a, b));
}


TEST(generic_test, test_2) {
    auto a = TYPE("String", {});
    auto b = TYPE("Integer", {});
    EXPECT_FALSE(type_matches(a, b));
}

TEST(generic_test, test_3) {
    auto a = TYPE("List", { new T_INT });
    auto b = TYPE("Integer", {});
    EXPECT_FALSE(type_matches(a, b));
}

TEST(generic_test, test_4) {
    auto a = TYPE("List", { new T_INT });
    auto b = TYPE("Dict", { new T_INT });
    EXPECT_FALSE(type_matches(a, b));
}

TEST(generic_test, test_5) {
    auto a = TYPE("List", { new T_INT });
    auto b = TYPE("List", { new T_STRING });
    EXPECT_FALSE(type_matches(a, b));
}

TEST(generic_test, test_6) {
    auto a = TYPE("List", { TYPE("a", {}) });
    auto b = TYPE("List", { new T_STRING });
    EXPECT_TRUE(type_matches(a, b));
}

TEST(generic_test, test_7) {
    auto a = TYPE("List", { TYPE("a", {}) });
    VectorOfTypes t = {new T_STRING, new T_INT};
    auto b = TYPE("Dict", t);
    EXPECT_FALSE(type_matches(a, b));
}

TEST(generic_test, test_8) {
    auto a = new T_LIST(new T_LIST(TYPE("a", {})));
    auto b = new T_LIST(new T_LIST(TYPE("Integer", {})));
    EXPECT_TRUE(type_matches(a, b));
}

TEST(generic_test, test_9) {
    auto a = T_DICT(new T_LIST(TYPE("a", {})), TYPE("b", {}));
    auto b = T_DICT(new T_LIST(TYPE("Integer", {})), new T_STRING);
    EXPECT_TRUE(type_matches(a, b));
}


TEST(generic_test, test_20) {
    VectorOfTypes pt = {};
    TypeNode* rt = TYPE("NoneType", {});
    auto a = FUNCTION_TYPE(pt, rt);
    EXPECT_TRUE(type_matches(a, a));
}

TEST(generic_test, test_21) {
    VectorOfTypes pt = {};
    TypeNode* rt = TYPE("NoneType", {});
    auto a = FUNCTION_TYPE(pt, rt);
    auto b = FUNCTION_TYPE({ new T_INT }, rt);
    EXPECT_FALSE(type_matches(a, b));
}


TEST(generic_test, test_22) {
    VectorOfTypes pt = {TYPE("a", {})};
    TypeNode* rt = TYPE("NoneType", {});
    auto a = FUNCTION_TYPE(pt, rt);
    EXPECT_TRUE(is_generic(*a));
}

TEST(generic_test, test_23) {
    VectorOfTypes pt = {TYPE("Integer", {})};
    TypeNode* rt = TYPE("NoneType", {});
    auto a = FUNCTION_TYPE(pt, rt);
    EXPECT_FALSE(is_generic(*a));
}

TEST(generic_test, test_24) {
    VectorOfTypes pt = {TYPE("a", {}), TYPE("a", {}), TYPE("b", {})};
    auto a = FUNCTION_TYPE(pt, new T_INT);
    VectorOfTypes ptb = {new T_INT, new T_INT, new T_STRING};
    auto b = FUNCTION_TYPE(ptb, new T_INT);
    EXPECT_TRUE(type_matches(a, b));
}

TEST(generic_test, test_25) {
    VectorOfTypes pt = {TYPE("a", {}), TYPE("a", {}), TYPE("b", {})};
    auto a = FUNCTION_TYPE(pt, new T_INT);
    VectorOfTypes ptb = {new T_INT, new T_STRING, new T_STRING};
    auto b = FUNCTION_TYPE(ptb, new T_INT);
    EXPECT_FALSE(type_matches(a, b));
}


// TEST(generic_test, test_26) {
//     auto a = TYPE("a", {});
//     VectorOfTypes ptb = {new T_INT, new T_INT, new T_STRING};
//     auto b = FUNCTION_TYPE(ptb, new T_INT);
//     EXPECT_TRUE(type_matches(a, b));
// }

TEST(generic_test, test_27) {
    VectorOfTypes pta = {TYPE("a", {})};
    auto a = FUNCTION_TYPE(pta, TYPE("a", {}));
    VectorOfTypes ptb = {new T_INT};
    auto b = FUNCTION_TYPE(ptb, new T_STRING);
    EXPECT_FALSE(type_matches(a, b));
}


TEST(generic_test, test_28) {
    VectorOfTypes pt = {TYPE("Integer", {})};
    TypeNode* rt = TYPE("a", {});
    auto a = FUNCTION_TYPE(pt, rt);
    EXPECT_TRUE(is_generic(*a));
}

TEST(generic_test, make_replacement_1) {
    auto a = TYPE("a", {});
    auto b = TYPE("Integer", {});
    auto repl = make_replacements(a, b);
    EXPECT_TRUE(repl.count("a"));
    EXPECT_TRUE(repl["a"]->equal(*b));
}

TEST(generic_test, make_replacement_2) {
    auto a = new T_LIST(new T_LIST(TYPE("a", {})));
    auto b = new T_LIST(new T_LIST(new T_INT));
    auto repl = make_replacements(a, b);
    EXPECT_TRUE(repl.count("a"));
    EXPECT_TRUE(repl["a"]->equal(T_INT));
}

TEST(generic_test, make_replacement_3) {
    auto a = T_DICT(TYPE("a", {}), TYPE("b", {}));
    auto b = T_DICT(new T_INT, new T_STRING);
    auto repl = make_replacements(a, b);
    EXPECT_TRUE(repl.count("a"));
    EXPECT_TRUE(repl["a"]->equal(T_INT));
    EXPECT_TRUE(repl.count("b"));
    EXPECT_TRUE(repl["b"]->equal(T_STRING));
}

TEST(generic_test, make_replacement_4) {
    auto a = T_DICT(TYPE("a", {}), new T_LIST(TYPE("b", {})));
    auto b = T_DICT(new T_INT, new T_LIST(new T_STRING));
    auto repl = make_replacements(a, b);
    EXPECT_TRUE(repl.count("a"));
    EXPECT_TRUE(repl["a"]->equal(T_INT));
    EXPECT_TRUE(repl.count("b"));
    EXPECT_TRUE(repl["b"]->equal(T_STRING));
}

TEST(generic_test, function_call_1) {
    FunctionTypeNode* ftn = FUNCTION_TYPE({ TYPE("a", {}) }, TYPE("a", {}));
    VectorOfTypes ats = {new T_INT};
    SymbolInfo retv = match_arguments_to_generic_function(*ftn, ats);
    EXPECT_EQ(retv.type(), T_INT);
}

TEST(generic_test, function_call_2) {
    VectorOfTypes pt = {TYPE("a", {}), TYPE("b", {})};
    FunctionTypeNode* ftn = FUNCTION_TYPE(pt, TYPE("a", {}));
    VectorOfTypes ats = {new T_INT, new T_STRING};
    SymbolInfo retv = match_arguments_to_generic_function(*ftn, ats);
    EXPECT_EQ(retv.type(), T_INT);
}


TEST(generic_test, function_call_3) {
    VectorOfTypes pt = {TYPE("a", {}), TYPE("b", {})};
    FunctionTypeNode* ftn = FUNCTION_TYPE(pt, TYPE("b", {}));
    VectorOfTypes ats = {new T_INT, new T_STRING};
    SymbolInfo retv = match_arguments_to_generic_function(*ftn, ats);
    EXPECT_EQ(retv.type(), T_STRING);
}

TEST(generic_test, function_call_4) {

    VectorOfTypes pt = {TYPE("a", {}), FUNCTION_TYPE(VectorOfTypes({TYPE("a", {})}), TYPE("b", {}))};
    // fun(a,fun(a)->b) -> b
    FunctionTypeNode* ftn = FUNCTION_TYPE(pt, TYPE("b", {}));

    // Integer, fun(Integer)->String
    VectorOfTypes ats = {new T_INT, FUNCTION_TYPE({ new T_INT }, new T_STRING)};

    SymbolInfo retv = match_arguments_to_generic_function(*ftn, ats);

    EXPECT_EQ(retv.type(), T_STRING);
}

TEST(generic_test, function_call_5) {

    VectorOfTypes pt = {FUNCTION_TYPE(VectorOfTypes({TYPE("a", {})}), TYPE("b", {})), TYPE("a", {})};
    FunctionTypeNode* ftn = FUNCTION_TYPE(pt, TYPE("b", {}));

    VectorOfTypes ats = {FUNCTION_TYPE({ new T_INT }, new T_STRING), new T_INT};

    SymbolInfo retv = match_arguments_to_generic_function(*ftn, ats);

    EXPECT_EQ(retv.type(), T_STRING);
}

TEST(generic_test, function_call_6) {

    VectorOfTypes pt = {FUNCTION_TYPE(VectorOfTypes({TYPE("a", {})}), TYPE("b", {})), TYPE("a", {})};
    FunctionTypeNode* ftn = FUNCTION_TYPE(pt, TYPE("b", {}));

    VectorOfTypes ats = {FUNCTION_TYPE({ TYPE("t", {}) }, new T_STRING), new T_INT};

    SymbolInfo retv = match_arguments_to_generic_function(*ftn, ats);

    EXPECT_EQ(retv.type(), T_STRING);
}

TEST(generic_test, function_call_7) {

    VectorOfTypes pt = {FUNCTION_TYPE(VectorOfTypes({TYPE("a", {})}), new T_STRING), TYPE("a", {})};
    FunctionTypeNode* ftn = FUNCTION_TYPE(pt, new T_STRING);

    VectorOfTypes ats = {FUNCTION_TYPE({ TYPE("t", {}) }, new T_STRING), new T_INT};

    SymbolInfo retv = match_arguments_to_generic_function(*ftn, ats);

    EXPECT_EQ(retv.type(), T_STRING);
}

TEST(generic_test, function_call_8) {

    VectorOfTypes pt = {FUNCTION_TYPE(VectorOfTypes({TYPE("a", {})}), new T_STRING), TYPE("a", {})};
    // fun(fun(a)->String, a) -> String
    // fun(fun(t)->String, t) -> String
    FunctionTypeNode* ftn = FUNCTION_TYPE(pt, new T_STRING);
    // fun(t)->t, Integer
    VectorOfTypes ats = {FUNCTION_TYPE({ TYPE("t", {}) }, TYPE("t", {})), new T_INT};
    // fun(a)->String // fun(a)->String           --> a = String --> fun(String)->String
    // fun(t)->t      // fun(t)->t --> t = String --> fun(String)->String
    try {
        SymbolInfo retv = match_arguments_to_generic_function(*ftn, ats);
        FAIL();
    } catch (...) {

    }
}

TEST(generic_test, function_call_9) {

    VectorOfTypes pt = {TYPE("a", {}), FUNCTION_TYPE(VectorOfTypes({TYPE("a", {})}), new T_STRING)};
    FunctionTypeNode* ftn = FUNCTION_TYPE(pt, new T_STRING);

    VectorOfTypes ats = {new T_STRING, FUNCTION_TYPE({ TYPE("t", {}) }, TYPE("t", {}))};

    SymbolInfo retv = match_arguments_to_generic_function(*ftn, ats);
    EXPECT_EQ(retv.type(), T_STRING);
}

TEST(generic_test, function_call_10) {

    VectorOfTypes pt = {FUNCTION_TYPE(VectorOfTypes({TYPE("a", {})}), new T_STRING), TYPE("a", {})};
    FunctionTypeNode* ftn = FUNCTION_TYPE(pt, new T_STRING);

    VectorOfTypes ats = {FUNCTION_TYPE({ TYPE("t", {}) }, TYPE("t", {})), new T_STRING};

    SymbolInfo retv = match_arguments_to_generic_function(*ftn, ats);
    EXPECT_EQ(retv.type(), T_STRING);
}

TEST(generic_test, function_call_11) {

    VectorOfTypes pt = {new T_STRING};
    // fun(String)->String
    // t
    FunctionTypeNode* ftn = FUNCTION_TYPE(pt, new T_STRING);

    VectorOfTypes ats = {TYPE("t", {})};
    try {
        SymbolInfo retv = match_arguments_to_generic_function(*ftn, ats);
        FAIL();
    } catch (...) {
    }
}

TEST(generic_test, function_call_12) {

    VectorOfTypes pt = {FUNCTION_TYPE(VectorOfTypes({TYPE("a", {})}), TYPE("b", {})), TYPE("a", {})};
    // fun(fun(a)->b, a) -> b
    FunctionTypeNode* ftn = FUNCTION_TYPE(pt, TYPE("b", {}));

    VectorOfTypes ats = {FUNCTION_TYPE({ TYPE("t", {}) }, TYPE("t", {})), new T_STRING};

    SymbolInfo retv = match_arguments_to_generic_function(*ftn, ats);
    EXPECT_EQ(retv.type(), T_STRING);
}

TEST(generic_test, unify_function_call_1) {
    auto fun = FUNCTION_TYPE(VectorOfTypes({TYPE("t", {})}), TYPE("t", {}));
    auto args = VectorOfTypes({TYPE("Integer", {})});
    unify_function_call(*fun, args);
    auto exp_funcall = FUNCTION_TYPE(VectorOfTypes({TYPE("Integer", {})}), TYPE("Integer", {}));
    EXPECT_EQ(*fun, *exp_funcall) << "got: " << fun->to_string() << " expected: " << exp_funcall->to_string();
}

TEST(generic_test, unify_function_call_2) {
    auto fun = FUNCTION_TYPE(VectorOfTypes({TYPE("t", {})}), TYPE("t", {}));
    auto args = VectorOfTypes({TYPE("String", {}), TYPE("Integer", {})});
    try {
        unify_function_call(*fun, args);
        FAIL();
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
    }
}


TEST(generic_test, unify_function_call_3) {
    // auto fun = FUNCTION_TYPE(VectorOfTypes({TYPE("t", {}), TYPE("u", {})}),
    //                          TYPE("Tuple", VectorOfTypes({TYPE("t", {}), TYPE("u", {})})));
    // auto args = VectorOfTypes({TYPE("Integer", {}), TYPE("String", {})});
    // unify_function_call(*fun, args);
    // auto exp_funcall = FUNCTION_TYPE(VectorOfTypes({TYPE("Integer", {}), TYPE("String", {})}),
    //                                  TYPE("Tuple", VectorOfTypes({TYPE("Integer", {}), TYPE("String", {})})));
    // EXPECT_EQ(*fun, *exp_funcall) << "got: " << fun->to_string() << " expected: " << exp_funcall->to_string();

    std::string text = "fun(t,u)->Tuple[t,u];Integer;String;fun(Integer, String)->Tuple[Integer, String]";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    FunctionTypeNode* fun = parser.parse_function_type();
    parser.next();
    TypeNode* arg0 = parser.parse_type_node();
    parser.next();
    TypeNode* arg1 = parser.parse_type_node();
    parser.next();
    TypeNode* exp_fun = parser.parse_type_node();
    std::cout << fun->to_string() << std::endl;
    std::cout << arg0->to_string() << std::endl;
    std::cout << arg1->to_string() << std::endl;
    std::cout << exp_fun->to_string() << std::endl;
    auto args = VectorOfTypes({arg0, arg1});
    unify_function_call(*fun, args);
    std::cout << fun->to_string() << std::endl;
    EXPECT_EQ(*fun, *exp_fun) << "got: " << fun->to_string() << " expected: " << exp_fun->to_string();
}

TEST(generic_test, unify_function_call_4) {
    // fun(fun(a)->String, a) -> String
    // fun(t)->t, String


    std::string text = "fun(fun(a)->String, a)->String;fun(t)->t; String;fun(fun(String)->String, String)->String";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    FunctionTypeNode* fun = parser.parse_function_type();
    parser.next();
    TypeNode* arg0 = parser.parse_type_node();
    parser.next();
    TypeNode* arg1 = parser.parse_type_node();
    parser.next();
    TypeNode* exp_fun = parser.parse_type_node();
    std::cout << fun->to_string() << std::endl;
    std::cout << arg0->to_string() << std::endl;
    std::cout << arg1->to_string() << std::endl;
    std::cout << exp_fun->to_string() << std::endl;
    auto args = VectorOfTypes({arg0, arg1});
    unify_function_call(*fun, args);
    std::cout << fun->to_string() << std::endl;
    EXPECT_EQ(*fun, *exp_fun) << "got: " << fun->to_string() << " expected: " << exp_fun->to_string();

    // auto fun = FUNCTION_TYPE(VectorOfTypes(
    //                                  {
    //                                      FUNCTION_TYPE(VectorOfTypes({TYPE("a", {})}), TYPE("a", {})), TYPE(
    //                                              "a",
    //                                              {}
    //                                      )
    //                                  }
    //                          ), TYPE("String", {}));
    // auto args = VectorOfTypes({FUNCTION_TYPE(VectorOfTypes({TYPE("t", {})}), TYPE("t", {})), TYPE("String", {})});
    // unify_function_call(*fun, args);
    // auto exp_funcall = FUNCTION_TYPE(VectorOfTypes(
    //                                          {
    //                                              FUNCTION_TYPE(
    //                                                      VectorOfTypes({TYPE("String", {})}),
    //                                                      TYPE("String", {})), TYPE(
    //                                                      "String",
    //                                                      {}
    //                                              )
    //                                          }
    //                                  ), TYPE("String", {}));
    // EXPECT_EQ(*fun, *exp_funcall) << "got: " << fun->to_string() << " expected: " << exp_funcall->to_string();
}


TEST(generic_test, unify_function_call_5) {
    // fun(fun(a)->String, a) -> String
    // fun(t)->t, String
    std::string text = "fun(fun(a)->String)->String;fun(t)->t; fun(fun(String)->String)->String";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    FunctionTypeNode* fun = parser.parse_function_type();
    parser.next();
    TypeNode* arg0 = parser.parse_type_node();
    parser.next();
    TypeNode* exp_fun = parser.parse_type_node();
    std::cout << fun->to_string() << std::endl;
    std::cout << arg0->to_string() << std::endl;
    std::cout << exp_fun->to_string() << std::endl;
    auto args = VectorOfTypes({arg0});
    unify_function_call(*fun, args);
    std::cout << fun->to_string() << std::endl;

}


TEST(generic_test, unify_function_call_6) {
    // fun(fun(a)->String, a) -> String
    // fun(t)->t, String
    std::string text = "fun(fun(a)->String, a)->String;fun(t)->t; Integer";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    FunctionTypeNode* fun = parser.parse_function_type();
    parser.next();
    TypeNode* arg0 = parser.parse_type_node();
    parser.next();
    TypeNode* arg1 = parser.parse_type_node();
    std::cout << fun->to_string() << std::endl;
    std::cout << arg0->to_string() << std::endl;
    std::cout << arg1->to_string() << std::endl;
    auto args = VectorOfTypes({arg0, arg1});
    try {
        unify_function_call(*fun, args);
        FAIL() << fun->to_string() << std::endl;
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
    }

}

TEST(generic_test, foo_1) {
    // fun(fun(a)->String, a) -> String
    // fun(t)->t, Integer
    // FAILS
    std::string text = "fun(fun(a)->String, a)->String;fun(t)->t; Integer";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    FunctionTypeNode* fun = parser.parse_function_type();
    parser.next();
    TypeNode* arg0 = parser.parse_type_node();
    parser.next();
    TypeNode* arg1 = parser.parse_type_node();
    std::cout << fun->to_string() << std::endl;
    std::cout << arg0->to_string() << std::endl;
    std::cout << arg1->to_string() << std::endl;
    auto args = VectorOfTypes({arg0, arg1});
    try {
        unify_function_call(*fun, args);
        FAIL() << fun->to_string() << std::endl;
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
        std::cout << fun->to_string() << std::endl;
    }

}

TEST(generic_test, foo_2) {
    // fun(fun(a)->String, a) -> String
    // fun(t)->t, Integer
    // FAILS
    std::string text = "fun(a, fun(a)->String)->String;Integer; fun(t)->t;";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    FunctionTypeNode* fun = parser.parse_function_type();
    parser.next();
    TypeNode* arg0 = parser.parse_type_node();
    parser.next();
    TypeNode* arg1 = parser.parse_type_node();
    std::cout << fun->to_string() << std::endl;
    std::cout << arg0->to_string() << std::endl;
    std::cout << arg1->to_string() << std::endl;
    auto args = VectorOfTypes({arg0, arg1});
    try {
        unify_function_call(*fun, args);
        FAIL() << fun->to_string() << std::endl;
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
        std::cout << fun->to_string() << std::endl;
    }

}

TEST(generic_test, foo_3) {
    // fun(fun(a)->String, a) -> String
    // fun(t)->t, Integer
    // FAILS
    std::string text = "fun(List[a], fun(a)->String)->String;List[Integer]; fun(t)->t;";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    FunctionTypeNode* fun = parser.parse_function_type();
    parser.next();
    TypeNode* arg0 = parser.parse_type_node();
    parser.next();
    TypeNode* arg1 = parser.parse_type_node();
    std::cout << fun->to_string() << std::endl;
    std::cout << arg0->to_string() << std::endl;
    std::cout << arg1->to_string() << std::endl;
    auto args = VectorOfTypes({arg0, arg1});
    try {
        unify_function_call(*fun, args);
        FAIL() << fun->to_string() << std::endl;
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
        std::cout << fun->to_string() << std::endl;
    }

}

TEST(generic_test, foo_4) {
    // fun(fun(a)->String, a) -> String
    // fun(t)->t, Integer
    // FAILS
    std::string text = "fun(List[a], fun(a)->String)->String;List[String]; fun(t)->t;fun(List[String],fun(String)->String)->String";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    FunctionTypeNode* fun = parser.parse_function_type();
    parser.next();
    TypeNode* arg0 = parser.parse_type_node();
    parser.next();
    TypeNode* arg1 = parser.parse_type_node();
    parser.next();
    TypeNode* exp_fun = parser.parse_type_node();
    std::cout << fun->to_string() << std::endl;
    std::cout << arg0->to_string() << std::endl;
    std::cout << arg1->to_string() << std::endl;
    auto args = VectorOfTypes({arg0, arg1});
    unify_function_call(*fun, args);
    EXPECT_EQ(*fun, *exp_fun);
}

TEST(generic_test, foo_5) {
    // fun(fun(a)->String, a) -> String
    // fun(t)->t, Integer
    // FAILS
    std::string text = "fun(a, b)->Tuple[a,b];Integer;String;fun(Integer, String)->Tuple[Integer,String]";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    FunctionTypeNode* fun = parser.parse_function_type();
    parser.next();
    TypeNode* arg0 = parser.parse_type_node();
    parser.next();
    TypeNode* arg1 = parser.parse_type_node();
    parser.next();
    TypeNode* exp_fun = parser.parse_type_node();
    std::cout << fun->to_string() << std::endl;
    std::cout << arg0->to_string() << std::endl;
    std::cout << arg1->to_string() << std::endl;
    auto args = VectorOfTypes({arg0, arg1});
    unify_function_call(*fun, args);
    EXPECT_EQ(*fun, *exp_fun);
}

TEST(generic_test, foo_6) {
    // fun(fun(a)->String, a) -> String
    // fun(t)->t, Integer
    // FAILS
    std::string text = "fun(fun(a)->b)->fun(a)->b;fun(t)->t;fun(fun(t)->t)->fun(t)->t";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    FunctionTypeNode* fun = parser.parse_function_type();
    parser.next();
    TypeNode* arg0 = parser.parse_type_node();
    parser.next();
    TypeNode* exp_fun = parser.parse_type_node();
    std::cout << fun->to_string() << std::endl;
    std::cout << arg0->to_string() << std::endl;
    auto args = VectorOfTypes({arg0});
    unify_function_call(*fun, args);
    EXPECT_EQ(*fun, *exp_fun);
}

TEST(generic_test, foo_7) {
    // fun(fun(a)->String, a) -> String
    // fun(t)->t, Integer
    // FAILS
    std::string text = "fun(String)->String;t";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    FunctionTypeNode* fun = parser.parse_function_type();
    parser.next();
    TypeNode* arg0 = parser.parse_type_node();
    std::cout << fun->to_string() << std::endl;
    std::cout << arg0->to_string() << std::endl;
    auto args = VectorOfTypes({arg0});
    try {
        unify_function_call(*fun, args);
        FAIL() << fun->to_string() << std::endl;
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
        std::cout << fun->to_string() << std::endl;
    }
}

TEST(generic_test, foo_8) {
    // fun(fun(a)->String, a) -> String
    // fun(t)->t, Integer
    // FAILS
    std::string text = "fun(Object[String])->String;t";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    FunctionTypeNode* fun = parser.parse_function_type();
    parser.next();
    TypeNode* arg0 = parser.parse_type_node();
    std::cout << fun->to_string() << std::endl;
    std::cout << arg0->to_string() << std::endl;
    auto args = VectorOfTypes({arg0});
    try {
        unify_function_call(*fun, args);
        FAIL() << fun->to_string() << std::endl;
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
        std::cout << fun->to_string() << std::endl;
    }
}

TEST(generic_test, foo_9) {
    // fun(fun(a)->String, a) -> String
    // fun(t)->t, Integer
    // FAILS
    std::string text = "fun(Object[a])->String;t";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    FunctionTypeNode* fun = parser.parse_function_type();
    parser.next();
    TypeNode* arg0 = parser.parse_type_node();
    std::cout << fun->to_string() << std::endl;
    std::cout << arg0->to_string() << std::endl;
    auto args = VectorOfTypes({arg0});
    try {
        unify_function_call(*fun, args);
        FAIL() << fun->to_string() << std::endl;
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
        std::cout << fun->to_string() << std::endl;
    }
}

TEST(generic_test, foo_10) {
    // fun(fun(a)->String, a) -> String
    // fun(t)->t, Integer
    // FAILS
    std::string text = "fun(a)->String;t;fun(t)->String";
    Scanner scanner(text);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(tokens);
    FunctionTypeNode* fun = parser.parse_function_type();
    parser.next();
    TypeNode* arg0 = parser.parse_type_node();
    parser.next();
    TypeNode* exp_fun = parser.parse_type_node();
    std::cout << fun->to_string() << std::endl;
    std::cout << arg0->to_string() << std::endl;
    auto args = VectorOfTypes({arg0});
    unify_function_call(*fun, args);
    EXPECT_EQ(*fun, *exp_fun);
}