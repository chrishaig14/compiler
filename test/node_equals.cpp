//
// Created by chris on 15/11/20.
//

//
// Created by chris on 14/6/20.
//

#include <gtest/gtest.h>
#include <scanner/Scanner.h>
#include <parser/Parser.h>
#include <parser/UnexpectedToken.h>


#define BODY_NODE new BlockNode({N_ASN(N_ID("x"), N_BIN(OpType::ADD, N_ID("a"), N_ID("b"))),N_ASN(N_ID("x"), N_ID("y"))})
#define FUN_FOO_NODE FUN("foo", { "x" }, std::vector<TypeNode>({ COMPLEX_TYPE }), TYPE("List", {T_INT}), BODY_NODE)
#define FUN_FOO_STRING "fun foo(x:List[List[Integer]])->List[Integer]{x=a+b; x = y;}"
#define COMPLEX_TYPE T_LIST(T_LIST(T_INT))

#define COMPLETE_TEST  EXPECT_EQ(node.ntype, expected_node.ntype);
#define EXPECT_NOT_EQUAL EXPECT_FALSE(node->equal(expected_node)); delete node; delete expected_node;
#define N_NUM(x) NodeContainer(NUM(x))
#define N_SUB(a, b) NodeContainer(SUB(a,b))
#define N_ID(x) NodeContainer(ID(x))
#define N_BIN(a, b, c) NodeContainer(BIN(a,b,c))
#define N_CALL(a, b) NodeContainer(CALL(a,b))
#define N_ASN(a, b) NodeContainer(ASN(a,b))
#define N_LST(a) NodeContainer(LST(a))
#define N_FOR(a, b, c) NodeContainer(FOR(a,b,c))
#define N_WHILE(a, b) NodeContainer(WHILE(a,b))
#define N_BOOL(a) NodeContainer(BOOL(a))
#define N_STRING(a) NodeContainer(STR(a))
#define N_TERNARY(a, b, c) NodeContainer(TERNARY(a,b,c))
#define N_DECL(a, b, c) NodeContainer(DECL(a,b,c))
#define N_MEM(a, b) NodeContainer(MEM(a,b))


TEST(parser_test, id) {
    EXPECT_EQ(IdNode("a"), IdNode("a"));
    EXPECT_NE(IdNode("a"), IdNode("b"));
}

TEST(parser_test, n_id) {
    EXPECT_EQ(N_ID("a"), N_ID("a"));
    EXPECT_NE(N_ID("a"), N_ID("b"));
}

TEST(parser_test, number) {
    EXPECT_EQ(NumberNode(7), NumberNode(7));
    EXPECT_NE(NumberNode(7), NumberNode(8));
}

TEST(parser_test, n_number) {
    EXPECT_EQ(N_NUM(7), N_NUM(7));
    EXPECT_NE(N_NUM(7), N_NUM(8));
}

TEST(parser_test, strng) {
    EXPECT_EQ(StringNode("foo"), StringNode("foo"));
    EXPECT_NE(StringNode("foo"), StringNode("bar"));
}

TEST(parser_test, n_strng) {
    EXPECT_EQ(N_STRING("foo"), N_STRING("foo"));
    EXPECT_NE(N_STRING("foo"), N_STRING("bar"));
}

TEST(parser_test, boolean) {
    EXPECT_EQ(BooleanNode(true), BooleanNode(true));
    EXPECT_NE(BooleanNode(true), BooleanNode(false));
}

TEST(parser_test, n_boolean) {
    EXPECT_EQ(N_BOOL(true), N_BOOL(true));
    EXPECT_NE(N_BOOL(true), N_BOOL(false));
}

TEST(parser_test, binop) {
    EXPECT_EQ(BinopNode(OpType::ADD, N_ID("a"), N_ID("b")), BinopNode(OpType::ADD, N_ID("a"), N_ID("b")));
    EXPECT_NE(BinopNode(OpType::ADD, N_ID("a"), N_ID("b")), BinopNode(OpType::ADD, N_ID("a"), N_ID("c")));
    EXPECT_NE(BinopNode(OpType::ADD, N_ID("a"), N_ID("b")), BinopNode(OpType::ADD, N_ID("c"), N_ID("b")));
    EXPECT_NE(BinopNode(OpType::SUB, N_ID("a"), N_ID("b")), BinopNode(OpType::ADD, N_ID("a"), N_ID("b")));
    EXPECT_NE(BinopNode(OpType::ADD, N_ID("a"), N_ID("b")), BinopNode(OpType::ADD, N_NUM(2), N_ID("b")));
    EXPECT_NE(BinopNode(OpType::ADD, N_ID("a"), N_ID("b")), BinopNode(OpType::ADD, N_ID("a"), N_NUM(2)));
}

