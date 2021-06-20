//
// Created by chris on 19/6/21.
//

#include "TryCatchNode.h"
#include "TypeNode.h"

bool TryCatchNode::equal(const Node& other) const {
    return false;
}

TryCatchNode::TryCatchNode(BlockNode* body, std::string eid, ObjectType* et, BlockNode* catch_body, TextPosition start,
                           TextPosition end) : Node(NodeType::TRY_CATCH, start, end) {
    this->body = body;
    this->catch_body = catch_body;
    this->et = et;
    this->eid = eid;
}
