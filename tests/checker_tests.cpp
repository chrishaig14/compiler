#include "catch.hpp"
#include <scanner/Scanner.h>
#include <parser/Parser.h>
#include <ast/expressions/include/UnaryOp.h>
#include <ast/expressions/include/CallExp.h>
#include <semantic/ModulePrechecker.h>
#include <semantic/ModuleChecker.h>
#include <compiler/Compiler.h>
#include <compiler/analyze.h>
#include <simple_nodes/common/include/TypeObject.h>
#include <simple_nodes/common/include/TypeFunction.h>

#include <semantic/errors/include/errors.h>

const ast::ObjectType NO_TYPE(".None");

const TextPosition& _POS = {1, 1};

#define CHECKER() std::unique_ptr<Compiler> cp = analyze(code);Compiler& c = *cp;Module& module = c.root_package.units["tmp"]->module();resolve_module_imports(module, c.top_package);std::map<std::string,std::string> instances;ModuleChecker checker(c.top_package, module,instances);
#define REQUIRE_CHECKER_ONE_ERROR() REQUIRE(not checker.error_reporter.ok());REQUIRE(checker.error_reporter.errors.size() == 1);
#define REQUIRE_CHECKER_OK() REQUIRE(checker.error_reporter.ok());REQUIRE(checker.error_reporter.errors.empty());

std::unique_ptr<Compiler> analyze(std::string code) {
    const std::string& tmp_in = "tmp_in";
    mkdir(tmp_in.c_str(), 0700);
    const std::string& tmp_out = "tmp_out";
    mkdir(tmp_out.c_str(), 0700);
    std::ofstream tmp_req(tmp_in + "/requirements.txt");
    tmp_req << "libcore==1.0.0";
    tmp_req.flush();
    std::ofstream tmp(tmp_in + "/tmp.xl");
    tmp << code;
    tmp.flush();
    std::string project_dir = tmp_in;
    std::string project_output_dir = tmp_out;
    std::string output_name = "test";
    std::string lib_path = "/home/chris/CLionProjects/compiler/lib";
    bool is_lib = false;
    std::string version = "1.0.0";
    auto c = std::make_unique<Compiler>(project_dir, project_output_dir, output_name, lib_path, is_lib, version);
    c->pre();
    return c;
}

TEST_CASE("basic_function", "[checker]") {
    std::string code = "fun foo()->Integer{return 0;}";

    CHECKER();
    checker.visit_function(module.ast->functions[0]);
    REQUIRE(checker.error_reporter.ok());
}

TEST_CASE("basic_function_bad_return_type", "[checker]") {
    std::string code = "fun foo()->Integer{return false;}";

    CHECKER();
    ast::Function& ast_func = (ast::Function&) module.ast->functions[0];
    ast::Return& ast_ret = (ast::Return&) *ast_func.body->nodes[0];
    ast::ExpNode& ast_exp = *ast_ret.expression;
    checker.visit_function(ast_func);

    REQUIRE_CHECKER_ONE_ERROR();
    Error& error = *checker.error_reporter.errors.back();
    ast::Boolean node(false, _POS, _POS);
    sem::TypeObject expected("Integer");
    ErrorTypeMismatch exp(expected, ast_exp, *checker.entity_from_type(ast::ObjectType("Boolean")));
    REQUIRE(error == exp);
}

TEST_CASE("basic_declaration", "[checker]") {
    std::string code = "fun main()->Integer{var x = 9;return 0;}";

    CHECKER();
    checker.visit_declaration((ast::Declaration&) *((std::unique_ptr<ast::Function>&) module.ast->functions[0])->body->nodes[0]);
    REQUIRE(checker.error_reporter.ok());
}

TEST_CASE("basic_declaration_type_ok", "[checker]") {
    std::string code = "fun main()->Integer{var x : Integer = 9;return 0;}";

    CHECKER();
    checker.visit_declaration((ast::Declaration&) *((std::unique_ptr<ast::Function>&) module.ast->functions[0])->body->nodes[0]);
    REQUIRE(checker.error_reporter.ok());
}

