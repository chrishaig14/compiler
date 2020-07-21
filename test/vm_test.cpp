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