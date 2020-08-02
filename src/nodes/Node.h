//
// Created by chris on 1/8/20.
//

#ifndef UNTITLED1_NODE_H
#define UNTITLED1_NODE_H


class Visitor;

class Node {
public:
    virtual void accept(Visitor& visitor) = 0;

    virtual bool equal(Node* other) const = 0;
};

#endif //UNTITLED1_NODE_H