TEST_CASE("basic_declaration_bad_type", "[checker]") {
    std::string code = "fun foo()->Integer{var x: Boolean = 9;return 0;}";

    CHECKER();
    ast::Declaration& ast_decl = (ast::Declaration&) *((std::unique_ptr<ast::Function>&) module.ast->functions[0])->body->nodes[0];
    ast::ExpNode& ast_exp = ast_decl.expression;
    checker.visit_declaration(ast_decl);

    REQUIRE_CHECKER_ONE_ERROR();
    Error& error = *checker.error_reporter.errors.back();
    // ast::Number node(NumberType::INTEGER, "9", _POS, _POS);
    sem::TypeObject expected("Boolean");
    ErrorTypeMismatch exp(expected, ast_exp, *checker.entity_from_type(ast::ObjectType("Integer")));
    REQUIRE(error == exp);
}

TEST_CASE("error_redeclared", "[checker]") {
    std::string code = "fun foo()->Integer{var x = 9; var x = 10;return 0;}";

    CHECKER();
    checker.visit_function(module.ast->functions[0]);

    REQUIRE_CHECKER_ONE_ERROR();
    Error& error = *checker.error_reporter.errors.back();
    ast::Number node(NumberType::INTEGER, "9", _POS, _POS);
    ast::ObjectType expected("Boolean");
    ErrorRedeclared exp("x",
                        (ast::Declaration&) *((std::unique_ptr<ast::Function>&) module.ast->functions[0])->body->nodes[1]);
    REQUIRE(error == exp);
}

TEST_CASE("list_ok", "[checker]") {
    std::string code = "fun foo()->Integer{var x = [4,1];return 0;}";

    CHECKER();
    checker.visit_function(module.ast->functions[0]);

    REQUIRE_CHECKER_OK();
}

TEST_CASE("list_bad", "[checker]") {
    std::string code = "fun foo()->Integer{var x = [4,\"a\"];return 0;}";

    CHECKER();
    ast::Function& ast_func = module.ast->functions[0];
    ast::Declaration& ast_decl = (ast::Declaration&) *ast_func.body->nodes[0];
    ast::List& ast_list = (ast::List&) ast_decl.expression;
    checker.visit_function(module.ast->functions[0]);

    REQUIRE_CHECKER_ONE_ERROR();
    Error& error = *checker.error_reporter.errors.back();

    sem::TypeObject expected("Integer");
    ErrorTypeMismatch exp(expected, ast_list.elements[1], *checker.entity_from_type(ast::ObjectType("String")));

    REQUIRE(error == exp);
}

TEST_CASE("empty_dict_ok", "[checker]") {
    std::string code = "fun foo()->Integer{var x = {}::[Integer,String];return 0;}";

    CHECKER();
    checker.visit_function(module.ast->functions[0]);

    REQUIRE_CHECKER_OK();
}

TEST_CASE("dict_ok", "[checker]") {
    std::string code = "fun foo()->Integer{var x = {7:\"seven\",9:\"nine\"};return 0;}";

    CHECKER();
    checker.visit_function(module.ast->functions[0]);

    REQUIRE_CHECKER_OK();
}

TEST_CASE("dict_key_type_error", "[checker]") {
    std::string code = "fun foo()->Integer{var x = {7:\"seven\",\"nine\":\"ten\"};return 0;}";

    CHECKER();
    checker.visit_function(module.ast->functions[0]);

    REQUIRE_CHECKER_ONE_ERROR();
}

TEST_CASE("dict_value_type_error", "[checker]") {
    std::string code = "fun foo()->Integer{var x = {7:\"seven\",9:false};return 0;}";

    CHECKER();
    checker.visit_function(module.ast->functions[0]);

    REQUIRE_CHECKER_ONE_ERROR();
}


TEST_CASE("int_literal", "[checker]") {
    std::string code = "fun foo()->Integer{var x = 9;return 0;}";

    CHECKER();
    ast::ExpNode& expression = ((ast::Declaration&) *(module.ast->functions[0].get().body->nodes[0])).expression;
    UExpressionInfo info = checker.dispatch_rvalue(expression);

    REQUIRE(!not checker.error_reporter.ok());
    REQUIRE(checker.error_reporter.errors.size() == 0);
    REQUIRE(info->entity.get().is_value());
    REQUIRE(info->entity.get().get_value().metatype == Meta::CLASS);
    REQUIRE(info->entity.get().get_value().type == sem::TypeObject("Integer"));
}

