//
// Created by chris on 20/7/20.
//

#include <gtest/gtest.h>
#include <Frame.h>
#include "ValueStack.h"

static Value* value_7 = new Value(new Integer(7));
TEST(vm_test, inst_push) {
    ValueStack stack;
    Frame f(std::vector<Inst*>(), &stack);
    PushInst* inst = new PushInst(value_7);
    f.run_inst(inst);
    EXPECT_TRUE(equal(stack.top(), value_7));
}

TEST(vm_test, inst_pop) {
    ValueStack stack;
    Frame f(std::vector<Inst*>(), &stack);
    stack.push(value_7);
    PopInst* inst = new PopInst();
    f.run_inst(inst);
    EXPECT_TRUE(stack.empty());
}

TEST(vm_test, inst_declare) {
    ValueStack stack;
    Frame f(std::vector<Inst*>(), &stack);
    DeclareInst* inst = new DeclareInst("a");
    f.run_inst(inst);
    EXPECT_TRUE(f.env->is_declared("a"));
}

TEST(vm_test, inst_store) {
    ValueStack stack;
    Frame f(std::vector<Inst*>(), &stack);
    PushInst* push_inst = new PushInst(value_7);
    DeclareInst* declare_inst = new DeclareInst("a");
    StoreInst* store_inst = new StoreInst("a");
    f.run_inst(declare_inst);
    f.run_inst(push_inst);
    f.run_inst(store_inst);
    EXPECT_TRUE(equal(f.env->get("a"), value_7));
    EXPECT_TRUE(stack.empty());
}

TEST(vm_test, inst_load) {
    ValueStack stack;
    Frame f(std::vector<Inst*>(), &stack);
    Value* value_9 = new Value(new Integer(9));
    PushInst* push_inst_7 = new PushInst(value_7);
    PushInst* push_inst_9 = new PushInst(value_9);
    DeclareInst* declare_inst = new DeclareInst("a");
    StoreInst* store_inst = new StoreInst("a");
    LoadInst* load_inst = new LoadInst("a");
    f.run_inst(declare_inst);
    f.run_inst(push_inst_7);
    f.run_inst(store_inst);
    f.run_inst(push_inst_9);
    f.run_inst(load_inst);
    EXPECT_TRUE(equal(stack.top(), value_7));
}

TEST(vm_test, inst_call) {
    ValueStack stack;
    Frame f(std::vector<Inst*>(), &stack);
    std::vector<Inst*> code_vector = {new Inst()};
    BuiltinSum builtinSum;
    Value* builtin_sum = new Value(new Code(&builtinSum));
    Value* value_9 = new Value(new Integer(9));
    Value* value_16 = new Value(new Integer(16));
    PushInst* push_sum_value = new PushInst(builtin_sum);
    PushInst* push_inst_9 = new PushInst(value_9);
    PushInst* push_inst_7 = new PushInst(value_7);
    DeclareInst* declare_sum = new DeclareInst("__sum__");
    StoreInst* store_sum = new StoreInst("__sum__");
    LoadInst* load_sum = new LoadInst("__sum__");
    CallInst* call_inst = new CallInst();
    f.run_inst(declare_sum);
    f.run_inst(push_sum_value);
    f.run_inst(store_sum);
    f.run_inst(push_inst_7);
    f.run_inst(push_inst_9);
    f.run_inst(load_sum);
    f.run_inst(call_inst);
    EXPECT_TRUE(equal(stack.pop(), value_16));
    EXPECT_TRUE(stack.empty());
}

