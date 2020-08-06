//
// Created by chris on 20/7/20.
//

#include <gtest/gtest.h>
#include <vm/CodeRunner.h>
#include <vm/BuiltinSum.h>
#include "vm/ObjectStack.h"

static Object* value_7 = new IntegerObject(7);

TEST(vm_test, inst_push_integer) {
    ObjectStack stack;
    Code code;
    CodeRunner code_runner(code, stack, {});
    PushIntegerInst inst(7);
    inst.accept(code_runner);
    EXPECT_TRUE(stack.top()->equal(value_7));
}


TEST(vm_test, inst_declare) {
    ObjectStack stack;
    Code code;
    CodeRunner code_runner(code, stack, {});
    DeclareInst inst("a");
    inst.accept(code_runner);
    EXPECT_TRUE(code_runner.env->is_declared("a"));
}

TEST(vm_test, inst_store) {
    ObjectStack stack;
    Code code;
    CodeRunner code_runner(code, stack, {});
    PushIntegerInst push_inst(7);
    DeclareInst declare_inst("a");
    SetInst set_inst("a");
    declare_inst.accept(code_runner);
    push_inst.accept(code_runner);
    set_inst.accept(code_runner);
    EXPECT_TRUE(code_runner.env->get("a")->equal(value_7));
    EXPECT_TRUE(stack.empty());
}

TEST(vm_test, inst_load) {
    ObjectStack stack;
    Code code;
    CodeRunner code_runner(code, stack, {});

    PushIntegerInst push_inst_7(7);
    PushIntegerInst push_inst_9(9);
    DeclareInst declare_inst("a");
    SetInst store_inst("a");
    GetInst get_inst("a");
    declare_inst.accept(code_runner);
    push_inst_7.accept(code_runner);
    store_inst.accept(code_runner);
    push_inst_9.accept(code_runner);
    get_inst.accept(code_runner);
    EXPECT_TRUE(stack.top()->equal(value_7));
}

TEST(vm_test, inst_call) {
    ObjectStack stack;
    Code code;
    BuiltinSum builtinSum;
    Object* builtin_sum = new CodeObject(&builtinSum);

    CodeRunner code_runner(code, stack, {{"__sum__", builtin_sum}});
    Object* value_16 = new IntegerObject(16);
    PushIntegerInst push_inst_9(9);
    PushIntegerInst push_inst_7(7);
    GetInst load_sum("__sum__");
    CallInst call_inst;
    push_inst_7.accept(code_runner);
    push_inst_9.accept(code_runner);
    load_sum.accept(code_runner);
    call_inst.accept(code_runner);
    EXPECT_TRUE(stack.pop()->equal(value_16));
    EXPECT_TRUE(stack.empty());
}

TEST(vm_test, inst_call_user) {
    Code user_function_code = {
            I_DECL("x"),
            I_DECL("y"),
            I_SET("y"),
            I_SET("x"),
            I_GET("x"),
            I_GET("y"),
            I_GET("__sum__"),
            I_CALL,
            I_PUSHI(9),
            I_GET("__sum__"),
            I_CALL,
            I_RET};
    BuiltinSum builtin_sum_function;

    std::vector<std::string> free_variables = {"__sum__"};

    Code main_code = {
            I_PUSHF(std::vector<std::string>(), user_function_code, free_variables),
            I_DECL("user_function"),
            I_SET("user_function"),
            I_PUSHI(5),
            I_PUSHI(7),
            I_GET("user_function"),
            I_CALL
    };
    ObjectStack stack;
    Object* builtin_sum_object = new CodeObject(&builtin_sum_function);
    CodeRunner code_runner(main_code, stack, {{"__sum__", builtin_sum_object}});
    code_runner.run();
    EXPECT_TRUE(stack.top()->equal(new IntegerObject(21)));
}

TEST(vm_test, inst_make_object) {
    std::vector<std::string> f = {"foo", "bar"};
    Code main_code = {
            I_MAKE_OBJECT("Foo", f),
    };
    ObjectStack stack;
    CodeRunner code_runner(main_code, stack, {});
    code_runner.run();
    Object* top = stack.pop();
    UserObject* user_object = dynamic_cast<UserObject*>(top);
    EXPECT_NE(user_object, nullptr);
    EXPECT_EQ(user_object->type, "Foo");
    EXPECT_TRUE(stack.empty());
    std::map<std::string, Object*> expected_fields = {{"foo", nullptr},
                                                      {"bar", nullptr}};
    EXPECT_EQ(user_object->fields, expected_fields);
}

