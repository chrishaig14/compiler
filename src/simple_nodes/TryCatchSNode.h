//
// Created by chris on 19/6/21.
//

#ifndef XLANG_TRYCATCHSNODE_H
#define XLANG_TRYCATCHSNODE_H

#include "SNode.h"
#include "BlockSNode.h"

class TryCatchSNode : public SNode {
public:
    TryCatchSNode(BlockSNode* body, std::string eid, BlockSNode* catch_body) : SNode(SNodeType::TRY_CATCH) {
        this->body = body;
        this->catch_body = catch_body;
        this->eid = eid;
    }

    std::string eid;
    BlockSNode* body;
    BlockSNode* catch_body;
};


#endif //XLANG_TRYCATCHSNODE_H
