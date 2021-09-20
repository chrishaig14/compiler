#include "catch.hpp"
#include "../src/scanner/Scanner.h"
#include "../src/parser/Parser.h"
#include "../src/ast/expressions/include/UnaryOp.h"
#include "../src/ast/expressions/include/ExpNode.h"
#include "../src/ast/expressions/include/CallExp.h"
#include "../src/ast/top/Typeclass.h"
#include "../src/ast/top/Instance.h"

const TextPosition DUMMY_POS = {0, 0};

struct TestNode {
    std::string text;
    ast::Statement* node;
};

struct TestExpNode {
    std::string text;
    ast::ExpNode* node;
};


struct TestNodeU {
    std::string text;
    ast::UNode node;
};

struct TestExpNodeU {
    std::string text;
    ast::UExpNode node;
};

struct TestTopNodeU {
    std::string text;
    std::unique_ptr<ast::TopNode> node;
};

struct TestTypeNode {
    std::string text;
    ast::Type* node;
};

const std::string& ID = "baz";
const std::string& ID_1 = "foo";
const std::string& ID_2 = "bar";

const TestTypeNode TYPE{"Integer", new ast::ObjectType("Integer")};
const TestTypeNode TYPE_1{"String", new ast::ObjectType("String")};
const TestTypeNode TYPE_2{"Integer", new ast::ObjectType("Integer")};
const TestTypeNode TYPE_3{"Boolean", new ast::ObjectType("Boolean")};

const TestExpNode EXP_ID_1{"foo", new ast::Id("foo", DUMMY_POS, DUMMY_POS)};
const TestExpNode EXP_ID_2{"bar", new ast::Id("bar", DUMMY_POS, DUMMY_POS)};

TestExpNodeU EXP_ID_1_U() {
    return {"foo", ast::Id::make("foo", DUMMY_POS, DUMMY_POS)};
}

TestExpNodeU EXP_ID_2_U() {
    return {"bar", ast::Id::make("bar", DUMMY_POS, DUMMY_POS)};
}

// const TestNode EXPRESSION{"x", new IdNode("x", DUMMY_POS, DUMMY_POS)};
// const TestNode FACTOR_EXPRESSION = EXPRESSION;

TestExpNodeU EXPRESSION_U() {
    return {"x", ast::Id::make("x", DUMMY_POS, DUMMY_POS)};
}

TestExpNodeU FACTOR_U() {
    return EXPRESSION_U();
}


// const TestNode DECLARATION{"var " + ID + " = " + EXPRESSION.text,
//                            new ast::DeclarationNode(ID, nullptr, EXPRESSION.node, DUMMY_POS, DUMMY_POS, DUMMY_POS)};
//

TestNodeU DECLARATION_U() {
    auto EXPRESSION = EXPRESSION_U();
    return {"var " + ID + " = " + EXPRESSION.text, std::make_unique<ast::Declaration>(ID,
                                                                                      nullptr,
                                                                                      std::move(EXPRESSION.node),
                                                                                      DUMMY_POS,
                                                                                      DUMMY_POS,
                                                                                      DUMMY_POS)};
}


TestExpNodeU EXPRESSION_1_U() {
    auto exp_id_1_u = EXP_ID_1_U();
    auto exp_id_2_u = EXP_ID_2_U();
    return {EXP_ID_1.text + "+" + EXP_ID_2.text, ast::BinaryOp::make(OpType::ADD,
                                                                     std::move(exp_id_1_u.node),
                                                                     std::move(exp_id_2_u.node),
                                                                     DUMMY_POS,
                                                                     DUMMY_POS)};
}

TestExpNodeU EXPRESSION_2_U() {
    auto exp_id_2_u = EXP_ID_2_U();
    auto exp_id_1_u = EXP_ID_1_U();
    return {exp_id_2_u.text + "*" + exp_id_1_u.text, ast::BinaryOp::make(OpType::MUL,
                                                                         std::move(exp_id_2_u.node),
                                                                         std::move(exp_id_1_u.node),
                                                                         DUMMY_POS,
                                                                         DUMMY_POS)};
}

// const TestNode ASSIGNMENT{EXP_ID_1.text + " = " + EXPRESSION_1_U.text,
//                           new AssignmentNode(EXP_ID_1_U.node, EXPRESSION_1_U.node, DUMMY_POS, DUMMY_POS)};

TestNodeU ASSIGNMENT() {
    auto expression_1_U = EXPRESSION_1_U();
    auto exp_id_1_U = EXP_ID_1_U();
    return {EXP_ID_1.text + " = " + expression_1_U.text, std::make_unique<ast::Assignment>(std::move(exp_id_1_U.node),
                                                                                           std::move(expression_1_U.node),
                                                                                           DUMMY_POS,
                                                                                           DUMMY_POS)};
}

// const TestNode EMPTY_BLOCK{"{}", new BlockNode(ast::VectorOfNodesU{}, DUMMY_POS, DUMMY_POS)};

TestNodeU EMPTY_BLOCK_U() {
    return {"{}", ast::Block::make(ast::VectorOfNodesU{}, DUMMY_POS, DUMMY_POS)};
}

