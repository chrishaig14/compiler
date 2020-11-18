//
// Created by chris on 1/8/20.
//

#ifndef MEMBERNODE_H
#define MEMBERNODE_H


#include <string>
#include "Node.h"
#include "Visitor.h"
#include "Node.h"

class MemberNode : public Node {
public:
    Node* parent;
    std::string child;

    MemberNode(Node* parent, const std::string& child);

    void accept(Visitor& visitor) override;

    bool equal(const Node& x) const override {
        auto& other = x.member();
        return *this->parent == *other.parent && this->child == other.child;
    }

    ~MemberNode() {
//        delete this->parent;
    }

    json to_json() const override {
        json j;
        j["node"] = "member";
//        j["parent"] = this->parent->to_json();
        j["child"] = this->child;
        return j;
    }

    MemberNode& member() override;
    const MemberNode& member() const override;
};


#endif //MEMBERNODE_H
