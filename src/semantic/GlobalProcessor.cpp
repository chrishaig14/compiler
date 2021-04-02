//
// Created by chris on 28/6/20.
//

#include <iostream>
#include "GlobalProcessor.h"
#include "ClassInfo.h"
#include "../logging/logging.h"
#include "../scanner/Scanner.h"
#include "../parser/Parser.h"
#include "util.h"


GlobalProcessor::GlobalProcessor(
        std::map<std::string, std::unique_ptr<std::map<std::string, std::string>>>& module_mappings,
        ClassTable* imported_classes, FunctionTable* imported_functions, std::string module_name) : module_mappings(
        module_mappings) {
    this->module_name = module_name;
    this->function_table = imported_functions;
    this->class_table = imported_classes;
}

void GlobalProcessor::visit(ImportNode& node) {
    for (auto imported_name: node.imports) {
        (*this->module_mappings[this->module_name])[imported_name] = (*this->module_mappings[node.module_name])[imported_name];
    }
}

void GlobalProcessor::visit(FunctionNode& node) {
    VectorOfTypes x;
    for (auto p: node.parameter_types) {
        x.emplace_back(p->clone());
    }
    FunctionType function_info(x, node.return_type->clone());
    if (this->module_mappings[this->module_name]->find(node.identifier) !=
        this->module_mappings[this->module_name]->end()) {
        std::string msg;
        msg = E_FMT("Name ") + E_HLT(node.identifier) + E_FMT(" already declared at ") +
              E_HLT(text_pos_to_string(this->__file__, node.start));
        std::cout << msg << std::endl;
        exit(1);
        // throw std::runtime_error("Error " + node.identifier + " already declared!");
    }
    std::string mangled_name = mangle_function_name(this->module_name, node.identifier);
    this->function_table->add(mangled_name, function_info.clone());
    (*this->module_mappings[this->module_name])[node.identifier] = mangled_name;
}

void GlobalProcessor::visit(BlockNode& node) {
    for (auto n: node.nodes) {
        this->dispatch(n);
    }
}

void GlobalProcessor::visit(ClassNode& node) {
    std::string mangled_name = mangle_class_name(this->module_name, node.class_name);
    ClassInfo* class_info = new ClassInfo();
    for (auto mn: node.members_ordered) {
        auto mt = node.members[mn];
        class_info->member_names.push_back(mn);
        class_info->member_types.push_back(mt->clone());
        class_info->members[mn] = mt->clone();
    }
    for (auto mn: node.static_members) {
        class_info->static_members[mn.first] = mn.second->clone();
    }
    bool has_init = false;
    for (auto f: node.methods) {
        FunctionNode& method = *f.second;
        has_init = f.first == "init";
        VectorOfTypes x;
        for (auto p: method.parameter_types) {
            x.emplace_back(p->clone());
        }
        class_info->methods.insert(make_pair(f.first, new FunctionType(x, method.return_type->clone())));
        (*this->module_mappings[this->module_name])[node.class_name + "." +
                                                    method.identifier] = mangle_method_name(this->module_name,
                                                                                            node.class_name,
                                                                                            method.identifier);
    }

    for (auto f: node.static_methods) {
        FunctionNode& method = *f.second;
        has_init = f.first == "init";
        VectorOfTypes x;
        for (auto p: method.parameter_types) {
            x.emplace_back(p->clone());
        }
        class_info->static_methods.insert(make_pair(f.first, new FunctionType(x, method.return_type->clone())));
        (*this->module_mappings[this->module_name])[node.class_name + "." +
                                                    method.identifier] = mangle_method_name(this->module_name,
                                                                                            node.class_name,
                                                                                            method.identifier);
    }


    if (!has_init) {
        class_info->methods["init"] = new FunctionType(class_info->member_types, new ObjectType(node.class_name));
        (*this->module_mappings[this->module_name])[node.class_name + "." +
                                                    "init"] = mangle_method_name(this->module_name,
                                                                                 node.class_name,
                                                                                 "init");
    }
    class_info->methods["str"] = new FunctionType({}, new T_STRING);
    (*this->module_mappings[this->module_name])[node.class_name + "." + "str"] = mangle_method_name(this->module_name,
                                                                                                    node.class_name,
                                                                                                    "str");
    VectorOfTypes tp;
    for (int i = 0; i < node.type_parameters.size(); i++) {
        tp.push_back(new ObjectType(node.type_parameters[i]));
    }
    class_info->methods["eq"] = new FunctionType({new ObjectType(node.class_name, tp)}, new T_BOOL);
    (*this->module_mappings[this->module_name])[node.class_name + "." + "eq"] = mangle_method_name(this->module_name,
                                                                                                   node.class_name,
                                                                                                   "eq");
    class_info->class_name = node.class_name;
    class_info->type_params = node.type_parameters;
    if (this->module_mappings[this->module_name]->find(node.class_name) !=
        this->module_mappings[this->module_name]->end()) {
        std::string msg;
        msg = E_FMT("Name ") + E_HLT(node.class_name) + E_FMT(" already declared at ") +
              E_HLT(text_pos_to_string(this->__file__, node.start));
        std::cout << msg << std::endl;
        exit(1);
    }
    (*this->module_mappings[this->module_name])[node.class_name] = mangled_name;
    this->class_table->set(mangled_name, class_info);
}

void GlobalProcessor::dispatch(Node* nod) {
    auto& n = *nod;
    switch (n.ntype) {
        case NodeType::CLS:
            this->visit(n.cls());
            break;
        case NodeType::FUNC:
            this->visit(n.func());
            break;
        case NodeType::IMPORT:
            this->visit(n.import());
            break;
    }
}


const FunctionType& FunctionTable::get(std::string function_name) {
    if (functions.find(function_name) == functions.end()) {
        throw std::runtime_error("Error, function " + function_name + " not found in function table");
    }
    return *functions.find(function_name)->second;
}

void FunctionTable::add(std::string function_name, FunctionType* function_type) {
    if (functions.find(function_name) == functions.end()) {
        functions.insert(std::make_pair(function_name, function_type));
    } else {
        throw std::runtime_error("Cant overload function " + function_name);
    }
}

bool FunctionTable::has_function(std::string name) {
    return this->functions.find(name) != this->functions.end();
}

FunctionTable::~FunctionTable() {
    for (auto f: this->functions) {
        delete f.second;
    }
}
