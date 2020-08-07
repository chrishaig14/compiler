//
// Created by chris on 28/6/20.
//

#include "GlobalProcessor.h"
#include "ClassInfo.h"

GlobalProcessor::GlobalProcessor() {
    this->globals = new SymbolTable("global", nullptr);
    this->class_table = new ClassTable();
}

void GlobalProcessor::visit(AssignmentNode& node) {

}

void GlobalProcessor::visit(BinopNode& node) {

}

void GlobalProcessor::visit(CallNode& node) {

}

void GlobalProcessor::visit(DeclarationNode& node) {

}

void GlobalProcessor::visit(IdNode& node) {

}

void GlobalProcessor::visit(IfNode& node) {

}

void GlobalProcessor::visit(ListNode& node) {

}

void GlobalProcessor::visit(MemberNode& node) {

}

void GlobalProcessor::visit(NumberNode& node) {

}

void GlobalProcessor::visit(ReturnNode& node) {

}

void GlobalProcessor::visit(StringNode& node) {

}

void GlobalProcessor::visit(SubscriptNode& node) {

}

void GlobalProcessor::visit(TypeNode& node) {

}

void GlobalProcessor::visit(ClassNode& node) {
    ClassInfo* class_info = new ClassInfo();
    for (auto f: node.fields) {
        class_info->fields[f->identifier] = new SymbolInfo(new ObjectInfo(f->type));
    }
    for (auto m: node.methods) {
        class_info->methods[m->identifier] = new FunctionInfo(m->parameter_types, m->return_type);
    }
    this->class_table->set(node.identifier, class_info);
}

void GlobalProcessor::visit(FunctionNode& node) {
    FunctionInfo* function_info = new FunctionInfo(node.parameter_types, node.return_type);
    SymbolInfo* ginfo = new SymbolInfo(function_info);
    this->globals->set(node.identifier, ginfo);
}

void GlobalProcessor::visit(VectorOfNodes program) {
    for (auto n: program) {
        n->accept(*this);
    }
}

void GlobalProcessor::visit(BlockNode& node) {
    for (auto n: node.nodes) {
        n->accept(*this);
    }
}

void GlobalProcessor::visit(ClassLiteralExpressionNode& node) {

}

void GlobalProcessor::visit(ClassLiteralFieldNode& node) {

}



