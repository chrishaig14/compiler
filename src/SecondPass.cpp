//
// Created by chris on 28/6/20.
//

#include "SecondPass.h"


SymbolInfo* w_finfo(VectorOfTypes parameter_types, TypeNode* return_type) {
    FunctionInfo* finfo = new FunctionInfo(parameter_types, return_type);
    SymbolInfo* ginfo = new SymbolInfo(finfo);
    return ginfo;
}

SymbolInfo* w_sinfo(std::string type) {
    ObjectInfo* sinfo = new ObjectInfo(i_type(type, {}));
    SymbolInfo* ginfo = new SymbolInfo(sinfo);
    return ginfo;
}

FunctionInfo* f_info(VectorOfTypes parameter_types, TypeNode* return_type) {
    return new FunctionInfo(parameter_types, return_type);
}

ObjectInfo* s_info(TypeNode* type) {
    return new ObjectInfo(type);
}

SymbolInfo* wrap_function_info(FunctionInfo* finfo) {
    SymbolInfo* ginfo = new SymbolInfo(finfo);
    return ginfo;
}

SymbolInfo* wrap_simple_info(ObjectInfo* sinfo) {
    SymbolInfo* ginfo = new SymbolInfo(sinfo);
    return ginfo;
}

RedeclareError::RedeclareError(std::string name) : runtime_error("Name " + name + "already declared in current scope") {
}

bool RedeclareError::operator==(const RedeclareError &other) const {
    std::string a = this->what();
    std::string b = other.what();
    return a == b;
}

bool ReturnError::operator==(const ReturnError &other) const {
    std::string a = this->what();
    std::string b = other.what();
    bool t = a == b;
    return t;
}

ReturnError::ReturnError(std::string actual_type, std::string expected_type) : runtime_error(
        "Expected to return " + expected_type + ", actual: " + actual_type) {

}

bool ScopeError::operator==(const ScopeError &other) const {
    std::string a = this->what();
    std::string b = other.what();
    bool t = a == b;
    return t;
}

ScopeError::ScopeError(std::string name) : runtime_error("Name " + name + "not found in current scope") {
}

