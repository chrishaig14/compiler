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

void GlobalProcessor::add_builtins(std::vector<Builtin>& builtins) {
    for (int i = 0; i < builtins.size(); i++) {
        Builtin b = builtins[i];
        assert(b.second.ftype != nullptr);
        int index = this->function_table->add(b.first, b.second.ftype);
        b.first = b.first;
        builtins[i] = b;
    }

}

void int_to_str(std::map<std::string, std::map<std::string, Code>>& structs, ObjectStack& stack,
                Environment* global_env) {
    IntegerObject* x = stack.pop_integer();
    stack.push(new StringObject(std::to_string(x->value)));
}

void str_to_str(std::map<std::string, std::map<std::string, Code>>& structs, ObjectStack& stack,
                Environment* global_env) {
}

void print(std::map<std::string, std::map<std::string, Code>>& structs, ObjectStack& stack,
           Environment* global_env) {
    StringObject* st = stack.pop_string();
    std::cout << "<< " << st->str << std::endl;
}

void list_len(std::map<std::string, std::map<std::string, Code>>& structs, ObjectStack& stack,
              Environment* global_env) {
    ListObject* ls = stack.pop_list();
    stack.push(new IntegerObject(ls->list.size()));
}

void string_len(std::map<std::string, std::map<std::string, Code>>& structs, ObjectStack& stack,
                Environment* global_env) {
    StringObject* ls = stack.pop_string();
    stack.push(new IntegerObject(ls->str.size()));
}

