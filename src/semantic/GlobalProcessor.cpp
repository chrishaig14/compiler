//
// Created by chris on 28/6/20.
//

#include <iostream>
#include "GlobalProcessor.h"
#include "ClassInfo.h"
#include "../logging/logging.h"

void GlobalProcessor::add_builtins(std::vector<Builtin>& builtins) {
    for (int i = 0; i < builtins.size(); i++) {
        this->function_table->add(builtins[i].first, *builtins[i].second.ftype);
    }

}

GlobalProcessor::GlobalProcessor(std::vector<Builtin>& builtins, ClassTable* imported_classes,
                                 FunctionTable* imported_functions) {
    this->function_table = imported_functions;
    this->globals = new SymbolTable("global", nullptr);
    this->class_table = imported_classes;
    auto ft = FUNCTION_TYPE({ new ObjectType("a", {}) }, new ObjectType("b", {}));
    auto at = new T_LIST(new ObjectType("a", {}));
    auto none = new ObjectType(".None", {});
    VectorOfTypes w = {at, ft};
    builtins.push_back(
            {"map", CodeBuiltin{new FunctionType(w, new T_LIST(new ObjectType("b", {}))), nullptr}}
    );
    builtins.push_back({"File.read_line", CodeBuiltin{new FunctionType({}, new T_STRING), nullptr}});
    builtins.push_back({"Integer.str", CodeBuiltin{new FunctionType({new T_INT}, new T_STRING), nullptr}});
    builtins.push_back({"Float.str", CodeBuiltin{new FunctionType({new T_FLOAT}, new T_STRING), nullptr}});

    builtins.push_back(
            {"List.len",
             CodeBuiltin{new FunctionType({new T_LIST(new ObjectType("a", {}))}, new T_INT), nullptr}}
    );
    VectorOfTypes x = {new T_LIST(TYPE("a", {})), new ObjectType("a", {})};
    builtins.push_back({"List.pop", CodeBuiltin{new FunctionType(x, none), nullptr}});
    builtins.push_back(
            {"List.push", CodeBuiltin{
                    new FunctionType({new T_LIST(new ObjectType("a", {}))}, new ObjectType("a", {})),
                    nullptr}}
    );
    auto function_from_t_to_u = FUNCTION_TYPE({ TYPE("t", {}) }, new ObjectType("b", {}));
    builtins.push_back(
            {"List.unordered_map",
             CodeBuiltin{new FunctionType({function_from_t_to_u}, new T_LIST(new ObjectType("b", {}))),
                         nullptr}}
    );
    builtins.push_back({"String.len", CodeBuiltin{new FunctionType({new T_STRING}, new T_INT), nullptr}});
    builtins.push_back({"print", CodeBuiltin{new FunctionType({new T_STRING}, none), nullptr}});
    builtins.push_back({"open", CodeBuiltin{new FunctionType({new T_STRING}, new ObjectType("File", {})), nullptr}});
    VectorOfTypes a1 = {new T_LIST(new T_STRING), new T_STRING};
    builtins.push_back(
            {"join", CodeBuiltin{new FunctionType(a1, new T_STRING), nullptr}}
    );
    VectorOfTypes a2 = {new T_INT, new T_INT, new T_INT};
    builtins.push_back(
            {"range", CodeBuiltin{new FunctionType(a2, new T_LIST(new T_INT)), nullptr}}
    );

    builtins.push_back(
            {"input", CodeBuiltin{new FunctionType({}, new T_STRING), nullptr}}
    );

    this->add_builtins(builtins);
}

GlobalProcessor::GlobalProcessor() {
    this->function_table = new FunctionTable();

    this->globals = new SymbolTable("global", nullptr);
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
    this->function_table->add(node.identifier, function_info);
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
    for (auto f: node.methods) {
        FunctionNode& method = *f.second;
        VectorOfTypes x;
        for (auto p: method.parameter_types) {
            x.emplace_back(p->clone());
        }
        class_info->methods.insert(
                make_pair(f.first, new FunctionType(x, method.return_type->clone())));
    }
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

void FunctionTable::add(std::string function_name, const FunctionType& function_type) {
    if (functions.find(function_name) == functions.end()) {
        functions.insert(std::make_pair(function_name, function_type.clone()));
    } else {
        throw std::runtime_error("Cant overload function " + function_name);
    }
}

bool FunctionTable::has_function(std::string name) {
    return this->functions.find(name) != this->functions.end();
}
