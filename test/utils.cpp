//
// Created by chris on 15/6/20.
//

#include "utils.h"


TypeNode* t_list(TypeNode* el) {
    return i_type("List", {el});
}

TypeNode* t_integer() {
    return i_type("Integer", {});
}

TypeNode* t_string() {
    return i_type("String", {});
}