//
// Created by chris on 21/7/20.
//

#include <gtest/gtest.h>
#include "ValueStack.h"
#include "Value.h"
static Value* value_7 = new Value(new Integer(7));

TEST(vm_test, stack_push) {
    ValueStack stack;
    stack.push(value_7);
    Value* a = stack.top();
    EXPECT_TRUE(equal(a, value_7));
    EXPECT_FALSE(stack.empty());
}

TEST(vm_test, stack_pop) {
    ValueStack stack;
    Value* value = new Value(new Integer(7));
    stack.push(value);
    Value* a = stack.pop();
    EXPECT_TRUE(equal(a, value));
    EXPECT_TRUE(stack.empty());
}

TEST(vm_test, stack_empty_is_empty) {
    ValueStack stack;
    EXPECT_TRUE(stack.empty());
}

TEST(vm_test, stack_integration) {
    ValueStack stack;
    Value* value_4 = new Value(new Integer(4));
    Value* value_5 = new Value(new Integer(5));
    Value* value_2 = new Value(new Integer(2));
    Value* value_3 = new Value(new Integer(3));
    Value* value_1 = new Value(new Integer(1));
    stack.push(value_1); // 1
    stack.push(value_2); // 1 2
    stack.pop(); // 1
    stack.push(value_3); // 1 3
    stack.push(value_4); // 1 3 4
    stack.pop(); // 1 3
    stack.pop(); // 1
    stack.push(value_5); // 1 5
    EXPECT_FALSE(stack.empty());
    EXPECT_TRUE(equal(stack.top(), value_5));
    EXPECT_TRUE(equal(stack.pop(), value_5));
    EXPECT_FALSE(stack.empty());
    EXPECT_TRUE(equal(stack.top(), value_1));
    EXPECT_TRUE(equal(stack.pop(), value_1));
    EXPECT_TRUE(stack.empty());
}