// const TestNode IF{"if(" + EXPRESSION.text + ")" + EMPTY_BLOCK.text,
//                   new IfNode(EXPRESSION.node, (BlockNode*) EMPTY_BLOCK.node, {}, nullptr, DUMMY_POS, DUMMY_POS)};
//
TestNodeU IF_U() {
    auto EXPRESSION = EXPRESSION_U();
    auto EMPTY_BLOCK = EMPTY_BLOCK_U();
    std::unique_ptr<ast::Block> u(nullptr);
    return {"if(" + EXPRESSION.text + ")" + EMPTY_BLOCK.text, std::make_unique<ast::If>(std::move(EXPRESSION.node),
                                                                                        std::move((ast::UBlock&) EMPTY_BLOCK.node),
                                                                                        std::vector<std::pair<ast::UExpNode, ast::UBlock>>{},
                                                                                        std::move((ast::UBlock&) u),
                                                                                        DUMMY_POS,
                                                                                        DUMMY_POS)};
}


TestNodeU BLOCK_U() {

    auto DECLARATION = DECLARATION_U();
    auto t = DECLARATION.text;
    auto IF = IF_U();
    auto ti = IF.text;
    ast::VectorOfNodesU v;
    v.push_back(std::move(DECLARATION.node));
    v.push_back(std::move(IF.node));
    return {"{" + t + ";" + IF.text + "}", ast::Block::make(std::move(v), DUMMY_POS, DUMMY_POS)};
}

TestNodeU BLOCK_1() {
    auto assignment = ASSIGNMENT();
    ast::VectorOfNodesU v;
    v.push_back(std::move(assignment.node));
    return {"{" + assignment.text + ";}", ast::Block::make(std::move(v), DUMMY_POS, DUMMY_POS)};
}

// const TestNode block_1{"{" + assignment.text + ";}",
//                        new BlockNode(ast::VectorOfNodesU{assignment.node}, DUMMY_POS, DUMMY_POS)};

TestTopNodeU FUNCTION() {
    auto block = BLOCK_U();
    ast::VectorOfUTypes vt;
    vt.push_back(ast::UTypeNode(TYPE_1.node->clone()));
    vt.push_back(ast::UTypeNode(TYPE_2.node->clone()));
    ast::UTypeNode u(TYPE_3.node->clone());
    return {"fun " + ID + "(" + ID_1 + ":" + TYPE_1.text + "," + ID_2 + ":" + TYPE_2.text + ")->" + TYPE_3.text +
            block.text, std::make_unique<ast::Function>(ID,
                                                        VectorOfStrings{ID_1, ID_2},
                                                        vt,
                                                        u,
                                                        (std::unique_ptr<ast::Block>&) block.node,
                                                        DUMMY_POS,
                                                        DUMMY_POS)};
}

const ast::ObjectType NO_TYPE(".None");

TEST_CASE("parse_assignment", "[parser]") {
    Scanner scanner;
    auto assignment = ASSIGNMENT();
    std::string code = assignment.text;
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);

    ast::UNode ast = parser.parse_assignment_or_call();

    REQUIRE(ast->to_json() == assignment.node->to_json());
}

TEST_CASE("parse_decl_simple", "[parser]") {
    Scanner scanner;
    auto DECLARATION = DECLARATION_U();
    std::string code = DECLARATION.text;
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);

    std::unique_ptr<ast::Declaration> ast = parser.parse_variable_declaration();

    REQUIRE(ast->to_json() == DECLARATION.node->to_json());
}
//
// TEST_CASE("parse_error", "[parser]") {
//     Scanner scanner;
//     std::string code = "fun foo()->Integer{return1 0;}";
//     scanner.load_text(code);
//     std::vector<Token> tokens = scanner.scan_all();
//     Parser parser("test", scanner.code_lines, tokens);
//
//     std::unique_ptr<ast::Function> ast = parser.parse_function_definition();
// }


TEST_CASE("parse_decl_with_type", "[parser]") {
    Scanner scanner;
    auto EXPRESSION = EXPRESSION_U();
    std::string code = "var " + ID + " : " + TYPE.text + " = " + EXPRESSION.text;
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);

    std::unique_ptr<ast::Declaration> ast = parser.parse_variable_declaration();

    REQUIRE(ast->identifier == ID);
    REQUIRE(ast->type->to_json() == TYPE.node->to_json());
    REQUIRE(ast->expression.to_json() == EXPRESSION.node->to_json());
}

TEST_CASE("parse_if", "[parser]") {
    Scanner scanner;
    auto IF = IF_U();
    std::string code = IF.text;
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);

    std::unique_ptr<ast::If> ast = parser.parse_if();
    REQUIRE(ast->to_json() == IF.node->to_json());
}

