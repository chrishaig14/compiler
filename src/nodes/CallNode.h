//
// Created by chris on 1/8/20.
//

#ifndef CALLNODE_H
#define CALLNODE_H


#include <vector>
#include "Node.h"
#include "Visitor.h"
#include "NodeContainer.h"
class CallNode : public Node {
public:
    NodeContainer function;
    std::vector<NodeContainer> arguments;

    CallNode(NodeContainer function, const std::vector<NodeContainer>& arguments);

    void accept(Visitor& visitor) override;

    bool operator==(const CallNode& other) const;
    bool operator!=(const CallNode& other) const;

    ~CallNode() {
//        delete this->function;
        for (auto a: this->arguments) {
//            delete a;
        }
    }

    json to_json() const override {
        json j;
        j["node"] = "call";
//        j["function"] = this->function->to_json();
        j["arguments"] = {};
        for (auto a: this->arguments) {
//            j["arguments"].push_back(a->to_json());
        }
        return j;
    }

};


#endif //CALLNODE_H
