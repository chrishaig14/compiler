//
// Created by chris on 28/6/20.
//

#include "Checker.h"


Checker::Checker(SymbolTable* globals, ClassTable* class_table) {
    this->class_table = class_table;
    this->scope = globals;
    this->scopes["global"] = this->scope;
    this->class_table->set("Integer", new ClassInfo(std::vector<std::string>(), std::vector<TypeNode*>()));
}

void Checker::enter_scope(std::string name) {
    std::string new_scope_name = this->scope->name + "." + name;
    if (this->scopes.count(new_scope_name) == 1) {
        this->scope = this->scopes[new_scope_name];
        return;
    }
    this->scope = new SymbolTable(new_scope_name, this->scope);
    this->scopes[new_scope_name] = this->scope;
}

void Checker::leave_scope() {
    this->scope = this->scope->parent;
}


void Checker::visit(FunctionNode& n) {
    this->enter_scope(n.identifier);
    for (int i = 0; i < n.parameter_names.size(); i++) {
        this->scope->set(n.parameter_names[i], n.parameter_types[i]);
    }
    this->scope->set("__return__", n.return_type);
    n.body->accept(*this);
    SemanticInfo body_info = this->rv;
    this->leave_scope();
    SemanticInfo semantic_info;
//    semantic_info.symbol_info = ;
    for (auto fv: body_info.free_variables) {
        bool is_a_param = false;
        for (auto p: n.parameter_names) {
            if (fv.first == p) {
                is_a_param = true;
                break;
            }
        }
        if (!is_a_param) {
            semantic_info.free_variables[fv.first] = 1;
        }
    }
    n.free_variables = semantic_info.free_variables;
    this->rv = semantic_info;
}

void Checker::visit(IdNode& n) {
    if (!this->scope->has(n.identifier)) {
        throw ScopeError(n.identifier);
    }
    SemanticInfo semantic_info;
    semantic_info.symbol_info = this->scope->get(n.identifier);
    semantic_info.free_variables[n.identifier] = 1;
    this->rv = semantic_info;
}

void Checker::visit(DeclarationNode& n) {
    if (this->scope->declared(n.identifier)) {
        throw RedeclareError(n.identifier);
    }
    SemanticInfo semantic_info;
    semantic_info.declared_variables[n.identifier] = 1;

    if (n.expression != nullptr and n.type != nullptr) {
        n.expression->accept(*this);
        SemanticInfo expression_info = this->rv;
        if (!n.type->equal(expression_info.symbol_info)) {
            throw AssignmentTypeError(n.type, expression_info.symbol_info);
        }
        semantic_info.free_variables = expression_info.free_variables;
        semantic_info.symbol_info = expression_info.symbol_info;
    } else if (n.expression != nullptr) {
        n.expression->accept(*this);
        SemanticInfo expression_info = this->rv;
        semantic_info.free_variables = expression_info.free_variables;
        semantic_info.symbol_info = expression_info.symbol_info;
    }
    this->rv = semantic_info;
    this->scope->set(n.identifier, semantic_info.symbol_info);
}

void Checker::visit(AssignmentNode& n) {
    n.lvalue->accept(*this);
    SemanticInfo linfo = this->rv;
    n.rvalue->accept(*this);
    SemanticInfo expression_type = this->rv;
    if (!linfo.symbol_info->equal(expression_type.symbol_info)) {
        throw AssignmentTypeError(linfo.symbol_info, expression_type.symbol_info);
    }
    SemanticInfo semantic_info;
    semantic_info.free_variables = expression_type.free_variables;
    for (auto fv: linfo.free_variables) {
        semantic_info.free_variables[fv.first] = 1;
    }
    this->rv = semantic_info;
}

void Checker::visit(MemberNode& n) {
    n.parent->accept(*this);
    SemanticInfo semantic_info = this->rv;
    ObjectTypeNode* object = dynamic_cast<ObjectTypeNode*>(semantic_info.symbol_info);
    if (object == nullptr) {
        throw std::runtime_error("Accessing member " + n.child + " of non object");
    }
    ClassInfo* class_info = this->class_table->get(object->identifier);
    if (class_info->fields.count(n.child) == 1) {
        // It's a field
        semantic_info.symbol_info = class_info->fields[n.child];
        this->rv = semantic_info;
    } else {
        throw ScopeError(n.child);
    }
}

