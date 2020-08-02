//
// Created by chris on 1/8/20.
//

#ifndef UNTITLED1_CALLNODE_H
#define UNTITLED1_CALLNODE_H


#include <vector>
#include "Node.h"
#include "Visitor.h"

class CallNode : public Node {
public:
    Node* function;
    std::vector<Node*> arguments;

    CallNode(Node* function, const std::vector<Node*>& arguments);

    void accept(Visitor& visitor) override;

    bool equal(Node* other) const override;

    bool operator==(CallNode& other) const;

    ~CallNode() {
        delete this->function;
        for (auto a: this->arguments) {
            delete a;
        }
    }

};


#endif //UNTITLED1_CALLNODE_H
