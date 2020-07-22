//
// Created by chris on 21/7/20.
//
#include <gtest/gtest.h>
#include <Frame.h>
#include "ValueStack.h"

TEST(env_test, env_test_1) {
    Environment env(nullptr);
    Value* value = new Value(Integer(7));
    try {
        env.set("a", value);
        FAIL() << "Error not thrown!";
    } catch (const std::runtime_error &e) {
    }
}

TEST(env_test, env_test_2) {
    Environment env(nullptr);
    Value* value = new Value(Integer(7));
    try {
        env.get("a");
        FAIL() << "Error not thrown!";
    } catch (const std::runtime_error &e) {
    }
}

TEST(env_test, env_test_3) {
    Environment env(nullptr);
    Value* value = new Value(Integer(7));
    env.declare("a");
}

TEST(env_test, env_test_4) {
    Environment env(nullptr);
    Value* value = new Value(Integer(7));
    env.declare("a");
    try {
        env.declare("a");
        FAIL() << "Error not thrown!";
    } catch (const std::runtime_error &e) {

    }
}

TEST(env_test, env_test_5) {
    Environment env(nullptr);
    Value* value = new Value(Integer(7));
    env.declare("a");
    env.set("a", value);
    Value* val = env.get("a");
    EXPECT_TRUE(equal(val, value));
}

TEST(env_test, env_test_6) {
    Environment env(nullptr);
    Value* value = new Value(Integer(7));
    env.declare("a");
    try {
        env.get("a");
        FAIL() << "Error not thrown";
    } catch (const std::runtime_error &e) {

    }
}

TEST(env_test, env_test_7) {
    Environment env(nullptr);
    Value* value = new Value(Integer(7));
    env.declare("a");
    env.set("a", value);
    Environment* new_env = env.enter();
    Value* val = new_env->get("a");
    EXPECT_TRUE(equal(val, value));
}

TEST(env_test, env_test_8) {
    Environment env(nullptr);
    Value* value = new Value(Integer(7));
    env.declare("a");
    env.set("a", value);
    Environment* new_env = env.enter();
    new_env->declare("a");
}

TEST(env_test, env_test_9) {
    Environment* env = new Environment(nullptr); // env1
    Value* value_7 = new Value(Integer(7));
    env = env->enter(); // env2
    env->declare("a");
    env->set("a", value_7);
    EXPECT_TRUE(equal(env->get("a"), value_7));
    env = env->leave(); // env1
    try {
        env->set("a", value_7);
        FAIL() << "Error not thrown";
    } catch (const std::runtime_error &e) {

    }
}

TEST(env_test, env_test_10) {
    Environment* env = new Environment(nullptr); // env1
    Value* value_7 = new Value(Integer(7));
    Value* value_9 = new Value(Integer(9));
    env->declare("a");
    env->set("a", value_7);
    env = env->enter(); // env2
    env->declare("a");
    env->set("a", value_9);
    env = env->enter(); // env3
    EXPECT_TRUE(equal(env->get("a"), value_9));
    env = env->leave(); // env2
    EXPECT_TRUE(equal(env->get("a"), value_9));
    env = env->leave(); // env1
    EXPECT_TRUE(equal(env->get("a"), value_7));
}

TEST(env_test, env_test_11) {
    Environment* env = new Environment(nullptr); // env1
    Value* value_7 = new Value(Integer(7));
    Value* value_9 = new Value(Integer(9));
    env->declare("a");
    env->set("a", value_7);
    env = env->enter(); // env2
    env->declare("b");
    env->set("b", value_9);
    EXPECT_TRUE(equal(env->get("a"), value_7));
    EXPECT_TRUE(equal(env->get("b"), value_9));
    env = env->leave(); // env1
    EXPECT_TRUE(equal(env->get("a"), value_7));
    try {
        env->get("b");
        FAIL() << "Error not thrown";
    } catch (const std::runtime_error &e) {

    }
}