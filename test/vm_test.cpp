//
// Created by chris on 20/7/20.
//

#include <gtest/gtest.h>
#include <Frame.h>
#include "ValueStack.h"


TEST(vm_test, inst_push) {
    ValueStack stack;
    Frame f(std::vector<Inst*>(), &stack);
    Value* value = new Value(Integer(7));
    PushInst* inst = new PushInst(value);
    f.run_inst(inst);
    EXPECT_TRUE(equal(stack.top(), value));
}

TEST(vm_test, inst_pop) {
    ValueStack stack;
    Frame f(std::vector<Inst*>(), &stack);
    Value* value = new Value(Integer(7));
    stack.push(value);
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
    Value* value = new Value(Integer(7));
    PushInst* push_inst = new PushInst(value);
    DeclareInst* declare_inst = new DeclareInst("a");
    StoreInst* store_inst = new StoreInst("a");
    f.run_inst(declare_inst);
    f.run_inst(push_inst);
    f.run_inst(store_inst);
    EXPECT_TRUE(equal(f.env->get("a"), value));
    EXPECT_TRUE(stack.empty());
}