//
// Created by chris on 1/8/20.
//

#ifndef UNTITLED1_IDNODE_H
#define UNTITLED1_IDNODE_H


#include <string>
#include "Node.h"
#include "Visitor.h"

class IdNode : public Node {
public:
    void accept(Visitor& visitor) override;

    std::string identifier;

    IdNode(std::string identifier);

    bool equal(Node* other) const;

    bool operator==(IdNode& other) const;
};


#endif //UNTITLED1_IDNODE_H