TEST_CASE("bool_literal", "[checker]") {
    std::string code = "fun foo()->Integer{var x = false;return 0;}";

    CHECKER();
    ast::ExpNode& expression = ((ast::Declaration&) *(module.ast->functions[0].get().body->nodes[0])).expression;
    UExpressionInfo info = checker.dispatch_rvalue(expression);


    REQUIRE(!not checker.error_reporter.ok());
    REQUIRE(checker.error_reporter.errors.size() == 0);
    REQUIRE(info->entity.get().is_value());
    REQUIRE(info->entity.get().get_value().metatype == Meta::CLASS);
    REQUIRE(info->entity.get().get_value().type == sem::TypeObject("Boolean"));
}

TEST_CASE("list_literal", "[checker]") {
    std::string code = "fun foo()->Integer{var x = [4,1];return 0;}";

    CHECKER();
    ast::ExpNode& expression = ((ast::Declaration&) *(module.ast->functions[0].get().body->nodes[0])).expression;
    UExpressionInfo info = checker.dispatch_rvalue(expression);


    REQUIRE(!not checker.error_reporter.ok());
    REQUIRE(checker.error_reporter.errors.size() == 0);
    REQUIRE(info->entity.get().is_value());
    REQUIRE(info->entity.get().get_value().metatype == Meta::CLASS);
    REQUIRE(info->entity.get().get_value().type == sem::TypeObject("List", {new sem::TypeObject("Integer")}));
}

TEST_CASE("empty_list_literal", "[checker]") {
    std::string code = "fun foo()->Integer{var x = []::String;return 0;}";

    CHECKER();
    ast::ExpNode& expression = ((ast::Declaration&) *(module.ast->functions[0].get().body->nodes[0])).expression;
    UExpressionInfo info = checker.dispatch_rvalue(expression);


    REQUIRE(!not checker.error_reporter.ok());
    REQUIRE(checker.error_reporter.errors.size() == 0);
    REQUIRE(info->entity.get().is_value());
    REQUIRE(info->entity.get().get_value().metatype == Meta::CLASS);
    REQUIRE(info->entity.get().get_value().type == sem::TypeObject("List", {new sem::TypeObject("String")}));
}

TEST_CASE("empty_dict_literal", "[checker]") {
    std::string code = "fun foo()->Integer{var x = {}::[Integer,String];return 0;}";

    CHECKER();
    ast::ExpNode& expression = ((ast::Declaration&) *(module.ast->functions[0].get().body->nodes[0])).expression;
    UExpressionInfo info = checker.dispatch_rvalue(expression);


    REQUIRE(!not checker.error_reporter.ok());
    REQUIRE(checker.error_reporter.errors.size() == 0);
    REQUIRE(info->entity.get().is_value());
    REQUIRE(info->entity.get().get_value().metatype == Meta::CLASS);
    REQUIRE(info->entity.get().get_value().type ==
            sem::TypeObject("Dict", {new sem::TypeObject("Integer"), new sem::TypeObject("String")}));
}


TEST_CASE("dict_literal", "[checker]") {
    std::string code = "fun foo()->Integer{var x = {\"one\":1,\"two\":2};return 0;}";

    CHECKER();
    ast::ExpNode& expression = ((ast::Declaration&) *(module.ast->functions[0].get().body->nodes[0])).expression;
    UExpressionInfo info = checker.dispatch_rvalue(expression);


    REQUIRE(!not checker.error_reporter.ok());
    REQUIRE(checker.error_reporter.errors.size() == 0);
    REQUIRE(info->entity.get().is_value());
    REQUIRE(info->entity.get().get_value().metatype == Meta::CLASS);
    REQUIRE(info->entity.get().get_value().type ==
            sem::TypeObject("Dict", {new sem::TypeObject("String"), new sem::TypeObject("Integer")}));
}

