//
// Created by chris on 28/6/20.
//

#include "GlobalProcessor.h"
#include "ClassInfo.h"
#include "../vm/Object.h"

void GlobalProcessor::add_builtin(std::string name, FunctionTypeNode* ftype) {
    this->globals->set(name, ftype);
}

GlobalProcessor::GlobalProcessor(std::map<std::string, CodeBuiltin*> builtins) {
    this->globals = new SymbolTable("global", nullptr);
    this->class_table = new ClassTable();
    for (auto b: builtins) {
        std::string params;
        for (auto p:b.second->ftype->parameter_types) {
            params += p->to_string() + ".";
        }
        params = params.substr(0, params.size() - 1);
        this->add_builtin(b.first + ":" + params, b.second->ftype);
    }
}

GlobalProcessor::GlobalProcessor() : GlobalProcessor(std::map<std::string, CodeBuiltin*>({})) {

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

void GlobalProcessor::visit(StructNode& node) {
    ClassInfo* class_info = new ClassInfo();
    for (auto f: node.fields) {
        class_info->field_names.push_back(f.first);
        class_info->field_types.push_back(f.second);
        class_info->fields[f.first] = f.second;
    }
    this->class_table->set(node.identifier, class_info);
}

void GlobalProcessor::visit(FunctionNode& node) {
    FunctionTypeNode* function_info = new FunctionTypeNode(node.parameter_types, node.return_type);
    std::string params;
    for (auto p:node.parameter_types) {
        params += p->to_string() + ".";
    }
    params = params.substr(0, params.size() - 1);
    std::string new_name = node.identifier + ":" + params;
    this->globals->set(new_name, function_info);
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

void GlobalProcessor::visit(ForNode& node) {

}

void GlobalProcessor::visit(BooleanNode& node) {

}

void GlobalProcessor::visit(WhileNode& node) {

}

void GlobalProcessor::visit(BreakNode& node) {

}

void GlobalProcessor::visit(TernaryNode& node) {

}

void GlobalProcessor::visit(NoneNode& node) {

}

void GlobalProcessor::visit(EmptyListNode& node) {

}



