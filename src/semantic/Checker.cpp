//
// Created by chris on 28/6/20.
//
#include <iostream>
#include <set>
#include "Checker.h"
#include "TypeClassInfo.h"

bool function_is_generic(FunctionTypeNode& ft) {
    for (auto param_type: ft.parameter_types) {
        if (is_generic(param_type)) {
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
    int_class_info->methods["str"] = FUNCTION_TYPE({}, T_STRING);

    auto list_class_info = new ClassInfo();
    list_class_info->class_name = "List";
    list_class_info->methods["len"] = FUNCTION_TYPE({}, T_INT);
    list_class_info->methods["map"] = FUNCTION_TYPE({ FUNCTION_TYPE({TYPE("t", {})}, TYPE("b", {})) },
                                                    T_LIST(TYPE("b", {})));
    list_class_info->type_parameters = {"t"};


    this->class_table->set("Integer", int_class_info);
    this->class_table->set("List", list_class_info);

    auto string_class_info = new ClassInfo();
    string_class_info->class_name = "String";
    string_class_info->methods["len"] = FUNCTION_TYPE({}, T_INT);
    this->class_table->set("String", string_class_info);
    this->replace_me = false;
    this->class_table->set("Option",
                           new ClassInfo("Option", std::vector<std::string>(), std::vector<TypeNode*>(), {"t"}));

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
        ObjectTypeNode* otn = TO_OBJECT_TYPE(n.parameter_types[i]);
        if (otn != nullptr) {
            if (!is_generic(otn) && !this->class_table->declared(otn->identifier)) {
                throw std::runtime_error("type " + n.parameter_types[i]->to_string() + " doesn't exist!");
            }
        }
        this->scope->set(n.parameter_names[i], n.parameter_types[i]);
    }
    ObjectTypeNode* rotn = TO_OBJECT_TYPE(n.return_type);
    if (rotn != nullptr) {
        if (!is_generic(rotn) && !this->class_table->declared(rotn->identifier)) {
            throw std::runtime_error("type " + n.return_type->to_string() + " doesn't exist!");
        }
    }
    this->scope->set("__return__", n.return_type);
    n.body->accept(*this);
    SymbolInfo body_info = this->rv;
    this->leave_scope();
    SymbolInfo symbol_info;
//    type.type = ;
    symbol_info.is_function = false;
    this->rv = symbol_info;
}

void Checker::visit(IdNode& n) {
    SymbolInfo symbol_info;
    symbol_info.is_function = false;
    symbol_info.is_method = false;
    if (!this->scope->has(n.identifier)) {
        // it might be a function name
        if (this->function_table->has_function(n.identifier)) {
            symbol_info.is_function = true;
            symbol_info.type = this->function_table->get(n.identifier);
            FunctionTypeNode* ft = TO_FUNCTION_TYPE(symbol_info.type);
        } else {
            std::cout << n << std::endl;
            throw ScopeError(n.identifier);
        }
    } else {
        symbol_info.type = this->scope->get(n.identifier);
        ObjectTypeNode* otn = TO_OBJECT_TYPE(symbol_info.type);
        if (otn != nullptr) {
            if (otn->identifier == "Option") {
                if (this->scope->get_not_none(n.identifier)) {
                    symbol_info.type = otn->type_parameters[0];
                }
            }

        }
        if (symbol_info.type->is_function()) {
            symbol_info.is_function = true;
            symbol_info.is_method = false;
            symbol_info.is_class_method = false;
        }
    }
    this->rv = symbol_info;
}

void Checker::visit(DeclarationNode& n) {
    if (this->scope->declared(n.identifier)) {
        throw RedeclareError(n.identifier);
    }
    SymbolInfo symbol_info;
    symbol_info.is_function = false;
    if (n.expression != nullptr and n.type != nullptr) {
        n.expression->accept(*this);
        if (this->replace_me) {
            n.expression = replacement;
            this->replace_me = false;
        }
        auto ft = TO_FUNCTION_TYPE(n.type);
        if (ft != nullptr) {
            // it's a function
            if (!ft->equal(this->rv.type)) {
                throw AssignmentTypeError(n.type, this->rv.type);
            }
        } else {
            SymbolInfo expression_info = this->rv;
            auto actual_type = TO_OBJECT_TYPE(n.type);
            if (actual_type->identifier == "Option") {
                if (!actual_type->type_parameters[0]->equal(expression_info.type)) {
                    auto foo = TO_OBJECT_TYPE(expression_info.type);
                    if (foo->identifier != "NoneType") {
                        throw AssignmentTypeError(n.type, expression_info.type);
                    }
                }
            } else if (actual_type->identifier == "Union") {
                bool ok = false;
                for (auto type_param: actual_type->type_parameters) {
                    if (type_param->equal(expression_info.type)) {
                        ok = true;
                        break;
                    }
                }
                if (!ok) {
                    throw AssignmentTypeError(n.type, expression_info.type);
                }
            } else {
                if (!n.type->equal(expression_info.type)) {
                    throw AssignmentTypeError(n.type, expression_info.type);
                }
            }
        }
        symbol_info.type = n.type;

    } else if (n.expression != nullptr) {
        n.expression->accept(*this);
        if (this->replace_me) {
            n.expression = replacement;
            this->replace_me = false;
        }
        SymbolInfo expression_info = this->rv;
        symbol_info.type = expression_info.type;
    }
    this->rv = symbol_info;
    this->scope->set(n.identifier, symbol_info.type);
}

void Checker::visit(AssignmentNode& n) {
    n.lvalue->accept(*this);
    SymbolInfo linfo = this->rv;
    n.rvalue->accept(*this);
    if (this->replace_me) {
        n.rvalue = replacement;
        this->replace_me = false;
    }
    SymbolInfo expression_type = this->rv;

    auto actual_type = TO_OBJECT_TYPE(linfo.type);
    IdNode* lid = TO_ID(n.lvalue);
    if (lid != nullptr && actual_type->identifier == "Option") {
        // special treatment if we are assigning to an id of a variable of type Option[t]
        if (expression_type.type->equal(actual_type->type_parameters[0])) {
            std::cout << "p cant be none" << std::endl;
            this->scope->set_not_none(lid->identifier, true);
        } else {
            if (!linfo.type->equal(expression_type.type)) {
                auto foo = TO_OBJECT_TYPE(expression_type.type);
                if (foo->identifier != "NoneType") {
                    throw AssignmentTypeError(linfo.type, expression_type.type);
                }
                // assigning none, ok
            }
            // type matches exactly, no proble
            std::cout << "p may be none" << std::endl;
            this->scope->set_not_none(lid->identifier, false);
        }
    } else {
        if (!linfo.type->equal(expression_type.type)) {
            if (actual_type->identifier == "Option") {
                // if type doesn't match exactly, we may be assigning to an Option[t]
                if (!actual_type->type_parameters[0]->equal(expression_type.type)) {
                    auto foo = TO_OBJECT_TYPE(expression_type.type);
                    if (foo->identifier != "NoneType") {
                        throw AssignmentTypeError(linfo.type, expression_type.type);
                    }
                }
            } else {
                // if it's not Option[t], then it's an error
                throw AssignmentTypeError(linfo.type, expression_type.type);
            }
        }
        // else, type matches don't do anything
    }

    SymbolInfo symbol_info;
    this->rv = symbol_info;
}

void Checker::visit(MemberNode& n) {
    IdNode* id_node = TO_ID(n.parent);
    if (id_node != nullptr) {
        // It might be something like <class>.<method>, so we need to handle this case differently
        if (this->class_table->declared(id_node->identifier)) {
            ClassInfo* class_info = this->class_table->get(id_node->identifier);
            if (class_info->methods.count(n.child) == 1) {
                this->rv.type = class_info->methods[n.child];
                this->rv.class_info = class_info;

                FunctionTypeNode* ftn = TO_FUNCTION_TYPE(this->rv.type);
                FunctionTypeNode* copy_ftn = FUNCTION_TYPE(ftn->parameter_types, ftn->return_type);
                std::vector<TypeNode*> tp;
                for (auto tttp: this->rv.class_info->type_parameters) {
                    tp.push_back(TYPE(tttp, {}));
                }
                auto instance_type = TYPE(this->rv.class_info->class_name, tp);
                copy_ftn->parameter_types.insert(copy_ftn->parameter_types.begin(), instance_type);

                this->rv.type = copy_ftn;
                this->rv.is_method = false;
                this->rv.is_class_method = true;
                this->replace_me = true;
                this->replacement = ID(class_info->class_name + "." + n.child);
                return;
            } else {
                throw std::runtime_error("Class " + class_info->class_name + " has no method " + n.child);
            }
        }
    }
    n.parent->accept(*this);
    SymbolInfo symbol_info = this->rv;
    ObjectTypeNode* object = TO_OBJECT_TYPE(symbol_info.type);
    if (object == nullptr) {
        throw std::runtime_error("Accessing member " + n.child + " of non object");
    }
    IdNode* idn = TO_ID(n.parent);
    if (idn != nullptr) {
        if (object->identifier == "Option") {
            if (this->scope->get_not_none(idn->identifier)) {
                // we can guarantee that it's not null, so we can access the members
                object = TO_OBJECT_TYPE(object->type_parameters[0]);
            } else {
                throw std::runtime_error("Error: line " + std::to_string(idn->line + 1) + " -> " + idn->identifier +
                                         " might be none here, make sure to wrap this in a if XXX != none {...}!");
            }
        }
    }
    ClassInfo* class_info;
    if (this->class_table->declared(object->to_string())) {
        class_info = this->class_table->get(object->to_string());
    } else {
        if (is_generic(object) && object->type_parameters.size() == 0) {
            throw std::runtime_error(
                    "Cannot access member of totally generic value of generic type " + object->identifier + "!");
        }
        class_info = this->class_table->get(object->identifier);
        class_info = instantiate_generic(class_info, object);
        this->class_table->set(object->to_string(), class_info);
    }
//    if (!this->class_table->declared(type.type->to_string())) {
//        throw std::runtime_error("Class " + type.type->to_string() + " not declared!");
//    }
//    = this->class_table->get(type.type->to_string());
    if (class_info->members.count(n.child) == 1) {
        // It's a member
        symbol_info.type = class_info->members[n.child];
        this->rv = symbol_info;
        this->rv.is_function = false;
        this->rv.is_method = false;
    } else if (class_info->methods.count(n.child) == 1) {
        // It's a method
        symbol_info.type = class_info->methods[n.child];
        this->rv = symbol_info;
        this->rv.is_method = true;
        this->rv.class_info = class_info;
    } else {
        throw std::runtime_error("Type " + object->to_string() + " has no member " + n.child);
    }

}

void Checker::visit(IfNode& n) {
    SymbolInfo symbol_info;
    n.condition->accept(*this);
    SymbolInfo condition_info = this->rv;
    if (!condition_info.type->equal(T_BOOL)) {
        throw std::runtime_error("Expected a Boolean expression as a condition for if statement!, got " +
                                 condition_info.type->to_string());
    }
    BinopNode* bop = TO_BINOP(n.condition);
    if (bop != nullptr) {
        if (bop->right->equal(new NoneNode())) {
            if (bop->op == OpType::NEQ) {
                IdNode* left = TO_ID(bop->left);
                NoneNode* right = TO_NONE(bop->right);
                if (left != nullptr && right != nullptr) {
                    this->enter_scope("if");
                    std::cout << "Cant be none: " << left->identifier << std::endl;
                    this->scope->set_not_none(left->identifier, true);
                    this->leave_scope();
                }
            }
            if (bop->op == OpType::EQ) {
                IdNode* left = TO_ID(bop->left);
                NoneNode* right = TO_NONE(bop->right);
                if (left != nullptr && right != nullptr) {
                    this->enter_scope("if");
                    std::cout << "MAY be none: " << left->identifier << std::endl;
                    this->scope->set_not_none(left->identifier, false);
                    this->leave_scope();
                }
            }
        }
    }
    this->enter_scope("if");
    n.then->accept(*this);
    this->leave_scope();

    if (n._else != nullptr) {
        this->enter_scope("else");
        n._else->accept(*this);
        this->leave_scope();
    }
    this->rv = symbol_info;
}

void Checker::visit(BinopNode& n) {
    n.left->accept(*this);
    SymbolInfo left_info = this->rv;
    n.right->accept(*this);
    SymbolInfo right_info = this->rv;
    SymbolInfo symbol_info;
    bool is_boolean = item_in_vec(n.op, {OpType::EQ, OpType::AND, OpType::OR, OpType::LEQ, OpType::GEQ, OpType::LT,
                                         OpType::GT, OpType::NEQ});
    if (is_boolean) {
        auto left = TO_OBJECT_TYPE(left_info.type);
        if (left != nullptr) {
            auto right = TO_OBJECT_TYPE(right_info.type);
            if (right != nullptr) {
                if (left->identifier == "Option" && right->identifier == "NoneType") {
                    symbol_info.type = TYPE("Boolean", {});
                }
            }
        } else {
            if (!left_info.type->equal(right_info.type)) {
                throw std::runtime_error(
                        "Cannot perform binary op betweeen types " + left_info.type->to_string() + " and " +
                        right_info.type->to_string());
            }
        }
        symbol_info.type = TYPE("Boolean", {});
    } else {
        auto left = TO_OBJECT_TYPE(left_info.type);
        auto right = TO_OBJECT_TYPE(right_info.type);
        auto ltype = left->identifier;
        auto rtype = right->identifier;
        bool ok = false;
        if (ltype == "Integer" && rtype == "Integer") {
            symbol_info.type = TYPE("Integer", {});
            symbol_info.is_function = false;
            ok = true;
        }
        if (ltype == "String" && rtype == "String") {
            if (n.op == OpType::ADD) {
                symbol_info.type = TYPE("String", {});
                symbol_info.is_function = false;
                ok = true;
            }
        }
        if (ltype == "List" && rtype == "List" && left->equal(right)) {
            if (n.op == OpType::ADD) {
                symbol_info.type = left;
                symbol_info.is_function = false;
                ok = true;
            }
        }

        if (!ok) {
            throw std::runtime_error(
                    "Cannot perform binary op betweeen types " + left->to_string() + " and " + right->to_string());
        }
    }

    this->rv = symbol_info;
}

void Checker::visit(ReturnNode& n) {
    n.expression->accept(*this);
    if (this->replace_me) {
        n.expression = replacement;
        this->replace_me = false;
    }
    SymbolInfo expression_info = this->rv;
    TypeNode* return_type = this->scope->get("__return__");
    assert(expression_info.type != nullptr);
    assert(return_type != nullptr);
    if (!this->can_assign(expression_info.type, return_type)) {
        throw ReturnError(return_type, expression_info.type);
    }
    SymbolInfo symbol_info;
    this->rv = symbol_info;
}

bool is_generic(TypeNode* t) {

    ObjectTypeNode* o = TO_OBJECT_TYPE(t);
    if (o != nullptr) {
        if (o->identifier.size() == 1 && islower(o->identifier[0])) {
            // a is generic
            assert(o->type_parameters.size() == 0);
            return true;
        }
        for (auto type_param: o->type_parameters) {
            if (is_generic(type_param)) return true;
        }
    } else {
        FunctionTypeNode* fo = TO_FUNCTION_TYPE(t);
        if (fo != nullptr) {
            for (auto param_type: fo->parameter_types) {
                if (is_generic(param_type)) return true;
            }
            if (is_generic(fo->return_type)) return true;

        }
    }
    return false;
}

std::map<std::string, TypeNode*> make_replacements(TypeNode* a, TypeNode* b) {
    std::map<std::string, TypeNode*> replacements;
    ObjectTypeNode* oa = TO_OBJECT_TYPE(a);
    ObjectTypeNode* ob = TO_OBJECT_TYPE(b);
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
        FunctionTypeNode* fa = TO_FUNCTION_TYPE(a);
        FunctionTypeNode* fb = TO_FUNCTION_TYPE(b);

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
    ObjectTypeNode* oa = TO_OBJECT_TYPE(a);
    ObjectTypeNode* ob = TO_OBJECT_TYPE(b);
    if ((oa == nullptr && ob != nullptr)) {
        return false;
    } else if (oa == nullptr && ob == nullptr) {
        // both are not an object
        FunctionTypeNode* fa = TO_FUNCTION_TYPE(a);
        FunctionTypeNode* fb = TO_FUNCTION_TYPE(b);
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
        if (TO_FUNCTION_TYPE(b) != nullptr) return false;
        if (oa->identifier != ob->identifier) return false;
        if (oa->type_parameters.size() != ob->type_parameters.size()) return false;
        for (int i = 0; i < oa->type_parameters.size(); i++) {
            if (!type_matches(oa->type_parameters[i], ob->type_parameters[i])) return false;
        }
        return true;
    }
    return false;
}

std::map<std::string, TypeNode*>
make_generic_replacements(TypeNode* t_generic_type, TypeNode* t_matching_type) {
    ObjectTypeNode* generic_type = TO_OBJECT_TYPE(t_generic_type);
    ObjectTypeNode* matching_type = TO_OBJECT_TYPE(t_matching_type);
    assert(generic_type != nullptr);
    assert(matching_type != nullptr);
    std::map<std::string, TypeNode*> replacements;
    if (generic_type->type_parameters.size() == 0) {
        replacements[generic_type->identifier] = matching_type;
    } else {
        for (int i = 0; i < generic_type->type_parameters.size(); i++) {
            std::map<std::string, TypeNode*> param_replacements = make_generic_replacements(
                    generic_type->type_parameters[i],
                    matching_type->type_parameters[i]);
            replacements.insert(param_replacements.begin(), param_replacements.end());
        }
    }
    return replacements;
}

void Checker::match_arguments_to_generic_function(FunctionTypeNode* function_type, VectorOfTypes arg_types) {
    std::map<std::string, TypeNode*> generic_replacements;
    for (int i = 0; i < function_type->parameter_types.size(); i++) {
        TypeNode* param_type = function_type->parameter_types[i];
        ObjectTypeNode* otn = TO_OBJECT_TYPE(param_type);
        if (is_generic(param_type)) {
            if (type_matches(param_type, arg_types[i])) {
                std::map<std::string, TypeNode*> param_generic_replacements = make_replacements(param_type,
                                                                                                arg_types[i]);
                for (auto gtr: param_generic_replacements) {
                    if (generic_replacements.count(gtr.first) == 1) {
                        // this type has already been replaced, see if it matches
                        if (!gtr.second->equal(generic_replacements[gtr.first])) {
                            throw std::runtime_error(
                                    "Type has already been replacen by something that doesn't match!");
                        }
                    }
                }
                generic_replacements.insert(param_generic_replacements.begin(), param_generic_replacements.end());
            } else {
                throw std::runtime_error("Argument type error!");
            }
        } else {
            if (!arg_types[i]->equal(function_type->parameter_types[i])) {
                throw std::runtime_error("Function call type mismatch! Expected " +
                                         function_type->parameter_types[i]->to_string() + " but got " +
                                         arg_types[i]->to_string() + "  instead");
            }
        }
    }
    if (is_generic(function_type->return_type)) {
        ObjectTypeNode* rtn = TO_OBJECT_TYPE(function_type->return_type);
        this->rv.type = make_type(rtn, generic_replacements);
    } else { this->rv.type = function_type->return_type; }

}


void Checker::visit(CallNode& n) {
    n.function->accept(*this);
    bool is_a_method = false;
    Node* object_node;
    SymbolInfo retv;
    if (this->rv.is_method) {
        // Since it's a method, we have to transform it and prepare it for the translation step,
        // where instead of calling object.method(args), we call <class>.method(object, args)
        MemberNode* member_node = TO_MEMBER(n.function);
        assert(member_node != nullptr);
        n.function = ID(this->rv.class_info->class_name + "." + member_node->child);
        this->replace_me = false;
        object_node = member_node->parent;
        is_a_method = true;
    } else if (this->rv.is_class_method) {
        MemberNode* member_node = TO_MEMBER(n.function);
        assert(member_node != nullptr);
        n.function = ID(this->rv.class_info->class_name + "." + member_node->child);
        this->replace_me = false;
        FunctionTypeNode* ftn = TO_FUNCTION_TYPE(this->rv.type);
        FunctionTypeNode* copy_ftn = FUNCTION_TYPE(ftn->parameter_types, ftn->return_type);
        copy_ftn->parameter_types.insert(copy_ftn->parameter_types.begin(), TYPE(this->rv.class_info->class_name, {}));
        retv.type = copy_ftn;
        object_node = member_node->parent;
    }
    if (this->rv.is_function || this->rv.is_method || this->rv.is_class_method) {
        // ok
        FunctionTypeNode* function_type = TO_FUNCTION_TYPE(this->rv.type);
        if (n.arguments.size() != function_type->parameter_types.size()) {
            throw std::runtime_error("Calling function with wrong number of arguments");
        }
        VectorOfTypes arg_types;
        for (auto& arg: n.arguments) {
            arg->accept(*this);
            if (this->replace_me) {
                arg = replacement;
                this->replace_me = false;
            }
            TypeNode* arg_type = this->rv.type;
            arg_types.push_back(arg_type);
        }
        if (function_is_generic(*function_type)) {
            match_arguments_to_generic_function(function_type, arg_types);
            retv = this->rv;
        } else {
            for (int i = 0; i < n.arguments.size(); i++) {
                if (!arg_types[i]->equal(function_type->parameter_types[i])) {
                    throw std::runtime_error("Function call type mismatch! Expected " +
                                             function_type->parameter_types[i]->to_string() + " but got " +
                                             arg_types[i]->to_string() + "  instead");
                }
            }
            retv.type = function_type->return_type;
        }
    } else {
        throw std::runtime_error("calling something that's not a function!");
    }
    if (is_a_method) {
        // prepend the "this" argument (the object on which the method is being called)
        n.arguments.insert(n.arguments.begin(), object_node);
    }
    this->rv = retv;
}

bool Checker::type_exists(TypeNode* type) {
    return false;
//    auto function_type = TO_FUNCTION_TYPE(type);
//    if (function_type != nullptr) {
//        for (auto pt: function_type->parameter_types) {
//            if (!this->type_exists(pt)) return false;
//        }
//        if (!this->type_exists(function_type->return_type)) return false;
//    } else {
//        auto object_type = TO_OBJECT_TYPE(type);
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
    SymbolInfo symbol_info;
    for (auto n: program.nodes) {
        n->accept(*this);
        SymbolInfo node_info = this->rv;
    }
    this->rv = symbol_info;
}

void Checker::visit(ClassLiteralExpressionNode& node) {
    if (!this->class_table->declared(node.type->identifier))
        throw std::runtime_error("No struct named " + node.type->identifier);
    ClassInfo* class_info = this->class_table->get(node.type->identifier);
    if (class_info->type_parameters.size() != 0) {
        // it's a generic class
        if (class_info->type_parameters.size() != node.type->type_parameters.size()) {
            throw std::runtime_error(
                    "Error: generic class requires " + std::to_string(class_info->type_parameters.size()) +
                    " type parameters, but " + std::to_string(node.type->type_parameters.size()) + " given");
        }
        if (this->class_table->declared(node.type->to_string())) {
            class_info = this->class_table->get(node.type->to_string());
        } else {
            class_info = instantiate_generic(class_info, node.type);
            this->class_table->set(node.type->to_string(), class_info);
        }
    }
    auto class_field_types_ordered = class_info->member_types;
    auto class_field_names_ordered = class_info->member_names;

    if (class_field_names_ordered.size() != node.init.size())
        throw std::runtime_error(
                "In struct \"" + node.type->identifier + "\" initialization: " + "Expected " +
                std::to_string(class_field_names_ordered.size()) + " initializers but got " +
                std::to_string(node.init.size()));

    for (int i = 0; i < node.init.size(); i++) {
        Node* exp = node.init[i];
        exp->accept(*this);
        if (this->replace_me) {
            node.init[i] = this->replacement;
            this->replace_me = false;
        }
        SymbolInfo semanticInfo = this->rv;
        TypeNode* field_type = class_field_types_ordered[i];
        if (!this->can_assign(semanticInfo.type, field_type)) {
            throw std::runtime_error(
                    "In struct \"" + node.type->identifier + "\" initialization: " + "field \"" +
                    class_field_names_ordered[i] +
                    "\" is of type " +
                    field_type->to_string() +
                    " but got " + semanticInfo.type->to_string());
        }
    }
    node.names = class_field_names_ordered;
    this->rv = SymbolInfo();
    rv.type = node.type;
}


bool Checker::can_assign(TypeNode* from, TypeNode* to) {
    auto to_object = TO_OBJECT_TYPE(to);
    if (to_object->identifier == "Option") {
        if (!to_object->type_parameters[0]->equal(from)) {
            auto foo = TO_OBJECT_TYPE(from);
            if (foo->identifier != "NoneType") {
                return false;
            }
        }
        return true;
    } else if (to_object->identifier == "Union") {
        for (auto type_param: to_object->type_parameters) {
            if (type_param->equal(from)) {
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
    auto to_object = TO_OBJECT_TYPE(to);
    if (to_object->type_parameters.size() == 0) {
        for (auto tp:type_params) {
            if (to_object->identifier == tp) {
                return true;
            }
        }
    }
    if (to_object->identifier == "Option") {
        if (!to_object->type_parameters[0]->equal(from)) {
            auto foo = TO_OBJECT_TYPE(from);
            if (foo->identifier != "NoneType") {
                return false;
            }
        }
        return true;
    } else if (to_object->identifier == "Union") {
        for (auto type_param: to_object->type_parameters) {
            if (type_param->equal(from)) {
                return true;
            }
        }
        return false;
    }
    return to->equal(from);
}

TypeNode* make_type(TypeNode* original, std::map<std::string, TypeNode*>& replacements) {
    auto object_type = TO_OBJECT_TYPE(original);
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
        new_type = TYPE(type_identifier, new_type_params);
    } else {
        FunctionTypeNode* ftn = TO_FUNCTION_TYPE(original);
        VectorOfTypes new_param_types;
        for (auto pt: ftn->parameter_types) {
            TypeNode* new_pt = make_type(pt, replacements);
            new_param_types.push_back(new_pt);
        }
        TypeNode* new_return_type = make_type(ftn->return_type, replacements);
        new_type = FUNCTION_TYPE(new_param_types, new_return_type);
//        throw std::runtime_error("Making non object concrete type template!");
    }
    return new_type;
}

ClassInfo* Checker::instantiate_generic(ClassInfo* generic, ObjectTypeNode* instance) {
    std::map<std::string, TypeNode*> replacements;
    for (int i = 0; i < generic->type_parameters.size(); i++) {
        std::string tp = generic->type_parameters[i];
        TypeNode* type_replacement = instance->type_parameters[i];
        replacements[tp] = type_replacement;
    }
    auto field_names = generic->member_names;
    std::vector<TypeNode*> concrete_field_types;
    for (auto f: generic->member_types) {
        TypeNode* concrete_type = make_type(f, replacements);
        concrete_field_types.push_back(concrete_type);
    }

    std::map<std::string, FunctionTypeNode*> concrete_methods;
    for (auto m: generic->methods) {
        TypeNode* concrete_type = make_type(m.second, replacements);
        concrete_methods[m.first] = TO_FUNCTION_TYPE(concrete_type);
        assert(concrete_methods[m.first] != nullptr);
    }

    ClassInfo* concrete = new ClassInfo();
    concrete->class_name = generic->class_name;
    concrete->methods = concrete_methods;
    concrete->member_names = generic->member_names;
    concrete->member_types = concrete_field_types;
    for (int i = 0; i < generic->member_names.size(); i++) {
        concrete->members[generic->member_names[i]] = concrete_field_types[i];
    }
    return concrete;
}

void Checker::visit(ClassLiteralFieldNode& node) {
    if (!this->class_table->declared(node.type->identifier))
        throw std::runtime_error("No struct named " + node.type->identifier);

    if (!this->class_table->declared(node.type->identifier))
        throw std::runtime_error("No struct named " + node.type->identifier);
    ClassInfo* class_info = this->class_table->get(node.type->identifier);
    if (class_info->type_parameters.size() != 0) {
        // it's a generic class
        if (class_info->type_parameters.size() != node.type->type_parameters.size()) {
            throw std::runtime_error(
                    "Error: generic class requires " + std::to_string(class_info->type_parameters.size()) +
                    " type parameters, but " + std::to_string(node.type->type_parameters.size()) + " given");
        }
        if (this->class_table->declared(node.type->to_string())) {
            class_info = this->class_table->get(node.type->to_string());
        } else {
            class_info = instantiate_generic(class_info, node.type);
            this->class_table->set(node.type->to_string(), class_info);
        }
    }
    auto class_fields = class_info->members;

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
        if (this->replace_me) {
            node.init[f.first] = this->replacement;
            this->replace_me = false;
        }
        SymbolInfo semanticInfo = this->rv;
        TypeNode* field_type = class_fields[f.first];
        if (!this->can_assign(semanticInfo.type, field_type)) {
            throw std::runtime_error(
                    "In struct \"" + node.type->identifier + "\" initialization: " + "field \"" + f.first +
                    "\" is of type " +
                    field_type->to_string() +
                    " but got " + semanticInfo.type->to_string());
        }
    }
    this->rv = SymbolInfo();
    rv.type = node.type;
}

void Checker::visit(ForNode& node) {
    node.exp->accept(*this);
    SymbolInfo symbol_info = this->rv;
    ObjectTypeNode* obj = TO_OBJECT_TYPE(symbol_info.type);
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
    if (this->replace_me) {
        node.elements[0] = this->replacement;
        this->replace_me = false;
    }
    auto element_type = this->rv.type;
    for (int i = 1; i < node.elements.size(); i++) {
        node.elements[i]->accept(*this);
        if (this->replace_me) {
            node.elements[i] = this->replacement;
            this->replace_me = false;
        }
        auto current_type = this->rv.type;
        if (!current_type->equal(element_type)) {
            throw std::runtime_error("List literal with more than one element type, first element has type: " +
                                     element_type->to_string() + " but at index " + std::to_string(i) +
                                     " got type " +
                                     current_type->to_string());
        }
    }
    SymbolInfo return_info;
    return_info.is_function = false;
    return_info.type = T_LIST(element_type);
    this->rv = return_info;
}

void Checker::visit(BooleanNode& node) {
    SymbolInfo symbol_info;
    symbol_info.type = TYPE("Boolean", {});
    symbol_info.is_function = false;
    this->rv = symbol_info;
}

void Checker::visit(WhileNode& node) {
    node.condition->accept(*this);
    SymbolInfo condition = this->rv;
    if (!condition.type->equal(T_BOOL)) {
        throw std::runtime_error("At line " +
                                 std::to_string(node.condition->line + 1) + " column " +
                                 std::to_string(node.condition->column + 1) +
                                 ": Expected Boolean expression as while loop condition, got " +
                                 condition.type->to_string());
    }
    this->enter_scope("while");
    node.body->accept(*this);
    this->leave_scope();
}

void Checker::visit(NumberNode& node) {
    SymbolInfo semanticInfo;
    semanticInfo.type = TYPE("Integer", {});
    semanticInfo.is_function = false;
    this->rv = semanticInfo;
}

void Checker::visit(StringNode& node) {
    SymbolInfo semanticInfo;
    semanticInfo.type = TYPE("String", {});
    semanticInfo.is_function = false;
    this->rv = semanticInfo;
}

void Checker::visit(SubscriptNode& node) {
    node.parent->accept(*this);
    SymbolInfo parent = this->rv;
//    node.child->accept(*this);
    SymbolInfo child = this->rv;
    ObjectTypeNode* object_type = TO_OBJECT_TYPE(parent.type);
    if (object_type == nullptr) { throw std::runtime_error("Accessing subscript of non object!"); }
    SymbolInfo symbol_info;
    if (object_type->identifier == "List") {
        symbol_info.type = object_type->type_parameters[0];
    }
    symbol_info.is_function = false;
    this->rv = symbol_info;
}

void Checker::visit(BreakNode& node) {

}

void Checker::visit(TernaryNode& node) {
    node.expression->accept(*this);
    SymbolInfo expression_info = this->rv;
    auto expression_type = TO_OBJECT_TYPE(expression_info.type);
    if (expression_type == nullptr) {
        throw std::runtime_error("Unexpected non-object");
    }
    if (expression_type->identifier != "Option") {
        throw std::runtime_error("Expected an Option[T], got: " + expression_type->to_string());
    }
    SymbolInfo semanticInfo;
    TypeNode* type = expression_type->type_parameters[0];
    semanticInfo.type = type;
    this->enter_scope("true_case");
    this->scope->set("it", type);
    node.true_case->accept(*this);
    if (this->replace_me) {
        node.true_case = this->replacement;
        this->replace_me = false;
    }
    this->leave_scope();
    SymbolInfo true_case = this->rv;
    node.false_case->accept(*this);
    if (this->replace_me) {
        node.false_case = this->replacement;
        this->replace_me = false;
    }
    SymbolInfo false_case = this->rv;
    if (!false_case.type->equal(true_case.type)) {
        throw std::runtime_error(
                "True case and false case type don't match: " + true_case.type->to_string() + " != " +
                false_case.type->to_string());
//        semanticInfo.type = TYPE("Union", {true_case.type, false_case.type});
    } else {
        semanticInfo.type = true_case.type;
    }
    this->rv = semanticInfo;
}

void Checker::visit(NoneNode& node) {
    SymbolInfo semanticInfo;
    semanticInfo.type = TYPE("NoneType", {});
    semanticInfo.is_function = false;
    this->rv = semanticInfo;
}

void Checker::visit(EmptyListNode& node) {
    SymbolInfo semanticInfo;
    semanticInfo.type = node.type;
    semanticInfo.is_function = false;
    this->rv = semanticInfo;
}

void Checker::visit(ClassNode& node) {
    for (auto method: node.methods) {
        this->enter_scope(method.first);

        std::vector<TypeNode*> tp;
        for (auto type_param: node.type_parameters) {
            tp.push_back(TYPE(type_param, {}));
        }
        this->scope->set("this", TYPE(node.class_name, tp));
        this->leave_scope();
        method.second->accept(*this);
    }
}

void Checker::visit(InstanceNode& node) {

}


