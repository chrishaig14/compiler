//
// Created by chris on 1/8/20.
//

#ifndef IDNODE_H
#define IDNODE_H


#include <string>
#include "Node.h"

#include "Node.h"
#include "../semantic/SymbolTable.h"

class IdNode : public Node {
public:
    VariableLocation location;
    std::string _id;
    bool is_global_function;

    IdNode(std::string identifier);

    bool equal(const Node& other) const override;

    json to_json() const override;

    IdNode& id() override;
    const IdNode& id() const override;
};


#endif //IDNODE_H
