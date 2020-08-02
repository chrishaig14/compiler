//
// Created by chris on 2/8/20.
//

#include <gtest/gtest.h>
#include <translator/Translator.h>
#include <utils.h>

std::ostream& operator<<(std::ostream& out, const std::vector<Instruction*> v) {
    for (auto i: v) {
        out << i->to_string() << std::endl;
    }
    return out;
}

bool operator==(const std::vector<Instruction*> a, const std::vector<Instruction*> b) {
    for (int i = 0; i < a.size(); i++) {
        if (!a[i]->equal(b[i])) return false;
    }
    return true;
}

TEST(translator_test, test_declaration_with_expression) {
    Translator translator;
    Node* node = DECL("x", nullptr, BIN(OpType::ADD, NUM(5), NUM(7)));
    node->accept(translator);
    Code expected_code = {I_PUSHI(5), I_PUSHI(7), I_BIN(OpType::ADD), I_DECL("x"), I_SET("x")};
    EXPECT_EQ(translator.code, expected_code)
                        << "GOT:\n----\n" << translator.code << "----\nEXPECTED:\n----\n" << expected_code << "----\n";
}

TEST(translator_test, test_call) {
    Translator translator;
    Node* node = CALL(ID("x"), std::vector<Node*>({NUM(7), BIN(OpType::SUB, NUM(3), ID("y"))}));
    node->accept(translator);
    Code expected_code = {I_PUSHI(7), I_PUSHI(3), I_GET("y"), I_BIN(OpType::SUB), I_GET("x"), I_CALL};
    EXPECT_EQ(translator.code, expected_code)
                        << "GOT:\n----\n" << translator.code << "----\nEXPECTED:\n----\n" << expected_code << "----\n";
}

TEST(translator_test, test_member_get) {
    Translator translator;
    Node* node = MEM(CALL(ID("x"), VectorOfNodes({NUM(7), ID("y")})), "w");
    node->accept(translator);
    Code expected_code = {I_PUSHI(7), I_GET("y"), I_GET("x"), I_CALL, I_GETM("w")};
    EXPECT_EQ(translator.code, expected_code)
                        << "GOT:\n----\n" << translator.code << "----\nEXPECTED:\n----\n" << expected_code << "----\n";
}

TEST(translator_test, test_member_get_rvalue) {
    Translator translator;
    Node* node = DECL("z", nullptr, MEM(CALL(ID("x"), VectorOfNodes({NUM(7), ID("y")})), "w"));
    node->accept(translator);
    Code expected_code = {I_PUSHI(7), I_GET("y"), I_GET("x"), I_CALL, I_GETM("w"), I_DECL("z"), I_SET("z")};
    EXPECT_EQ(translator.code, expected_code)
                        << "GOT:\n----\n" << translator.code << "----\nEXPECTED:\n----\n" << expected_code << "----\n";
}

TEST(translator_test, test_member_set_lvalue) {
    Translator translator;
    Node* node = ASN(MEM(CALL(ID("x"), VectorOfNodes({NUM(7), ID("y")})), "w"), NUM(14));
    node->accept(translator);
    Code expected_code = {I_PUSHI(14), I_PUSHI(7), I_GET("y"), I_GET("x"), I_CALL, I_SETM("w")};
    EXPECT_EQ(translator.code, expected_code)
                        << "GOT:\n----\n" << translator.code << "----\nEXPECTED:\n----\n" << expected_code << "----\n";
}

TEST(translator_test, test_id_set) {
    Translator translator;
    Node* node = ASN(ID("x"), NUM(14));
    node->accept(translator);
    Code expected_code = {I_PUSHI(14), I_SET("x")};
    EXPECT_EQ(translator.code, expected_code)
                        << "GOT:\n----\n" << translator.code << "----\nEXPECTED:\n----\n" << expected_code << "----\n";
}

TEST(translator_test, test_id_get) {
    Translator translator;
    Node* node = ASN(ID("y"), BIN(OpType::ADD, ID("x"), NUM(7)));
    node->accept(translator);
    Code expected_code = {I_GET("x"), I_PUSHI(7), I_BIN(OpType::ADD), I_SET("y")};
    EXPECT_EQ(translator.code, expected_code)
                        << "GOT:\n----\n" << translator.code << "----\nEXPECTED:\n----\n" << expected_code << "----\n";
}