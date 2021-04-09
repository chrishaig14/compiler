//
// Created by chris on 28/6/20.
//

#include <iostream>
#include "GlobalProcessor.h"
#include "../units/Class.h"
#include "../logging/logging.h"
#include "../scanner/Scanner.h"
#include "../parser/Parser.h"
#include "util.h"
#include "../units/ConstFunction.h"

GlobalProcessor::GlobalProcessor() {
}

std::string join_path(VectorOfStrings path) {
    std::string o;
    for (auto p: path) {
        o += p + ".";
    }
    o = o.substr(0, o.size() - 1);
    return o;
}

void GlobalProcessor::visit(ImportNode& node) {
    this->imported_paths[node.path.back()] = node.path;
    // for (auto imported_name: node.path) {
    //     (*this->module_mappings[this->module_name])[imported_name] = (*this->module_mappings[node.module_name])[imported_name];
    // }
}

void GlobalProcessor::visit(FunctionNode& node) {
    VectorOfTypes x;
    for (auto p: node.parameter_types) {
        x.emplace_back(p->clone());
    }
    FunctionType function_info(x, node.return_type->clone());
    std::string function_path = module_dotted_path + "." + node.identifier;
    ConstFunction* const_function = new ConstFunction();
    const_function->ft = function_info.clone();
    this->module->functions[node.identifier] = const_function;
}

void GlobalProcessor::visit(BlockNode& node) {
    for (auto n: node.nodes) {
        this->dispatch(n);
    }
}

void GlobalProcessor::visit(ClassNode& node) {
    std::string mangled_name = mangle_class_name(this->module_name, node.class_name);
    Class* class_info = new Class();
    for (auto mn: node.members_ordered) {
        auto mt = node.members[mn];
        class_info->member_names.push_back(mn);
        class_info->member_types.push_back(mt);
        class_info->members[mn] = mt;
    }
    for (auto mn: node.static_members) {
        class_info->static_members[mn.first] = std::make_pair(mn.second.first->clone(), mn.second.second);
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
        // (*this->module_mappings[this->module_name])[node.class_name + "." +
        //                                             method.identifier] = mangle_method_name(this->module_name,
        //                                                                                     node.class_name,
        //                                                                                     method.identifier);
    }

    for (auto f: node.static_methods) {
        FunctionNode& method = *f.second;
        has_init = f.first == "init";
        VectorOfTypes x;
        for (auto p: method.parameter_types) {
            x.emplace_back(p->clone());
        }
        class_info->static_methods.insert(make_pair(f.first, new FunctionType(x, method.return_type->clone())));
        // (*this->module_mappings[this->module_name])[node.class_name + "." +
        //                                             method.identifier] = mangle_method_name(this->module_name,
        //                                                                                     node.class_name,
        //                                                                                     method.identifier);
    }


    // if (!has_init) {
    //     class_info->methods["init"] = new FunctionType(class_info->member_types, new ObjectType(node.class_name));
    //     (*this->module_mappings[this->module_name])[node.class_name + "." +
    //                                                 "init"] = mangle_method_name(this->module_name,
    //                                                                              node.class_name,
    //                                                                              "init");
    // }
    class_info->methods["str"] = new FunctionType({}, new T_STRING);
    // (*this->module_mappings[this->module_name])[node.class_name + "." + "str"] = mangle_method_name(this->module_name,
    //                                                                                                 node.class_name,
    //                                                                                                 "str");
    VectorOfTypes tp;
    for (int i = 0; i < node.type_parameters.size(); i++) {
        tp.push_back(new ObjectType(node.type_parameters[i]));
    }
    class_info->methods["eq"] = new FunctionType({new ObjectType(node.class_name, tp)}, new T_BOOL);
    // (*this->module_mappings[this->module_name])[node.class_name + "." + "eq"] = mangle_method_name(this->module_name,
    //                                                                                                node.class_name,
    //                                                                                                "eq");
    class_info->class_name = node.class_name;
    class_info->type_params = node.type_parameters;
    // if (this->module_mappings[this->module_name]->find(node.class_name) !=
    //     this->module_mappings[this->module_name]->end()) {
    //     std::string msg;
    //     msg = E_FMT("Name ") + E_HLT(node.class_name) + E_FMT(" already declared at ") +
    //           E_HLT(text_pos_to_string(this->__file__, node.start));
    //     std::cout << msg << std::endl;
    //     exit(1);
    // }
    // (*this->module_mappings[this->module_name])[node.class_name] = mangled_name;
    this->module->classes[node.class_name] = class_info;
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