TEST_CASE("float_literal", "[checker]") {
    std::string code = "fun foo()->Integer{var x = 9.5;return 0;}";

    CHECKER();
    ast::ExpNode& expression = ((ast::Declaration&) *(module.ast->functions[0].get().body->nodes[0])).expression;
    UExpressionInfo info = checker.dispatch_rvalue(expression);


    REQUIRE(!not checker.error_reporter.ok());
    REQUIRE(checker.error_reporter.errors.size() == 0);
    REQUIRE(info->entity.get().is_value());
    REQUIRE(info->entity.get().get_value().metatype == Meta::CLASS);
    REQUIRE(info->entity.get().get_value().type == sem::TypeObject("Float"));
}

// TEST_CASE("none_literal", "[checker]") {
//     std::string code = "fun foo()->Integer{var x = none;return 0;}";
//
//     std::unique_ptr<Compiler> cp =  analyze(code);Compiler& c = *cp;
//     Module& module = *c.root_package.units["tmp"].module;
//     analyze_module_result(module, c.top_package);
//     Checker checker(c.top_package,module);
//     ast::Node* expression = ((ast::DeclarationNode*) ((FunctionNode&)* module.ast->functions[0]).body->nodes[0])->expression;
//     UExpressionInfo info = checker.dispatch_rvalue(expression);

//
//     REQUIRE(!not checker.error_reporter.ok());
//     REQUIRE(checker.error_reporter.errors.size() == 0);
//     REQUIRE(info->entity.get().type == E_TYPE::VALUE);
//     REQUIRE(((EntityValue*)(info->entity))->value->metatype == Meta::CLASS);
//     REQUIRE(*((EntityValue*)(info->entity))->value->type == ast::ObjectType("NoneType"));
// }

TEST_CASE("decl_error_expected_expression", "[checker]") {
    std::string code = "fun foo()->Integer{var x = Integer;return 0;}";

    CHECKER();
    checker.init();
    ast::Function& function_node = module.ast->functions[0];
    ast::Declaration& declaration_node = (ast::Declaration&) *function_node.body->nodes[0];
    checker.visit_function(function_node);

    REQUIRE_CHECKER_ONE_ERROR();

    Error& error = *checker.error_reporter.errors.back();
    ConcreteClass* cl = &module.get(Path("Integer"))->klass();
    EntityClass ec(*cl);
    ErrorExpectedExpression exp(ec, declaration_node.expression);
    REQUIRE(error == exp);
}

TEST_CASE("error_no_member", "[checker]") {
    std::string code = "class Foo{foo: Integer;}\nfun bar(f: Foo)->Integer{var x = f.lala;return 0;}";

    auto cp = analyze(code);
    Compiler& c = *cp;
    Module& module = c.root_package.units["tmp"]->module();
    resolve_module_imports(module, c.top_package);
    std::map<std::string,std::string> instances;
    ModuleChecker checker(c.top_package, module, instances);
    std::cout << "Starting checker" << std::endl;
    checker.init();
    ast::Function& function_node = module.ast->functions[0];
    ast::Declaration& declaration_node = (ast::Declaration&) *function_node.body->nodes[0];
    checker.check_module();

    REQUIRE_CHECKER_ONE_ERROR();

    Error& error = *checker.error_reporter.errors.back();
    ModuleMember& module_member = *module.get(Path("Foo"));
    REQUIRE(module_member.is_klass());
    sem::TypeObject type("Foo");
    std::cout << "Making error: " << &declaration_node.expression << std::endl;
    ErrorNoMemberSuggestions exp(type, (ast::Member&) declaration_node.expression, module_member.klass());
    REQUIRE(error == exp);
}

TEST_CASE("member_ok", "[checker]") {
    std::string code = "class Foo{foo: String;}\nfun bar(f: Foo)->Integer{var x : String = f.foo;return 0;}";

    CHECKER();
    checker.init();
    checker.check_module();

    CHECK(!not checker.error_reporter.ok());
    CHECK(checker.error_reporter.errors.empty());
}

TEST_CASE("method_ok", "[checker]") {
    std::string code = R"(class Foo{
    foo: String
    fun method() -> String {
        return "lorem ipsum"
    }
}
fun bar(f: Foo)->Integer{
    var x : String = f.method()
    return 0
})";

    CHECKER();
    checker.init();
    checker.check_module();

    CHECK(!not checker.error_reporter.ok());
    CHECK(checker.error_reporter.errors.empty());
}

