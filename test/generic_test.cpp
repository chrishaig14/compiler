#include <gtest/gtest.h>
#include <parser/Parser.h>
#include <scanner/Scanner.h>
#include <semantic/GlobalProcessor.h>
#include <semantic/Checker.h>
#include <gtest/gtest.h>
#include <scanner/Scanner.h>
#include <parser/Parser.h>
#include <parser/UnexpectedToken.h>


TEST(generic_test, test_11) {
    auto a = TYPE("a", {});
    EXPECT_TRUE(is_generic(a));
}

TEST(generic_test, test_12) {
    EXPECT_FALSE(is_generic(T_INT));
}

TEST(generic_test, test_13) {
    auto t = T_LIST(TYPE("a", {}));
    EXPECT_TRUE(is_generic(t));
}

TEST(generic_test, test_14) {
    auto t = T_LIST(T_INT);
    EXPECT_FALSE(is_generic(t));
}

TEST(generic_test, test_15) {
    auto t = T_LIST(T_LIST(TYPE("a", {})));
    EXPECT_TRUE(is_generic(t));
}

TEST(generic_test, test_16) {
    auto t = T_LIST(T_LIST(T_INT));
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
    auto a = TYPE("List", { T_INT });
    auto b = TYPE("Integer", {});
    EXPECT_FALSE(type_matches(a, b));
}

TEST(generic_test, test_4) {
    auto a = TYPE("List", { T_INT });
    auto b = TYPE("Dict", { T_INT });
    EXPECT_FALSE(type_matches(a, b));
}

TEST(generic_test, test_5) {
    auto a = TYPE("List", { T_INT });
    auto b = TYPE("List", { T_STRING });
    EXPECT_FALSE(type_matches(a, b));
}

TEST(generic_test, test_6) {
    auto a = TYPE("List", { TYPE("a", {}) });
    auto b = TYPE("List", { T_STRING });
    EXPECT_TRUE(type_matches(a, b));
}

TEST(generic_test, test_7) {
    auto a = TYPE("List", { TYPE("a", {}) });
    VectorOfNodes t = {T_STRING, T_INT};
    auto b = TYPE("Dict", t);
    EXPECT_FALSE(type_matches(a, b));
}

TEST(generic_test, test_8) {
    auto a = T_LIST(T_LIST(TYPE("a", {})));
    auto b = T_LIST(T_LIST(TYPE("Integer", {})));
    EXPECT_TRUE(type_matches(a, b));
}

TEST(generic_test, test_9) {
    auto a = T_DICT(T_LIST(TYPE("a", {})), TYPE("b", {}));
    auto b = T_DICT(T_LIST(TYPE("Integer", {})), T_STRING);
    EXPECT_TRUE(type_matches(a, b));
}


TEST(generic_test, test_20) {
    VectorOfNodes pt = {};
    TypeNode* rt = TYPE("NoneType", {});
    auto a = FUNCTION_TYPE(pt, rt);
    EXPECT_TRUE(type_matches(a, a));
}

TEST(generic_test, test_21) {
    VectorOfNodes pt = {};
    TypeNode* rt = TYPE("NoneType", {});
    auto a = FUNCTION_TYPE(pt, rt);
    auto b = FUNCTION_TYPE({ T_INT }, rt);
    EXPECT_FALSE(type_matches(a, b));
}


TEST(generic_test, test_22) {
    VectorOfNodes pt = {TYPE("a", {})};
    TypeNode* rt = TYPE("NoneType", {});
    auto a = FUNCTION_TYPE(pt, rt);
    EXPECT_TRUE(is_generic(a));
}

TEST(generic_test, test_23) {
    VectorOfNodes pt = {TYPE("Integer", {})};
    TypeNode* rt = TYPE("NoneType", {});
    auto a = FUNCTION_TYPE(pt, rt);
    EXPECT_FALSE(is_generic(a));
}

TEST(generic_test, test_24) {
    VectorOfNodes pt = {TYPE("a", {}), TYPE("a", {}), TYPE("b", {})};
    auto a = FUNCTION_TYPE(pt, T_INT);
    VectorOfNodes ptb = {T_INT, T_INT, T_STRING};
    auto b = FUNCTION_TYPE(ptb, T_INT);
    EXPECT_TRUE(type_matches(a, b));
}

TEST(generic_test, test_25) {
    VectorOfNodes pt = {TYPE("a", {}), TYPE("a", {}), TYPE("b", {})};
    auto a = FUNCTION_TYPE(pt, T_INT);
    VectorOfNodes ptb = {T_INT, T_STRING, T_STRING};
    auto b = FUNCTION_TYPE(ptb, T_INT);
    EXPECT_FALSE(type_matches(a, b));
}


TEST(generic_test, test_26) {
    auto a = TYPE("a", {});
    VectorOfNodes ptb = {T_INT, T_INT, T_STRING};
    auto b = FUNCTION_TYPE(ptb, T_INT);
    EXPECT_TRUE(type_matches(a, b));
}

TEST(generic_test, test_27) {
    VectorOfNodes pta = {TYPE("a", {})};
    auto a = FUNCTION_TYPE(pta, TYPE("a", {}));
    VectorOfNodes ptb = {T_INT};
    auto b = FUNCTION_TYPE(ptb, T_STRING);
    EXPECT_FALSE(type_matches(a, b));
}


TEST(generic_test, test_28) {
    VectorOfNodes pt = {TYPE("Integer", {})};
    TypeNode* rt = TYPE("a", {});
    auto a = FUNCTION_TYPE(pt, rt);
    EXPECT_TRUE(is_generic(a));
}

TEST(generic_test, make_replacement_1) {
    auto a = TYPE("a", {});
    auto b = TYPE("Integer", {});
    auto repl = make_replacements(a, b);
    EXPECT_TRUE(repl.count("a"));
    EXPECT_TRUE(repl["a"]->equal(b));
}

TEST(generic_test, make_replacement_2) {
    auto a = T_LIST(T_LIST(TYPE("a", {})));
    auto b = T_LIST(T_LIST(T_INT));
    auto repl = make_replacements(a, b);
    EXPECT_TRUE(repl.count("a"));
    EXPECT_TRUE(repl["a"]->equal(T_INT));
}

TEST(generic_test, make_replacement_3) {
    auto a = T_DICT(TYPE("a", {}), TYPE("b", {}));
    auto b = T_DICT(T_INT, T_STRING);
    auto repl = make_replacements(a, b);
    EXPECT_TRUE(repl.count("a"));
    EXPECT_TRUE(repl["a"]->equal(T_INT));
    EXPECT_TRUE(repl.count("b"));
    EXPECT_TRUE(repl["b"]->equal(T_STRING));
}

TEST(generic_test, make_replacement_4) {
    auto a = T_DICT(TYPE("a", {}), T_LIST(TYPE("b", {})));
    auto b = T_DICT(T_INT, T_LIST(T_STRING));
    auto repl = make_replacements(a, b);
    EXPECT_TRUE(repl.count("a"));
    EXPECT_TRUE(repl["a"]->equal(T_INT));
    EXPECT_TRUE(repl.count("b"));
    EXPECT_TRUE(repl["b"]->equal(T_STRING));
}
