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

void GlobalProcessor::add_builtins(std::vector<Builtin>& builtins) {
    for (int i = 0; i < builtins.size(); i++) {
        this->function_table->add(builtins[i].first, parse_function_type(builtins[i].second));
    }

}

GlobalProcessor::GlobalProcessor(std::vector<Builtin>& builtins, ClassTable* imported_classes,
                                 FunctionTable* imported_functions) {
    this->function_table = imported_functions;
    this->class_table = imported_classes;

    builtins.push_back({"map", "fun(List[a],fun(a)->b)->List[b]"});
    builtins.push_back({"File.read_line", "fun()->String"});
    builtins.push_back({"Integer.str", "fun(Integer)->String"});
    builtins.push_back({"Float.str", "fun(Float)->String"});
    builtins.push_back({"Double.str", "fun(Double)->String"});
    builtins.push_back({"List.len", "fun(List[a])->Integer"});
    builtins.push_back({"List.pop", "fun(List[a],a)"});
    builtins.push_back({"List.push", "fun(List[a])->a"});
    builtins.push_back({"List.unordered_map", "fun(fun(t)->b)->List[b]"});
    builtins.push_back({"String.len", "fun(String)->Integer"});
    builtins.push_back({"print", "fun(String)"});
    builtins.push_back({"open", "fun(String)->File"});
    builtins.push_back({"join", "fun(List[String],String)->String"});
    builtins.push_back({"range", "fun(Integer,Integer,Integer)->List[Integer])->String"});
    builtins.push_back({"input", "fun()->String"});

    this->add_builtins(builtins);
}

GlobalProcessor::GlobalProcessor() {
    this->function_table = new FunctionTable();
    this->class_table = new ClassTable();
}

void GlobalProcessor::visit(FunctionNode& node) {
    VectorOfTypes x;
    for (auto p: node.parameter_types) {
        x.emplace_back(p->clone());
    }
    FunctionType function_info(x, node.return_type->clone());
    if (this->class_table->declared(node.identifier) || this->function_table->has_function(node.identifier)) {
        std::string msg;
        msg = E_FMT("Name ") + E_HLT(node.identifier) + E_FMT(" already declared at ") +
              E_HLT(text_pos_to_string(this->__file__, node.start));
        std::cout << msg << std::endl;
        exit(1);
        // throw std::runtime_error("Error " + node.identifier + " already declared!");
    }
    this->function_table->add(node.identifier, function_info.clone());
    node.identifier = node.identifier;
}

void GlobalProcessor::visit(BlockNode& node) {
    for (auto n: node.nodes) {
        this->dispatch(n);
    }
}

void GlobalProcessor::visit(ClassNode& node) {
    ClassInfo* class_info = new ClassInfo();
    for (auto mn: node.members_ordered) {
        auto mt = node.members[mn];
        class_info->member_names.push_back(mn);
        class_info->member_types.push_back(mt->clone());
        class_info->members[mn] = mt->clone();
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
    }
    if (!has_init) {
        class_info->methods["init"] = new FunctionType(class_info->member_types, new ObjectType(node.class_name, {}));
    }
    class_info->methods["str"] = new FunctionType({}, new T_STRING);
    VectorOfTypes tp;
    for (int i = 0; i < node.type_parameters.size(); i++) {
        tp.push_back(new ObjectType(node.type_parameters[i], {}));
    }
    class_info->methods["eq"] = new FunctionType({new ObjectType(node.class_name, tp)}, new T_BOOL);
    class_info->class_name = node.class_name;
    class_info->type_params = node.type_parameters;
    if (this->class_table->declared(node.class_name) || this->function_table->has_function(node.class_name)) {
        std::string msg;
        msg = E_FMT("Name ") + E_HLT(node.class_name) + E_FMT(" already declared at ") +
              E_HLT(text_pos_to_string(this->__file__, node.start));
        std::cout << msg << std::endl;
        exit(1);
    }
    this->class_table->set(node.class_name, class_info);
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
    }
}


const FunctionType& FunctionTable::get(std::string function_name) {
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
