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

    ImportNode(const std::string module_name, const VectorOfStrings& imports) {
        this->module_name = module_name;
        this->imports = imports;
        this->ntype = IMPORT;
    }

    std::string module_name;
    VectorOfStrings imports;
};


#endif //UNTITLED1_IMPORTNODE_H
