//
// Created by chris on 2/8/20.
//

#include <gtest/gtest.h>
#include <translator/Translator.h>
#include <utils.h>
#include <vm/LabelledCode.h>


TEST(translator_test, test_declaration_with_expression) {
    Translator translator;
    Node* node = DECL("x", nullptr, BIN(OpType::ADD, NUM(5), NUM(7)));
    translator.dispatch(node);
    CodeLabel expected_code = {NL(I_PUSHI(5)), NL(I_PUSHI(7)), NL(I_BIN(OpType::ADD)), NL(I_DECL("x")), NL(I_SET("x"))};
    EXPECT_EQ(translator.code, expected_code)
                        << "GOT:\n----\n" << translator.code << "----\nEXPECTED:\n----\n" << expected_code << "----\n";
}

TEST(translator_test, test_call) {
    Translator translator;
    Node* node = CALL(ID("x"), VectorOfNodes({NUM(7), BIN(OpType::SUB, NUM(3), ID("y"))}));
    translator.dispatch(node);
    CodeLabel expected_code = {NL(I_PUSHI(7)), NL(I_PUSHI(3)), NL(I_GET("y")), NL(I_BIN(OpType::SUB)), NL(I_GET("x")),
                               NL(I_CALL)};
    EXPECT_EQ(translator.code, expected_code)
                        << "GOT:\n----\n" << translator.code << "----\nEXPECTED:\n----\n" << expected_code << "----\n";
}

TEST(translator_test, test_member_get) {
    Translator translator;
    Node* node = MEM(CALL(ID("x"), VectorOfNodes({NUM(7), ID("y")})), "w");
    translator.dispatch(node);
    CodeLabel expected_code = {NL(I_PUSHI(7)), NL(I_GET("y")), NL(I_GET("x")), NL(I_CALL), NL(I_GETM("w"))};
    EXPECT_EQ(translator.code, expected_code)
                        << "GOT:\n----\n" << translator.code << "----\nEXPECTED:\n----\n" << expected_code << "----\n";
}

TEST(translator_test, test_member_get_rvalue) {
    Translator translator;
    Node* node = DECL("z", nullptr, MEM(CALL(ID("x"), VectorOfNodes({NUM(7), ID("y")})), "w"));
    translator.dispatch(node);
    CodeLabel expected_code = {NL(I_PUSHI(7)), NL(I_GET("y")), NL(I_GET("x")), NL(I_CALL), NL(I_GETM("w")),
                               NL(I_DECL("z")), NL(I_SET("z"))};
    EXPECT_EQ(translator.code, expected_code)
                        << "GOT:\n----\n" << translator.code << "----\nEXPECTED:\n----\n" << expected_code << "----\n";
}

TEST(translator_test, test_member_set_lvalue) {
    Translator translator;
    Node* node = ASN(MEM(CALL(ID("x"), VectorOfNodes({NUM(7), ID("y")})), "w"), NUM(14));
    translator.dispatch(node);
    CodeLabel expected_code = {NL(I_PUSHI(14)), NL(I_PUSHI(7)), NL(I_GET("y")), NL(I_GET("x")), NL(I_CALL),
                               NL(I_SETM("w"))};
    EXPECT_EQ(translator.code, expected_code)
                        << "GOT:\n----\n" << translator.code << "----\nEXPECTED:\n----\n" << expected_code << "----\n";
}

TEST(translator_test, test_id_set) {
    Translator translator;
    Node* node = ASN(ID("x"), NUM(14));
    translator.dispatch(node);
    CodeLabel expected_code = {NL(I_PUSHI(14)), NL(I_SET("x"))};
    EXPECT_EQ(translator.code, expected_code)
                        << "GOT:\n----\n" << translator.code << "----\nEXPECTED:\n----\n" << expected_code << "----\n";
}

TEST(translator_test, test_id_get) {
    Translator translator;
    Node* node = ASN(ID("y"), BIN(OpType::ADD, ID("x"), NUM(7)));
    translator.dispatch(node);
    CodeLabel expected_code = {NL(I_GET("x")), NL(I_PUSHI(7)), NL(I_BIN(OpType::ADD)), NL(I_SET("y"))};
    EXPECT_EQ(translator.code, expected_code)
                        << "GOT:\n----\n" << translator.code << "----\nEXPECTED:\n----\n" << expected_code << "----\n";
}