TEST_CASE("this_within_method_ok", "[checker]") {
    std::string code = R"(class Foo{
    foo: String
    fun method() -> String {
        return this.foo
    }
}
fun bar(f: Foo)->Integer{
    var x : String = f.method()
    return 0
    })";

    CHECKER();
    checker.init();
    checker.check_module();

    CHECK(!not checker.error_reporter.ok());
    CHECK(checker.error_reporter.errors.empty());
}

TEST_CASE("binop_ok", "[checker]") {
    std::string code = "fun bar()->Integer{var x = 2 + 5;return 0;}";

    CHECKER();
    checker.init();

    ast::ExpNode& expression = ((ast::Declaration&) *(module.ast->functions[0].get().body->nodes[0])).expression;
    UExpressionInfo info = checker.dispatch_rvalue(expression);


    REQUIRE_CHECKER_OK();
    CHECK(info->entity.get().is_value());
    EntityValue& entity_value = info->entity.get().get_value();
    CHECK(entity_value.metatype == Meta::CLASS);
    CHECK(entity_value.type == sem::TypeObject("Integer"));
}

TEST_CASE("boolop_ok", "[checker]") {
    std::string code = "fun bar()->Integer{var x = 2 < 5;return 0;}";

    CHECKER();
    checker.init();

    ast::ExpNode& expression = ((ast::Declaration&) *(module.ast->functions[0].get().body->nodes[0])).expression;
    UExpressionInfo info = checker.dispatch_rvalue(expression);


    REQUIRE_CHECKER_OK();
    CHECK(info->entity.get().is_value());
    EntityValue& entity_value = info->entity.get().get_value();
    CHECK(entity_value.metatype == Meta::CLASS);
    CHECK(entity_value.type == sem::TypeObject("Boolean"));
}


TEST_CASE("binop_type_error", "[checker]") {
    std::string code = "fun bar()->Integer{var x = 2 + \"Hello\";return 0;}";

    CHECKER();
    checker.init();

    ast::Declaration& ast_decl = (ast::Declaration&) *(module.ast->functions[0].get().body->nodes[0]);
    ast::BinaryOp& ast_binop = (ast::BinaryOp&) ast_decl.expression;
    UExpressionInfo info = checker.dispatch_rvalue(ast_binop);

    REQUIRE_CHECKER_ONE_ERROR();

    Error& error = *checker.error_reporter.errors.back();
    sem::TypeObject expected("Integer");
    ErrorTypeMismatch exp(expected, ast_binop.right, *checker.entity_from_type(ast::ObjectType("String")));
    REQUIRE(error == exp);
}


TEST_CASE("binop_error", "[checker]") {
    std::string code = R"(fun bar()->Integer{var x = "Hello" - "Bye" ;return 0;})";

    CHECKER();
    checker.init();

    ast::ExpNode& expression = ((ast::Declaration&) *(module.ast->functions[0].get().body->nodes[0])).expression;
    UExpressionInfo info = checker.dispatch_rvalue(expression);


    REQUIRE_CHECKER_ONE_ERROR();

    // Error& error = *checker.error_reporter.errors.back();
    // ast::UExpNode left = std::make_unique<ast::String>("Hello", _POS, _POS);
    // ast::UExpNode right = std::make_unique<ast::String>("Bye", _POS, _POS);
    // ast::BinaryOp node(OpType::SUB, std::move(left), std::move(right), _POS, _POS);
    // ast::ObjectType expected("Integer");
    // ErrorClassNoMethodForOp exp("String", "__sub__", node);
    // REQUIRE(error == exp);
}

TEST_CASE("subscript_ok", "[checker]") {
    std::string code = "fun bar()->Integer{var x = [1,3,4][2] ;return 0;}";

    CHECKER();
    checker.init();

    ast::ExpNode& expression = ((ast::Declaration&) *(module.ast->functions[0].get().body->nodes[0])).expression;
    UExpressionInfo info = checker.dispatch_rvalue(expression);


    REQUIRE_CHECKER_OK();
}

