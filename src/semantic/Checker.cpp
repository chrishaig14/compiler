//
// Created by chris on 28/6/20.
//
#include <iostream>
#include <set>
#include "Checker.h"
#include "TypeClassInfo.h"

bool function_is_generic(FunctionTypeNode& ft) {
    for (int i = 0; i < ft.parameter_types.size(); i++) {
        if (is_generic(ft.parameter_types[i])) {
            return true;
            break;
        }
    }
    return false;
}

Checker::Checker(SymbolTable* globals, ClassTable* class_table) {
    this->class_table = class_table;
    this->scope = globals;
    this->scopes["global"] = this->scope;
    auto int_class_info = new ClassInfo();
    int_class_info->class_name = "Integer";
    int_class_info->methods["str"] = new FunctionTypeNode({}, T_STRING);

    auto list_class_info = new ClassInfo();
    list_class_info->class_name = "List";
    list_class_info->methods["len"] = new FunctionTypeNode({}, T_INT);


    this->class_table->set("Integer", int_class_info);
    this->class_table->set("List", list_class_info);

    auto string_class_info = new ClassInfo();
    string_class_info->class_name = "String";
    string_class_info->methods["len"] = new FunctionTypeNode({}, T_INT);
    this->class_table->set("String", string_class_info);

    this->class_table->set("Option",
                           new ClassInfo("Option", std::vector<std::string>(), std::vector<TypeNode*>(), {"T"}));

    this->check_structs();
}

void Checker::check_structs() {
}

void Checker::enter_scope(std::string name) {
    std::string new_scope_name = this->scope->name + "." + name;
    if (this->scopes.count(new_scope_name) == 1) {
        this->scope = this->scopes[new_scope_name];
        return;
    }
    this->scope = new SymbolTable(new_scope_name, this->scope);
    this->scopes[new_scope_name] = this->scope;
}

void Checker::leave_scope() {
    this->scope = this->scope->parent;
}


void Checker::visit(FunctionNode& n) {
    this->enter_scope(n.identifier);
    for (int i = 0; i < n.parameter_names.size(); i++) {
        ObjectTypeNode* otn = dynamic_cast<ObjectTypeNode*>(n.parameter_types[i]);
        if (otn != nullptr) {
            if (!is_generic(otn) && !this->class_table->declared(otn->identifier)) {
                throw std::runtime_error("type " + n.parameter_types[i]->to_string() + " doesn't exist!");
            }
        }
        this->scope->set(n.parameter_names[i], n.parameter_types[i]);
    }
    ObjectTypeNode* rotn = dynamic_cast<ObjectTypeNode*>(n.return_type);
    if (rotn != nullptr) {
        if (!is_generic(rotn) && !this->class_table->declared(rotn->identifier)) {
            throw std::runtime_error("type " + n.return_type->to_string() + " doesn't exist!");
        }
    }
    this->scope->set("__return__", n.return_type);
    n.body->accept(*this);
    SymbolInfo body_info = this->rv;
    this->leave_scope();
    SymbolInfo semantic_info;
//    semantic_info.symbol_info = ;
    semantic_info.is_a_function = false;
    this->rv = semantic_info;
}

void Checker::visit(IdNode& n) {
    SymbolInfo semantic_info;
    semantic_info.is_a_function = false;
    semantic_info.is_a_method = false;
    if (!this->scope->has(n.identifier)) {
        // it might be a function name
        if (this->function_table->has_function(n.identifier)) {
            semantic_info.is_a_function = true;
            semantic_info.symbol_info = this->function_table->get_simple_function(n.identifier);
            FunctionTypeNode* ft = dynamic_cast<FunctionTypeNode*>(semantic_info.symbol_info);
        } else {
            std::cout << n << std::endl;
            throw ScopeError(n.identifier);
        }
    } else {
        semantic_info.is_a_function = false;
        semantic_info.symbol_info = this->scope->get(n.identifier);
    }
    this->rv = semantic_info;
}

