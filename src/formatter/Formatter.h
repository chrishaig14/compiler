////
//// Created by chris on 26/8/20.
////
//
//#ifndef FORMATTER_H
//#define FORMATTER_H
//
//#include <map>
//#include "../nodes/Visitor.h"
//#include "../nodes/nodes.h"
//
//class Formatter : public Visitor {
//    bool semicolon;
//    int indent_level = 0;
//
//    inline std::string indentation() {
//        return std::string(this->indent_level * 4, ' ');
//    }
//
//    void visit(AssignmentNode& node) override;
//
//    void visit(BinopNode& node) override;
//
//    void visit(BreakNode& node) override;
//
//    void visit(BlockNode& node) override;
//
//    void visit(CallNode& node) override;
//
//    void visit(ClassLiteralExpressionNode& node) override;
//
//    void visit(ClassLiteralFieldNode& node) override;
//
//    void visit(DeclarationNode& node) override;
//
//    void visit(ForNode& node) override;
//
//    void visit(FunctionNode& node) override;
//
//    void visit(BooleanNode& node) override;
//
//    void visit(IdNode& node) override;
//
//    void visit(IfNode& node) override;
//
//    void visit(ListNode& node) override;
//
//    void visit(MemberNode& node) override;
//
//    void visit(NumberNode& node) override;
//
//    void visit(ReturnNode& node) override;
//
//    void visit(StringNode& node) override;
//
//    void visit(StructNode& node) override;
//
//    void visit(SubscriptNode& node) override;
//
//    void visit(TypeNode* node) override;
//
//    void visit(WhileNode& node) override;
//
//public:
//    void visit(TernaryNode& node) override;
//
//    void visit(NoneNode& node) override;
//
//    void visit(EmptyListNode& node) override;
//
//    void visit(InstanceNode& node) override;
//
//    void visit(ClassNode& node) override;
//
//public:
//    std::string output;
//};
//
//
//#endif //FORMATTER_H