TEST(translator_test, test_if) {
    Translator translator;
    VectorOfNodes then = {ASN(ID("y"), BIN(OpType::ADD, ID("x"), NUM(9)))};
    BlockNode then_node(then);
//    Node* node = IF(BIN(OpType::EQ, ID("x"), NUM(3)), &then_node);
//    node->accept(translator);
    CodeLabel expected_code =
            {
                    NL(I_GET("x")),
                    NL(I_PUSHI(3)),
                    NL(I_BIN(OpType::EQ)),
                    NL(I_JUMPF(7)),
                    NL(I_ENTER("if")),
                    NL(I_GET("x")),
                    NL(I_PUSHI(9)),
                    NL(I_BIN(OpType::ADD)),
                    NL(I_SET("y")),
                    NL(I_LEAVE("if"))
            };
    EXPECT_EQ(translator.code, expected_code)
                        << "GOT:\n----\n" << translator.code << "----\nEXPECTED:\n----\n" << expected_code << "----\n";
}

TEST(translator_test, test_class_literal_fields) {
    Translator translator;
    Node* node = new ClassLiteralFieldNode(OBJECT_TYPE("Foo", {}), {{"foo", BIN(OpType::MUL, NUM(7), ID("a"))},
                                                                    {"bar", NUM(65)}});
    translator.dispatch(node);
    VectorOfStrings fields = {"foo", "bar"};
    CodeLabel expected_code = {NL(I_PUSHI(7)), NL(I_GET("a")), NL(I_BIN(OpType::MUL)), NL(I_PUSHI(65)),
                               NL(I_MAKE_OBJECT("Foo", fields))};
    EXPECT_EQ(translator.code, expected_code)
                        << "GOT:\n----\n" << translator.code << "----\nEXPECTED:\n----\n" << expected_code << "----\n";
}

TEST(translator_test, translate_empty_list) {
    Translator translator;
    Node* node = LST(VectorOfNodes());
    translator.dispatch(node);
    CodeLabel expected_code = {NL(I_MAKE_LIST(0))};
    EXPECT_EQ(translator.code.size(), expected_code.size());
    EXPECT_EQ(translator.code, expected_code)
                        << "GOT:\n----\n" << translator.code << "----\nEXPECTED:\n----\n" << expected_code << "----\n";
}

TEST(translator_test, translate_list_one_element) {
    Translator translator;
    Node* node = LST(VectorOfNodes({NUM(1)}));
    translator.dispatch(node);
    CodeLabel expected_code = {NL(I_PUSHI(1)), NL(I_MAKE_LIST(1))};
    EXPECT_EQ(translator.code.size(), expected_code.size());
    EXPECT_EQ(translator.code, expected_code)
                        << "GOT:\n----\n" << translator.code << "----\nEXPECTED:\n----\n" << expected_code << "----\n";
}

TEST(translator_test, translate_list_multiple_elements) {
    Translator translator;
    VectorOfNodes list = {NUM(3), NUM(1), NUM(4)};
    Node* node = LST(list);
    translator.dispatch(node);
    CodeLabel expected_code = {NL(I_PUSHI(3)), NL(I_PUSHI(1)), NL(I_PUSHI(4)), NL(I_MAKE_LIST(3))};
    EXPECT_EQ(translator.code.size(), expected_code.size());
    EXPECT_EQ(translator.code, expected_code)
                        << "GOT:\n----\n" << translator.code << "----\nEXPECTED:\n----\n" << expected_code << "----\n";
}

TEST(translator_test, test_boolean) {
    Translator translator;
    Node* node = ASN(ID("x"), BOOL(true));
    translator.dispatch(node);
    CodeLabel expected_code = {NL(I_PUSHB(true)), NL(I_SET("x"))};
    EXPECT_EQ(translator.code, expected_code)
                        << "GOT:\n----\n" << translator.code << "----\nEXPECTED:\n----\n" << expected_code << "----\n";
}

TEST(translator_test, test_while) {
    Translator translator;
    Node* node = WHILE(ID("x"), new BlockNode({CALL(ID("print"), {ID("y")})}));
    translator.dispatch(node);
    CodeLabel expected_code = {
            LC("start_loop.0", I_GET("x")),
            NL(I_JUMPF(8)),
            NL(I_ENTER("while")),
            NL(I_GET("y")),
            NL(I_GET("print")),
            NL(I_CALL),
            NL(I_LEAVE("while")),
            NL(I_JUMP("start_loop.0")),
            LC("break_loop.0", I_LEAVE("while"))
    };
    EXPECT_EQ(translator.code, expected_code)
                        << "GOT:\n----\n" << translator.code << "----\nEXPECTED:\n----\n" << expected_code << "----\n";
}