void Checker::visit(DeclarationNode& n) {
    if (this->scope->declared(n.identifier)) {
        throw RedeclareError(n.identifier);
    }
    SymbolInfo semantic_info;
    semantic_info.is_a_function = false;
    if (n.expression != nullptr and n.type != nullptr) {
        n.expression->accept(*this);
        SymbolInfo expression_info = this->rv;
        auto actual_type = dynamic_cast<ObjectTypeNode*>(n.type);
        if (actual_type->identifier == "Option") {
            if (!actual_type->type_parameters[0]->equal(expression_info.symbol_info)) {
                auto foo = dynamic_cast<ObjectTypeNode*>(expression_info.symbol_info);
                if (foo->identifier != "NoneType") {
                    throw AssignmentTypeError(n.type, expression_info.symbol_info);
                }
            }
        } else if (actual_type->identifier == "Union") {
            bool ok = false;
            for (int i = 0; i < actual_type->type_parameters.size(); i++) {
                if (actual_type->type_parameters[i]->equal(expression_info.symbol_info)) {
                    ok = true;
                    break;
                }
            }
            if (!ok) {
                throw AssignmentTypeError(n.type, expression_info.symbol_info);
            }
        } else {
            if (!n.type->equal(expression_info.symbol_info)) {
                throw AssignmentTypeError(n.type, expression_info.symbol_info);
            }
        }
        semantic_info.symbol_info = n.type;
    } else if (n.expression != nullptr) {
        n.expression->accept(*this);
        SymbolInfo expression_info = this->rv;
        semantic_info.symbol_info = expression_info.symbol_info;
    }
    this->rv = semantic_info;
    this->scope->set(n.identifier, semantic_info.symbol_info);
}

void Checker::visit(AssignmentNode& n) {
    n.lvalue->accept(*this);
    SymbolInfo linfo = this->rv;
    n.rvalue->accept(*this);
    SymbolInfo expression_type = this->rv;
    if (!linfo.symbol_info->equal(expression_type.symbol_info)) {
        throw AssignmentTypeError(linfo.symbol_info, expression_type.symbol_info);
    }
    SymbolInfo semantic_info;
    this->rv = semantic_info;
}

void Checker::visit(MemberNode& n) {
    IdNode* id_node = dynamic_cast<IdNode*>(n.parent);
    if (id_node != nullptr) {
        // It might be something like <class>.<method>, so we need to handle this case differently
        if (this->class_table->declared(id_node->identifier)) {
            ClassInfo* class_info = this->class_table->get(id_node->identifier);
            if (class_info->methods.count(n.child) == 1) {
                this->rv.symbol_info = class_info->methods[n.child];
                this->rv.is_a_method = false;
                this->rv.is_a_class_method = true;
                this->rv.class_info = class_info;
                this->replace_me = true;
                this->replacement = new IdNode(class_info->class_name + "." + n.child);
                return;
            } else {
                throw std::runtime_error("Class " + class_info->class_name + " has no method " + n.child);
            }
        }
    }
    n.parent->accept(*this);
    SymbolInfo semantic_info = this->rv;
    ObjectTypeNode* object = dynamic_cast<ObjectTypeNode*>(semantic_info.symbol_info);
    if (object == nullptr) {
        throw std::runtime_error("Accessing member " + n.child + " of non object");
    }
    if (!this->class_table->declared(object->identifier)) {
        throw std::runtime_error("Class " + object->identifier + " not declared!");
    }
    ClassInfo* class_info = this->class_table->get(object->identifier);
    if (class_info->members.count(n.child) == 1) {
        // It's a member
        semantic_info.symbol_info = class_info->members[n.child];
        this->rv = semantic_info;
        this->rv.is_a_function = false;
        this->rv.is_a_method = false;
    } else if (class_info->methods.count(n.child) == 1) {
        // It's a method
        semantic_info.symbol_info = class_info->methods[n.child];
        this->rv = semantic_info;
        this->rv.is_a_method = true;
        this->rv.class_info = class_info;
    } else {
        throw std::runtime_error("Type " + object->to_string() + " has no member " + n.child);
    }

}

void Checker::visit(IfNode& n) {
    SymbolInfo semantic_info;
    n.condition->accept(*this);
    SymbolInfo condition_info = this->rv;
    if (!condition_info.symbol_info->equal(T_BOOL)) {
        throw std::runtime_error("Expected a Boolean expression as a condition for if statement!, got " +
                                 condition_info.symbol_info->to_string());
    }
    this->enter_scope("if");
    n.then->accept(*this);
    SymbolInfo then_info = this->rv;
    this->leave_scope();
    this->rv = semantic_info;
}