void Checker::visit(IfNode& n) {
    SemanticInfo semantic_info;
    n.condition->accept(*this);
    SemanticInfo condition_info = this->rv;
    this->enter_scope("if");
    n.then->accept(*this);
    SemanticInfo then_info = this->rv;
    this->leave_scope();
    semantic_info.free_variables = condition_info.free_variables;
    for (auto fv: then_info.free_variables) {
        semantic_info.free_variables[fv.first] = 1;
    }
    this->rv = semantic_info;
}

void Checker::visit(BinopNode& n) {
    n.left->accept(*this);
    SemanticInfo left_info = this->rv;
    n.right->accept(*this);
    SemanticInfo right_info = this->rv;
    SemanticInfo semantic_info;
    semantic_info.free_variables = left_info.free_variables;
    for (auto fv: right_info.free_variables) {
        semantic_info.free_variables[fv.first] = 1;
    }
    bool is_boolean = true;
    switch (n.op) {
        case OpType::EQ:
            break;
        case OpType::AND:
            break;
        case OpType::OR:
            break;
        case OpType::LEQ:
            break;
        case OpType::GEQ:
            break;
        case OpType::LT:
            break;
        case OpType::GT:
            break;
        case OpType::NEQ:
            break;
        default:
            is_boolean = false;
    }
    if (is_boolean) {
        semantic_info.symbol_info = new ObjectTypeNode("Boolean", {});
    } else { semantic_info.symbol_info = new ObjectTypeNode("Integer", {}); }
    this->rv = semantic_info;
}

void Checker::visit(ReturnNode& n) {
    n.expression->accept(*this);
    SemanticInfo expression_info = this->rv;
    TypeNode* return_type = this->scope->get("__return__");
    if (!expression_info.symbol_info->equal(return_type)) {
        throw ReturnError(expression_info.symbol_info, return_type);
    }
    SemanticInfo semantic_info;
    semantic_info.free_variables = expression_info.free_variables;
    this->rv = semantic_info;
}

void Checker::visit(CallNode& n) {
    n.function->accept(*this);
    SemanticInfo function_semantic_info = this->rv;
    FunctionTypeNode* function = dynamic_cast<FunctionTypeNode*>(function_semantic_info.symbol_info);
    if (function == nullptr) {
        throw std::runtime_error("Expected a function! Got something else!");
    }
    if (n.arguments.size() != function->parameter_types.size())
        throw BadArguments();
    SemanticInfo semantic_info;

    semantic_info.free_variables = function_semantic_info.free_variables;

    for (int i = 0; i < n.arguments.size(); i++) {
        n.arguments[i]->accept(*this);
        SemanticInfo arg = this->rv;
        if (!arg.symbol_info->equal(function->parameter_types[i])) {
            throw BadArguments();
        }
        for (auto fv: arg.free_variables) {
            semantic_info.free_variables[fv.first] = 1;
        }
    }
    semantic_info.symbol_info = function->return_type;
    this->rv = semantic_info;
}

void Checker::visit(StructNode& n) {
}

void Checker::visit(BlockNode& program) {
    SemanticInfo semantic_info;
    for (auto n: program.nodes) {
        n->accept(*this);
        SemanticInfo node_info = this->rv;
        for (auto fv: node_info.free_variables) {
            if (semantic_info.declared_variables.count(fv.first) == 0) {
                semantic_info.free_variables[fv.first] = 1;
            }
        }
        for (auto fv: node_info.declared_variables) {
            semantic_info.declared_variables[fv.first] = 1;
        }
    }
    this->rv = semantic_info;
}

