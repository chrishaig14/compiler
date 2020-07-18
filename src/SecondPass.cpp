//
// Created by chris on 28/6/20.
//

#include "SecondPass.h"

SymbolInfo* w_cinfo(MapStringToSimple fields, MapStringToFunction methods) {
    SymbolInfo* ginfo = new SymbolInfo;
    ginfo->type = SINFO::CLASS;
    ClassInfo* cinfo = new ClassInfo;
    cinfo->methods = methods;
    cinfo->fields = fields;
    ginfo->class_info = cinfo;
    return ginfo;
}

SymbolInfo* w_finfo(VectorOfTypes parameter_types, TypeNode* return_type) {
    SymbolInfo* ginfo = new SymbolInfo;
    ginfo->type = SINFO::FUNCTION;
    FunctionInfo* finfo = new FunctionInfo(parameter_types, return_type);
    ginfo->function_info = finfo;
    return ginfo;
}

SymbolInfo* w_sinfo(std::string type) {
    SymbolInfo* ginfo = new SymbolInfo;
    ginfo->type = SINFO::SIMPLE;
    SimpleInfo* sinfo = new SimpleInfo(i_type(type, {}));
    ginfo->simple_info = sinfo;
    return ginfo;
}

FunctionInfo* f_info(VectorOfTypes parameter_types, TypeNode* return_type) {
    return new FunctionInfo(parameter_types, return_type);
}

SimpleInfo* s_info(TypeNode* type) {
    return new SimpleInfo(type);
}

SymbolInfo* wrap_function_info(FunctionInfo* finfo) {
    SymbolInfo* ginfo = new SymbolInfo;
    ginfo->type = SINFO::FUNCTION;
    ginfo->function_info = finfo;
    return ginfo;
}

SymbolInfo* wrap_simple_info(SimpleInfo* sinfo) {
    SymbolInfo* ginfo = new SymbolInfo;
    ginfo->type = SINFO::SIMPLE;
    ginfo->simple_info = sinfo;
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
    std::cout << "COMPARING ERRORS" << std::endl;
    std::string a = this->what();
    std::string b = other.what();
    bool t = a == b;
    return t;
}

ReturnError::ReturnError(std::string actual_type, std::string expected_type) : runtime_error(
        "Expected to return " + expected_type + ", actual: " + actual_type) {

}

bool ScopeError::operator==(const ScopeError &other) const {
    std::cout << "COMPARING ERRORS" << std::endl;
    std::string a = this->what();
    std::string b = other.what();
    bool t = a == b;
    return t;
}

ScopeError::ScopeError(std::string name) : runtime_error("Name " + name + "not found in current scope") {
}

