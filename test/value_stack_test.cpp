//
// Created by chris on 21/7/20.
//

#include <gtest/gtest.h>
#include "ObjectStack.h"
#include "Object.h"
static Object* value_7 = new Object(new Integer(7));

TEST(vm_test, stack_push) {
    ObjectStack stack;
    stack.push(value_7);
    Object* a = stack.top();
    EXPECT_TRUE(equal(a, value_7));
    EXPECT_FALSE(stack.empty());
}

TEST(vm_test, stack_pop) {
    ObjectStack stack;
    Object* value = new Object(new Integer(7));
    stack.push(value);
    Object* a = stack.pop();
    EXPECT_TRUE(equal(a, value));
    EXPECT_TRUE(stack.empty());
}

TEST(vm_test, stack_empty_is_empty) {
    ObjectStack stack;
    EXPECT_TRUE(stack.empty());
}

TEST(vm_test, stack_integration) {
    ObjectStack stack;
    Object* value_4 = new Object(new Integer(4));
    Object* value_5 = new Object(new Integer(5));
    Object* value_2 = new Object(new Integer(2));
    Object* value_3 = new Object(new Integer(3));
    Object* value_1 = new Object(new Integer(1));
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