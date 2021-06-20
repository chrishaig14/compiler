//
// Created by chris on 19/6/21.
//

#ifndef XLANG_TRYCATCHNODE_H
#define XLANG_TRYCATCHNODE_H


#include "Node.h"
#include "TypeNode.h"

class TryCatchNode : public Node {
public:
    TryCatchNode(BlockNode* body, std::string eid, ObjectType* et, BlockNode* catch_body, TextPosition start,
                 TextPosition end);

    bool equal(const Node& other) const override;

    BlockNode* body;
    BlockNode* catch_body;
    ObjectType* et;
    std::string eid;
};


#endif //XLANG_TRYCATCHNODE_H