TEST(translator_test, test_while_with_break) {
    Translator translator;
    Node* node = WHILE(ID("x"), new BlockNode({BREAK, CALL(ID("foo"), {NUM(13)})}));
    translator.dispatch(node);
    CodeLabel expected_code = {
            LC("start_loop.0", I_GET("x")),
            NL(I_JUMPF(9)),
            NL(I_ENTER("while")),
            NL(I_JUMP("break_loop.0")),
            NL(I_PUSHI(13)),
            NL(I_GET("foo")),
            NL(I_CALL),
            NL(I_LEAVE("while")),
            NL(I_JUMP("start_loop.0")),
            LC("break_loop.0", I_LEAVE("while"))
    };
    EXPECT_EQ(translator.code, expected_code)
                        << "GOT:\n----\n" << translator.code << "----\nEXPECTED:\n----\n" << expected_code << "----\n";
}

TEST(translator_test, test_nested_while) {
    Translator translator;
    Node* node = WHILE(ID("x"), new BlockNode(
            {WHILE(BIN(OpType::EQ, ID("y"), NUM(7)), new BlockNode({BREAK, CALL(ID("foo"), {NUM(13)})})), BREAK,
             ASN(ID("x"), NUM(9))}));
    translator.dispatch(node);
    CodeLabel expected_code = {
            LC("start_loop.0", I_GET("x")),
            NL(I_JUMPF(20)), // outer loop condition false -> jump
            NL(I_ENTER("while")), // enter outer loop scope
            LC("start_loop.1", I_GET("y")),
            NL(I_PUSHI(7)),
            NL(I_BIN(OpType::EQ)),
            NL(I_JUMPF(9)), // inner loop condition false -> jump
            NL(I_ENTER("while")), // enter inner loop scope
            NL(I_JUMP("break_loop.1")), // break inner loop
            NL(I_PUSHI(13)),
            NL(I_GET("foo")),
            NL(I_CALL),
            NL(I_LEAVE("while")), // leave inner loop scope
            NL(I_JUMP("start_loop.1")), // go back to start of inner loop
            LC("break_loop.1", I_LEAVE("while")), // break inner loop
            NL(I_JUMP("break_loop.0")),
            NL(I_PUSHI(9)),
            NL(I_SET("x")),
            NL(I_LEAVE("while")), // leave outer loop scope
            NL(I_JUMP("start_loop.0")), // go back to start of outer loop,
            LC("break_loop.0", I_LEAVE("while")) // break outer loop
    };
    EXPECT_EQ(translator.code, expected_code)
                        << "GOT:\n----\n" << translator.code << "----\nEXPECTED:\n----\n" << expected_code << "----\n";
}
//
//TEST(translator_test, test_very_complex_nested_while) {
//    Translator translator;
//    Node* node = new BlockNode({
//                                       WHILE(ID("x"), new BlockNode(
//                                               {
//                                                       WHILE(BIN(OpType::EQ, ID("y"), NUM(7)), new BlockNode({
//                                                                                                                     BREAK,
//                                                                                                                     CALL(ID("foo"),
//                                                                                                                          {NUM(13)})
//                                                                                                             })),
//                                                       BREAK,
//                                                       ASN(ID("x"), NUM(9)),
//                                                       IF(ID("z"), new BlockNode({
//                                                                                         BREAK
//                                                                                 }))
//                                               })),
//                                       WHILE(ID("w"), new BlockNode({
//                                                                            IF(ID("t"), new BlockNode({
//                                                                                                              BREAK
//                                                                                                      })),
//                                                                            ASN(ID("g"), NUM(8)),
//                                                                            BREAK
//                                                                    }))
//                               });
//    node->accept(translator);
//    CodeLabel expected_code = {
//            LC("start_loop.0", I_GET("x")),
//
//            NL(I_JUMPF(25)), // outer loop condition false -> jump
//
//            NL(I_ENTER("while")), // enter outer loop scope
//
//            LC("start_loop.1", I_GET("y")),
//            NL(I_PUSHI(7)),
//            NL(I_BIN(OpType::EQ)),
//
//            NL(I_JUMPF(9)), // inner loop condition false -> jump
//
//            NL(I_ENTER("while")), // enter inner loop scope
//            NL(I_JUMP("break_loop.1")), // break inner loop
//            NL(I_PUSHI(13)),
//            NL(I_GET("foo")),
//            NL(I_CALL),
//            NL(I_LEAVE("while")),
//            NL(I_JUMP("start_loop.1")),
//            LC("break_loop.1", I_LEAVE("while")),
//
//            NL(I_JUMP("break_loop.0")),
//
//            NL(I_PUSHI(9)),
//            NL(I_SET("x")),
//
//            NL(I_GET("z")),
//            NL(I_JUMPF(4)),
//            NL(I_ENTER("if")),
//            NL(I_JUMP("break_loop.0")),
//            NL(I_LEAVE("if")),
//
//            NL(I_LEAVE("while")), // leave inner loop scope
//            NL(I_JUMP("start_loop.0")), // go back to start of inner loop
//            LC("break_loop.0", I_LEAVE("while")), // break inner loop
//
//            LC("start_loop.2", I_GET("w")),
//            NL(I_JUMPF(13)), // jump to after loop
//            NL(I_ENTER("while")),
//            NL(I_GET("t")),
//            NL(I_JUMPF(4)), // if false
//            NL(I_ENTER("if")),
//            NL(I_JUMP("break_loop.2")),
//            NL(I_LEAVE("if")),
//            NL(I_PUSHI(8)),
//            NL(I_SET("g")),
//            NL(I_JUMP("break_loop.2")),
//            NL(I_LEAVE("while")),
//            NL(I_JUMP("start_loop.2")),
//            LC("break_loop.2", I_LEAVE("while"))
//    };
//    EXPECT_EQ(translator.code, expected_code)
//                        << "GOT:\n----\n" << translator.code << "----\nEXPECTED:\n----\n" << expected_code << "----\n";
//}

