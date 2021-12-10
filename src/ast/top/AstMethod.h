//
// Created by chris on 9/12/21.
//

#ifndef XLANG_ASTMETHOD_H
#define XLANG_ASTMETHOD_H

#include "../top/Function.h"

class AstMethod {
public:
    bool is_static;
    ast::UFunctionNode func;
    AstMethod(bool is_static, ast::UFunctionNode func);
};


#endif //XLANG_ASTMETHOD_H