TEST_CASE("parse_if_with_else", "[parser]") {
    Scanner scanner;
    auto block_1 = BLOCK_1();
    auto block = BLOCK_U();
    auto EXPRESSION = EXPRESSION_U();
    std::string code = "if " + EXPRESSION.text + block.text + "else " + block_1.text;
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);

    std::unique_ptr<ast::If> ast = parser.parse_if();
    std::vector<std::pair<ast::UExpNode, ast::UBlock>> v;
    REQUIRE(ast->to_json() == ast::If(std::move(EXPRESSION.node),
                                      std::move((ast::UBlock&) block.node),
                                      std::vector<std::pair<ast::UExpNode, ast::UBlock>>{},
                                      std::move((ast::UBlock&) block_1.node),
                                      DUMMY_POS,
                                      DUMMY_POS).to_json());
}

TEST_CASE("parse_call_no_args", "[parser]") {
    Scanner scanner;
    auto FACTOR_EXPRESSION = FACTOR_U();
    std::string code = FACTOR_EXPRESSION.text + "()";
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);

    ast::UExpNode ast = parser.parse_expression();
    ast::VectorOfExpNodesU v;
    REQUIRE(ast->to_json() ==
            ast::CallExp(std::move(FACTOR_EXPRESSION.node), std::move(v), DUMMY_POS, DUMMY_POS).to_json());
}

TEST_CASE("parse_call_one_arg", "[parser]") {
    Scanner scanner;
    auto EXPRESSION_1 = EXPRESSION_1_U();
    auto FACTOR_EXPRESSION = FACTOR_U();
    std::string code = FACTOR_EXPRESSION.text + "(" + EXPRESSION_1.text + ")";
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);

    ast::UExpNode ast = parser.parse_expression();
    ast::VectorOfExpNodesU v;
    v.push_back(std::move(EXPRESSION_1.node));
    REQUIRE(ast->to_json() ==
            ast::CallExp(std::move(FACTOR_EXPRESSION.node), std::move(v), DUMMY_POS, DUMMY_POS).to_json());
}

TEST_CASE("parse_call_mult_arg", "[parser]") {
    Scanner scanner;
    auto EXPRESSION_1 = EXPRESSION_1_U();
    auto EXPRESSION_2 = EXPRESSION_2_U();
    auto FACTOR_EXPRESSION = FACTOR_U();
    std::string code = FACTOR_EXPRESSION.text + "(" + EXPRESSION_1.text + "," + EXPRESSION_2.text + ")";
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);

    ast::UExpNode ast = parser.parse_expression();
    ast::VectorOfExpNodesU v;
    v.push_back(std::move(EXPRESSION_1.node));
    v.push_back(std::move(EXPRESSION_2.node));
    REQUIRE(ast->to_json() ==
            ast::Call(std::move(FACTOR_EXPRESSION.node), std::move(v), DUMMY_POS, DUMMY_POS).to_json());
}

TEST_CASE("parse_for", "[parser]") {
    Scanner scanner;
    auto BLOCK = BLOCK_U();
    // auto expression_1_u = EXPRESSION_1_U();
    auto EXPRESSION_1 = EXPRESSION_1_U();

    std::string code = "for " + ID + " @ " + EXPRESSION_1.text + BLOCK.text;
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);

    std::unique_ptr<ast::For> ast = parser.parse_for_loop();
    REQUIRE(ast->to_json() == ast::For(ID,
                                       std::move(EXPRESSION_1.node),
                                       std::move((ast::UBlock&) BLOCK.node),
                                       DUMMY_POS,
                                       DUMMY_POS).to_json());
}


TEST_CASE("parse_while", "[parser]") {
    Scanner scanner;
    auto BLOCK = BLOCK_U();
    auto EXPRESSION_1 = EXPRESSION_1_U();

    std::string code = "while " + EXPRESSION_1.text + BLOCK.text;
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);

    std::unique_ptr<ast::While> ast = parser.parse_while_loop();

    REQUIRE(ast->to_json() ==
            ast::While(EXPRESSION_1.node, (std::unique_ptr<ast::Block>&) BLOCK.node, DUMMY_POS, DUMMY_POS).to_json());
}

TEST_CASE("parse_fun_simple", "[parser]") {
    Scanner scanner;
    auto BLOCK = BLOCK_U();
    std::string code = "fun " + ID + "()" + BLOCK.text;
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);

    std::unique_ptr<ast::Function> ast = parser.parse_function_definition();

    ast::VectorOfUTypes vt;
    ast::UTypeNode u(NO_TYPE.clone());
    REQUIRE(ast->to_json() == ast::Function(ID,
                                            VectorOfStrings{},
                                            vt,
                                            u,
                                            (std::unique_ptr<ast::Block>&) BLOCK.node,
                                            DUMMY_POS,
                                            DUMMY_POS).to_json());
}

TEST_CASE("parse_fun_one_arg", "[parser]") {
    Scanner scanner;
    auto BLOCK = BLOCK_U();
    std::string code = "fun " + ID + "(" + ID_1 + ":" + TYPE.text + ")" + BLOCK.text;
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);

    std::unique_ptr<ast::Function> ast = parser.parse_function_definition();

    ast::VectorOfUTypes vt;
    vt.push_back(ast::UTypeNode(TYPE.node->clone()));
    ast::UTypeNode u(NO_TYPE.clone());
    REQUIRE(ast->to_json() == ast::Function(ID, {ID_1}, vt, u, (std::unique_ptr<ast::Block>&) BLOCK.node,

                                            DUMMY_POS, DUMMY_POS).to_json());
}

