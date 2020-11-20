//
// Created by chris on 1/8/20.
//

#ifndef IDNODE_H
#define IDNODE_H


#include <string>
#include "Node.h"

#include "Node.h"

class IdNode : public Node {
public:

    std::string identifier;

    IdNode(std::string identifier);

    bool equal(const Node& other) const override;

    json to_json() const override;

    IdNode& id() override;
    const IdNode& id() const override;
};


#endif //IDNODE_H
