//
// Created by chris on 1/8/20.
//

#ifndef UNTITLED1_MEMBERNODE_H
#define UNTITLED1_MEMBERNODE_H


#include <string>
#include "Node.h"
#include "Visitor.h"

class MemberNode : public Node {
public:
    Node* parent;
    std::string child;

    MemberNode(Node* parent, const std::string& child);

    void accept(Visitor& visitor) override;

    bool equal(Node* other) const {
        auto other_ptr = dynamic_cast<MemberNode*>(other);
        if (other_ptr == nullptr) return false;
        return *this == *other_ptr;
    }

    bool operator==(MemberNode& other) const {
        return this->parent->equal(other.parent) && this->child == other.child;
    }

    ~MemberNode() {
        delete this->parent;
    }

    json to_json() const override {
        json j;
        j["node"] = "member";
        j["parent"] = this->parent->to_json();
        j["child"] = this->child;
        return j;
    }
};


#endif //UNTITLED1_MEMBERNODE_H