TEST_CASE("subscript_index_type_error", "[checker]") {
    std::string code = "fun bar()->Integer{var x = [1,3,4][\"foo\"] ;return 0;}";

    CHECKER();
    checker.init();

    ast::Declaration& ast_decl = (ast::Declaration&) *(module.ast->functions[0].get().body->nodes[0]);
    ast::Subscript& ast_subs = (ast::Subscript&) ast_decl.expression;
    UExpressionInfo info = checker.dispatch_rvalue(ast_subs);

    REQUIRE_CHECKER_ONE_ERROR();

    Error& error = *checker.error_reporter.errors.back();
    sem::TypeObject expected("Integer");
    ErrorTypeMismatch exp(expected, *ast_subs.child[0], *checker.entity_from_type(ast::ObjectType("String")));
    REQUIRE(error == exp);
}

TEST_CASE("subscript_no_method_error", "[checker]") {
    std::string code = "class Foo{foo: String;}\nfun bar(f: Foo)->Integer{var x = f[1];return 0;}";

    CHECKER();
    checker.init();
    checker.check_module();

    REQUIRE_CHECKER_ONE_ERROR();

    Error& error = *checker.error_reporter.errors.back();
    ast::String left("Hello", _POS, _POS);
    ast::String right("Bye", _POS, _POS);
    ast::UExpNode p_node = ast::Id::make("f", _POS, _POS);
    ast::VectorOfExpNodesU v;
    v.push_back(ast::Number::make(NumberType::INTEGER, "1", _POS, _POS));
    ast::Subscript node(p_node, v, _POS, _POS);
    ast::ObjectType expected("Integer");
    sem::TypeObject type("Foo");
    ErrorObjectNoSpecialMethod exp(type, "__get_item__", node);
    REQUIRE(error == exp);
}

TEST_CASE("call_no_args_ok", "[checker]") {
    std::string code = "fun bar()->Integer{return 0;}\nfun foo()->Integer{var x : Integer = bar();return 0;}";

    CHECKER();
    checker.init();
    checker.check_module();

    REQUIRE_CHECKER_OK();
}

TEST_CASE("call_args_ok", "[checker]") {
    std::string code = "fun bar(a: Integer, b: String)->Integer{return 0;}\nfun foo()->Integer{var x : Integer = bar(8, \"Hello\");return 0;}";

    CHECKER();
    checker.init();
    checker.check_module();

    REQUIRE_CHECKER_OK();
}

TEST_CASE("call_args_type_error", "[checker]") {
    std::string code = "fun bar(a: Integer, b: String)->Integer{return 0;}\nfun foo()->Integer{var x : Integer = bar(\"Hello\",\"Bye\");return 0;}";

    CHECKER();
    checker.init();
    checker.check_module();

    REQUIRE_CHECKER_ONE_ERROR();
    ast::ExpNode& node = static_cast<ast::CallExp&>(static_cast<ast::Declaration&>(*module.ast->functions[1].get().body->nodes[0]).expression).arguments[0];
    // std::cout << (2 == 2);
    Error& error = *checker.error_reporter.errors.back();
    // ast::String node("Hello", _POS, _POS);
    sem::TypeObject expected("Integer");
    ErrorTypeMismatch exp(expected, node, *checker.entity_from_type(ast::ObjectType("String")));
    REQUIRE(error == exp);
}

TEST_CASE("function_no_return_as_exp_error", "[checker]") {
    std::string code = R"(
fun bar(a: Integer, b: String){
    var c = a
}
fun foo()->Integer{
    var x : Integer = bar(7,"Bye")
    return 0
}
)";

    CHECKER();
    checker.init();
    checker.check_module();
    REQUIRE_CHECKER_ONE_ERROR();
    Error& error = *checker.error_reporter.errors.back();
    sem::TypeObject expected("Integer");
    ast::Function& ast_func = module.ast->functions[1];
    ast::Declaration& ast_decl = (ast::Declaration&) *ast_func.body->nodes[0];
    ErrorExpectedExpression exp(EntityNothing(), ast_decl.expression);
    REQUIRE(error == exp);
}