void Checker::visit(BinopNode& n) {
    n.left->accept(*this);
    SymbolInfo left_info = this->rv;
    n.right->accept(*this);
    SymbolInfo right_info = this->rv;
    SymbolInfo semantic_info;
    bool is_boolean = true;
    switch (n.op) {
        case OpType::EQ:
            break;
        case OpType::AND:
            break;
        case OpType::OR:
            break;
        case OpType::LEQ:
            break;
        case OpType::GEQ:
            break;
        case OpType::LT:
            break;
        case OpType::GT:
            break;
        case OpType::NEQ:
            break;
        default:
            is_boolean = false;
    }
    if (is_boolean) {
        semantic_info.symbol_info = new ObjectTypeNode("Boolean", {});
    } else {
        auto left = dynamic_cast<ObjectTypeNode*>(left_info.symbol_info);
        auto right = dynamic_cast<ObjectTypeNode*>(right_info.symbol_info);
        auto ltype = left->identifier;
        auto rtype = right->identifier;
        bool ok = false;
        if (ltype == "Integer" && rtype == "Integer") {
            semantic_info.symbol_info = new ObjectTypeNode("Integer", {});
            semantic_info.is_a_function = false;
            ok = true;
        }
        if (ltype == "String" && rtype == "String") {
            if (n.op == OpType::ADD) {
                semantic_info.symbol_info = new ObjectTypeNode("String", {});
                semantic_info.is_a_function = false;
                ok = true;
            }
        }
        if (ltype == "List" && rtype == "List" && left->equal(right)) {
            if (n.op == OpType::ADD) {
                semantic_info.symbol_info = left;
                semantic_info.is_a_function = false;
                ok = true;
            }
        }

        if (!ok) {
            throw std::runtime_error(
                    "Cannot perform binary op betweeen types " + left->to_string() + " and " + right->to_string());
        }
    }

    this->rv = semantic_info;
}

void Checker::visit(ReturnNode& n) {
    n.expression->accept(*this);
    SymbolInfo expression_info = this->rv;
    TypeNode* return_type = this->scope->get("__return__");
    assert(expression_info.symbol_info != nullptr);
    assert(return_type != nullptr);
    if (!this->can_assign(expression_info.symbol_info, return_type)) {
        throw ReturnError(return_type, expression_info.symbol_info);
    }
    SymbolInfo semantic_info;
    this->rv = semantic_info;
}

bool is_generic(TypeNode* t) {
    ObjectTypeNode* o = dynamic_cast<ObjectTypeNode*>(t);
    if (o != nullptr) {
        if (o->identifier.size() == 1 && islower(o->identifier[0])) {
            // a is generic
            assert(o->type_parameters.size() == 0);
            return true;
        }
        for (int i = 0; i < o->type_parameters.size(); i++) {
            if (is_generic(o->type_parameters[i])) return true;
        }
    } else {
        FunctionTypeNode* fo = dynamic_cast<FunctionTypeNode*>(t);
        if (fo != nullptr) {
            for (int i = 0; i < fo->parameter_types.size(); i++) {
                if (is_generic(fo->parameter_types[i])) return true;
            }
        }
    }
    return false;
}

std::map<std::string, TypeNode*> make_replacements(TypeNode* a, TypeNode* b) {
    std::map<std::string, TypeNode*> replacements;
    ObjectTypeNode* oa = dynamic_cast<ObjectTypeNode*>(a);
    ObjectTypeNode* ob = dynamic_cast<ObjectTypeNode*>(b);
    if (oa != nullptr) {
        if (oa->identifier.size() == 1 && islower(oa->identifier[0])) {
            replacements[oa->identifier] = b;
        }
        for (int i = 0; i < oa->type_parameters.size(); i++) {
            if (is_generic(oa->type_parameters[i])) {
                std::map<std::string, TypeNode*> rep = make_replacements(oa->type_parameters[i],
                                                                         ob->type_parameters[i]);
                replacements.insert(rep.begin(), rep.end());
            }
        }
    } else {
        FunctionTypeNode* fa = dynamic_cast<FunctionTypeNode*>(a);
        FunctionTypeNode* fb = dynamic_cast<FunctionTypeNode*>(b);

        if (fa != nullptr && fb != nullptr) {
            for (int i = 0; i < fa->parameter_types.size(); i++) {
                if (is_generic(fa->parameter_types[i])) {
                    std::map<std::string, TypeNode*> rep = make_replacements(fa->parameter_types[i],
                                                                             fb->parameter_types[i]);
                    replacements.insert(rep.begin(), rep.end());
                }
            }
            if (is_generic(fa->return_type)) {
                std::map<std::string, TypeNode*> rep = make_replacements(fa->return_type, fb->return_type);
                replacements.insert(rep.begin(), rep.end());
            }
        }
    }
    return replacements;
}