TEST(translator_test, function) {
    Translator translator;
    VectorOfStrings parameter_names = {"x", "y"};
    VectorOfNodes parameter_types = {T_INT, T_INT};
    BlockNode* function_code = new BlockNode({RET(BIN(OpType::ADD, ID("x"), ID("y")))});
    Node* node = FUN("foo", parameter_names, parameter_types, T_INT, function_code);
    translator.dispatch(node);
    CodeLabel expected_code = {
            NL(I_START_FUNCTION("foo")),
            NL(I_ENTER("foo")),
            NL(I_DECL("y")),
            NL(I_SET("y")),
            NL(I_DECL("x")),
            NL(I_SET("x")),
            NL(I_GET("x")),
            NL(I_GET("y")),
            NL(I_BIN(OpType::ADD)),
            NL(I_LEAVE("foo")),
            NL(I_END_FUNCTION("foo"))
    };
    EXPECT_EQ(translator.code, expected_code)
                        << "GOT:\n----\n" << translator.code << "----\nEXPECTED:\n----\n" << expected_code << "----\n";
}

TEST(translator_test, ternary) {
//    var s = x ? "one" : "two";
    Translator translator;
    Node* node = DECL("s", nullptr, TERNARY(ID("x"), STR("one"), STR("two")));
    translator.dispatch(node);
    CodeLabel expected_code = {
            NL(I_DECL("s")),
            NL(I_GET("x")),
    };
    EXPECT_EQ(translator.code, expected_code)
                        << "GOT:\n----\n" << translator.code << "----\nEXPECTED:\n----\n" << expected_code << "----\n";
}

TEST(translator_test, overload) {
//    var s = x ? "one" : "two";
    Translator translator;
    auto to_string_string = FUN("to_string.0",{"s"},{T_STRING},T_STRING,new BlockNode({RET(ID("s"))}));
    auto to_string_integer = FUN("to_string.1",{"i"},{T_INT},T_STRING,new BlockNode({RET(STR("Integer string"))}));

    BlockNode* node = new BlockNode({to_string_string, to_string_integer});
    translator.dispatch(node);
    CodeLabel expected_code = {
            NL(I_DECL("s")),
            NL(I_GET("x")),
    };
    EXPECT_EQ(translator.code, expected_code)
                        << "GOT:\n----\n" << translator.code << "----\nEXPECTED:\n----\n" << expected_code << "----\n";
}