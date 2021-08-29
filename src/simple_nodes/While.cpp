//
// Created by chris on 10/4/21.
//

#include "While.h"

using namespace sem;

While::While(USNode condition, Block* body) : SNode(SNodeType::WHILE), condition(std::move(condition)), body(body) {
}
