//
// Created by chris on 10/4/21.
//

#include "While.h"

While::While(USNode condition, Block* body)
        : SNode(SNodeType::WHILE), condition(std::move(condition)), body(body) {
}
