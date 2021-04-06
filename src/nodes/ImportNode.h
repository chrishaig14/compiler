//
// Created by chris on 24/12/20.
//

#ifndef UNTITLED1_IMPORTNODE_H
#define UNTITLED1_IMPORTNODE_H


#include "Node.h"
#include "../types.h"

class ImportNode : public Node {
public:
    bool equal(const Node& other) const override;
    ImportNode& import() override;
    const ImportNode& import() const override;

    ImportNode(const VectorOfStrings& path, TextPosition start, TextPosition end)
            : Node(NodeType::IMPORT, start, end) {
        this->path = path;
    }

    VectorOfStrings path;
};


#endif //UNTITLED1_IMPORTNODE_H
