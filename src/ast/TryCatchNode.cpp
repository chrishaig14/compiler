//
// Created by chris on 19/6/21.
//

#include "TryCatchNode.h"
#include "TypeNode.h"

bool TryCatchNode::equal(const Node& other) const {
    return false;
}

TryCatchNode::TryCatchNode(BlockNode* body, std::vector<std::pair<std::string, ObjectType*>> catches,
                           VectorOfNodes catches_bodies, TextPosition start, TextPosition end) : Node(NodeType::TRY_CATCH, start, end) {
    this->body = body;
    this->catches = catches;
    this->catches_bodies = catches_bodies;
}
