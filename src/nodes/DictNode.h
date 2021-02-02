//
// Created by chris on 31/1/21.
//

#ifndef XLANG_DICTNODE_H
#define XLANG_DICTNODE_H


#include "Node.h"

class DictNode : public Node {
    DictNode& dict() override;
    const DictNode& dict() const override;
    bool equal(const Node& other) const override;
public:
    DictNode() {
        this->ntype = DICT;
    }

    std::vector<std::pair<Node*, Node*>> items;
};


#endif //XLANG_DICTNODE_H
