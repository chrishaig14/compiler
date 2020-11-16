//
// Created by chris on 18/8/20.
//

#ifndef BREAKNODE_H
#define BREAKNODE_H


#include "Node.h"
#include "Visitor.h"

class BreakNode : public Node {
public:
    void accept(Visitor& visitor) override;

    bool operator==(const BreakNode& other) const;

    bool operator!=(const BreakNode& other) const;

};


#endif //BREAKNODE_H