void Checker::visit(ClassLiteralExpressionNode& node) {
    if (!this->class_table->declared(node.identifier)) throw std::runtime_error("No struct named " + node.identifier);
    ClassInfo* class_info = this->class_table->get(node.identifier);
    auto class_fields = class_info->fields;
    if (class_fields.size() != node.init.size())
        throw std::runtime_error(
                "In struct \"" + node.identifier + "\" initialization: " + "Expected " +
                std::to_string(class_fields.size()) + " initializers but got " +
                std::to_string(node.init.size()));
    for (int i = 0; i < node.init.size(); i++) {
        Node* exp = node.init[i];
        exp->accept(*this);
        SemanticInfo semanticInfo = this->rv;
        if (!semanticInfo.symbol_info->equal(class_info->field_types[i])) {
            throw std::runtime_error(
                    "Field type doesn't match: " + class_info->field_names[i] + " ( field # " + std::to_string(i) + " )" +
                    " expected " +
                    class_info->field_types[i]->to_string() + ", got " + semanticInfo.symbol_info->to_string());
        }
    }
    this->rv = SemanticInfo();
    rv.symbol_info = new ObjectTypeNode(node.identifier, {});
}

void Checker::visit(ClassLiteralFieldNode& node) {
    if (!this->class_table->declared(node.identifier)) throw std::runtime_error("No struct named " + node.identifier);
    auto class_fields = this->class_table->get(node.identifier)->fields;
    for (auto f: node.init) {
        if (class_fields.count(f.first) == 0) throw std::runtime_error("No field named " + f.first);
    }
    if (class_fields.size() != node.init.size())
        throw std::runtime_error(
                "In struct \"" + node.identifier + "\" initialization: " + "Expected " +
                std::to_string(class_fields.size()) + " initializers but got " +
                std::to_string(node.init.size()));
    for (auto f: node.init) {
        Node* exp = f.second;
        exp->accept(*this);
        SemanticInfo semanticInfo = this->rv;
        if (!semanticInfo.symbol_info->equal(this->class_table->get(node.identifier)->fields[f.first])) {
            throw std::runtime_error(
                    "In struct \"" + node.identifier + "\" initialization: " + "field \"" + f.first + "\" is of type " +
                    class_fields[f.first]->to_string() +
                    " but got " + semanticInfo.symbol_info->to_string());
        }
    }
    this->rv = SemanticInfo();
    rv.symbol_info = new ObjectTypeNode(node.identifier, {});
}

void Checker::visit(ForNode& node) {
    node.exp->accept(*this);
    SemanticInfo semantic_info = this->rv;
    ObjectTypeNode* obj = dynamic_cast<ObjectTypeNode*>(semantic_info.symbol_info);
    if (obj == nullptr) {
        throw std::runtime_error("Iterating over something bad!");
    }
    TypeNode* var_type = nullptr;
    if (obj->identifier == "List") {
        var_type = obj->type_parameters[0];
    }
    this->enter_scope("for");
    this->scope->set(node.var, var_type);
    node.body->accept(*this);
    this->leave_scope();
}

void Checker::visit(ListNode& node) {
    node.elements[0]->accept(*this);
    SemanticInfo semantic_info = this->rv;
    SemanticInfo return_info;
    return_info.symbol_info = new ObjectTypeNode("List", {semantic_info.symbol_info});
    this->rv = return_info;
}

void Checker::visit(BooleanNode& node) {
    SemanticInfo semantic_info;
    semantic_info.symbol_info = new ObjectTypeNode("Boolean", {});
    this->rv = semantic_info;
}

void Checker::visit(WhileNode& node) {
    node.condition->accept(*this);
    SemanticInfo condition = this->rv;
    if (!condition.symbol_info->equal(new ObjectTypeNode("Boolean", {}))) {
        throw std::runtime_error("Expected boolean expression as while loop condition!");
    }
    this->enter_scope("while");
    node.body->accept(*this);
    this->leave_scope();
}

void Checker::visit(NumberNode& node) {
    SemanticInfo semanticInfo;
    semanticInfo.symbol_info = new ObjectTypeNode("Integer", {});
    this->rv = semanticInfo;
}

void Checker::visit(StringNode& node) {
    SemanticInfo semanticInfo;
    semanticInfo.symbol_info = new ObjectTypeNode("String", {});
    this->rv = semanticInfo;
}

void Checker::visit(SubscriptNode& node) {

}

void Checker::visit(BreakNode& node) {

}
