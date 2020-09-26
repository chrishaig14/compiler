//
// Created by chris on 28/6/20.
//

#include <iostream>
#include "GlobalProcessor.h"
#include "ClassInfo.h"
#include "../vm/Object.h"
#include "../vm/ObjectStack.h"

void GlobalProcessor::add_builtins(std::vector<Builtin>& builtins) {
    for (int i = 0; i < builtins.size(); i++) {
        Builtin b = builtins[i];
        assert(b.second->ftype != nullptr);
        int index = this->function_table->add(b.first, b.second->ftype);
        b.first = b.first + "." + std::to_string(index);
        builtins[i] = b;
    }

}

void int_to_str(ObjectStack& stack) {
    IntegerObject* x = stack.pop_integer();
    stack.push(new StringObject(std::to_string(x->value)));
}

void print(ObjectStack& stack) {
    StringObject* st = stack.pop_string();
    std::cout << "<< " << st->str << std::endl;
}

void list_len(ObjectStack& stack) {
    ListObject* ls = stack.pop_list();
    stack.push(new IntegerObject(ls->list.size()));
}

void range(ObjectStack& stack) {
    IntegerObject* end = stack.pop_integer();
    IntegerObject* step = stack.pop_integer();
    IntegerObject* start = stack.pop_integer();
    std::vector<Object*> ls;
    for (int i = start->value; i < end->value; i += step->value) {
        ls.push_back(new IntegerObject(i));
    }
    stack.push(new ListObject(ls));
}


void GlobalProcessor::call(std::string function_name, FunctionTypeNode* ftype, void (* function)(ObjectStack&)) {
    new CodeBuiltin(ftype, function);
}

GlobalProcessor::GlobalProcessor(std::vector<Builtin>& builtins) {
    this->function_table = new FunctionTable();

    this->globals = new SymbolTable("global", nullptr);
    this->class_table = new ClassTable();

    builtins.push_back({"str", new CodeBuiltin(FUNCTION_TYPE({ T_INT }, T_STRING), int_to_str)});
    builtins.push_back({"print", new CodeBuiltin(FUNCTION_TYPE({ T_STRING }, T_INT), print)});
    builtins.push_back({"len", new CodeBuiltin(FUNCTION_TYPE({ T_LIST(TYPE("a", {})) }, T_INT), list_len)});
    builtins.push_back(
            {"range", new CodeBuiltin(FUNCTION_TYPE(VectorOfTypes({T_INT, T_INT, T_INT}), T_LIST(T_INT)), range)});

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
    ClassInfo* class_info = new ClassInfo();
    for (auto f: node.fields) {
        class_info->field_names.push_back(f.first);
        class_info->field_types.push_back(f.second);
        class_info->fields[f.first] = f.second;
    }
    class_info->type_parameters = node.template_parameters;
    this->class_table->set(node.identifier, class_info);
}

void GlobalProcessor::visit(FunctionNode& node) {
    FunctionTypeNode* function_info = new FunctionTypeNode(node.parameter_types, node.return_type);
//    std::string params;
//    for (auto p:node.parameter_types) {
//        params += p->to_string() + ".";
//    }
//    params = params.substr(0, params.size() - 1);
//    std::string new_name = node.identifier + ":" + params;
    int index = this->function_table->add(node.identifier, function_info);
    node.identifier = node.identifier + "." + std::to_string(index);
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
    if (!function_exists(function_name)) throw std::runtime_error("Function '" + function_name + "' doesnt exist!");
    return functions[function_name]->size() > 1;
}

int FunctionTable::add(std::string function_name, FunctionTypeNode* function_type) {
    int n = functions.size();
    if (functions.count(function_name) == 0) {
        functions[function_name] = new std::vector<FunctionTypeNode*>();
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
