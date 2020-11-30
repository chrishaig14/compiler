//
// Created by chris on 28/6/20.
//

#include <iostream>
#include "GlobalProcessor.h"
#include "ClassInfo.h"
#include "../vm/Object.h"
#include "../vm/ObjectStack.h"
#include "../vm/CodeObject.h"
#include "../vm/CodeRunner.h"
#include "../macros.h"
#include "../nodes/FunctionNode.h"

void GlobalProcessor::add_builtins(std::vector<Builtin>& builtins) {
    for (int i = 0; i < builtins.size(); i++) {
        this->function_table->add(builtins[i].first, *builtins[i].second.ftype);
    }

}

void int_to_str(std::unordered_map<std::string, std::unordered_map<std::string, Code>>& structs, ObjectStack& stack,
                Environment* global_env) {
    IntegerObject* x = stack.pop_integer();
    Object* obj = new StringObject(std::to_string(x->value));
    ObjectStore::register_object(obj);
    stack.push(obj);
}

void float_to_str(std::unordered_map<std::string, std::unordered_map<std::string, Code>>& structs, ObjectStack& stack,
                Environment* global_env) {
    FloatObject* x = stack.pop_float();
    Object* obj = new StringObject(std::to_string(x->value));
    ObjectStore::register_object(obj);
    stack.push(obj);
}

void str_to_str(std::unordered_map<std::string, std::unordered_map<std::string, Code>>& structs, ObjectStack& stack,
                Environment* global_env) {
}

void print(std::unordered_map<std::string, std::unordered_map<std::string, Code>>& structs, ObjectStack& stack,
           Environment* global_env) {
    StringObject* st = stack.pop_string();
    std::cout << "<< " << st->str << std::endl;
}

void list_len(std::unordered_map<std::string, std::unordered_map<std::string, Code>>& structs, ObjectStack& stack,
              Environment* global_env) {
    ListObject* ls = stack.pop_list();
    stack.push(new IntegerObject(ls->list.size()));
}

void list_pop(std::unordered_map<std::string, std::unordered_map<std::string, Code>>& structs, ObjectStack& stack,
              Environment* global_env) {
    ListObject* ls = stack.pop_list();
    if (ls->list.size() == 0) {
        throw std::runtime_error("RUNTIME ERROR: pop from empty list!");
    }
    Object* last = ls->list[ls->list.size() - 1];
    ls->list.pop_back();
    stack.push(last);
}

void list_push(std::unordered_map<std::string, std::unordered_map<std::string, Code>>& structs, ObjectStack& stack,
               Environment* global_env) {
    Object* new_el = stack.pop();
    ListObject* ls = stack.pop_list();
    ls->list.push_back(new_el);
}

void string_len(std::unordered_map<std::string, std::unordered_map<std::string, Code>>& structs, ObjectStack& stack,
                Environment* global_env) {
    StringObject* ls = stack.pop_string();
    stack.push(new IntegerObject(ls->str.size()));
}

void range(std::unordered_map<std::string, std::unordered_map<std::string, Code>>& structs, ObjectStack& stack,
           Environment* global_env) {
    IntegerObject* end = stack.pop_integer();
    IntegerObject* step = stack.pop_integer();
    IntegerObject* start = stack.pop_integer();
    std::vector<Object*> ls;
    for (int i = start->value; i < end->value; i += step->value) {
        ls.push_back(new IntegerObject(i));
    }
    stack.push(new ListObject(ls));
}

void join(std::unordered_map<std::string, std::unordered_map<std::string, Code>>& structs, ObjectStack& stack,
          Environment* global_env) {
    StringObject* sep = stack.pop_string();
    ListObject* ls = stack.pop_list();
    std::string res;
    for (int i = 0; i < ls->list.size(); i++) {
        StringObject* str = dynamic_cast<StringObject*>(ls->list[i]);
        if (str == nullptr) {
            throw "Joining list with no strings!";
        }
        res += str->str + sep->str;
    }
    if (ls->list.size() != 0) {
        res = res.substr(0, res.size() - sep->str.size());
    }
    stack.push(new StringObject(res));
}

void input(std::unordered_map<std::string, std::unordered_map<std::string, Code>>& structs, ObjectStack& stack,
           Environment* global_env) {
    std::string line;
    std::cin >> line;
    stack.push(new StringObject(line));
}

