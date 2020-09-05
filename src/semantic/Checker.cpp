//
// Created by chris on 28/6/20.
//

#include <set>
#include "Checker.h"


Checker::Checker(SymbolTable* globals, ClassTable* class_table) {
    this->class_table = class_table;
    this->scope = globals;
    this->scopes["global"] = this->scope;
    this->class_table->set("Integer", new ClassInfo(std::vector<std::string>(), std::vector<TypeNode*>()));
    this->class_table->set("Option", new ClassInfo(std::vector<std::string>(), std::vector<TypeNode*>()));
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
        auto actual_type = dynamic_cast<ObjectTypeNode*>(n.type);
        if (actual_type->identifier == "Option") {
            if (!actual_type->type_parameters[0]->equal(expression_info.symbol_info)) {
                auto foo = dynamic_cast<ObjectTypeNode*>(expression_info.symbol_info);
                if (foo->identifier != "NoneType") {
                    throw AssignmentTypeError(n.type, expression_info.symbol_info);
                }
            }
        } else if (actual_type->identifier == "Union") {
            bool ok = false;
            for (int i = 0; i < actual_type->type_parameters.size(); i++) {
                if (actual_type->type_parameters[i]->equal(expression_info.symbol_info)) {
                    ok = true;
                    break;
                }
            }
            if (!ok) {
                throw AssignmentTypeError(n.type, expression_info.symbol_info);
            }
        } else {
            if (!n.type->equal(expression_info.symbol_info)) {
                throw AssignmentTypeError(n.type, expression_info.symbol_info);
            }
        }
        semantic_info.free_variables = expression_info.free_variables;
        semantic_info.symbol_info = n.type;
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
    if (!this->class_table->declared(object->identifier)) {
        throw std::runtime_error("Class " + object->identifier + " not declared!");
    }
    ClassInfo* class_info = this->class_table->get(object->identifier);
    if (class_info->fields.count(n.child) == 1) {
        // It's a field
        semantic_info.symbol_info = class_info->fields[n.child];
        this->rv = semantic_info;
    } else {
        throw std::runtime_error("Type " + object->to_string() + " has no member " + n.child);
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
    } else {
        auto left = dynamic_cast<ObjectTypeNode*>(left_info.symbol_info);
        auto right = dynamic_cast<ObjectTypeNode*>(right_info.symbol_info);
        if (left->identifier != "Integer" || right->identifier != "Integer") {
            throw std::runtime_error(
                    "Cannot perform binary op betweeen types " + left->to_string() + " and " + right->to_string());
        }

        semantic_info.symbol_info = new ObjectTypeNode("Integer", {});
    }
    this->rv = semantic_info;
}

void Checker::visit(ReturnNode& n) {
    n.expression->accept(*this);
    SemanticInfo expression_info = this->rv;
    TypeNode* return_type = this->scope->get("__return__");
    if (!this->can_assign(expression_info.symbol_info, return_type)) {
        throw ReturnError(return_type, expression_info.symbol_info);
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
    if (n.arguments.size() != function->parameter_types.size()) {
        throw std::runtime_error("Function called with wrong number of arguments");
    }

//        throw BadArguments(function->parameter_types, n.arguments);
    SemanticInfo semantic_info;

    semantic_info.free_variables = function_semantic_info.free_variables;
    VectorOfTypes args;
    for (int i = 0; i < n.arguments.size(); i++) {
        n.arguments[i]->accept(*this);
        SemanticInfo arg = this->rv;
        args.push_back(arg.symbol_info);
//        if (!arg.symbol_info->equal(function->parameter_types[i])) {
//            throw BadArguments();
//        }
        for (auto fv: arg.free_variables) {
            semantic_info.free_variables[fv.first] = 1;
        }
    }
    for (int i = 0; i < args.size(); i++) {
        if (!args[i]->equal(function->parameter_types[i])) {
            throw BadArguments(function->parameter_types, args);
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
                    "Field type doesn't match: " + class_info->field_names[i] + " ( field # " + std::to_string(i) +
                    " )" +
                    " expected " +
                    class_info->field_types[i]->to_string() + ", got " + semanticInfo.symbol_info->to_string());
        }
    }
    this->rv = SemanticInfo();
    rv.symbol_info = new ObjectTypeNode(node.identifier, {});
}