TEST(parser_test, n_binop) {
    EXPECT_EQ(N_BIN(OpType::ADD, N_ID("a"), N_ID("b")), N_BIN(OpType::ADD, N_ID("a"), N_ID("b")));
    EXPECT_NE(N_BIN(OpType::ADD, N_ID("a"), N_ID("b")), N_BIN(OpType::ADD, N_ID("a"), N_ID("c")));
    EXPECT_NE(N_BIN(OpType::ADD, N_ID("a"), N_ID("b")), N_BIN(OpType::ADD, N_ID("c"), N_ID("b")));
    EXPECT_NE(N_BIN(OpType::SUB, N_ID("a"), N_ID("b")), N_BIN(OpType::ADD, N_ID("a"), N_ID("b")));
    EXPECT_NE(N_BIN(OpType::ADD, N_ID("a"), N_ID("b")), N_BIN(OpType::ADD, N_NUM(2), N_ID("b")));
    EXPECT_NE(N_BIN(OpType::ADD, N_ID("a"), N_ID("b")), N_BIN(OpType::ADD, N_ID("a"), N_NUM(2)));
}


TEST(parser_test, call) {
    EXPECT_EQ(CallNode(N_ID("foo"), {}), CallNode(N_ID("foo"), {}));
    EXPECT_EQ(CallNode(N_ID("foo"), {N_ID("a")}), CallNode(N_ID("foo"), {N_ID("a")}));
    EXPECT_EQ(CallNode(N_ID("foo"), {N_ID("a"), N_NUM(7)}), CallNode(N_ID("foo"), {N_ID("a"), N_NUM(7)}));
    EXPECT_NE(CallNode(N_ID("foo"), {}), CallNode(N_ID("bar"), {}));
    EXPECT_NE(CallNode(N_ID("foo"), {}), CallNode(N_MEM(N_ID("foo"),"bar"), {}));
    EXPECT_NE(CallNode(N_ID("foo"), {}), CallNode(N_ID("foo"), {N_ID("a")}));
    EXPECT_NE(CallNode(N_ID("foo"), {N_ID("a")}), CallNode(N_ID("foo"), {N_ID("b")}));
    EXPECT_NE(CallNode(N_ID("foo"), {N_ID("a")}), CallNode(N_ID("foo"), {N_NUM(7)}));
    EXPECT_NE(CallNode(N_ID("foo"), {N_ID("a")}), CallNode(N_ID("foo"), {N_ID("a"), N_ID("b")}));
    EXPECT_NE(CallNode(N_ID("foo"), {N_ID("a"), N_NUM(7)}), CallNode(N_ID("foo"), {N_ID("a"), N_NUM(8)}));
    EXPECT_NE(CallNode(N_ID("foo"), {N_ID("a")}), CallNode(N_ID("foo"), {}));
}

TEST(parser_test, n_call) {
    EXPECT_EQ(N_CALL(N_ID("foo"), VectorOfNodes({})), N_CALL(N_ID("foo"), VectorOfNodes({})));
    EXPECT_EQ(N_CALL(N_ID("foo"), VectorOfNodes({N_ID("a")})), N_CALL(N_ID("foo"), VectorOfNodes({N_ID("a")})));
    EXPECT_EQ(N_CALL(N_ID("foo"), VectorOfNodes({N_ID("a"), N_NUM(7)})), N_CALL(N_ID("foo"), VectorOfNodes({N_ID("a"), N_NUM(7)})));
    EXPECT_NE(N_CALL(N_ID("foo"), VectorOfNodes({})), N_CALL(N_ID("bar"), VectorOfNodes({})));
    EXPECT_NE(N_CALL(N_ID("foo"), VectorOfNodes({})), N_CALL(N_MEM(N_ID("foo"),"bar"), VectorOfNodes({})));
    EXPECT_NE(N_CALL(N_ID("foo"), VectorOfNodes({})), N_CALL(N_ID("foo"), VectorOfNodes({N_ID("a")})));
    EXPECT_NE(N_CALL(N_ID("foo"), VectorOfNodes({N_ID("a")})), N_CALL(N_ID("foo"), VectorOfNodes({N_ID("b")})));
    EXPECT_NE(N_CALL(N_ID("foo"), VectorOfNodes({N_ID("a")})), N_CALL(N_ID("foo"), VectorOfNodes({N_NUM(7)})));
    EXPECT_NE(N_CALL(N_ID("foo"), VectorOfNodes({N_ID("a")})), N_CALL(N_ID("foo"), VectorOfNodes({N_ID("a"), N_ID("b")})));
    EXPECT_NE(N_CALL(N_ID("foo"), VectorOfNodes({N_ID("a"), N_NUM(7)})), N_CALL(N_ID("foo"), VectorOfNodes({N_ID("a"), N_NUM(8)})));
    EXPECT_NE(N_CALL(N_ID("foo"), VectorOfNodes({N_ID("a")})), N_CALL(N_ID("foo"), VectorOfNodes({})));
}