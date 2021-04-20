//
// Created by chris on 24/12/20.
//

#ifndef UNTITLED1_IMPORTNODE_H
#define UNTITLED1_IMPORTNODE_H


#include "Node.h"
#include "../types.h"

class ImportNode : public Node {
public:
    bool has_alias;
    bool equal(const Node& other) const override;
    ImportNode& import() override;
    const ImportNode& import() const override;

    ImportNode(const VectorOfStrings& path, std::string alias, TextPosition start, TextPosition end)
            : Node(NodeType::IMPORT, start, end) {
        this->alias = alias;
        this->path = path;
        this->has_alias = true;
    }

    ImportNode(const VectorOfStrings& path, TextPosition start, TextPosition end)
            : Node(NodeType::IMPORT, start, end) {
        this->has_alias = false;
        this->path = path;
    }

    VectorOfStrings path;
    std::string alias;
};


#endif //UNTITLED1_IMPORTNODE_H
