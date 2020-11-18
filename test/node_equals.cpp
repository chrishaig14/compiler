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


#define BODY_NODE new BlockNode({N_ASN(new IdNode("x"), N_BIN(OpType::ADD, new IdNode("a"), new IdNode("b"))),N_ASN(new IdNode("x"), new IdNode("y"))})
#define FUN_FOO_NODE FUN("foo", { "x" }, VectorOfTypes({ COMPLEX_TYPE }), TYPE("List", {T_INT}), BODY_NODE)
#define FUN_FOO_STRING "fun foo(x:List[List[Integer]])->List[Integer]{x=a+b; x = y;}"
#define COMPLEX_TYPE T_LIST(T_LIST(T_INT))

#define COMPLETE_TEST  EXPECT_EQ(node.ntype, expected_node.ntype);
#define EXPECT_NOT_EQUAL EXPECT_FALSE(node->equal(expected_node)); delete node; delete expected_node;


TEST(node_test, id) {
    EXPECT_EQ(IdNode("a"), IdNode("a"));
    EXPECT_NE(IdNode("a"), IdNode("b"));
}

TEST(node_test, number) {
    EXPECT_EQ(NumberNode(7), NumberNode(7));
    EXPECT_NE(NumberNode(7), NumberNode(8));
}

TEST(node_test, strng) {
    EXPECT_EQ(StringNode("foo"), StringNode("foo"));
    EXPECT_NE(StringNode("foo"), StringNode("bar"));
}


TEST(node_test, boolean) {
    EXPECT_EQ(BooleanNode(true), BooleanNode(true));
    EXPECT_NE(BooleanNode(true), BooleanNode(false));
}


TEST(node_test, binop) {
    EXPECT_EQ(BinopNode(OpType::ADD, new IdNode("a"), new IdNode("b")),
              BinopNode(OpType::ADD, new IdNode("a"), new IdNode("b")));
    EXPECT_NE(BinopNode(OpType::ADD, new IdNode("a"), new IdNode("b")),
              BinopNode(OpType::ADD, new IdNode("a"), new IdNode("c")));
    EXPECT_NE(BinopNode(OpType::ADD, new IdNode("a"), new IdNode("b")),
              BinopNode(OpType::ADD, new IdNode("c"), new IdNode("b")));
    EXPECT_NE(BinopNode(OpType::SUB, new IdNode("a"), new IdNode("b")),
              BinopNode(OpType::ADD, new IdNode("a"), new IdNode("b")));
    EXPECT_NE(BinopNode(OpType::ADD, new IdNode("a"), new IdNode("b")),
              BinopNode(OpType::ADD, new NumberNode(2), new IdNode("b")));
    EXPECT_NE(BinopNode(OpType::ADD, new IdNode("a"), new IdNode("b")),
              BinopNode(OpType::ADD, new IdNode("a"), new NumberNode(2)));
}


TEST(node_test, call) {
    EXPECT_EQ(CallNode(new IdNode("foo"), {}), CallNode(new IdNode("foo"), {}));
    EXPECT_EQ(CallNode(new IdNode("foo"), {new IdNode("a")}), CallNode(new IdNode("foo"), {new IdNode("a")}));
    EXPECT_EQ(CallNode(new IdNode("foo"), {new IdNode("a"), new NumberNode(7)}),
              CallNode(new IdNode("foo"), {new IdNode("a"), new NumberNode(7)}));
    EXPECT_NE(CallNode(new IdNode("foo"), {}), CallNode(new IdNode("bar"), {}));
    EXPECT_NE(CallNode(new IdNode("foo"), {}), CallNode(new MemberNode(new IdNode("foo"), "bar"), {}));
    EXPECT_NE(CallNode(new IdNode("foo"), {}), CallNode(new IdNode("foo"), {new IdNode("a")}));
    EXPECT_NE(CallNode(new IdNode("foo"), {new IdNode("a")}), CallNode(new IdNode("foo"), {new IdNode("b")}));
    EXPECT_NE(CallNode(new IdNode("foo"), {new IdNode("a")}), CallNode(new IdNode("foo"), {new NumberNode(7)}));
    EXPECT_NE(CallNode(new IdNode("foo"), {new IdNode("a")}),
              CallNode(new IdNode("foo"), {new IdNode("a"), new IdNode("b")}));
    EXPECT_NE(CallNode(new IdNode("foo"), {new IdNode("a"), new NumberNode(7)}),
              CallNode(new IdNode("foo"), {new IdNode("a"), new NumberNode(8)}));
    EXPECT_NE(CallNode(new IdNode("foo"), {new IdNode("a")}), CallNode(new IdNode("foo"), {}));
}


TEST(node_test, retrn) {
    EXPECT_EQ(ReturnNode(new IdNode("a")), ReturnNode(new IdNode("a")));
    EXPECT_NE(ReturnNode(new IdNode("a")), ReturnNode(new IdNode("b")));
}

TEST(node_test, block) {
    EXPECT_EQ(BlockNode({}), BlockNode({}));
    EXPECT_NE(BlockNode({new ReturnNode(new IdNode("b"))}), BlockNode({new ReturnNode(new IdNode("a"))}));
    EXPECT_EQ(BlockNode({new ReturnNode(new IdNode("b"))}), BlockNode({new ReturnNode(new IdNode("b"))}));
    EXPECT_NE(BlockNode({new ReturnNode(new IdNode("a"))}), BlockNode({}));
    EXPECT_NE(BlockNode({new ReturnNode(new IdNode("b"))}), BlockNode({new CallNode(new IdNode("b"), {})}));
}

TEST(node_test, decl) {
    EXPECT_EQ(DeclarationNode("x", nullptr, new IdNode("a")), DeclarationNode("x", nullptr, new IdNode("a")));
    EXPECT_NE(DeclarationNode("x", nullptr, new IdNode("a")), DeclarationNode("x", nullptr, new IdNode("b")));
    EXPECT_NE(DeclarationNode("x", nullptr, new IdNode("a")), DeclarationNode("y", nullptr, new IdNode("a")));
}