TEST_CASE("parse_fun_mult_arg", "[parser]") {
    Scanner scanner;
    auto BLOCK = BLOCK_U();
    std::string code = "fun " + ID + "(" + ID_1 + ":" + TYPE_1.text + "," + ID_2 + ":" + TYPE_2.text + ")" + BLOCK.text;
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);

    std::unique_ptr<ast::Function> ast = parser.parse_function_definition();

    ast::VectorOfUTypes vt;
    vt.push_back(ast::UTypeNode(TYPE_1.node->clone()));
    vt.push_back(ast::UTypeNode(TYPE_2.node->clone()));
    ast::UTypeNode u(NO_TYPE.clone());
    REQUIRE(ast->to_json() == ast::Function(ID,
                                            {ID_1, ID_2},
                                            vt,
                                            u,
                                            (std::unique_ptr<ast::Block>&) BLOCK.node,
                                            DUMMY_POS,
                                            DUMMY_POS).to_json());
}

TEST_CASE("parse_class_empty", "[parser]") {
    Scanner scanner;
    std::string code = "class " + ID + "{}";
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);

    std::unique_ptr<ast::Klass> ast = parser.parse_class_definition();

    std::unordered_map<std::string, ast::UFunctionNode> v;
    REQUIRE(ast->to_json() == ast::Klass(ID, {}, {}, {}, {}, v, DUMMY_POS, DUMMY_POS).to_json());
}

TEST_CASE("parse_class_one_member", "[parser]") {
    Scanner scanner;
    std::string code = "class " + ID + "{" + ID_1 + ":" + TYPE_1.text + ";}";
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);

    std::unique_ptr<ast::Klass> ast = parser.parse_class_definition();
    std::unordered_map<std::string, ast::UFunctionNode> v;
    std::vector<std::pair<std::string, ast::UTypeNode >> members;
    members.emplace_back(ID_1, ast::UTypeNode(TYPE_1.node->clone()));
    REQUIRE(ast->to_json() == ast::Klass(ID, {}, std::move(members), {}, {}, v, DUMMY_POS, DUMMY_POS).to_json());
}

TEST_CASE("parse_class_mult_member", "[parser]") {
    Scanner scanner;
    std::string code = "class " + ID + "{" + ID_2 + ":" + TYPE_2.text + ";" + ID_1 + ":" + TYPE_1.text + ";}";
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);

    std::unique_ptr<ast::Klass> ast = parser.parse_class_definition();
    std::unordered_map<std::string, ast::UFunctionNode> v;
    std::vector<std::pair<std::string, ast::UTypeNode >> members;
    members.emplace_back(ID_2, ast::UTypeNode(TYPE_2.node->clone()));
    members.emplace_back(ID_1, ast::UTypeNode(TYPE_1.node->clone()));
    REQUIRE(ast->to_json() == ast::Klass(ID, {}, std::move(members), {}, {}, v, DUMMY_POS, DUMMY_POS).to_json());
}

TEST_CASE("parse_class_with_method", "[parser]") {
    Scanner scanner;
    auto function = FUNCTION();
    std::string code =
            "class " + ID + "{" + ID_2 + ":" + TYPE_2.text + ";" + ID_1 + ":" + TYPE_1.text + ";" + function.text + "}";
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);

    std::unique_ptr<ast::Klass> ast = parser.parse_class_definition();

    ast::UFunctionNode fp = std::move((ast::UFunctionNode&) function.node);
    std::unordered_map<std::string, ast::UFunctionNode> v;
    std::vector<std::pair<std::string, ast::UTypeNode >> members;
    members.emplace_back(ID_2, ast::UTypeNode(TYPE_2.node->clone()));
    members.emplace_back(ID_1, ast::UTypeNode(TYPE_1.node->clone()));
    //     std::unordered_map<std::string, std::unique_ptr<KMethod>> methods;
    //     methods[fp->identifier] = std::make_unique<KMethod>(nullptr, std::move(fp));
    //     REQUIRE(ast->to_json() ==
    //             ast::Klass(ID, {}, std::move(members), std::move(methods), {}, v, DUMMY_POS, DUMMY_POS).to_json());
    // }
}

TEST_CASE("parse_class_with_static_method", "[parser]") {
    Scanner scanner;
    auto function = FUNCTION();

    std::string code = "class " + ID + "{ static " + function.text + "}";
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);

    std::unique_ptr<ast::Klass> ast = parser.parse_class_definition();
    std::unordered_map<std::string, ast::UFunctionNode> v;
    std::string id = ((ast::UFunctionNode&) function.node)->identifier;
    v[id] = std::move((ast::UFunctionNode&) function.node);
    REQUIRE(ast->to_json() == ast::Klass(ID, {}, {}, {}, {}, v, DUMMY_POS, DUMMY_POS).to_json());
}