bool Checker::can_assign(TypeNode* from, TypeNode* to) {
    auto to_object = dynamic_cast<ObjectTypeNode*>(to);
    if (to_object->identifier == "Option") {
        if (!to_object->type_parameters[0]->equal(from)) {
            auto foo = dynamic_cast<ObjectTypeNode*>(from);
            if (foo->identifier != "NoneType") {
                return false;
            }
        }
        return true;
    } else if (to_object->identifier == "Union") {
        for (int i = 0; i < to_object->type_parameters.size(); i++) {
            if (to_object->type_parameters[i]->equal(from)) {
                return true;
            }
        }
        return false;
    }
    return to->equal(from);
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
        if (!this->can_assign(semanticInfo.symbol_info, this->class_table->get(node.identifier)->fields[f.first])) {
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
    auto element_type = this->rv.symbol_info;
    for (int i = 1; i < node.elements.size(); i++) {
        node.elements[i]->accept(*this);
        auto current_type = this->rv.symbol_info;
        if (!current_type->equal(element_type)) {
            throw std::runtime_error("List literal with more than one element type, first element has type: " +
                                     element_type->to_string() + " but at index " + std::to_string(i) + " got type " +
                                     current_type->to_string());
        }
    }
    SemanticInfo semantic_info = this->rv;
    SemanticInfo return_info;
    return_info.symbol_info = T_LIST(element_type);
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
    node.parent->accept(*this);
    SemanticInfo parent = this->rv;
    node.child->accept(*this);
    SemanticInfo child = this->rv;
    ObjectTypeNode* object_type = dynamic_cast<ObjectTypeNode*>(parent.symbol_info);
    if (object_type == nullptr) { throw std::runtime_error("Accessing subscript of non object!"); }
    SemanticInfo semantic_info;
    if (object_type->identifier == "List") {
        semantic_info.symbol_info = object_type->type_parameters[0];
    }
    this->rv = semantic_info;
}

void Checker::visit(BreakNode& node) {

}

void Checker::visit(TernaryNode& node) {
    node.expression->accept(*this);
    SemanticInfo expression_info = this->rv;
    auto expression_type = dynamic_cast<ObjectTypeNode*>(expression_info.symbol_info);
    if (expression_type == nullptr) {
        throw std::runtime_error("Unexpected non-object");
    }
    if (expression_type->identifier != "Option") {
        throw std::runtime_error("Expected an Option[T], got: " + expression_type->to_string());
    }
    SemanticInfo semanticInfo;
    TypeNode* type = expression_type->type_parameters[0];
    semanticInfo.symbol_info = type;
    this->enter_scope("true_case");
    this->scope->set("it", type);
    node.true_case->accept(*this);
    this->leave_scope();
    SemanticInfo true_case = this->rv;
    node.false_case->accept(*this);
    SemanticInfo false_case = this->rv;
    if (!false_case.symbol_info->equal(true_case.symbol_info)) {
        throw std::runtime_error(
                "True case and false case type don't match: " + true_case.symbol_info->to_string() + " != " +
                false_case.symbol_info->to_string());
//        semanticInfo.symbol_info = new ObjectTypeNode("Union", {true_case.symbol_info, false_case.symbol_info});
    } else {
        semanticInfo.symbol_info = true_case.symbol_info;
    }
    this->rv = semanticInfo;
}

void Checker::visit(NoneNode& node) {
    SemanticInfo semanticInfo;
    semanticInfo.symbol_info = new ObjectTypeNode("NoneType", {});
    this->rv = semanticInfo;
}
