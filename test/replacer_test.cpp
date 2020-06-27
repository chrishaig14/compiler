//
// Created by chris on 15/6/20.
//

#include <gtest/gtest.h>
#include <Scanner.h>
#include <Parser.h>
#include <AstNode.h>
#include <replacer.h>
#include "utils.h"

TEST(replacer_test, replace_simple_type_in_declaration) {
    DeclarationNode* decl = i_decl_type("x", i_type("T", {}), NULL);
    replace_types(decl, {"T"}, {t_integer()});
    EXPECT_EQ(equal(decl, i_decl_type("x", t_integer(), NULL)), true);
}

TEST(replacer_test, replace_simple_type_in_block) {
    DeclarationNode* decl = i_decl_type("x", i_type("T", {}), NULL);
    replace_types(decl, {"T"}, {t_integer()});
    EXPECT_EQ(equal(decl, i_decl_type("x", t_integer(), NULL)), true);
}

TEST(replacer_test, replace_simple_type) {
    TypeNode* type = i_type("T", {});
    replace_types(type, {"T"}, {t_integer()});
    EXPECT_EQ(equal(type, t_integer()), true);
}

TEST(replacer_test, replace_type) {
    TypeNode* type = t_list(i_type("T", {}));
    replace_types(type, {"T"}, {t_integer()});
    EXPECT_EQ(equal(type, t_list(t_integer())), true);
}

TEST(replacer_test, replace_recursive_type) {
    TypeNode* type = t_list(i_type("T", {}));
    TypeNode* r_type_1 = t_list(t_integer());
    replace_types(type, {"T"}, {r_type_1});
    EXPECT_EQ(equal(type, t_list(r_type_1)), true);
}

TEST(replacer_test, replace_template_class) {
    ClassNode* node = i_class("Foo", {"T"}, {}, {i_decl_type("x", i_type("T", {}), NULL)}, {});
    TypeNode* r_type_1 = t_list(t_integer());
    replace_types(node, {"T"}, {r_type_1});
    EXPECT_EQ(equal(node, i_class("Foo", {"T"}, {}, {i_decl_type("x", i_type("T", {}), NULL)}, {})), true);
}


