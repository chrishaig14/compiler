//
// Created by chris on 2/8/20.
//

#include <gtest/gtest.h>
#include <translator/Translator.h>
#include <utils.h>
#include <vm/LabelledCode.h>

std::ostream& operator<<(std::ostream& out, const CodeLabel v) {
    for (auto i: v) {
        std::string spaces;
        for (int k = i.first.size(); k < 32; k++) {
            spaces += " ";
        }
        out << i.first + spaces + i.second->to_string() << std::endl;
    }
    return out;
}

bool operator==(const CodeLabel a, const CodeLabel b) {
    if (a.size() != b.size()) return false;
    for (int i = 0; i < a.size(); i++) {
        auto a_inst = a[i].second;
        auto b_inst = b[i].second;
        std::cout << "comparing " << a_inst->to_string() << " == " << b_inst->to_string() << ": "
                  << (a_inst->equal(b_inst) ? "True" : "False") << std::endl;
        if (!a_inst->equal(b_inst)) return false;
    }
    return true;
}

TEST(translator_test, test_declaration_with_expression) {
    Translator translator;
    Node* node = DECL("x", nullptr, BIN(OpType::ADD, NUM(5), NUM(7)));
    node->accept(translator);
    CodeLabel expected_code = {NL(I_PUSHI(5)), NL(I_PUSHI(7)), NL(I_BIN(OpType::ADD)), NL(I_DECL("x")), NL(I_SET("x"))};
    EXPECT_EQ(translator.code, expected_code)
                        << "GOT:\n----\n" << translator.code << "----\nEXPECTED:\n----\n" << expected_code << "----\n";
}

TEST(translator_test, test_call) {
    Translator translator;
    Node* node = CALL(ID("x"), std::vector<Node*>({NUM(7), BIN(OpType::SUB, NUM(3), ID("y"))}));
    node->accept(translator);
    CodeLabel expected_code = {NL(I_PUSHI(7)), NL(I_PUSHI(3)), NL(I_GET("y")), NL(I_BIN(OpType::SUB)), NL(I_GET("x")),
                               NL(I_CALL)};
    EXPECT_EQ(translator.code, expected_code)
                        << "GOT:\n----\n" << translator.code << "----\nEXPECTED:\n----\n" << expected_code << "----\n";
}

TEST(translator_test, test_member_get) {
    Translator translator;
    Node* node = MEM(CALL(ID("x"), VectorOfNodes({NUM(7), ID("y")})), "w");
    node->accept(translator);
    CodeLabel expected_code = {NL(I_PUSHI(7)), NL(I_GET("y")), NL(I_GET("x")), NL(I_CALL), NL(I_GETM("w"))};
    EXPECT_EQ(translator.code, expected_code)
                        << "GOT:\n----\n" << translator.code << "----\nEXPECTED:\n----\n" << expected_code << "----\n";
}

TEST(translator_test, test_member_get_rvalue) {
    Translator translator;
    Node* node = DECL("z", nullptr, MEM(CALL(ID("x"), VectorOfNodes({NUM(7), ID("y")})), "w"));
    node->accept(translator);
    CodeLabel expected_code = {NL(I_PUSHI(7)), NL(I_GET("y")), NL(I_GET("x")), NL(I_CALL), NL(I_GETM("w")),
                               NL(I_DECL("z")), NL(I_SET("z"))};
    EXPECT_EQ(translator.code, expected_code)
                        << "GOT:\n----\n" << translator.code << "----\nEXPECTED:\n----\n" << expected_code << "----\n";
}

TEST(translator_test, test_member_set_lvalue) {
    Translator translator;
    Node* node = ASN(MEM(CALL(ID("x"), VectorOfNodes({NUM(7), ID("y")})), "w"), NUM(14));
    node->accept(translator);
    CodeLabel expected_code = {NL(I_PUSHI(14)), NL(I_PUSHI(7)), NL(I_GET("y")), NL(I_GET("x")), NL(I_CALL),
                               NL(I_SETM("w"))};
    EXPECT_EQ(translator.code, expected_code)
                        << "GOT:\n----\n" << translator.code << "----\nEXPECTED:\n----\n" << expected_code << "----\n";
}

