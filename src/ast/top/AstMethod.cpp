//
// Created by chris on 9/12/21.
//

#include "AstMethod.h"

AstMethod::AstMethod(bool is_static, ast::UFunctionNode func) : is_static(is_static), func(std::move(func)) {
}