SecondPass::SecondPass(SymbolTable* globals, ClassTable* class_table) {
    this->class_table = class_table;
    this->scope = globals;
    this->scopes["global"] = this->scope;
    this->class_table->set("Integer", new ClassInfo(MapStringToSimple(), std::map<std::string, FunctionInfo*>()));
//    this->scope->set("Integer", w_cinfo());
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

SemanticInfo* SecondPass::analyze(FunctionNode* n) {
    this->enter_scope(n->name);
    for (int i = 0; i < n->parameter_names.size(); i++) {
        ObjectInfo* object_info = new ObjectInfo(n->parameter_types[i]);
        SymbolInfo* sinfo = new SymbolInfo(object_info);
        this->scope->set(n->parameter_names[i], sinfo);
    }
    this->scope->set("__return__", wrap_simple_info(new ObjectInfo(n->return_type)));
    SemanticInfo* body_info = this->analyze(n->body);
    this->leave_scope();
    SemanticInfo* semantic_info = new SemanticInfo;
//    semantic_info->symbol_info = ;
    for (auto fv: body_info->free_variables) {
        bool is_a_param = false;
        for (auto p: n->parameter_names) {
            if (fv.first == p) {
                is_a_param = true;
                break;
            }
        }
        if (!is_a_param) {
            semantic_info->free_variables[fv.first] = 1;
        }
    }
    return semantic_info;
}

SemanticInfo* SecondPass::analyze(IdentifierNode* n) {
    if (!this->scope->has(n->name)) {
        throw ScopeError(n->name);
    }
    SemanticInfo* semantic_info = new SemanticInfo;
    semantic_info->symbol_info = this->scope->get(n->name);
    return semantic_info;
}

SemanticInfo* SecondPass::analyze(DeclarationNode* n) {
    if (this->scope->declared(n->identifier)) {
        throw RedeclareError(n->identifier);
    }
    SemanticInfo* semantic_info = new SemanticInfo;
    semantic_info->declared_variables[n->identifier] = 1;

    if (n->expression != nullptr and n->type != nullptr) {
        SemanticInfo* expression_info = this->analyze(n->expression);
        if (!equal(wrap_simple_info(new ObjectInfo(n->type)), expression_info->symbol_info)) {
            throw ReturnError(expression_info->symbol_info->object_info->parent, n->type->name);
        }
        semantic_info->free_variables = expression_info->free_variables;
    }
    this->scope->set(n->identifier, wrap_simple_info(new ObjectInfo(n->type)));
    return semantic_info;
}

SemanticInfo* SecondPass::analyze(AssignmentNode* n) {
    SemanticInfo* linfo = this->analyze(n->lvalue);
    SemanticInfo* expression_type = this->analyze(n->rvalue);
    if (!equal(linfo->symbol_info, expression_type->symbol_info)) {
        throw ReturnError(expression_type->symbol_info->object_info->parent, linfo->symbol_info->object_info->parent);
    }
    SemanticInfo* semantic_info = new SemanticInfo;
    semantic_info->free_variables = expression_type->free_variables;
    for (auto fv: linfo->free_variables) {
        semantic_info->free_variables[fv.first] = 1;
    }
    return semantic_info;
}

SemanticInfo* SecondPass::analyze(MemberNode* n) {
    SemanticInfo* semantic_info = this->analyze(n->parent);
    if (!semantic_info->symbol_info->is_object()) {
        throw std::runtime_error("Accessing member " + n->child + " of non object");
    }
    ObjectInfo* object_info = semantic_info->symbol_info->object_info;
    ClassInfo* class_info = this->class_table->get(object_info->parent);
    if (class_info->fields.count(n->child) == 1) {
        // It's a field
        semantic_info->symbol_info = wrap_simple_info(class_info->fields[n->child]);
        return semantic_info;
    }
    if (class_info->methods.count(n->child) == 1) {
        // It's a method
        semantic_info->symbol_info = wrap_function_info(class_info->methods[n->child]);
        return semantic_info;
    }
    throw ScopeError(n->child);
}

SemanticInfo* SecondPass::analyze(IfNode* n) {
    SemanticInfo* semantic_info = new SemanticInfo;
    SemanticInfo* condition_info = this->analyze(n->condition);
    this->enter_scope("if");
    SemanticInfo* then_info = this->analyze(n->then);
    this->leave_scope();
    semantic_info->free_variables = condition_info->free_variables;
    for (auto fv: then_info->free_variables) {
        semantic_info->free_variables[fv.first] = 1;
    }
    return semantic_info;
}

SemanticInfo* SecondPass::analyze(BinopNode* n) {
    SemanticInfo* left_info = this->analyze(n->left);
    SemanticInfo* right_info = this->analyze(n->right);
    SemanticInfo* semantic_info = new SemanticInfo;
    semantic_info->symbol_info = wrap_simple_info(new ObjectInfo(new TypeNode("Integer", {})));
    semantic_info->free_variables = left_info->free_variables;
    for (auto fv: right_info->free_variables) {
        semantic_info->free_variables[fv.first] = 1;
    }
    return semantic_info;
}

SemanticInfo* SecondPass::analyze(ReturnNode* n) {
    SemanticInfo* expression_info = this->analyze(n->expression);
    SymbolInfo* return_type = this->scope->get("__return__");
    if (!equal(expression_info->symbol_info, return_type)) {
        throw ReturnError(expression_info->symbol_info->object_info->parent, return_type->object_info->parent);
    }
    SemanticInfo* semantic_info = new SemanticInfo;
    semantic_info->free_variables = expression_info->free_variables;
    return semantic_info;
}

SemanticInfo* SecondPass::analyze(CallNode* n) {
    SemanticInfo* function_semantic_info = this->analyze(n->function);
    if (!function_semantic_info->symbol_info->is_function()) {
        throw std::runtime_error("Expected a function! Got something else!");
    }
    if (n->arguments.size() != function_semantic_info->symbol_info->function_info->parameter_types.size())
        throw BadArguments();

    for (int i = 0; i < n->arguments.size(); i++) {
        SemanticInfo* arg = this->analyze(n->arguments[i]);
        if (!equal(arg->symbol_info->object_info,
                   function_semantic_info->symbol_info->function_info->parameter_types[i])) {
            throw BadArguments();
        }
    }
    SemanticInfo* semantic_info = new SemanticInfo;
    semantic_info->symbol_info = wrap_simple_info(function_semantic_info->symbol_info->function_info->return_type);
    return semantic_info;
}

SemanticInfo* SecondPass::analyze(ClassNode* n) {
    for (int i = 0; i < n->methods.size(); i++) {
        this->enter_scope(n->methods[i]->name);
        TypeNode* type_node = new TypeNode(n->name, {});
        ObjectInfo* simple_info = new ObjectInfo(type_node);
        SymbolInfo* this_info = new SymbolInfo(simple_info);
        this->scope->set("this", this_info);
        this->leave_scope();
        this->analyze(n->methods[i]);
    }
    return new SemanticInfo;
}

SemanticInfo* SecondPass::analyze(AstNode* n) {
    switch (n->type) {
        case AstType::FUNCTION:
            return this->analyze(n->ast_function);
        case AstType::IDENTIFIER:
            return this->analyze(n->ast_identifier);
        case AstType::IF:
            return this->analyze(n->ast_if);
        case AstType::RETURN:
            return this->analyze(n->ast_return);
        case AstType::LIST:
            break;
        case AstType::CLASS:
            return this->analyze(n->ast_class);
        case AstType::MEMBER:
            return this->analyze(n->ast_member);
        case AstType::ASSIGNMENT:
            return this->analyze(n->ast_assignment);
            break;
        case AstType::BINOP:
            return this->analyze(n->ast_binop);
        case AstType::DECLARATION:
            return this->analyze(n->ast_declaration);
        case AstType::TYPE:
            break;
        case AstType::NUMBER: {
            SemanticInfo* semantic_info = new SemanticInfo;
            semantic_info->symbol_info = w_sinfo("Integer");
            return semantic_info;
        }
        case AstType::STRING: {
            SemanticInfo* semantic_info = new SemanticInfo;
            semantic_info->symbol_info = w_sinfo("String");
            return semantic_info;
        }
        case AstType::CALL:
            return this->analyze(n->ast_call);
        default:
            throw std::runtime_error("Dont know what to do with node: " + ast_string(n->type));
        case AstType::SUB:
            break;
    }
    return nullptr;
}

SemanticInfo* SecondPass::analyze(VectorOfNodes program) {
    SemanticInfo* semantic_info = new SemanticInfo;
    for (auto n: program) {
        SemanticInfo* node_info = this->analyze(n);
        for (auto fv: node_info->free_variables) {
            if (semantic_info->declared_variables.count(fv.first) == 0) {
                semantic_info->free_variables[fv.first] = 1;
            }
        }
        for (auto fv: node_info->declared_variables) {
            semantic_info->declared_variables[fv.first] = 1;
        }
    }
    return semantic_info;
}

bool BadArguments::operator==(const BadArguments &other) const {
    return true;
}

BadArguments::BadArguments() : std::runtime_error("Bad Arguments") {
}
