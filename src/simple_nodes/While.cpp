//
// Created by chris on 10/4/21.
//

#include "While.h"

using namespace sem;

While::While(USNode condition, std::unique_ptr<Block> body) : SNode(SNodeType::WHILE), condition(std::move(condition)), body(std::move(body)) {
}
