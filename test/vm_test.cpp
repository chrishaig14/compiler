//
// Created by chris on 20/7/20.
//

#include <gtest/gtest.h>
#include <vm/CodeRunner.h>
#include <vm/BuiltinSum.h>
#include "vm/ObjectStack.h"

static Object* value_7 = new IntegerObject(7);
static Object* value_9 = new IntegerObject(9);

TEST(vm_test, inst_push_integer) {
    ObjectStack stack;
    Code code;
    CodeRunner code_runner(code, stack);
    PushIntegerInst inst(7);
    inst.accept(code_runner);
    EXPECT_TRUE(stack.top()->equal(value_7));
}


TEST(vm_test, inst_declare) {
    ObjectStack stack;
    Code code;
    CodeRunner code_runner(code, stack);
    DeclareInst inst("a");
    inst.accept(code_runner);
    EXPECT_TRUE(code_runner.env->is_declared("a"));
}

TEST(vm_test, inst_store) {
    ObjectStack stack;
    Code code;
    CodeRunner code_runner(code, stack);
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
    CodeRunner code_runner(code, stack);

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
    CodeRunner code_runner(code, stack);
    BuiltinSum builtinSum;
    Object* builtin_sum = new CodeObject(&builtinSum);
    code_runner.env->declare("__sum__");
    code_runner.env->set("__sum__", builtin_sum);
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