TEST_CASE("function_no_exp_but_returns", "[checker]") {
    std::string code = R"(
fun bar(a: Integer, b: String)->Integer{
    return a
}
fun foo()->Integer{
    bar(9, "Hello")
    return 0
}
    )";

    CHECKER();
    checker.init();
    checker.check_module();
    REQUIRE_CHECKER_ONE_ERROR();
    Error& error = *checker.error_reporter.errors.back();
    ast::Function& ast_func = module.ast->functions[1];
    ast::Call& ast_call = static_cast<ast::Call&>(*ast_func.body->nodes[0]);
    ErrorUnusedReturnValue exp(*checker.entity_from_type(ast::ObjectType("Integer")), ast_call);
    REQUIRE(error == exp);
}

TEST_CASE("union_ok_1", "[checker]") {
    std::string code = "fun foo()->Integer{var x : Union[Integer, String] = 3;return 0;}";

    CHECKER();
    checker.init();
    checker.check_module();

    REQUIRE_CHECKER_OK();
}

TEST_CASE("union_ok_2", "[checker]") {
    std::string code = "fun foo()->Integer{var x : Union[Integer, String] = \"String\";return 0;}";

    CHECKER();
    checker.init();
    checker.check_module();

    REQUIRE_CHECKER_OK();
}

TEST_CASE("union_error", "[checker]") {
    std::string code = "fun foo()->Integer{var x : Union[Integer, String] = false;return 0;}";

    CHECKER();
    checker.init();
    ast::Function& ast_func = module.ast->functions[0];
    ast::Declaration& ast_decl = (ast::Declaration&) *ast_func.body->nodes[0];
    checker.visit_function(ast_func);

    REQUIRE_CHECKER_ONE_ERROR();

    Error& error = *checker.error_reporter.errors.back();
    sem::TypeObject expected("Union", {new sem::TypeObject("Integer"), new sem::TypeObject("String")});
    ErrorTypeMismatch exp(expected, ast_decl.expression, *checker.entity_from_type(ast::ObjectType("Boolean")));
    REQUIRE(error == exp);
}

TEST_CASE("while_ok", "[checker]") {
    std::string code = "fun foo()->Integer{while true {var x = 1;}return 0;}";

    CHECKER();
    checker.init();
    checker.check_module();

    REQUIRE_CHECKER_OK();
}

TEST_CASE("while_boolean_error", "[checker]") {
    std::string code = "fun foo()->Integer{while 5 {var x = 1;}return 0;}";

    CHECKER();
    checker.init();
    ast::Function& ast_func = module.ast->functions[0];
    ast::While& ast_while = (ast::While&) *ast_func.body->nodes[0];
    checker.visit_function(ast_func);

    REQUIRE_CHECKER_ONE_ERROR();


    Error& error = *checker.error_reporter.errors.back();
    sem::TypeObject expected("Boolean");
    ErrorTypeMismatch exp(expected, *ast_while.condition, *checker.entity_from_type(ast::ObjectType("Integer")));
    REQUIRE(error == exp);
}

TEST_CASE("match_ok", "[checker]") {
    std::string code = "fun foo()->Integer{var x: Union[Integer,String] = 8;match x {u: Integer {return 8;} w: String {return 11;}}return 0;}";

    CHECKER();
    checker.init();
    checker.check_module();

    REQUIRE_CHECKER_OK();
}

// TEST_CASE("tuple_ok", "[checker]") {
//     std::string code = "fun foo()->Integer{var x: Tuple[Integer,String] = #(8, \"Hello\");return 0;}";
//
//     std::unique_ptr<Compiler> cp = analyze(code);
//     Compiler& c = *cp;
//     Module& module = *c.root_package.units["tmp"].module;
//     analyze_module_result(module, c.top_package);
//     Checker checker(c.top_package, module);
//     checker.init();
//     checker.visit_root(*module.ast);
//
//     REQUIRE(!not checker.error_reporter.ok());
//     REQUIRE(checker.error_reporter.errors.empty());
// }

TEST_CASE("union_ok", "[checker]") {
    std::string code = "fun foo()->Integer{var x: Union[Integer,String] = 8;return 0;}";

    CHECKER();
    checker.init();
    checker.check_module();

    REQUIRE_CHECKER_OK();
}