void list_map(std::unordered_map<std::string, std::unordered_map<std::string, Code>>& structs, ObjectStack& stack,
              Environment* global_env) {
    CodeObject* fun = dynamic_cast<CodeObject*>(stack.pop());
    if (fun == nullptr) {
        throw std::runtime_error("Popping a code object but it's not!");
    }
    ListObject* ls = stack.pop_list();
    std::vector<Object*> rv;
    for (int i = 0; i < ls->list.size(); i++) {
        stack.push(ls->list[i]);
        if (fun->type == CodeType::BUILTIN) {
            fun->stuff.builtin->function(structs, stack, global_env);
        } else {
            CodeRunner code_runner(fun->stuff.user->code, structs, stack, global_env);
            code_runner.run();
        }
        Object* obj = stack.top();
        rv.push_back(stack.pop());
    }
    stack.push(new ListObject(rv));
}

GlobalProcessor::GlobalProcessor(std::vector<Builtin>& builtins) {
    this->function_table = new FunctionTable();
    this->globals = new SymbolTable("global", nullptr);
    this->class_table = new ClassTable();
    auto ft = FUNCTION_TYPE({ new ObjectTypeNode("a", {}) }, new ObjectTypeNode("b", {}));
    auto at = new T_LIST(new ObjectTypeNode("a", {}));
    auto none = new ObjectTypeNode(".None", {});
    VectorOfTypes w = {at, ft};
    builtins.push_back(
            {"unordered_map", CodeBuiltin{new FunctionTypeNode(w, new T_LIST(new ObjectTypeNode("b", {}))), list_map}}
    );
    builtins.push_back({"Integer.str", CodeBuiltin{new FunctionTypeNode({new T_INT}, new T_STRING), int_to_str}});
    builtins.push_back({"Float.str", CodeBuiltin{new FunctionTypeNode({new T_FLOAT}, new T_STRING), float_to_str}});

    builtins.push_back(
            {"List.len",
             CodeBuiltin{new FunctionTypeNode({new T_LIST(new ObjectTypeNode("a", {}))}, new T_INT), list_len}}
    );
    VectorOfTypes x = {new T_LIST(TYPE("a", {})), new ObjectTypeNode("a", {})};
    builtins.push_back({"List.pop", CodeBuiltin{new FunctionTypeNode(x, none), list_pop}});
    builtins.push_back(
            {"List.push", CodeBuiltin{
                    new FunctionTypeNode({new T_LIST(new ObjectTypeNode("a", {}))}, new ObjectTypeNode("a", {})),
                    list_push}}
    );
    auto function_from_t_to_u = FUNCTION_TYPE({ TYPE("t", {}) }, new ObjectTypeNode("b", {}));
    builtins.push_back(
            {"List.unordered_map",
             CodeBuiltin{new FunctionTypeNode({function_from_t_to_u}, new T_LIST(new ObjectTypeNode("b", {}))),
                         list_map}}
    );
    builtins.push_back({"String.len", CodeBuiltin{new FunctionTypeNode({new T_STRING}, new T_INT), string_len}});
    builtins.push_back({"print", CodeBuiltin{new FunctionTypeNode({new T_STRING}, none), print}});
    VectorOfTypes a1 = {new T_LIST(new T_STRING), new T_STRING};
    builtins.push_back(
            {"join", CodeBuiltin{new FunctionTypeNode(a1, new T_STRING), join}}
    );
    VectorOfTypes a2 = {new T_INT, new T_INT, new T_INT};
    builtins.push_back(
            {"range", CodeBuiltin{new FunctionTypeNode(a2, new T_LIST(new T_INT)), range}}
    );

    builtins.push_back(
            {"input", CodeBuiltin{new FunctionTypeNode({}, new T_STRING), input}}
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
    FunctionTypeNode function_info(x, node.return_type->clone());
    if (this->class_table->declared(node.identifier) || this->function_table->has_function(node.identifier)) {
        throw std::runtime_error("Error " + node.identifier + " already declared!");
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
                make_pair(f.first, new FunctionTypeNode(x, method.return_type->clone())));
    }
    class_info->class_name = node.class_name;
    class_info->type_parameters = node.type_parameters;
    if (this->class_table->declared(node.class_name) || this->function_table->has_function(node.class_name)) {
        throw std::runtime_error("Error " + node.class_name + " already declared!");
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


const FunctionTypeNode& FunctionTable::get(std::string function_name) {
    return *functions.find(function_name)->second;
}

void FunctionTable::add(std::string function_name, FunctionTypeNode& function_type) {
    if (functions.find(function_name) == functions.end()) {
        functions.insert(std::make_pair(function_name, function_type.clone()));
    } else {
        throw std::runtime_error("Cant overload function " + function_name);
    }
}

bool FunctionTable::has_function(std::string name) {
    return this->functions.find(name) != this->functions.end();
}