bool type_matches(TypeNode* a, TypeNode* b) {
    ObjectTypeNode* oa = dynamic_cast<ObjectTypeNode*>(a);
    ObjectTypeNode* ob = dynamic_cast<ObjectTypeNode*>(b);
    if ((oa == nullptr && ob != nullptr)) {
        return false;
    } else if (oa == nullptr && ob == nullptr) {
        // both are not an object
        FunctionTypeNode* fa = dynamic_cast<FunctionTypeNode*>(a);
        FunctionTypeNode* fb = dynamic_cast<FunctionTypeNode*>(b);
        if (fa != nullptr && fb != nullptr) {
            // both are functions
            FunctionTypeNode* new_f = fa;
            std::map<std::string, TypeNode*> replacements;
            if (fa->parameter_types.size() != fb->parameter_types.size()) return false;
            VectorOfTypes param_types = fa->parameter_types;
            for (int i = 0; i < param_types.size(); i++) {
                if (type_matches(param_types[i], fb->parameter_types[i])) {
                    std::map<std::string, TypeNode*> rep = make_replacements(fa->parameter_types[i],
                                                                             fb->parameter_types[i]);
                    replacements.insert(rep.begin(), rep.end());
                    for (int j = 0; j < param_types.size(); j++) {
                        param_types[j] = make_type(param_types[j], replacements);
                    }
                } else { return false; }
            }
            return type_matches(make_type(fa->return_type, replacements), fb->return_type);
        }
    } else {
        // both are objects
        if (!is_generic(a)) {
            return a->equal(b);
        }
        // a is generic
        if (oa->type_parameters.size() == 0) return true;
        if (dynamic_cast<FunctionTypeNode*>(b) != nullptr) return false;
        if (oa->identifier != ob->identifier) return false;
        if (oa->type_parameters.size() != ob->type_parameters.size()) return false;
        for (int i = 0; i < oa->type_parameters.size(); i++) {
            if (!type_matches(oa->type_parameters[i], ob->type_parameters[i])) return false;
        }
        return true;
    }
    return false;
}

void Checker::visit(CallNode& n) {
    n.function->accept(*this);
    bool is_a_method = false;
    Node* object_node;
    if (this->rv.is_a_method) {
        // Since it's a method, we have to transform it and prepare it for the translation step,
        // where instead of calling object.method(args), we call <class>.method(object, args)
        MemberNode* member_node = dynamic_cast<MemberNode*>(n.function);
        assert(member_node != nullptr);
        n.function = new IdNode(this->rv.class_info->class_name + "." + member_node->child);
        object_node = member_node->parent;
        is_a_method = true;
    } else if (this->rv.is_a_class_method) {
        MemberNode* member_node = dynamic_cast<MemberNode*>(n.function);
        assert(member_node != nullptr);
        n.function = new IdNode(this->rv.class_info->class_name + "." + member_node->child);
        FunctionTypeNode* ftn = dynamic_cast<FunctionTypeNode*>(this->rv.symbol_info);
        ftn->parameter_types.insert(ftn->parameter_types.begin(), TYPE(this->rv.class_info->class_name, {}));
        this->rv.symbol_info = ftn;
        object_node = member_node->parent;
    }
    if (this->rv.is_a_function || this->rv.is_a_method || this->rv.is_a_class_method) {
        // ok
        FunctionTypeNode* function_type = dynamic_cast<FunctionTypeNode*>(this->rv.symbol_info);
        if (n.arguments.size() != function_type->parameter_types.size()) {

            throw std::runtime_error("Calling function with wrong number of arguments");
        }
        for (int i = 0; i < n.arguments.size(); i++) {
            Node* arg = n.arguments[i];
            arg->accept(*this);
            TypeNode* arg_type = this->rv.symbol_info;
            if (!arg_type->equal(function_type->parameter_types[i])) {
                throw std::runtime_error("Argument # " + std::to_string(i + 1) + " of type " + arg_type->to_string() +
                                         " doesn't match expected type " +
                                         function_type->parameter_types[i]->to_string());
            }
        }
        this->rv.symbol_info = function_type->return_type;

    } else {
        throw std::runtime_error("calling something that's not a function!");
    }
    if (is_a_method) {
        // prepend the "this" argument (the object on which the method is being called)
        n.arguments.insert(n.arguments.begin(), object_node);
    }
}