TEST_CASE("parse_return_nothing", "[parser]") {
    Scanner scanner;
    std::string code = "return;";
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);

    std::unique_ptr<ast::Return> ast = parser.parse_return();
    ast::UExpNode ptr;
    REQUIRE(ast->to_json() == ast::Return(ptr, DUMMY_POS, DUMMY_POS).to_json());
}

TEST_CASE("parse_return_expression", "[parser]") {
    Scanner scanner;
    auto EXPRESSION = EXPRESSION_U();
    std::string code = "return " + EXPRESSION.text;
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);

    std::unique_ptr<ast::Return> ast = parser.parse_return();

    REQUIRE(ast->to_json() == ast::Return(EXPRESSION.node, DUMMY_POS, DUMMY_POS).to_json());
}

TEST_CASE("parse_list_empty", "[parser]") {
    Scanner scanner;
    std::string code = "[]::" + TYPE.text;
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);

    ast::UExpNode ast = parser.parse_list_literal();

    REQUIRE(ast->to_json() == ast::EmptyList(ast::UTypeNode(TYPE.node->clone()), DUMMY_POS, DUMMY_POS).to_json());
}

TEST_CASE("parse_list_one_element", "[parser]") {
    Scanner scanner;
    auto EXPRESSION_1 = EXPRESSION_1_U();

    std::string code = "[" + EXPRESSION_1.text + "]";
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);

    ast::UExpNode ast = parser.parse_list_literal();
    ast::VectorOfExpNodesU v;
    v.push_back(std::move(EXPRESSION_1.node));
    REQUIRE(ast->to_json() == ast::List(std::move(v), DUMMY_POS, DUMMY_POS).to_json());
}

TEST_CASE("parse_list_mult_elements", "[parser]") {
    Scanner scanner;
    auto EXPRESSION_1 = EXPRESSION_1_U();
    auto EXPRESSION_2 = EXPRESSION_2_U();
    std::string code = "[" + EXPRESSION_1.text + "," + EXPRESSION_2.text + "]";
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);

    ast::UExpNode ast = parser.parse_list_literal();
    ast::VectorOfExpNodesU v;
    v.push_back(std::move(EXPRESSION_1.node));
    v.push_back(std::move(EXPRESSION_2.node));
    REQUIRE(ast->to_json() == ast::List(std::move(v), DUMMY_POS, DUMMY_POS).to_json());
}

TEST_CASE("parse_number_integer", "[parser]") {
    Scanner scanner;
    std::string code = "89";
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);

    ast::UExpNode ast = parser.parse_id_or_literal();

    REQUIRE(ast->to_json() == ast::Number(NumberType::INTEGER, "89", DUMMY_POS, DUMMY_POS).to_json());
}

TEST_CASE("parse_number_float", "[parser]") {
    Scanner scanner;
    std::string code = "3.14";
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);

    ast::UExpNode ast = parser.parse_id_or_literal();

    REQUIRE(ast->to_json() == ast::Number(NumberType::FLOAT, "3.14", DUMMY_POS, DUMMY_POS).to_json());
}

TEST_CASE("parse_empty_string", "[parser]") {
    Scanner scanner;
    std::string code = "\"\"";
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);

    ast::UExpNode ast = parser.parse_id_or_literal();

    REQUIRE(ast->to_json() == ast::String("", DUMMY_POS, DUMMY_POS).to_json());
}

TEST_CASE("parse_string", "[parser]") {
    Scanner scanner;
    std::string code = "\"hello, world\"";
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);

    ast::UExpNode ast = parser.parse_id_or_literal();

    REQUIRE(ast->to_json() == ast::String("hello, world", DUMMY_POS, DUMMY_POS).to_json());
}

TEST_CASE("parse_none", "[parser]") {
    Scanner scanner;
    std::string code = "none";
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);

    ast::UExpNode ast = parser.parse_id_or_literal();

    REQUIRE(ast->to_json() == ast::None(DUMMY_POS, DUMMY_POS).to_json());
}

TEST_CASE("parse_true", "[parser]") {
    Scanner scanner;
    std::string code = "true";
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);

    ast::UExpNode ast = parser.parse_id_or_literal();

    REQUIRE(ast->to_json() == ast::Boolean(true, DUMMY_POS, DUMMY_POS).to_json());
}

TEST_CASE("parse_false", "[parser]") {
    Scanner scanner;
    std::string code = "false";
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);

    ast::UExpNode ast = parser.parse_id_or_literal();

    REQUIRE(ast->to_json() == ast::Boolean(false, DUMMY_POS, DUMMY_POS).to_json());
}

TEST_CASE("parse_and_exp", "[parser]") {
    Scanner scanner;
    auto EXPRESSION_1 = EXPRESSION_1_U();
    auto EXPRESSION_2 = EXPRESSION_2_U();
    std::string code = EXPRESSION_1.text + " and " + EXPRESSION_2.text;
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);

    ast::UExpNode ast = parser.parse_and_expression();

    REQUIRE(ast->to_json() == ast::BinaryOp(OpType::AND,
                                            std::move(EXPRESSION_1.node),
                                            std::move(EXPRESSION_2.node),
                                            DUMMY_POS,
                                            DUMMY_POS).to_json());
}

