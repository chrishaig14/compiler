//
// Created by chris on 19/6/21.
//

#ifndef XLANG_TRYCATCHSNODE_H
#define XLANG_TRYCATCHSNODE_H

#include "SNode.h"
#include "BlockSNode.h"

class TryCatchSNode : public SNode {
public:
    TryCatchSNode(BlockSNode* body, std::vector<std::pair<std::string, std::string>> e_names_types,
                  std::vector<SNode*> catches_bodies) : SNode(SNodeType::TRY_CATCH) {
        this->body = body;
        this->catches_bodies = catches_bodies;
        this->e_names_types = e_names_types;
    }

    std::vector<std::pair<std::string, std::string>> e_names_types;
    BlockSNode* body;
    std::vector<SNode*> catches_bodies;
};


#endif //XLANG_TRYCATCHSNODE_H
