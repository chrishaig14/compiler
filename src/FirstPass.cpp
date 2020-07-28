//
// Created by chris on 28/6/20.
//

#include "FirstPass.h"

void FirstPass::analyze(ClassNode& node) {
    ClassInfo* class_info = new ClassInfo;
    for (auto f: node.fields) {
        class_info->fields[f->identifier] = new ObjectInfo(f->type);
    }
    for (auto m: node.methods) {
        class_info->methods[m->name] = new FunctionInfo(m->parameter_types, m->return_type);
    }
    this->class_table.set(node.name, class_info);
}

void FirstPass::analyze(FunctionNode& node) {
    FunctionInfo* function_info = new FunctionInfo(node.parameter_types, node.return_type);
    SymbolInfo* ginfo = new SymbolInfo(function_info);
    this->globals.set(node.name, ginfo);
}

void FirstPass::analyze(VectorOfNodes program) {
    for (auto n: program) {
        if (n->type == AstType::CLASS) {
            this->analyze(*n->ast_class);
        }
        if (n->type == AstType::FUNCTION) {
            this->analyze(*n->ast_function);
        }
    }
}

FirstPass::FirstPass() {
    this->globals = SymbolTable("global", nullptr);
    this->class_table = ClassTable();
}

FunctionInfo::FunctionInfo(std::vector<TypeNode*> parameter_types, TypeNode* return_type) {
    for (auto n: parameter_types) {
        this->parameter_types.push_back(new ObjectInfo(n));
    }
    if (return_type == nullptr) {
        this->return_type = nullptr;
    } else {
        this->return_type = new ObjectInfo(return_type);
    }
}

ObjectInfo::ObjectInfo(TypeNode* n) {
    this->parent = n->name;
    for (auto t: n->type_parameters) {
        this->type_parameters.push_back(new ObjectInfo(t));
    }
}


bool both_null(void* a, void* b) {
    return a == nullptr and b == nullptr;
}

bool one_null(void* a, void* b) {
    return a == nullptr or b == nullptr;
}

bool equal(ObjectInfo* a, ObjectInfo* b) {
    if (both_null(a, b)) return true;
    if (one_null(a, b)) return false;
    if (a->parent == b->parent) {
        if (a->type_parameters.size() != b->type_parameters.size()) return false;
        for (int i = 0; i < a->type_parameters.size(); i++) {
            if (!equal(a->type_parameters[i], b->type_parameters[i]))return false;
        }
        return true;
    }
    return false;
}

bool equal(FunctionInfo* a, FunctionInfo* b) {
    if (both_null(a, b)) return true;
    if (one_null(a, b)) return false;
    if (!equal(a->return_type, b->return_type)) return false;

    if (a->parameter_types.size() != b->parameter_types.size()) return false;
    for (int i = 0; i < a->parameter_types.size(); i++) {
        if (!equal(a->parameter_types[i], b->parameter_types[i]))return false;
    }
    return true;
}

bool equal(ClassInfo* a, ClassInfo* b) {
    if (both_null(a, b)) return true;
    if (one_null(a, b)) return false;
    if (a->methods.size() != b->methods.size()) return false;
    for (auto it : a->methods) {
        if (b->methods.count(it.first) == 0)return false;
        if (!equal(it.second, b->methods[it.first])) return false;
    }
    if (a->fields.size() != b->fields.size()) return false;
    for (auto it : a->fields) {
        if (b->fields.count(it.first) == 0)return false;
        if (!equal(it.second, b->fields[it.first])) return false;
    }
    return true;
}

bool equal(SymbolInfo* a, SymbolInfo* b) {
    if (both_null(a, b)) return true;
    if (one_null(a, b)) return false;
    if (a->type == b->type) {
        switch (a->type) {
            case SINFO::FUNCTION:
                return equal(a->function_info, b->function_info);
            case SINFO::SIMPLE:
                return equal(a->object_info, b->object_info);
        }
    }
    return false;
}

