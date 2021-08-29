//
// Created by chris on 11/4/21.
//

#include "Bool.h"

Bool::Bool(bool v) : SNode(SNodeType::BOOLEAN) {
    this->v = v;
}
