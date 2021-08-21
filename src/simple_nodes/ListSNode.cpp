//
// Created by chris on 12/4/21.
//

#include "ListSNode.h"

ListSNode::ListSNode(std::vector<USNode>& elements) : SNode(SNodeType::LIST), elements(std::move(elements)) {
}