bool Checker::type_exists(TypeNode* type) {
    return false;
//    auto function_type = dynamic_cast<FunctionTypeNode*>(type);
//    if (function_type != nullptr) {
//        for (auto pt: function_type->parameter_types) {
//            if (!this->type_exists(pt)) return false;
//        }
//        if (!this->type_exists(function_type->return_type)) return false;
//    } else {
//        auto object_type = dynamic_cast<ObjectTypeNode*>(type);
//        if (object_type != nullptr) {
//            if (this->class_table->declared(object_type->identifier)) {
//                if (object_type->type_parameters.size() !=
//                    this->class_table->get(object_type->identifier)->type_parameters.size()) {
//                    throw std::runtime_error("Template struct " + object_type->identifier + " expects " +
//                                             std::to_string(this->class_table->get(
//                                                     object_type->identifier)->type_parameters.size()) +
//                                             " parameters, but " +
//                                             std::to_string(object_type->type_parameters.size()) +
//                                             " given");
//                }
//                for (auto tp: object_type->type_parameters) {
//                    if (!this->type_exists(tp)) return false;
//                }
//            } else {
//                if (object_type->type_parameters.size() == 0) {
//                    for (auto t: this->type_params) {
//                        if (t == object_type->identifier) return true;
//                    }
//                    return false;
//                } else {
//                    return false;
//                }
//            }
//        }
//    }
//    return true;
}

void Checker::visit(StructNode& n) {
    std::vector<std::string> type_params = n.template_parameters;
    this->type_params = type_params;
    for (auto f: n.fields) {
        if (!this->type_exists(f.second)) {
            throw std::runtime_error(
                    "Type " + f.second->to_string() + " for field '" + f.first + "' doesn't exist");
        }
    }
    this->type_params = {};
}

void Checker::visit(BlockNode& program) {
    SymbolInfo semantic_info;
    for (auto n: program.nodes) {
        n->accept(*this);
        SymbolInfo node_info = this->rv;
    }
    this->rv = semantic_info;
}

void Checker::visit(ClassLiteralExpressionNode& node) {
    if (!this->class_table->declared(node.type->identifier))
        throw std::runtime_error("No struct named " + node.type->identifier);
    ClassInfo* class_info = this->class_table->get(node.type->identifier);
    auto class_fields = class_info->members;
    if (class_fields.size() != node.init.size())
        throw std::runtime_error(
                "In struct \"" + node.type->identifier + "\" initialization: " + "Expected " +
                std::to_string(class_fields.size()) + " initializers but got " +
                std::to_string(node.init.size()));
//    for (int i = 0; i < node.init.size(); i++) {
//        Node* exp = node.init[i];
//        exp->accept(*this);
//        SymbolInfo semanticInfo = this->rv;
//        if (!semanticInfo.symbol_info->equal(class_info->member_types[i])) {
//            throw std::runtime_error(
//                    "Field type doesn't match: " + class_info->member_names[i] + " ( field # " + std::to_string(i) +
//                    " )" +
//                    " expected " +
//                    class_info->member_types[i]->to_string() + ", got " + semanticInfo.symbol_info->to_string());
//        }
//    }
//    this->rv = SymbolInfo();
//    rv.symbol_info = new ObjectTypeNode(node.type->identifier, {});
}

bool Checker::can_assign(TypeNode* from, TypeNode* to) {
    auto to_object = dynamic_cast<ObjectTypeNode*>(to);
    if (to_object->identifier == "Option") {
        if (!to_object->type_parameters[0]->equal(from)) {
            auto foo = dynamic_cast<ObjectTypeNode*>(from);
            if (foo->identifier != "NoneType") {
                return false;
            }
        }
        return true;
    } else if (to_object->identifier == "Union") {
        for (int i = 0; i < to_object->type_parameters.size(); i++) {
            if (to_object->type_parameters[i]->equal(from)) {
                return true;
            }
        }
        return false;
    }
    assert(from != nullptr);
    assert(to != nullptr);
    return to->equal(from);
}

