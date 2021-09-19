//
// Created by chris on 11/4/21.
//

#include "../include/NewObject.h"
using namespace sem;

NewObject::NewObject() : Exp(ExpType::NEW) {

}

bool NewObject::equals(const Exp& o) const {
    return false;
}
