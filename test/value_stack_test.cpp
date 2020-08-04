//
// Created by chris on 21/7/20.
//

#include <gtest/gtest.h>
#include "vm/ObjectStack.h"
#include "vm/Object.h"
#include "vm/IntegerObject.h"

static Object* value_7 = new IntegerObject(7);

TEST(vm_test, stack_push) {
    ObjectStack stack;
    stack.push(value_7);
    Object* a = stack.top();
    EXPECT_TRUE(a== value_7);
    EXPECT_FALSE(stack.empty());
}

TEST(vm_test, stack_pop) {
    ObjectStack stack;
    Object* value = new IntegerObject(7);
    stack.push(value);
    Object* a = stack.pop();
    EXPECT_TRUE(a->equal(value));
    EXPECT_TRUE(stack.empty());
}

TEST(vm_test, stack_empty_is_empty) {
    ObjectStack stack;
    EXPECT_TRUE(stack.empty());
}

TEST(vm_test, stack_integration) {
    ObjectStack stack;
    Object* value_4 = new IntegerObject(4);
    Object* value_5 = new IntegerObject(5);
    Object* value_2 = new IntegerObject(2);
    Object* value_3 = new IntegerObject(3);
    Object* value_1 = new IntegerObject(1);
    stack.push(value_1); // 1
    stack.push(value_2); // 1 2
    stack.pop(); // 1
    stack.push(value_3); // 1 3
    stack.push(value_4); // 1 3 4
    stack.pop(); // 1 3
    stack.pop(); // 1
    stack.push(value_5); // 1 5
    EXPECT_FALSE(stack.empty());
    EXPECT_TRUE(stack.top()== value_5);
    EXPECT_TRUE(stack.pop()== value_5);
    EXPECT_FALSE(stack.empty());
    EXPECT_TRUE(stack.top()== value_1);
    EXPECT_TRUE(stack.pop()== value_1);
    EXPECT_TRUE(stack.empty());
}