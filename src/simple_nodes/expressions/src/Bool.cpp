//
// Created by chris on 11/4/21.
//

#include "../include/Bool.h"

using namespace sem;

Bool::Bool(bool v) : Exp(ExpType::BOOLEAN) {
    this->v = v;
}