SecondPass::SecondPass(SymbolTable* globals) {
    this->scope = globals;
    this->scopes["global"] = this->scope;
    this->scope->set("Integer", w_cinfo(MapStringToSimple(), MapStringToFunction()));
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

SymbolInfo* SecondPass::analyze(FunctionNode* n) {
    this->enter_scope(n->name);
    for (int i = 0; i < n->parameter_names.size(); i++) {
        SymbolInfo* sinfo = new SymbolInfo();
        sinfo->type = SINFO::SIMPLE;
        sinfo->simple_info = new SimpleInfo(n->parameter_types[i]);
        this->scope->set(n->parameter_names[i], sinfo);
    }
    this->scope->set("__return__", wrap_simple_info(new SimpleInfo(n->return_type)));
    this->analyze(n->body);
    this->leave_scope();
    return nullptr;
}

SymbolInfo* SecondPass::analyze(IdentifierNode* n) {
    if (!this->scope->has(n->name)) {
        throw ScopeError(n->name);
    }
    return this->scope->get(n->name);
}

SymbolInfo* SecondPass::analyze(DeclarationNode* n) {
    if (this->scope->declared(n->identifier)) {
        throw RedeclareError(n->identifier);
    }
    this->scope->set(n->identifier, NULL);
    return nullptr;
}

SymbolInfo* SecondPass::analyze(MemberNode* n) {
    SymbolInfo* symbol_info = this->analyze(n->parent);
    if (symbol_info->type != SINFO::SIMPLE) {
        throw std::runtime_error("Accessing member " + n->child + " of non object");
    }
    SimpleInfo* simple_info = symbol_info->simple_info;
    ClassInfo* class_info = this->scope->get(simple_info->parent)->class_info;
    if (class_info->fields.count(n->child) == 1) {
        // It's a field
        return wrap_simple_info(class_info->fields[n->child]);
    }
    if (class_info->methods.count(n->child) == 1) {
        // It's a method
        return wrap_function_info(class_info->methods[n->child]);
    }
    throw ScopeError(n->child);
}

SymbolInfo* SecondPass::analyze(IfNode* n) {
    this->analyze(n->condition);
    this->enter_scope("if");
    this->analyze(n->then);
    this->leave_scope();
    return nullptr;
}

SymbolInfo* SecondPass::analyze(BinopNode* n) {
    this->analyze(n->left);
    this->analyze(n->right);
    return this->scope->get("Integer"); //TODO transform all binary operations into function calls
}

SymbolInfo* SecondPass::analyze(ReturnNode* n) {
    SymbolInfo* symbol_info = this->analyze(n->expression);
    SymbolInfo* return_type = this->scope->get("__return__");
    if (!equal(symbol_info, return_type)) {
        throw ReturnError(symbol_info->simple_info->parent, return_type->simple_info->parent);
    }
    return nullptr;
}

SymbolInfo* SecondPass::analyze(CallNode* n) {
    SymbolInfo* function_info = this->analyze(n->function);
    if (function_info->type != SINFO::FUNCTION){
        throw std::runtime_error("Expected a function! Got something else!");
    }
    return wrap_simple_info(function_info->function_info->return_type);
//    this->analyze(n->left);
//    this->analyze(n->right);
//    return this->scope->get("Integer"); //TODO transform all binary operations into function calls
}

SymbolInfo* SecondPass::analyze(ClassNode* n) {
    for (int i = 0; i < n->methods.size(); i++) {
        this->enter_scope(n->methods[i]->name);
        SymbolInfo* this_info = new SymbolInfo;
        this_info->type = SINFO::SIMPLE;
        TypeNode* type_node = new TypeNode(n->name, {});

        SimpleInfo* simple_info = new SimpleInfo(type_node);
        this_info->simple_info = simple_info;
        this->scope->set("this", this_info);
        this->leave_scope();
        this->analyze(n->methods[i]);
    }
    return nullptr;
}

SymbolInfo* SecondPass::analyze(AstNode* n) {
    switch (n->type) {
        case AstType::FUNCTION:
            return this->analyze(n->ast_function);
            break;
        case AstType::IDENTIFIER:
            return this->analyze(n->ast_identifier);
            break;
        case AstType::IF:
            return this->analyze(n->ast_if);
            break;
        case AstType::RETURN:
            return this->analyze(n->ast_return);
            break;
        case AstType::LIST:
            break;
        case AstType::CLASS:
            return this->analyze(n->ast_class);
            break;
        case AstType::MEMBER:
            return this->analyze(n->ast_member);
            break;
        case AstType::ASSIGNMENT:
            break;
        case AstType::BINOP:
            return this->analyze(n->ast_binop);
            break;
        case AstType::DECLARATION:
            return this->analyze(n->ast_declaration);
            break;
        case AstType::TYPE:
            break;
        case AstType::NUMBER:
            break;
        case AstType::CALL:
            return this->analyze(n->ast_call);
        default:
            throw std::runtime_error("Dont know what to do with node: " + ast_string(n->type));
    }
    return nullptr;
}

SymbolInfo* SecondPass::analyze(VectorOfNodes program) {
    for (auto n: program) {
        this->analyze(n);
    }
    return nullptr;
}