bool Checker::can_assign_generic(TypeNode* from, TypeNode* to, std::vector<std::string> type_params) {
    auto to_object = dynamic_cast<ObjectTypeNode*>(to);
    if (to_object->type_parameters.size() == 0) {
        for (auto tp:type_params) {
            if (to_object->identifier == tp) {
                return true;
            }
        }
    }
    if (to_object->identifier == "Option") {
        if (!to_object->type_parameters[0]->equal(from)) {
            auto foo = dynamic_cast<ObjectTypeNode*>(from);
            if (foo->identifier != "NoneType") {
                return false;
            }
        }
        return true;
    } else if (to_object->identifier == "Union") {
        for (int i = 0; i < to_object->type_parameters.size(); i++) {
            if (to_object->type_parameters[i]->equal(from)) {
                return true;
            }
        }
        return false;
    }
    return to->equal(from);
}

TypeNode* make_type(TypeNode* original, std::map<std::string, TypeNode*>& replacements) {
    auto object_type = dynamic_cast<ObjectTypeNode*>(original);
    TypeNode* new_type = nullptr;
    VectorOfTypes new_type_params;
    if (object_type != nullptr) {
        std::string type_identifier = object_type->identifier;

        for (auto r: replacements) {
            if (type_identifier == r.first) {
                if (object_type->type_parameters.size() != 0) {
                    throw std::runtime_error(
                            "Trying to make a type for a template for exmaple struct Foo[T]{foo:T[Integer];}!");
                }
                new_type = r.second;
                return new_type;
            }
        }
        // It's not the top level type
        for (auto tp: object_type->type_parameters) {
            TypeNode* new_tp = make_type(tp, replacements);
            new_type_params.push_back(new_tp);
        }
        new_type = new ObjectTypeNode(type_identifier, new_type_params);
    } else {
        throw std::runtime_error("Making non object concrete type template!");
    }
    return new_type;
}

ClassInfo* Checker::instantiate_generic(ClassInfo* generic, ObjectTypeNode* instance) {
    std::map<std::string, TypeNode*> replacements;
//    for (int i = 0; i < generic->type_parameters.size(); i++) {
//        std::string tp = generic->type_parameters[i];
//        TypeNode* type_replacement = instance->type_parameters[i];
//        replacements[tp] = type_replacement;
//    }
    auto field_names = generic->member_names;
    std::vector<TypeNode*> concrete_field_types;
    for (auto f: generic->member_types) {
        TypeNode* concrete_type = make_type(f, replacements);
        concrete_field_types.push_back(concrete_type);
    }
//    ClassInfo* concrete = new ClassInfo(field_names, concrete_field_types, {});
//    return concrete;
    return nullptr;
}

void Checker::visit(ClassLiteralFieldNode& node) {
    if (!this->class_table->declared(node.type->identifier))
        throw std::runtime_error("No struct named " + node.type->identifier);
    auto class_fields = this->class_table->get(node.type->identifier)->members;
    for (auto f: node.init) {
        if (class_fields.count(f.first) == 0) throw std::runtime_error("No field named " + f.first);
    }
    if (class_fields.size() != node.init.size())
        throw std::runtime_error(
                "In struct \"" + node.type->identifier + "\" initialization: " + "Expected " +
                std::to_string(class_fields.size()) + " initializers but got " +
                std::to_string(node.init.size()));
    for (auto f: node.init) {
        Node* exp = f.second;
        exp->accept(*this);
        SymbolInfo semanticInfo = this->rv;
        TypeNode* field_type = class_fields[f.first];
        if (!this->can_assign(semanticInfo.symbol_info, field_type)) {
            throw std::runtime_error(
                    "In struct \"" + node.type->identifier + "\" initialization: " + "field \"" + f.first +
                    "\" is of type " +
                    field_type->to_string() +
                    " but got " + semanticInfo.symbol_info->to_string());
        }
    }
    this->rv = SymbolInfo();
    rv.symbol_info = node.type;
}

void Checker::visit(ForNode& node) {
    node.exp->accept(*this);
    SymbolInfo semantic_info = this->rv;
    ObjectTypeNode* obj = dynamic_cast<ObjectTypeNode*>(semantic_info.symbol_info);
    if (obj == nullptr) {
        throw std::runtime_error("Iterating over something bad!");
    }
    TypeNode* var_type = nullptr;
    if (obj->identifier == "List") {
        var_type = obj->type_parameters[0];
    }
    this->enter_scope("for");
    this->scope->set(node.var, var_type);
    node.body->accept(*this);
    this->leave_scope();
}