void range(std::map<std::string, std::map<std::string, Code>>& structs, ObjectStack& stack,
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

void join(std::map<std::string, std::map<std::string, Code>>& structs, ObjectStack& stack,
          Environment* global_env) {
    StringObject* sep = stack.pop_string();
    ListObject* ls = stack.pop_list();
    std::string res;
    for (int i = 0; i < ls->list.size(); i++) {
        StringObject* str = dynamic_cast<StringObject*>(ls->list[i]);
        if (str == nullptr) throw "Joining list with no strings!";
        res += str->str + sep->str;
    }
    if (ls->list.size() != 0) {
        res = res.substr(0, res.size() - sep->str.size());
    }
    stack.push(new StringObject(res));
}

void list_map(std::map<std::string, std::map<std::string, Code>>& structs, ObjectStack& stack,
              Environment* global_env) {
    CodeObject* fun = dynamic_cast<CodeObject*>(stack.pop());
    if (fun == nullptr)throw std::runtime_error("Popping a code object but it's not!");
    ListObject* ls = stack.pop_list();
    std::vector<Object*> rv;
    for (int i = 0; i < ls->list.size(); i++) {
        stack.push(ls->list[i]);
        if (fun->type == CodeType::BUILTIN) {
            fun->builtin.function(structs, stack, global_env);
        } else {
            CodeRunner code_runner(fun->user->code, structs, stack, global_env);
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
    auto ft = FUNCTION_TYPE({ TYPE("a", {}) }, TYPE("b", {}));
    auto at = T_LIST(TYPE("a", {}));
    builtins.push_back({"map", CodeBuiltin{FUNCTION_TYPE(VectorOfTypes({at, ft}), T_LIST(TYPE("b", {}))), list_map}});
    builtins.push_back({"Integer.str", CodeBuiltin{FUNCTION_TYPE({ T_INT }, T_STRING), int_to_str}});
    builtins.push_back({"List.len", CodeBuiltin{FUNCTION_TYPE({ T_LIST(TYPE("a", {})) }, T_INT), list_len}});
    builtins.push_back({"String.len", CodeBuiltin{FUNCTION_TYPE({ T_STRING }, T_INT), string_len}});
    builtins.push_back({"print", CodeBuiltin{FUNCTION_TYPE({ T_STRING }, T_INT), print}});
    builtins.push_back(
            {"join", CodeBuiltin{FUNCTION_TYPE(VectorOfTypes({T_LIST(T_STRING), T_STRING}), T_STRING), join}});
    builtins.push_back(
            {"range", CodeBuiltin{FUNCTION_TYPE(VectorOfTypes({T_INT, T_INT, T_INT}), T_LIST(T_INT)), range}});

    this->add_builtins(builtins);
}

GlobalProcessor::GlobalProcessor() {
    this->function_table = new FunctionTable();

    this->globals = new SymbolTable("global", nullptr);
    this->class_table = new ClassTable();
}

void GlobalProcessor::visit(AssignmentNode& node) {

}

void GlobalProcessor::visit(BinopNode& node) {

}

void GlobalProcessor::visit(CallNode& node) {

}

void GlobalProcessor::visit(DeclarationNode& node) {

}

void GlobalProcessor::visit(IdNode& node) {

}

void GlobalProcessor::visit(IfNode& node) {

}

void GlobalProcessor::visit(ListNode& node) {

}

void GlobalProcessor::visit(MemberNode& node) {

}

void GlobalProcessor::visit(NumberNode& node) {

}

void GlobalProcessor::visit(ReturnNode& node) {

}

void GlobalProcessor::visit(StringNode& node) {

}

void GlobalProcessor::visit(SubscriptNode& node) {

}

void GlobalProcessor::visit(TypeNode& node) {

}

void GlobalProcessor::visit(StructNode& node) {
//    ClassInfo* class_info = new ClassInfo();
//    for (auto f: node.fields) {
//        class_info->member_names.push_back(f.first);
//        class_info->member_types.push_back(f.second);
//        class_info->fields[f.first] = f.second;
//    }
//    class_info->type_parameters = node.template_parameters;
//    this->class_table->set(node.identifier, class_info);
}

void GlobalProcessor::visit(FunctionNode& node) {
    FunctionTypeNode* function_info = new FunctionTypeNode(node.parameter_types, node.return_type);
    int index = this->function_table->add(node.identifier, function_info);
    node.identifier = node.identifier;
}

void GlobalProcessor::visit(VectorOfNodes program) {
    for (auto n: program) {
        n->accept(*this);
    }
}

void GlobalProcessor::visit(BlockNode& node) {
    for (auto n: node.nodes) {
        n->accept(*this);
    }
}

void GlobalProcessor::visit(ClassLiteralExpressionNode& node) {

}

void GlobalProcessor::visit(ClassLiteralFieldNode& node) {

}

void GlobalProcessor::visit(ForNode& node) {

}

void GlobalProcessor::visit(BooleanNode& node) {

}

void GlobalProcessor::visit(WhileNode& node) {

}

void GlobalProcessor::visit(BreakNode& node) {

}

void GlobalProcessor::visit(TernaryNode& node) {

}

void GlobalProcessor::visit(NoneNode& node) {

}

void GlobalProcessor::visit(EmptyListNode& node) {

}

void GlobalProcessor::visit(ClassNode& node) {
    ClassInfo* class_info = new ClassInfo();
    for (auto f: node.members) {
        class_info->member_names.push_back(f.first);
        class_info->member_types.push_back(f.second);
        class_info->members[f.first] = f.second;
    }
    for (auto f: node.methods) {
        class_info->method_names.push_back(f.first);
        FunctionTypeNode* ft = new FunctionTypeNode(f.second->parameter_types, f.second->return_type);
        class_info->method_types.push_back(ft);
        class_info->methods[f.first] = ft;
    }
    class_info->class_name = node.class_name;
    this->class_table->set(node.class_name, class_info);
}

void GlobalProcessor::visit(InstanceNode& node) {

}


FunctionTypeNode* FunctionTable::get_simple_function(std::string function_name) {
    if (this->is_overloaded(function_name)) {
        throw std::runtime_error("Function " + function_name + " is not simple!");
    }
    return (*functions[function_name])[0];
}

std::vector<FunctionTypeNode*>* FunctionTable::get_overloads(std::string function_name) {
    return this->functions[function_name];
}

bool FunctionTable::function_exists(std::string function_name) {
    return this->functions.count(function_name) == 1;
}

bool FunctionTable::is_overloaded(std::string function_name) {
    if (!function_exists(function_name))
        throw std::runtime_error("Function '" + function_name + "' doesnt exist!");
    return functions[function_name]->size() > 1;
}

int FunctionTable::add(std::string function_name, FunctionTypeNode* function_type) {
    int n = functions.size();
    if (functions.count(function_name) == 0) {
        functions[function_name] = new std::vector<FunctionTypeNode*>();
    } else {
        throw std::runtime_error("Cant overload function " + function_name);
    }
    functions[function_name]->push_back(function_type);
    return functions[function_name]->size() - 1;
}

FunctionTable::FunctionTable() {
    this->foo = "Pepito";
}

bool FunctionTable::has_function(std::string name) {
    return this->functions.count(name) == 1;
}
