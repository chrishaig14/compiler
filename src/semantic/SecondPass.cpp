//
// Created by chris on 28/6/20.
//

#include "SecondPass.h"


SecondPass::SecondPass(SymbolTable* globals, ClassTable* class_table) {
    this->class_table = class_table;
    this->scope = globals;
    this->scopes["global"] = this->scope;
    this->class_table->set("Integer",
                           new ClassInfo(std::map<std::string, SymbolInfo*>(), std::map<std::string, FunctionInfo*>()));
}

void SecondPass::enter_scope(std::string name) {
    std::string new_scope_name = this->scope->name + "." + name;
    if (this->scopes.count(new_scope_name) == 1) {
        this->scope = this->scopes[new_scope_name];
        return;
    }
    this->scope = new SymbolTable(new_scope_name, this->scope);
    this->scopes[new_scope_name] = this->scope;
}

void SecondPass::leave_scope() {
    this->scope = this->scope->parent;
}


void SecondPass::visit(FunctionNode& n) {
    this->enter_scope(n.identifier);
    for (int i = 0; i < n.parameter_names.size(); i++) {
        ObjectInfo* object_info = new ObjectInfo(n.parameter_types[i]);
        SymbolInfo* sinfo = new SymbolInfo(object_info);
        this->scope->set(n.parameter_names[i], sinfo);
    }
    this->scope->set("__return__", new SymbolInfo(new ObjectInfo(n.return_type)));
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

void SecondPass::visit(IdNode& n) {
    if (!this->scope->has(n.identifier)) {
        throw ScopeError(n.identifier);
    }
    SemanticInfo semantic_info;
    semantic_info.symbol_info = this->scope->get(n.identifier);
    semantic_info.free_variables[n.identifier] = 1;
    this->rv = semantic_info;
}

void SecondPass::visit(DeclarationNode& n) {
    if (this->scope->declared(n.identifier)) {
        throw RedeclareError(n.identifier);
    }
    SemanticInfo semantic_info;
    semantic_info.declared_variables[n.identifier] = 1;

    if (n.expression != nullptr and n.type != nullptr) {
        n.expression->accept(*this);
        SemanticInfo expression_info = this->rv;
        SymbolInfo* t = new SymbolInfo(new ObjectInfo(n.type));
        if (*t != *expression_info.symbol_info) {
            throw ReturnError(expression_info.symbol_info->object_info->parent, n.type->identifier);
        }
        semantic_info.free_variables = expression_info.free_variables;
    }
    this->scope->set(n.identifier, new SymbolInfo(new ObjectInfo(n.type)));
    this->rv = semantic_info;
}

void SecondPass::visit(AssignmentNode& n) {
    n.lvalue->accept(*this);
    SemanticInfo linfo = this->rv;
    n.rvalue->accept(*this);
    SemanticInfo expression_type = this->rv;
    if (*linfo.symbol_info != *expression_type.symbol_info) {
        throw ReturnError(expression_type.symbol_info->object_info->parent, linfo.symbol_info->object_info->parent);
    }
    SemanticInfo semantic_info;
    semantic_info.free_variables = expression_type.free_variables;
    for (auto fv: linfo.free_variables) {
        semantic_info.free_variables[fv.first] = 1;
    }
    this->rv = semantic_info;
}

void SecondPass::visit(MemberNode& n) {
    n.parent->accept(*this);
    SemanticInfo semantic_info = this->rv;
    if (!semantic_info.symbol_info->is_object()) {
        throw std::runtime_error("Accessing member " + n.child + " of non object");
    }
    ObjectInfo* object_info = semantic_info.symbol_info->object_info;
    ClassInfo* class_info = this->class_table->get(object_info->parent);
    if (class_info->fields.count(n.child) == 1) {
        // It's a field
        semantic_info.symbol_info = class_info->fields[n.child];
        this->rv = semantic_info;
    } else if (class_info->methods.count(n.child) == 1) {
        // It's a method
        semantic_info.symbol_info = new SymbolInfo(class_info->methods[n.child]);
        this->rv = semantic_info;
    } else {
        throw ScopeError(n.child);
    }
}

void SecondPass::visit(IfNode& n) {
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

void SecondPass::visit(BinopNode& n) {
    n.left->accept(*this);
    SemanticInfo left_info = this->rv;
    n.right->accept(*this);
    SemanticInfo right_info = this->rv;
    SemanticInfo semantic_info;
    semantic_info.symbol_info = new SymbolInfo(new ObjectInfo(new TypeNode("Integer", {})));
    semantic_info.free_variables = left_info.free_variables;
    for (auto fv: right_info.free_variables) {
        semantic_info.free_variables[fv.first] = 1;
    }
    this->rv = semantic_info;
}

void SecondPass::visit(ReturnNode& n) {
    n.expression->accept(*this);
    SemanticInfo expression_info = this->rv;
    SymbolInfo* return_type = this->scope->get("__return__");
    if (*expression_info.symbol_info != *return_type) {
        throw ReturnError(expression_info.symbol_info->object_info->parent, return_type->object_info->parent);
    }
    SemanticInfo semantic_info;
    semantic_info.free_variables = expression_info.free_variables;
    this->rv = semantic_info;
}

void SecondPass::visit(CallNode& n) {
    n.function->accept(*this);
    SemanticInfo function_semantic_info = this->rv;
    if (!function_semantic_info.symbol_info->is_function()) {
        throw std::runtime_error("Expected a function! Got something else!");
    }
    if (n.arguments.size() != function_semantic_info.symbol_info->function_info->parameter_types.size())
        throw BadArguments();

    for (int i = 0; i < n.arguments.size(); i++) {
        n.arguments[i]->accept(*this);
        SemanticInfo arg = this->rv;
        if (*arg.symbol_info->object_info != *function_semantic_info.symbol_info->function_info->parameter_types[i]) {
            throw BadArguments();
        }
    }
    SemanticInfo semantic_info;
    semantic_info.symbol_info = new SymbolInfo(function_semantic_info.symbol_info->function_info->return_type);
    this->rv = semantic_info;
}

void SecondPass::visit(ClassNode& n) {
    for (int i = 0; i < n.methods.size(); i++) {
        this->enter_scope(n.methods[i]->identifier);
        TypeNode* type_node = new TypeNode(n.identifier, {});
        ObjectInfo* simple_info = new ObjectInfo(type_node);
        SymbolInfo* this_info = new SymbolInfo(simple_info);
        this->scope->set("this", this_info);
        this->leave_scope();
        n.methods[i]->accept(*this);
    }
}

void SecondPass::visit(BlockNode& program) {
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