TEST_CASE("if_ok", "[checker]") {
    std::string code = "fun foo()->Integer{if true {var x = 1;}return 0;}";

    CHECKER();
    checker.init();
    checker.check_module();

    REQUIRE_CHECKER_OK();
}

TEST_CASE("assign_const_fun_ok", "[checker]") {
    std::string code = R"(fun foo()->Integer{
    return 0
}
fun bar() -> Integer {
    var x = foo
    return 7
}
)";

    CHECKER();
    checker.init();
    checker.check_module();

    REQUIRE_CHECKER_OK();
}

TEST_CASE("call_var_fun_ok", "[checker]") {
    std::string code = R"(fun foo()->Integer{
    return 0
}
fun bar() -> Integer {
    var x = foo
    var w : Integer = x()
    return 7
}
    )";

    CHECKER();
    checker.init();
    checker.check_module();

    REQUIRE_CHECKER_OK();
}


TEST_CASE("for_ok", "[checker]") {
    std::string code = R"(fun foo()->Integer{
    for x @ [1,2,3,4] {
        var i = x
    }
    return 0
}
)";

    CHECKER();
    checker.init();
    auto sem_func = checker.visit_function(module.ast->functions[0]);

    REQUIRE_CHECKER_OK();

    std::vector<sem::UExp> e;
    e.push_back(std::make_unique<sem::Integer>("1"));
    e.push_back(std::make_unique<sem::Integer>("2"));
    e.push_back(std::make_unique<sem::Integer>("3"));
    e.push_back(std::make_unique<sem::Integer>("4"));
    auto list = std::make_unique<sem::List>(std::move(e));
    auto body = std::make_unique<sem::Block>();
    body->nodes.push_back(std::make_unique<sem::Declaration>("i", std::make_unique<sem::Id>("x")));
    REQUIRE(*sem_func.get()->body->nodes[0] == sem::For("x", std::move(list), std::move(body)));
}

TEST_CASE("for_error_no_list", "[checker]") {
    std::string code = R"(fun foo()->Integer{
    for x @ false {
        var i = x
    }
    return 0
}
    )";

    CHECKER();
    checker.init();
    checker.check_module();
    ast::ExpNode& exp = static_cast<ast::For&>(*module.ast->functions[0].get().body->nodes[0]).exp;

    REQUIRE_CHECKER_ONE_ERROR();
    Error& error = *checker.error_reporter.errors.back();
    auto e = checker.entity_from_type(ast::ObjectType("Boolean"));
    REQUIRE(error == ErrorFor(*e, exp.start));
}

TEST_CASE("if_boolean_error", "[checker]") {
    std::string code = "fun foo()->Integer{if 5 {var x = 1;}return 0;}";

    CHECKER();
    checker.init();
    ast::Function& ast_func = module.ast->functions[0];
    ast::If& ast_if = (ast::If&) *ast_func.body->nodes[0];
    checker.visit_function(ast_func);

    REQUIRE_CHECKER_ONE_ERROR();

    Error& error = *checker.error_reporter.errors.back();
    sem::TypeObject expected("Boolean");
    ErrorTypeMismatch exp(expected, ast_if.condition, *checker.entity_from_type(ast::ObjectType("Integer")));
    REQUIRE(error == exp);
}

TEST_CASE("enum_error", "[checker]") {
    std::string code = "enum Foo {a, c}\n fun foo()->Integer{var x = Foo.b;return 0;}";

    CHECKER();
    checker.init();
    checker.check_module();

    REQUIRE_CHECKER_ONE_ERROR();


    Error& error = *checker.error_reporter.errors.back();
    ast::UExpNode u = ast::Id::make("Foo", _POS, _POS);
    ast::Member node(std::move(u), Token(TokType::ID, "b", _POS));
    ast::ObjectType expected("Boolean");
    Enum& enumm = module.members["Foo"]->enumm();
    ErrorEnumNoValue exp("Foo", "b", node, enumm);
    REQUIRE(error == exp);
}


TEST_CASE("enum_ok", "[checker]") {
    std::string code = "enum Foo {a, c}\n fun foo()->Integer{var x = Foo.a; var y = Foo.c; return 0;}";

    CHECKER();
    checker.init();
    checker.check_module();

    REQUIRE_CHECKER_OK();
}