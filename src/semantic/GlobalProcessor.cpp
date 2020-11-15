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
#include "../nodes/NodeFactory.h"

void GlobalProcessor::add_builtins(std::vector<Builtin>& builtins) {
    for (int i = 0; i < builtins.size(); i++) {
        Builtin b = builtins[i];
        assert(b.second.ftype != nullptr);
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

void list_pop(std::map<std::string, std::map<std::string, Code>>& structs, ObjectStack& stack,
              Environment* global_env) {
    ListObject* ls = stack.pop_list();
    if (ls->list.size() == 0) {
        throw std::runtime_error("RUNTIME ERROR: pop from empty list!");
    }
    Object* last = ls->list[ls->list.size() - 1];
    ls->list.pop_back();
    stack.push(last);
}

void list_push(std::map<std::string, std::map<std::string, Code>>& structs, ObjectStack& stack,
               Environment* global_env) {
    Object* new_el = stack.pop();
    ListObject* ls = stack.pop_list();
    ls->list.push_back(new_el);
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

void list_map(std::map<std::string, std::map<std::string, Code>>& structs, ObjectStack& stack,
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
    auto ft = FUNCTION_TYPE({ TypeNode(NodeFactory::otype("a", {})) }, TypeNode(NodeFactory::otype("b", {})));
    auto at = T_LIST(TypeNode(NodeFactory::otype("a", {})));
    auto none = NodeFactory::otype(".None", {});
    std::vector<TypeNode> w = {at, ft};
    builtins.push_back(
            {"map", CodeBuiltin{&FUNCTION_TYPE(w, T_LIST(TypeNode(NodeFactory::otype("b", {})))), list_map}});
    builtins.push_back({"Integer.str", CodeBuiltin{&FUNCTION_TYPE({ T_INT }, T_STRING), int_to_str}});
    builtins.push_back(
            {"List.len", CodeBuiltin{&FUNCTION_TYPE({ T_LIST(NodeFactory::otype("a", {})) }, T_INT), list_len}});
    std::vector<TypeNode> x = {T_LIST(TYPE("a", {})), NodeFactory::otype("a", {})};
    builtins.push_back({"List.pop", CodeBuiltin{&FUNCTION_TYPE(x, none), list_pop}});
    builtins.push_back({"List.push", CodeBuiltin{
            &FUNCTION_TYPE({ T_LIST(NodeFactory::otype("a", {})) }, NodeFactory::otype("a", {})), list_push}});
    auto function_from_t_to_u = FUNCTION_TYPE({ TYPE("t", {}) }, NodeFactory::otype("b", {}));
    builtins.push_back(
            {"List.map",
             CodeBuiltin{&FUNCTION_TYPE({ function_from_t_to_u }, T_LIST(NodeFactory::otype("b", {}))), list_map}});
    builtins.push_back({"String.len", CodeBuiltin{&FUNCTION_TYPE({ T_STRING }, T_INT), string_len}});
    builtins.push_back({"print", CodeBuiltin{&FUNCTION_TYPE({ T_STRING }, none), print}});
    std::vector<TypeNode> a1 = {T_LIST(T_STRING), T_STRING};
    builtins.push_back(
            {"join", CodeBuiltin{&FUNCTION_TYPE(a1, T_STRING), join}});
    std::vector<TypeNode> a2 = {T_LIST(T_STRING), T_STRING};
    builtins.push_back(
            {"range", CodeBuiltin{&FUNCTION_TYPE(a2, T_LIST(T_INT)), range}});

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
    FunctionTypeNode function_info(node.parameter_types, node.return_type);
    this->function_table->add(node.identifier, function_info);
    node.identifier = node.identifier;
}

void GlobalProcessor::visit(VectorOfNodes program) {
    for (auto n: program) {
//        n->accept(*this);
    }
}

void GlobalProcessor::visit(BlockNode& node) {
    for (auto n: node.nodes) {
        this->dispatch(n);
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
    for (auto mn: node.members_ordered) {
        auto mt = node.members[mn];
        class_info->member_names.push_back(mn);
//        class_info->member_types.push_back(mt);
//        class_info->members[mn] = mt;
    }
    for (auto f: node.methods) {
        FunctionNode& method = f.second;
        class_info->methods.insert(make_pair(f.first, FunctionTypeNode(method.parameter_types, method.return_type)));
//        class_info->methods[f.first] = ;
    }
    class_info->class_name = node.class_name;
    class_info->type_parameters = node.type_parameters;
    this->class_table->set(node.class_name, class_info);
}

void GlobalProcessor::visit(InstanceNode& node) {

}

void GlobalProcessor::visit(ContinueNode& node) {

}

void GlobalProcessor::dispatch(NodeContainer n) {
    switch (n.ntype) {
        case NodeType::ASSIGN:
            n.assign().accept(*this);
            break;
        case NodeType::BINOP:
            n.binop().accept(*this);
            break;
        case NodeType::BLOCK:
            n.block().accept(*this);
            break;
        case NodeType::BOOLEAN:
            n.boolean().accept(*this);
            break;
        case NodeType::BRK:
            n.brk().accept(*this);
            break;
        case NodeType::CALL:
            n.call().accept(*this);
            break;
        case NodeType::CLSEXP:
            n.clsexp().accept(*this);
            break;
        case NodeType::CLSFLD:
            n.clsfld().accept(*this);
            break;
        case NodeType::CLS:
            n.cls().accept(*this);
            break;
        case NodeType::CNTINUE:
            n.cntinue().accept(*this);
            break;
        case NodeType::DECL:
            n.decl().accept(*this);
            break;
        case NodeType::EMPTYLST:
            n.emptylst().accept(*this);
            break;
        case NodeType::FORLOOP:
            n.forloop().accept(*this);
            break;
        case NodeType::FUNC:
            n.func().accept(*this);
            break;
        case NodeType::ID:
            n.id().accept(*this);
            break;
        case NodeType::IFF:
            n.iff().accept(*this);
            break;
        case NodeType::INSTANCE:
//                n.instance().accept(*this);
            break;
        case NodeType::LST:
            n.lst().accept(*this);
            break;
        case NodeType::MEMBER:
            n.member().accept(*this);
            break;
        case NodeType::NONE:
            n.none().accept(*this);
            break;
        case NodeType::NUMBER:
            n.number().accept(*this);
            break;
        case NodeType::RETRN:
            n.retrn().accept(*this);
            break;
        case NodeType::STRNG:
            n.strng().accept(*this);
            break;
        case NodeType::STRCT:
            n.strct().accept(*this);
            break;
        case NodeType::SUB:
            n.sub().accept(*this);
            break;
        case NodeType::TERNARY:
            n.ternary().accept(*this);
            break;
//        case NodeType::TYPE:
//                n.type().accept(*this);
//            break;
        case NodeType::WHIL:
            n.whil().accept(*this);
            break;
        case NodeType::UNINITIALIZED:
            break;
        default:
            throw std::runtime_error("Dont know what to do here!");
    }
}


FunctionTypeNode& FunctionTable::get(std::string function_name) {
    return functions.find(function_name)->second;
}

void FunctionTable::add(std::string function_name, FunctionTypeNode& function_type) {
    if (functions.count(function_name) == 0) {
        functions.insert(std::make_pair(function_name, function_type));
    } else {
        throw std::runtime_error("Cant overload function " + function_name);
    }
}

bool FunctionTable::has_function(std::string name) {
    return this->functions.count(name) == 1;
}
