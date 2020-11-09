//
// Created by chris on 8/11/20.
//

#ifndef NODEFACTORY_H
#define NODEFACTORY_H

#include "NodeContainer.h"
#include "AssignmentNode.h"
#include "BinopNode.h"
#include "BlockNode.h"
#include "BreakNode.h"
#include "CallNode.h"
#include "IfNode.h"
#include "ReturnNode.h"
class NodeFactory {
public:
    static NodeContainer assign(NodeContainer lvalue, NodeContainer rvalue) {
        auto node = new AssignmentNode(lvalue, rvalue);
        return NodeContainer(node);
    }

    static NodeContainer binop(OpType op, NodeContainer left, NodeContainer right) {
        auto node = new BinopNode(op, left, right);
        return NodeContainer(node);
    }

    static NodeContainer block(std::vector<NodeContainer> nodes) {
        auto node = new BlockNode(nodes);
        return NodeContainer(node);
    }

    static NodeContainer brk() {
        auto node = new BreakNode();
        return NodeContainer(node);
    }
    static NodeContainer call(NodeContainer function, std::vector<NodeContainer> arguments) {
        auto node = new CallNode(function, arguments);
        return NodeContainer(node);
    }
    static NodeContainer iff(NodeContainer condition, NodeContainer body, std::vector<std::pair<NodeContainer, NodeContainer>> elifs, NodeContainer _else) {
        auto node = new IfNode(condition, body, elifs, _else);
        return NodeContainer(node);
    }
    static NodeContainer retrn(NodeContainer expression) {
        auto node = new ReturnNode(expression);
        return NodeContainer(node);
    }
};

#endif //NODEFACTORY_H
