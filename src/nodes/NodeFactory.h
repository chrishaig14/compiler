//
// Created by chris on 8/11/20.
//

#ifndef NODEFACTORY_H
#define NODEFACTORY_H

#include "NodeContainer.h"
#include "nodes.h"

class NodeFactory {
public:
    static TypeNode& type(ObjectTypeNode& otype) {
        auto node = new TypeNode(otype);
        return *node;
    }
    static TypeNode& type(FunctionTypeNode& ftype) {
        auto node = new TypeNode(ftype);
        return *node;
    }
    static NodeContainer assign(NodeContainer lvalue, NodeContainer rvalue) {
        auto node = new AssignmentNode(lvalue, rvalue);
        return NodeContainer(node);
    }

    static FunctionTypeNode& ftype(std::vector<TypeNode> parameter_types, TypeNode return_type) {
        auto node = new FunctionTypeNode(parameter_types, return_type);
        return *node;
    }

    static ObjectTypeNode& otype(std::string identifier, std::vector<TypeNode> type_parameters) {
        auto node = new ObjectTypeNode(identifier, type_parameters);
        return *node;
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

    static NodeContainer cntinue() {
        auto node = new ContinueNode();
        return NodeContainer(node);
    }

    static NodeContainer call(NodeContainer function, std::vector<NodeContainer> arguments) {
        auto node = new CallNode(function, arguments);
        return NodeContainer(node);
    }

    static NodeContainer decl(std::string identifier, TypeNode* type, NodeContainer expression) {
        auto node = new DeclarationNode(identifier, type, expression);
        return NodeContainer(node);
    }

    static NodeContainer emptylst(TypeNode& type) {
        auto node = new EmptyListNode(type);
        return NodeContainer(node);
    }

    static NodeContainer id(std::string identifier) {
        auto node = new IdNode(identifier);
        return NodeContainer(node);
    }

    static NodeContainer
    iff(NodeContainer condition, BlockNode& body,
        std::vector<std::pair<NodeContainer, std::reference_wrapper<BlockNode>>> elifs,
        BlockNode _else) {
        auto node = new IfNode(condition, body, elifs, _else);
        return NodeContainer(node);
    }

    static NodeContainer retrn(NodeContainer expression) {
        auto node = new ReturnNode(expression);
        return NodeContainer(node);
    }

    static NodeContainer lst(std::vector<NodeContainer> elements) {
        auto node = new ListNode(elements);
        return NodeContainer(node);
    }

    static NodeContainer member(NodeContainer parent, std::string child) {
        auto node = new MemberNode(parent, child);
        return NodeContainer(node);
    }

    static NodeContainer function(std::string identifier, std::vector<std::string> parameter_names,
                                  std::vector<TypeNode> parameter_types, TypeNode return_type,
                                  BlockNode& body) {
        auto node = new FunctionNode(identifier, parameter_names, parameter_types, return_type, body);
        return NodeContainer(node);
    }

    static NodeContainer forloop(const std::string& var, NodeContainer exp, BlockNode& body) {
        auto node = new ForNode(var, exp, body);
        return NodeContainer(node);
    }

    static NodeContainer whileloop(NodeContainer condition, BlockNode& body) {
        auto node = new WhileNode(condition, body);
        return NodeContainer(node);
    }

    static NodeContainer number(int num) {
        auto node = new NumberNode(num);
        return NodeContainer(node);
    }

    static NodeContainer boolean(bool value) {
        auto node = new BooleanNode(value);
        return NodeContainer(node);
    }

    static NodeContainer sub(NodeContainer parent, std::vector<NodeContainer> children) {
        auto node = new SubscriptNode(parent, children);
        return NodeContainer(node);
    }

    static NodeContainer ternary(NodeContainer exp, NodeContainer true_case, NodeContainer false_case) {
        auto node = new TernaryNode(exp, true_case, false_case);
        return NodeContainer(node);
    }

    static NodeContainer strng(std::string str) {
        auto node = new StringNode(str);
        return NodeContainer(node);
    }

    static NodeContainer none() {
        auto node = new NoneNode();
        return NodeContainer(node);
    }

    static NodeContainer clslitexp(ObjectTypeNode& type, std::vector<NodeContainer> init) {
        auto node = new ClassLiteralExpressionNode(type, init);
        return NodeContainer(node);
    }

    static NodeContainer clslitfil(ObjectTypeNode& type, std::map<std::string, NodeContainer> init) {
        auto node = new ClassLiteralFieldNode(type, init);
        return NodeContainer(node);
    }

    static NodeContainer cls(std::string basicString, std::vector<std::string> vector,
                             std::map<std::string, TypeNode> map,
                             std::map<std::string, FunctionNode> map1) {
        auto node = new ClassNode(basicString, vector, map, map1);
        return NodeContainer(node);
    }
};

#endif //NODEFACTORY_H