TEST_CASE("parse_or_exp", "[parser]") {
    Scanner scanner;
    auto EXPRESSION_1 = EXPRESSION_1_U();
    auto EXPRESSION_2 = EXPRESSION_2_U();
    std::string code = EXPRESSION_1.text + " or " + EXPRESSION_2.text;
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);

    ast::UExpNode ast = parser.parse_or_expression();

    REQUIRE(ast->to_json() == ast::BinaryOp(OpType::OR,
                                            std::move(EXPRESSION_1.node),
                                            std::move(EXPRESSION_2.node),
                                            DUMMY_POS,
                                            DUMMY_POS).to_json());
}

TEST_CASE("parse_eq_exp", "[parser]") {
    Scanner scanner;
    auto EXPRESSION_1 = EXPRESSION_1_U();
    auto EXPRESSION_2 = EXPRESSION_2_U();
    std::string code = EXPRESSION_1.text + " == " + EXPRESSION_2.text;
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);

    ast::UExpNode ast = parser.parse_and_expression();

    REQUIRE(ast->to_json() == ast::BinaryOp(OpType::EQ,
                                            std::move(EXPRESSION_1.node),
                                            std::move(EXPRESSION_2.node),
                                            DUMMY_POS,
                                            DUMMY_POS).to_json());
}

TEST_CASE("parse_ge_exp", "[parser]") {
    Scanner scanner;
    auto EXPRESSION_1 = EXPRESSION_1_U();
    auto EXPRESSION_2 = EXPRESSION_2_U();
    std::string code = EXPRESSION_1.text + " >= " + EXPRESSION_2.text;
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);

    ast::UExpNode ast = parser.parse_and_expression();

    REQUIRE(ast->to_json() == ast::BinaryOp(OpType::GE,
                                            std::move(EXPRESSION_1.node),
                                            std::move(EXPRESSION_2.node),
                                            DUMMY_POS,
                                            DUMMY_POS).to_json());
}

TEST_CASE("parse_le_exp", "[parser]") {
    Scanner scanner;
    auto EXPRESSION_1 = EXPRESSION_1_U();
    auto EXPRESSION_2 = EXPRESSION_2_U();
    std::string code = EXPRESSION_1.text + " <= " + EXPRESSION_2.text;
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);

    ast::UExpNode ast = parser.parse_and_expression();

    REQUIRE(ast->to_json() == ast::BinaryOp(OpType::LE,
                                            std::move(EXPRESSION_1.node),
                                            std::move(EXPRESSION_2.node),
                                            DUMMY_POS,
                                            DUMMY_POS).to_json());
}

TEST_CASE("parse_gt_exp", "[parser]") {
    Scanner scanner;
    auto EXPRESSION_1 = EXPRESSION_1_U();
    auto EXPRESSION_2 = EXPRESSION_2_U();
    std::string code = EXPRESSION_1.text + " > " + EXPRESSION_2.text;
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);

    ast::UExpNode ast = parser.parse_and_expression();

    REQUIRE(ast->to_json() == ast::BinaryOp(OpType::GT,
                                            std::move(EXPRESSION_1.node),
                                            std::move(EXPRESSION_2.node),
                                            DUMMY_POS,
                                            DUMMY_POS).to_json());
}

TEST_CASE("parse_lt_exp", "[parser]") {
    Scanner scanner;
    auto EXPRESSION_1 = EXPRESSION_1_U();
    auto EXPRESSION_2 = EXPRESSION_2_U();
    std::string code = EXPRESSION_1.text + " < " + EXPRESSION_2.text;
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);

    ast::UExpNode ast = parser.parse_and_expression();

    REQUIRE(ast->to_json() == ast::BinaryOp(OpType::LT,
                                            std::move(EXPRESSION_1.node),
                                            std::move(EXPRESSION_2.node),
                                            DUMMY_POS,
                                            DUMMY_POS).to_json());
}

TEST_CASE("parse_ne_exp", "[parser]") {
    Scanner scanner;
    auto EXPRESSION_1 = EXPRESSION_1_U();
    auto EXPRESSION_2 = EXPRESSION_2_U();
    std::string code = EXPRESSION_1.text + " != " + EXPRESSION_2.text;
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);

    ast::UExpNode ast = parser.parse_and_expression();

    REQUIRE(ast->to_json() == ast::BinaryOp(OpType::NE,
                                            std::move(EXPRESSION_1.node),
                                            std::move(EXPRESSION_2.node),
                                            DUMMY_POS,
                                            DUMMY_POS).to_json());
}

TEST_CASE("parse_not_exp", "[parser]") {
    Scanner scanner;
    auto EXPRESSION = EXPRESSION_U();
    std::string code = "not " + EXPRESSION.text;
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);

    ast::UExpNode ast = parser.parse_and_expression();

    REQUIRE(ast->to_json() ==
            ast::UnaryOp(UnaryOpType::NOT, std::move(EXPRESSION.node), DUMMY_POS, DUMMY_POS).to_json());
}