TEST(translator_test, test_id_set) {
    Translator translator;
    Node* node = ASN(ID("x"), NUM(14));
    node->accept(translator);
    CodeLabel expected_code = {NL(I_PUSHI(14)), NL(I_SET("x"))};
    EXPECT_EQ(translator.code, expected_code)
                        << "GOT:\n----\n" << translator.code << "----\nEXPECTED:\n----\n" << expected_code << "----\n";
}

TEST(translator_test, test_id_get) {
    Translator translator;
    Node* node = ASN(ID("y"), BIN(OpType::ADD, ID("x"), NUM(7)));
    node->accept(translator);
    CodeLabel expected_code = {NL(I_GET("x")), NL(I_PUSHI(7)), NL(I_BIN(OpType::ADD)), NL(I_SET("y"))};
    EXPECT_EQ(translator.code, expected_code)
                        << "GOT:\n----\n" << translator.code << "----\nEXPECTED:\n----\n" << expected_code << "----\n";
}

TEST(translator_test, test_if) {
    Translator translator;
    VectorOfNodes then = {ASN(ID("y"), BIN(OpType::ADD, ID("x"), NUM(9)))};
    BlockNode then_node(then);
    Node* node = IF(BIN(OpType::EQ, ID("x"), NUM(3)), &then_node);
    node->accept(translator);
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
    Node* node = new ClassLiteralFieldNode("Foo", {{"foo", BIN(OpType::MUL, NUM(7), ID("a"))},
                                                   {"bar", NUM(65)}});
    node->accept(translator);
    std::vector<std::string> fields = {"foo", "bar"};
    CodeLabel expected_code = {NL(I_PUSHI(7)), NL(I_GET("a")), NL(I_BIN(OpType::MUL)), NL(I_PUSHI(65)),
                               NL(I_MAKE_OBJECT("Foo", fields))};
    EXPECT_EQ(translator.code, expected_code)
                        << "GOT:\n----\n" << translator.code << "----\nEXPECTED:\n----\n" << expected_code << "----\n";
}

TEST(translator_test, translate_empty_list) {
    Translator translator;
    Node* node = LST(VectorOfNodes());
    node->accept(translator);
    CodeLabel expected_code = {NL(I_MAKE_LIST(0))};
    EXPECT_EQ(translator.code.size(), expected_code.size());
    EXPECT_EQ(translator.code, expected_code)
                        << "GOT:\n----\n" << translator.code << "----\nEXPECTED:\n----\n" << expected_code << "----\n";
}

TEST(translator_test, translate_list_one_element) {
    Translator translator;
    Node* node = LST(VectorOfNodes({NUM(1)}));
    node->accept(translator);
    CodeLabel expected_code = {NL(I_PUSHI(1)), NL(I_MAKE_LIST(1))};
    EXPECT_EQ(translator.code.size(), expected_code.size());
    EXPECT_EQ(translator.code, expected_code)
                        << "GOT:\n----\n" << translator.code << "----\nEXPECTED:\n----\n" << expected_code << "----\n";
}

TEST(translator_test, translate_list_multiple_elements) {
    Translator translator;
    VectorOfNodes list = {NUM(3), NUM(1), NUM(4)};
    Node* node = LST(list);
    node->accept(translator);
    CodeLabel expected_code = {NL(I_PUSHI(3)), NL(I_PUSHI(1)), NL(I_PUSHI(4)), NL(I_MAKE_LIST(3))};
    EXPECT_EQ(translator.code.size(), expected_code.size());
    EXPECT_EQ(translator.code, expected_code)
                        << "GOT:\n----\n" << translator.code << "----\nEXPECTED:\n----\n" << expected_code << "----\n";
}

TEST(translator_test, test_boolean) {
    Translator translator;
    Node* node = ASN(ID("x"), BOOL(true));
    node->accept(translator);
    CodeLabel expected_code = {NL(I_PUSHB(true)), NL(I_SET("x"))};
    EXPECT_EQ(translator.code, expected_code)
                        << "GOT:\n----\n" << translator.code << "----\nEXPECTED:\n----\n" << expected_code << "----\n";
}

TEST(translator_test, test_while) {
    Translator translator;
    Node* node = WHILE(ID("x"), new BlockNode({CALL(ID("print"), {ID("y")})}));
    node->accept(translator);
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
    node->accept(translator);
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