void Checker::visit(ListNode& node) {
    node.elements[0]->accept(*this);
    auto element_type = this->rv.symbol_info;
    for (int i = 1; i < node.elements.size(); i++) {
        node.elements[i]->accept(*this);
        auto current_type = this->rv.symbol_info;
        if (!current_type->equal(element_type)) {
            throw std::runtime_error("List literal with more than one element type, first element has type: " +
                                     element_type->to_string() + " but at index " + std::to_string(i) +
                                     " got type " +
                                     current_type->to_string());
        }
    }
    SymbolInfo return_info;
    return_info.is_a_function = false;
    return_info.symbol_info = T_LIST(element_type);
    this->rv = return_info;
}

void Checker::visit(BooleanNode& node) {
    SymbolInfo semantic_info;
    semantic_info.symbol_info = new ObjectTypeNode("Boolean", {});
    semantic_info.is_a_function = false;
    this->rv = semantic_info;
}

void Checker::visit(WhileNode& node) {
    node.condition->accept(*this);
    SymbolInfo condition = this->rv;
    if (!condition.symbol_info->equal(T_BOOL)) {
        throw std::runtime_error("At line " +
                                 std::to_string(node.condition->line + 1) + " column " +
                                 std::to_string(node.condition->column + 1) +
                                 ": Expected Boolean expression as while loop condition, got " +
                                 condition.symbol_info->to_string());
    }
    this->enter_scope("while");
    node.body->accept(*this);
    this->leave_scope();
}

void Checker::visit(NumberNode& node) {
    SymbolInfo semanticInfo;
    semanticInfo.symbol_info = new ObjectTypeNode("Integer", {});
    semanticInfo.is_a_function = false;
    this->rv = semanticInfo;
}

void Checker::visit(StringNode& node) {
    SymbolInfo semanticInfo;
    semanticInfo.symbol_info = new ObjectTypeNode("String", {});
    semanticInfo.is_a_function = false;
    this->rv = semanticInfo;
}

void Checker::visit(SubscriptNode& node) {
    node.parent->accept(*this);
    SymbolInfo parent = this->rv;
//    node.child->accept(*this);
    SymbolInfo child = this->rv;
    ObjectTypeNode* object_type = dynamic_cast<ObjectTypeNode*>(parent.symbol_info);
    if (object_type == nullptr) { throw std::runtime_error("Accessing subscript of non object!"); }
    SymbolInfo semantic_info;
    if (object_type->identifier == "List") {
        semantic_info.symbol_info = object_type->type_parameters[0];
    }
    semantic_info.is_a_function = false;
    this->rv = semantic_info;
}

void Checker::visit(BreakNode& node) {

}

void Checker::visit(TernaryNode& node) {
    node.expression->accept(*this);
    SymbolInfo expression_info = this->rv;
    auto expression_type = dynamic_cast<ObjectTypeNode*>(expression_info.symbol_info);
    if (expression_type == nullptr) {
        throw std::runtime_error("Unexpected non-object");
    }
    if (expression_type->identifier != "Option") {
        throw std::runtime_error("Expected an Option[T], got: " + expression_type->to_string());
    }
    SymbolInfo semanticInfo;
    TypeNode* type = expression_type->type_parameters[0];
    semanticInfo.symbol_info = type;
    this->enter_scope("true_case");
    this->scope->set("it", type);
    node.true_case->accept(*this);
    this->leave_scope();
    SymbolInfo true_case = this->rv;
    node.false_case->accept(*this);
    SymbolInfo false_case = this->rv;
    if (!false_case.symbol_info->equal(true_case.symbol_info)) {
        throw std::runtime_error(
                "True case and false case type don't match: " + true_case.symbol_info->to_string() + " != " +
                false_case.symbol_info->to_string());
//        semanticInfo.symbol_info = new ObjectTypeNode("Union", {true_case.symbol_info, false_case.symbol_info});
    } else {
        semanticInfo.symbol_info = true_case.symbol_info;
    }
    this->rv = semanticInfo;
}

void Checker::visit(NoneNode& node) {
    SymbolInfo semanticInfo;
    semanticInfo.symbol_info = new ObjectTypeNode("NoneType", {});
    semanticInfo.is_a_function = false;
    this->rv = semanticInfo;
}

void Checker::visit(EmptyListNode& node) {
    SymbolInfo semanticInfo;
    semanticInfo.symbol_info = node.type;
    semanticInfo.is_a_function = false;
    this->rv = semanticInfo;
}

void Checker::visit(ClassNode& node) {

}

void Checker::visit(InstanceNode& node) {

}