TEST_CASE("parse_tuple", "[parser]") {
    Scanner scanner;
    auto EXPRESSION_1 = EXPRESSION_1_U();
    auto EXPRESSION_2 = EXPRESSION_2_U();
    std::string code = "#(" + EXPRESSION_1.text + "," + EXPRESSION_2.text + ")";
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);

    ast::UExpNode ast = parser.parse_tuple_or_constructor();
    // auto EXPRESSION_1 = EXPRESSION_1_U();
    // auto EXPRESSION_2 = EXPRESSION_2_U();
    ast::VectorOfExpNodesU args;
    args.emplace_back(std::move(EXPRESSION_1.node));
    args.emplace_back(std::move(EXPRESSION_2.node));
    REQUIRE(ast->to_json() == ast::Tuple(std::move(args), DUMMY_POS, DUMMY_POS).to_json());
}

TEST_CASE("parse_dict_empty", "[parser]") {
    Scanner scanner;
    std::string code = "{}::[" + TYPE_1.text + "," + TYPE_2.text + "]";
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);

    ast::UExpNode ast = parser.parse_dictionary();
    ast::UTypeNode u1(TYPE_1.node->clone());
    ast::UTypeNode u2(TYPE_2.node->clone());
    REQUIRE(ast->to_json() == ast::EmptyDict(u1, u2, DUMMY_POS, DUMMY_POS).to_json());
}

TEST_CASE("parse_dict_one_element", "[parser]") {
    Scanner scanner;
    auto EXPRESSION_1 = EXPRESSION_1_U();
    auto EXPRESSION_2 = EXPRESSION_2_U();
    std::string code = "{" + EXPRESSION_1.text + ":" + EXPRESSION_2.text + "}";
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);

    ast::UExpNode ast = parser.parse_dictionary();
    std::vector<std::pair<ast::UExpNode, ast::UExpNode>> d;
    d.emplace_back(std::move(EXPRESSION_1.node), std::move(EXPRESSION_2.node));
    REQUIRE(ast->to_json() == ast::DictNode(std::move(d), DUMMY_POS, DUMMY_POS).to_json());
}

TEST_CASE("parse_dict_mult_elements", "[parser]") {
    Scanner scanner;
    auto EXPRESSION_1 = EXPRESSION_1_U();
    auto EXPRESSION_1_V = EXPRESSION_1_U();
    auto EXPRESSION_2 = EXPRESSION_2_U();
    auto EXPRESSION = EXPRESSION_U();
    std::string code =
            "{" + EXPRESSION_1.text + ":" + EXPRESSION_2.text + "," + EXPRESSION.text + ":" + EXPRESSION_1_V.text + "}";
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);

    ast::UExpNode ast = parser.parse_dictionary();

    std::vector<std::pair<ast::UExpNode, ast::UExpNode>> d;
    d.emplace_back(std::move(EXPRESSION_1.node), std::move(EXPRESSION_2.node));
    d.emplace_back(std::move(EXPRESSION.node), std::move(EXPRESSION_1_V.node));
    // {{EXPRESSION_1.node.release(), EXPRESSION_2.node.release()},
    //  {EXPRESSION.node.release(),   EXPRESSION_1_V.node.release()}}
    REQUIRE(ast->to_json() == ast::DictNode(std::move(d), DUMMY_POS, DUMMY_POS).to_json());
}

TEST_CASE("parse_member", "[parser]") {
    Scanner scanner;
    auto EXPRESSION = EXPRESSION_U();
    std::string code = EXPRESSION.text + "." + ID;
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);

    ast::UExpNode ast = parser.parse_factor();

    REQUIRE(ast->to_json() ==
            ast::Member(std::move(EXPRESSION.node), Token(TokType::ID, ID, DUMMY_POS, DUMMY_POS)).to_json());
}

TEST_CASE("parse_subscript", "[parser]") {
    Scanner scanner;
    auto EXPRESSION_1 = EXPRESSION_1_U();
    auto EXPRESSION_2 = EXPRESSION_2_U();
    auto EXPRESSION = EXPRESSION_U();
    std::string code = EXPRESSION.text + "[" + EXPRESSION_2.text + "]";
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);

    ast::UExpNode ast = parser.parse_factor();
    ast::VectorOfExpNodesU v;
    v.push_back(std::move(EXPRESSION_2.node));
    REQUIRE(ast->to_json() == ast::Subscript(EXPRESSION.node, v, DUMMY_POS, DUMMY_POS).to_json());
}

TEST_CASE("parse_partial_one_arg", "[parser]") {
    Scanner scanner;
    auto EXPRESSION_1 = EXPRESSION_1_U();
    auto EXPRESSION_2 = EXPRESSION_2_U();
    auto FACTOR_EXPRESSION = FACTOR_U();
    std::string code = "$" + FACTOR_EXPRESSION.text + "(" + EXPRESSION_1.text + ")";
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);

    ast::UExpNode ast = parser.parse_partial_application();
    ast::VectorOfExpNodesU args;
    args.emplace_back(std::move(EXPRESSION_1.node));
    REQUIRE(ast->to_json() == ast::PartialApplication(std::move(FACTOR_EXPRESSION.node),
                                                      std::move(args),
                                                      DUMMY_POS,
                                                      DUMMY_POS).to_json());
}

