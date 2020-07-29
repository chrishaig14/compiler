//
// Created by chris on 20/7/20.
//

#include <gtest/gtest.h>
#include <Frame.h>
#include "ObjectStack.h"

Object v7(new Integer(7));
Object v9(new Integer(9));
static Object* value_7 = &v7;
static Object* value_9 = &v9;

TEST(vm_test, inst_push) {
    ObjectStack stack;
    std::vector<Inst> code;
    Frame f(code, stack);
    PushInst inst(value_7);
    f.run_inst(inst);
    EXPECT_TRUE(equal(stack.top(), value_7));
}

TEST(vm_test, inst_pop) {
    ObjectStack stack;
    std::vector<Inst> code;
    Frame f(code, stack);
    stack.push(value_7);
    PopInst inst;
    f.run_inst(inst);
    EXPECT_TRUE(stack.empty());
}

TEST(vm_test, inst_declare) {
    ObjectStack stack;
    std::vector<Inst> code;
    Frame f(code, stack);
    DeclareInst inst("a");
    f.run_inst(inst);
    EXPECT_TRUE(f.env->is_declared("a"));
}

TEST(vm_test, inst_store) {
    ObjectStack stack;
    std::vector<Inst> code;
    Frame f(code, stack);
    PushInst push_inst(value_7);
    DeclareInst declare_inst("a");
    StoreInst store_inst("a");
    f.run_inst(declare_inst);
    f.run_inst(push_inst);
    f.run_inst(store_inst);
    EXPECT_TRUE(equal(f.env->get("a"), value_7));
    EXPECT_TRUE(stack.empty());
}

TEST(vm_test, inst_load) {
    ObjectStack stack;
    std::vector<Inst> code;
    Frame f(code, stack);
    PushInst push_inst_7(value_7);
    PushInst push_inst_9(value_9);
    DeclareInst declare_inst("a");
    StoreInst store_inst("a");
    LoadInst load_inst("a");
    f.run_inst(declare_inst);
    f.run_inst(push_inst_7);
    f.run_inst(store_inst);
    f.run_inst(push_inst_9);
    f.run_inst(load_inst);
    EXPECT_TRUE(equal(stack.top(), value_7));
}

TEST(vm_test, inst_call) {
    ObjectStack stack;
    std::vector<Inst> code;
    Frame f(code, stack);
    std::vector<Inst*> code_vector = {new Inst()};
    BuiltinSum builtinSum;
    Object builtin_sum(new Code(&builtinSum));
    Object value_16(new Integer(16));
    PushInst push_sum_value(&builtin_sum);
    PushInst push_inst_9(value_9);
    PushInst push_inst_7(value_7);
    DeclareInst declare_sum("__sum__");
    StoreInst store_sum("__sum__");
    LoadInst load_sum("__sum__");
    CallInst call_inst;
    f.run_inst(declare_sum);
    f.run_inst(push_sum_value);
    f.run_inst(store_sum);
    f.run_inst(push_inst_7);
    f.run_inst(push_inst_9);
    f.run_inst(load_sum);
    f.run_inst(call_inst);
    EXPECT_TRUE(equal(stack.pop(), &value_16));
    EXPECT_TRUE(stack.empty());
}