TEST(vm_test, inst_set_member) {
    std::vector<std::string> f = {"foo", "bar"};
    Code main_code = {
            I_DECL("x"),
            I_MAKE_OBJECT("Foo", f),
            I_SET("x"),
            I_PUSHI(7),
            I_GET("x"),
            I_SETM("foo"),
    };
    ObjectStack stack;
    CodeRunner code_runner(main_code, stack, {});
    code_runner.run();
    EXPECT_TRUE(stack.empty());
    Object* top = code_runner.env->get("x");
    UserObject* user_object = dynamic_cast<UserObject*>(top);
    EXPECT_TRUE(user_object->fields["foo"]->equal(new IntegerObject(7)));
}

TEST(vm_test, inst_get_member) {
    std::vector<std::string> f = {"foo", "bar"};
    Code main_code = {
            I_DECL("x"),
            I_MAKE_OBJECT("Foo", f),
            I_SET("x"),
            I_PUSHI(7),
            I_GET("x"),
            I_SETM("foo"),
            I_GET("x"),
            I_GETM("foo")
    };
    ObjectStack stack;
    CodeRunner code_runner(main_code, stack, {});
    code_runner.run();
    Object* tos = stack.pop();
    EXPECT_TRUE(stack.empty());
    EXPECT_TRUE(tos->equal(new IntegerObject(7)));
}

TEST(vm_test, inst_make_list) {
    std::vector<std::string> f = {"foo", "bar"};
    Code main_code = {
            I_PUSHI(0),
            I_PUSHI(1),
            I_PUSHI(1),
            I_PUSHI(2),
            I_PUSHI(3),
            I_PUSHI(5),
            I_MAKE_LIST(6),
    };
    ObjectStack stack;
    CodeRunner code_runner(main_code, stack, {});
    code_runner.run();
    Object* tos = stack.pop();
    EXPECT_TRUE(stack.empty());
    ListObject* list = dynamic_cast<ListObject*>(tos);
    EXPECT_NE(list, nullptr);
    EXPECT_EQ(list->list.size(), 6);
    EXPECT_TRUE(list->list[0]->equal(new IntegerObject(0)));
    EXPECT_TRUE(list->list[1]->equal(new IntegerObject(1)));
    EXPECT_TRUE(list->list[2]->equal(new IntegerObject(1)));
    EXPECT_TRUE(list->list[3]->equal(new IntegerObject(2)));
    EXPECT_TRUE(list->list[4]->equal(new IntegerObject(3)));
    EXPECT_TRUE(list->list[5]->equal(new IntegerObject(5)));
}

TEST(vm_test, inst_jump_if_false_yes) {
    Code main_code = {
            I_DECL("x"),
            I_PUSHI(3),
            I_SET("x"),
            I_PUSHB(false),
            I_JUMPF(3),
            I_PUSHI(7),
            I_SET("x"),
            I_PUSHI(9),
    };
    ObjectStack stack;
    CodeRunner code_runner(main_code, stack, {});
    code_runner.run();
    Object* tos = stack.pop();
    EXPECT_TRUE(tos->equal(new IntegerObject(9)));
    EXPECT_TRUE(stack.empty());
    EXPECT_TRUE(code_runner.env->get("x")->equal(new IntegerObject(3)));
    EXPECT_TRUE(stack.empty());
}

TEST(vm_test, inst_jump_if_false_no) {
    Code main_code = {
            I_DECL("x"),
            I_PUSHI(3),
            I_SET("x"),
            I_PUSHB(true),
            I_JUMPF(3),
            I_PUSHI(7),
            I_SET("x"),
            I_PUSHI(9),
    };
    ObjectStack stack;
    CodeRunner code_runner(main_code, stack, {});
    code_runner.run();
    Object* tos = stack.pop();
    EXPECT_TRUE(tos->equal(new IntegerObject(9)));
    EXPECT_TRUE(stack.empty());
    EXPECT_TRUE(code_runner.env->get("x")->equal(new IntegerObject(7)));
    EXPECT_TRUE(stack.empty());
}

TEST(vm_test, inst_factorial_test) {
    Code factorial_code = {
            I_DECL("n"),
            I_SET("n"),
            I_GET("n"),
            I_PUSHI(1),
            I_BIN(OpType::EQ),
            I_JUMPF(3),
            I_PUSHI(1),
            I_RET,
            I_GET("n"),
            I_GET("n"),
            I_PUSHI(1),
            I_BIN(OpType::SUB),
            I_GET("factorial"),
            I_CALL,
            I_BIN(OpType::MUL),
            I_RET,
    };
    std::vector<std::string> free_vars = {"factorial"};
    Code main_code = {
            I_DECL("factorial"),
            I_PUSHF(std::vector<std::string>(), factorial_code, free_vars),
            I_SET("factorial"),
            I_PUSHI(12),
            I_GET("factorial"),
            I_CALL
    };
    ObjectStack stack;
    CodeRunner code_runner(main_code, stack, {});
    code_runner.run();
    Object* tos = stack.pop();
    EXPECT_TRUE(tos->equal(new IntegerObject(479001600)));
    EXPECT_TRUE(stack.empty());
}