TEST_CASE("parse_partial_mult_arg_one", "[parser]") {
    Scanner scanner;
    auto EXPRESSION_1 = EXPRESSION_1_U();
    auto EXPRESSION_2 = EXPRESSION_2_U();
    auto FACTOR_EXPRESSION = FACTOR_U();
    std::string code = "$" + FACTOR_EXPRESSION.text + "(" + EXPRESSION_1.text + ",*)";
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);

    ast::UExpNode ast = parser.parse_partial_application();
    ast::VectorOfExpNodesU args;
    args.emplace_back(std::move(EXPRESSION_1.node));
    args.emplace_back(nullptr);

    REQUIRE(ast->to_json() == ast::PartialApplication(std::move(FACTOR_EXPRESSION.node),
                                                      std::move(args),
                                                      DUMMY_POS,
                                                      DUMMY_POS).to_json());
}

TEST_CASE("parse_partial_mult_arg_two", "[parser]") {
    Scanner scanner;
    auto EXPRESSION_1 = EXPRESSION_1_U();
    auto EXPRESSION_2 = EXPRESSION_2_U();
    auto FACTOR_EXPRESSION = FACTOR_U();
    std::string code = "$" + FACTOR_EXPRESSION.text + "(" + EXPRESSION_1.text + "," + EXPRESSION_2.text + ")";
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);

    ast::UExpNode ast = parser.parse_partial_application();
    ast::VectorOfExpNodesU args;
    args.emplace_back(std::move(EXPRESSION_1.node));
    args.emplace_back(std::move(EXPRESSION_2.node));

    REQUIRE(ast->to_json() == ast::PartialApplication(std::move(FACTOR_EXPRESSION.node),
                                                      std::move(args),
                                                      DUMMY_POS,
                                                      DUMMY_POS).to_json());
}

TEST_CASE("parse_typeclass", "[parser]") {
    Scanner scanner;
    std::string code = "typeclass Comparable[t] {fun eq(a:t, b:t)->Boolean;fun ne(a: t, b:t)->Boolean;} ";
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);

    std::unique_ptr<ast::Typeclass> ast = parser.parse_typeclass();
    std::unordered_map<std::string, ast::UFunctionType> cmethods;
    cmethods["eq"] = std::make_unique<ast::FunctionType>(ast::VectorOfTypes{new ast::ObjectType("t"),
                                                                            new ast::ObjectType("t")},
                                                         std::make_unique<ast::ObjectType>("Boolean"));
    cmethods["ne"] = std::make_unique<ast::FunctionType>(ast::VectorOfTypes{new ast::ObjectType("t"),
                                                                            new ast::ObjectType("t")},
                                                         std::make_unique<ast::ObjectType>("Boolean"));

    REQUIRE(ast->to_json() == ast::Typeclass("Comparable", "t", std::move(cmethods), DUMMY_POS, DUMMY_POS).to_json());
}

TEST_CASE("parse_instance", "[parser]") {
    Scanner scanner;
    std::string code = "instance Comparable[Foo] {fun eq(a:Foo, b:Foo)->Boolean {};fun ne(a: Foo, b:Foo)->Boolean {};} ";
    scanner.load_text(code);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser("test", scanner.code_lines, tokens);

    std::unique_ptr<ast::Instance> ast = parser.parse_instance();
    std::unique_ptr<ast::Block> b2 = ast::Block::make(ast::VectorOfNodesU{}, DUMMY_POS, DUMMY_POS);
    std::unique_ptr<ast::Block> b1 = ast::Block::make(ast::VectorOfNodesU{}, DUMMY_POS, DUMMY_POS);
    ast::VectorOfUTypes vt1;
    vt1.push_back(std::make_unique<ast::ObjectType>("Foo"));
    vt1.push_back(std::make_unique<ast::ObjectType>("Foo"));

    ast::VectorOfUTypes vt2;
    vt2.push_back(std::make_unique<ast::ObjectType>("Foo"));
    vt2.push_back(std::make_unique<ast::ObjectType>("Foo"));
    ast::UTypeNode r1 = std::make_unique<ast::ObjectType>("Boolean");
    ast::UTypeNode r2 = std::make_unique<ast::ObjectType>("Boolean");
    std::unordered_map<std::string, ast::UFunctionNode> methods;
    methods["eq"] = std::make_unique<ast::Function>("eq", VectorOfStrings{"a", "b"}, vt1, r1, b1, DUMMY_POS, DUMMY_POS);
    methods["ne"] = std::make_unique<ast::Function>("ne", VectorOfStrings{"a", "b"}, vt2, r2, b2, DUMMY_POS, DUMMY_POS);
    REQUIRE(ast->to_json() == ast::Instance("Comparable",
                                            std::make_unique<ast::ObjectType>("Foo"),
                                            std::move(methods),
                                            DUMMY_POS,
                                            DUMMY_POS).to_json());
}