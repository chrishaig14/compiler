//
// Created by chris on 12/4/21.
//

#include "List.h"

List::List(std::vector<USNode>& elements) : SNode(SNodeType::LIST), elements(std::move(elements)) {
}
