//
// Created by chris on 12/4/21.
//

#include "../include/List.h"
using namespace sem;
List::List(std::vector<USNode> elements) : SNode(SNodeType::LIST), elements(std::move(elements)) {
}
