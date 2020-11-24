#include <iostream>
#include <set>
#include "Checker.h"
#include "TypeClassInfo.h"
#include "../macros.h"

bool function_is_generic(const FunctionTypeNode& ft) {
    for (auto param_type: ft.parameter_types) {
        if (is_generic(*param_type)) {
            return true;
            break;
        }
    }
    return false;
}

ClassInfo* make_list_class_info() {
    auto list_class_info = new ClassInfo();
    list_class_info->class_name = "List";
    list_class_info->methods.insert(std::make_pair("len", new FunctionTypeNode({}, new T_INT)));

    ObjectTypeNode generic_type_t("t", {});
    ObjectTypeNode generic_type_b("b", {});

    list_class_info->methods.insert(
            std::make_pair("push", new FunctionTypeNode({(generic_type_t.clone())}, TYPE(".None", {}))));
    list_class_info->methods.insert(std::make_pair("pop", new FunctionTypeNode({}, generic_type_t.clone())));
    list_class_info->methods.insert(
            std::make_pair(
                    "unordered_map",
                    new FunctionTypeNode(
                            {FUNCTION_TYPE({ generic_type_t.clone() },
                                           generic_type_b.clone())},
                            new T_LIST(generic_type_b.clone()))));
    list_class_info->type_parameters = {"t"};
    return list_class_info;
}

ClassInfo* make_int_class_info() {
    auto int_class_info = new ClassInfo();
    int_class_info->class_name = "Integer";
    int_class_info->methods.insert(std::make_pair("str", new FunctionTypeNode({}, new T_STRING)));
    return int_class_info;
}

ClassInfo* make_string_class_info() {
    auto string_class_info = new ClassInfo();
    string_class_info->class_name = "String";
    string_class_info->methods.insert(std::make_pair("len", new FunctionTypeNode({}, new T_INT)));
    return string_class_info;
}

Checker::Checker(SymbolTable* globals, ClassTable* class_table, FunctionTable* function_table) {
    this->function_table = function_table;
    this->class_table = class_table;
    this->scope = globals;
    this->scopes["global"] = this->scope;

    this->add_this = false;
    this->this_type = nullptr;

    this->class_table->set("Integer", make_int_class_info());
    this->class_table->set("List", make_list_class_info());
    this->class_table->set("String", make_string_class_info());

    this->class_table->set("Tuple", nullptr);

    this->class_table->set("Option", new ClassInfo("Option", std::vector<std::string>(), {}, {"t"}));
    this->replace_me = false;
}

void Checker::enter_scope(std::string name) {
    std::string new_scope_name = this->scope->name + "." + name;
    if (this->scopes.find(new_scope_name) != this->scopes.end()) {
        delete this->scopes[new_scope_name];
        this->scopes.erase(new_scope_name);
    }
    this->scope = new SymbolTable(new_scope_name, this->scope);
    this->scopes[new_scope_name] = this->scope;
}

void Checker::leave_scope() {
    this->scope = this->scope->parent;
}


USymbolInfo Checker::visit(FunctionNode& n) {
    this->enter_scope(n.identifier);
    if (this->add_this) {
        this->scope->set("this", *this->this_type);
    }
    for (int i = 0; i < n.parameter_names.size(); i++) {
        TypeNode& type = *n.parameter_types[i];
        if (type.kind == Kind::OBJECT) {
            if (!is_generic(type) && !this->class_table->declared(type.object().identifier)) {
                throw std::runtime_error("type " + type.to_string() + " doesn't exist!");
            }
        }
        this->scope->set(n.parameter_names[i], type);
    }
    TypeNode& returnType = *n.return_type;
    if (!is_generic(returnType) && returnType.kind == Kind::OBJECT &&
        !this->class_table->declared(returnType.object().identifier) && returnType.object().identifier != ".None") {

        throw std::runtime_error("type " + returnType.to_string() + " doesn't exist!");
    }
    this->scope->set("__return__", returnType);
    this->visit(*n.body);
    if (returnType != ObjectTypeNode(".None", {})) {
        if (n.body->nodes.size() != 0) {
            Node* last_node = n.body->nodes[n.body->nodes.size() - 1];
            if (last_node->ntype != NodeType::RETRN) {
                // it's not a return statement, error
                throw std::runtime_error(
                        "Error in function \"" + n.identifier +
                        "\": the last statement in a function returning a value should be \"return\" EXPRESSION "
                );
            }
        } else {
            throw std::runtime_error(
                    "Error in function \"" + n.identifier +
                    "\"the last statement in a function returning a value should be \"return\" EXPRESSION "
            );
        }
    }
    this->leave_scope();
    return nullptr;
}

USymbolInfo Checker::visit(IdNode& n) {
    SymbolInfo symbol_info;
    symbol_info.is_function = false;
    symbol_info.is_method = false;
    if (!this->scope->has(n.identifier)) {
        // it might be a function name
        if (this->function_table->has_function(n.identifier)) {
            symbol_info.is_function = true;
            symbol_info.set_type(this->function_table->get(n.identifier));
        } else {
            throw ScopeError(n.identifier);
        }
    } else {
        symbol_info.set_type(this->scope->get(n.identifier));
        if (symbol_info.type().kind == Kind::OBJECT) {
            const ObjectTypeNode& otn = symbol_info.type().object();
            if (otn.identifier == "Option") {
                if (this->scope->get_not_none(n.identifier)) {
                    symbol_info.set_type(*otn.type_parameters[0]);
                }
            }

        }
        if (symbol_info.type().kind == Kind::FUNCTION) {
            symbol_info.is_function = true;
            symbol_info.is_method = false;
            symbol_info.is_class_method = false;
        }
    }
    return std::make_unique<SymbolInfo>(symbol_info);
}

USymbolInfo Checker::visit(DeclarationNode& n) {
    if (this->scope->declared(n.identifier)) {
        throw RedeclareError(n.identifier);
    }
    SymbolInfo symbol_info;
    symbol_info.is_function = false;
    if (n.expression->ntype != NodeType::UNINITIALIZED and n.type != nullptr) {
        USymbolInfo exp_info_p = this->dispatch(n.expression);
        SymbolInfo& exp_info = *exp_info_p;
        ObjectTypeNode& otn = n.type->object();
        if (this->replace_me) {
            n.expression = replacement;
            this->replace_me = false;
        }
        if (n.type->kind == Kind::FUNCTION) {
            // it's a function
            if (*n.type != exp_info.type()) {
                throw AssignmentTypeError(*n.type, exp_info.type());
            }
        } else {
            SymbolInfo expression_info = exp_info;
            const ObjectTypeNode& actual_type = n.type->object();
            if (actual_type.identifier == "Option") {
                if (*actual_type.type_parameters[0] != expression_info.type()) {
                    auto foo = expression_info.type().object();
                    if (foo.identifier != "NoneType") {
                        throw AssignmentTypeError(*n.type, expression_info.type());
                    }
                }
            } else if (actual_type.identifier == "Union") {
                bool ok = false;
                for (auto type_param: actual_type.type_parameters) {
                    if (*type_param != expression_info.type()) {
                        ok = true;
                        break;
                    }
                }
                if (!ok) {
                    throw AssignmentTypeError(*n.type, expression_info.type());
                }
            } else {
                if (*n.type != expression_info.type()) {
                    throw AssignmentTypeError(*n.type, expression_info.type());
                }
            }
        }
        symbol_info.set_type(*n.type);

    } else if (n.expression->ntype != NodeType::UNINITIALIZED) {
        USymbolInfo exp_info_p = this->dispatch(n.expression);
        SymbolInfo& exp_info = *exp_info_p;
        if (this->replace_me) {
            n.expression = replacement;
            this->replace_me = false;
        }
        symbol_info.set_type(exp_info.type());
    }
    this->scope->set(n.identifier, symbol_info.type());
    return std::make_unique<SymbolInfo>(symbol_info);
}

USymbolInfo Checker::visit(AssignmentNode& n) {
    if (n.lvalue->ntype == NodeType::ID) {
        if (n.lvalue->id().identifier == "_") {
            this->dispatch(n.rvalue);
            return nullptr;
        }
    }
    USymbolInfo linfo_p = this->dispatch(n.lvalue);
    if (n.lvalue->ntype == MEMBER && n.lvalue->member().type == MemberType::NUM) {
        throw std::runtime_error("Error: can't reassign a member of a tuple!");
    }
    USymbolInfo expression_type_p = this->dispatch(n.rvalue);
    SymbolInfo& linfo = *linfo_p;
    SymbolInfo& expression_type = *expression_type_p;
    if (this->replace_me) {
        n.rvalue = replacement;
        this->replace_me = false;
    }

    const ObjectTypeNode& actual_type = linfo.type().object();
    if (n.lvalue->ntype == NodeType::ID && actual_type.identifier == "Option") {
        // special treatment if we are assigning to an id of a variable of type Option[t]
        if (expression_type.type() == (*actual_type.type_parameters[0])) {
            std::cout << "p cant be none" << std::endl;
            this->scope->set_not_none(n.lvalue->id().identifier, true);
        } else {
            if (linfo.type() != (expression_type.type())) {
                auto foo = expression_type.type().object();
                if (foo.identifier != "NoneType") {
                    throw AssignmentTypeError(linfo.type(), expression_type.type());
                }
                // assigning none, ok
            }
            // type matches exactly, no proble
            std::cout << "p may be none" << std::endl;
            this->scope->set_not_none(n.lvalue->id().identifier, false);
        }
    } else {
        if (linfo.type() != expression_type.type()) {
            if (actual_type.identifier == "Option") {
                // if type doesn't match exactly, we may be assigning to an Option[t]
                if (*actual_type.type_parameters[0] != expression_type.type()) {
                    auto foo = expression_type.type().object();
                    if (foo.identifier != "NoneType") {
                        throw AssignmentTypeError(linfo.type(), expression_type.type());
                    }
                }
            } else {
                // if it's not Option[t], then it's an error
                throw AssignmentTypeError(linfo.type(), expression_type.type());
            }
        }
        // else, type matches don't do anything
    }
    return nullptr;
}

USymbolInfo Checker::visit(MemberNode& n) {
    SymbolInfo rv;
    if (n.parent->ntype == NodeType::ID) {
        IdNode& id_node = n.parent->id();
        // It might be something like <class>.<method>, so we need to handle this case differently
        if (this->class_table->declared(id_node.identifier)) {
            ClassInfo* class_info = this->class_table->get(id_node.identifier);
            if (class_info->methods.find(n.s_child) != class_info->methods.end()) {
                rv.set_type(*class_info->methods.find(n.s_child)->second);
                rv.class_info = class_info;

                const FunctionTypeNode& ftn = rv.type().function();
                FunctionTypeNode& copy_ftn = *ftn.clone();
                VectorOfTypes tp;
                for (auto tttp: rv.class_info->type_parameters) {
                    tp.push_back(new ObjectTypeNode(tttp, {}));
                }
                auto instance_type = new ObjectTypeNode(rv.class_info->class_name, tp);
                copy_ftn.parameter_types.insert(copy_ftn.parameter_types.begin(), instance_type);

                rv.set_type(copy_ftn);
                rv.is_method = false;
                rv.is_class_method = true;
                this->replace_me = true;
                this->replacement = new IdNode(class_info->class_name + "." + n.s_child);
                return std::make_unique<SymbolInfo>(rv);
            } else {
                throw std::runtime_error("Class " + class_info->class_name + " has no method " + n.s_child);
            }
        }
    }

    USymbolInfo symbol_info_p = this->dispatch(n.parent);
    SymbolInfo& symbol_info = *symbol_info_p;
    if (symbol_info.type().kind != Kind::OBJECT) {
        throw std::runtime_error("Accessing member " + n.s_child + " of non object");
    }
    const ObjectTypeNode& object = symbol_info.type().object();
    const ObjectTypeNode* option_type = nullptr;
    if (n.parent->ntype == NodeType::ID) {
        IdNode& idn = n.parent->id();
        if (object.identifier == "Option") {
            if (this->scope->get_not_none(idn.identifier)) {
                // we can guarantee that it's not null, so we can access the members
                option_type = &(object.type_parameters[0])->object();
            } else {
                throw std::runtime_error(
                        "Error: line " + std::to_string(idn.line + 1) + " -> " + idn.identifier +
                        " might be none here, make sure to  this in a if XXX != none {...}!"
                );
            }
        }
    }
    const ObjectTypeNode& final_type = option_type != nullptr ? *option_type : object;
    if (final_type.identifier == "Tuple") {
        // special treatment for tuples
        if (n.type != MemberType::NUM) {
            throw std::runtime_error(
                    "Error can only access members " + std::to_string(1) + " to " +
                    std::to_string(final_type.type_parameters.size()) + " of " + final_type.to_string());
        }
        if (n.n_child < 1 || n.n_child > final_type.type_parameters.size()) {
            throw std::runtime_error(
                    "Error can only access members " + std::to_string(1) + " to " +
                    std::to_string(final_type.type_parameters.size()) + " of " + final_type.to_string());
        }
        SymbolInfo s;
        s.set_type(*final_type.type_parameters[n.n_child - 1]);
        return std::make_unique<SymbolInfo>(s);
    } else {
        if (n.type != MemberType::STR) {
            throw std::runtime_error(
                    "Error: can access number member for tuple types only, but got " + final_type.to_string());
        }

        ClassInfo* class_info;
        if (this->class_table->declared(final_type.to_string())) {
            class_info = this->class_table->get(final_type.to_string());
        } else {
            if (is_generic((final_type)) && final_type.type_parameters.size() == 0) {
                throw std::runtime_error(
                        "Cannot access member of totally generic value of generic type " + object.identifier + "!"
                );
            }
            class_info = this->class_table->get(object.identifier);
            class_info = instantiate_generic(class_info, final_type);
            this->class_table->set(object.to_string(), class_info);
        }
        if (class_info->members.find(n.s_child) != class_info->members.end()) {
            // It's a member
            symbol_info.set_type(*class_info->members[n.s_child]);
            rv = symbol_info;
            rv.is_function = false;
            rv.is_method = false;
        } else if (class_info->methods.find(n.s_child) != class_info->methods.end()) {
            // It's a method
            symbol_info.set_type(*class_info->methods.find(n.s_child)->second);
            rv = symbol_info;
            rv.is_method = true;
            rv.class_info = class_info;
        } else {
            throw std::runtime_error("Type " + object.to_string() + " has no member " + n.s_child);
        }
    }

    return std::make_unique<SymbolInfo>(rv);

}

USymbolInfo Checker::visit(IfNode& n) {
    SymbolInfo symbol_info;
    USymbolInfo condition_info_p = this->dispatch(n.condition);
    SymbolInfo& condition_info = *condition_info_p;

    std::unordered_map<std::string, bool> not_null_vars;

    if (condition_info.type() != T_BOOL) {
        throw std::runtime_error(
                "Expected a Boolean expression as a condition for if statement at line " + std::to_string(n.line) +
                " column " + std::to_string(n.column) + ", got " +
                condition_info.type().to_string());
    }

    this->enter_scope("if");
    this->visit(*n.then);
    this->leave_scope();

    for (int i = 0; i < n.elifs.size(); i++) {
        condition_info_p = this->dispatch(n.elifs[i].first);
        condition_info = *condition_info_p;
        if (condition_info.type() != T_BOOL) {
            throw std::runtime_error(
                    "Expected a Boolean expression as a condition for elif statement!, got " +
                    condition_info.type().to_string());
        }
        this->enter_scope("elif");
        this->visit(*n.elifs[i].second);
        this->leave_scope();
    }
    if (n.selse != nullptr && !n.selse->nodes.empty()) {
        this->enter_scope("else");
        this->visit(*n.selse);
        this->leave_scope();
    }
    return std::make_unique<SymbolInfo>(symbol_info);
}

USymbolInfo Checker::visit(BinopNode& n) {
    USymbolInfo left_info_p = this->dispatch(n.left);
    USymbolInfo right_info_p = this->dispatch(n.right);

    SymbolInfo& left_info = *left_info_p;
    SymbolInfo& right_info = *right_info_p;

    SymbolInfo symbol_info;
    bool is_boolean = item_in_vec(
            n.op, {OpType::EQ, OpType::AND, OpType::OR, OpType::LEQ, OpType::GEQ, OpType::LT,
                   OpType::GT, OpType::NEQ}
    );
    if (is_boolean) {
        if (left_info.type().kind == Kind::OBJECT) {
            auto left = left_info.type().object();
            if (right_info.type().kind == Kind::OBJECT) {
                auto right = right_info.type().object();
                if (left.identifier == "Option" && right.identifier == "NoneType") {
                    symbol_info.set_type(ObjectTypeNode("Boolean", {}));
                }
            }
        } else {
            if (left_info.type() != right_info.type()) {
                throw std::runtime_error(
                        "Cannot perform binary op betweeen types " + left_info.type().to_string() + " and " +
                        right_info.type().to_string());
            }
        }
        symbol_info.set_type(ObjectTypeNode("Boolean", {}));
    } else {
        auto& left = left_info.type().object();
        auto& right = right_info.type().object();
        auto ltype = left.identifier;
        auto rtype = right.identifier;
        bool ok = false;
        if (ltype == "Integer" && rtype == "Integer") {
            symbol_info.set_type(ObjectTypeNode("Integer", {}));
            symbol_info.is_function = false;
            ok = true;
        } else if (ltype == "String" && rtype == "String") {
            if (n.op == OpType::ADD) {
                symbol_info.set_type(ObjectTypeNode("String", {}));
                symbol_info.is_function = false;
                ok = true;
            }
        } else if (ltype == "List" && rtype == "List" && left == (right)) {
            if (n.op == OpType::ADD) {
                symbol_info.set_type(left);
                symbol_info.is_function = false;
                ok = true;
            }
        }

        if (!ok) {
            throw std::runtime_error(
                    "Cannot perform binary op betweeen types " + left.to_string() + " and " + right.to_string());
        }
    }

    return std::make_unique<SymbolInfo>(symbol_info);
}

USymbolInfo Checker::visit(ReturnNode& n) {
    const TypeNode& return_type = this->scope->get("__return__");
    if (return_type == ObjectTypeNode(".None", {})) {
        if (n.expression->ntype != NodeType::UNINITIALIZED) {
            throw std::runtime_error("returning a value from a function returning no value!");
        }
        return nullptr;
    } else if (n.expression->ntype == NodeType::UNINITIALIZED) {
        throw std::runtime_error("not returning any value, but function expects type: " + return_type.to_string());
    }
    USymbolInfo expression_info_p = this->dispatch(n.expression);
    SymbolInfo& expression_info = *expression_info_p;
    if (this->replace_me) {
        n.expression = replacement;
        this->replace_me = false;
    }
    if (!this->can_assign(expression_info.type(), return_type)) {
        throw ReturnError(return_type, expression_info.type());
    }
    return nullptr;
}

bool is_generic(const TypeNode& t) {
    if (t.kind == Kind::OBJECT) {
        const ObjectTypeNode& o = t.object();
        if (o.identifier.size() == 1 && islower(o.identifier[0])) {
            // a is generic
            assert(o.type_parameters.size() == 0);
            return true;
        }
        for (auto type_param: o.type_parameters) {
            if (is_generic(*type_param)) {
                return true;
            }
        }
    } else {
        const FunctionTypeNode& fo = t.function();
        for (auto param_type: fo.parameter_types) {
            if (is_generic(*param_type)) {
                return true;
            }
        }
        if (is_generic(*fo.return_type)) {
            return true;
        }
    }
    return false;
}

std::unordered_map<std::string, TypeNode*> make_replacements(TypeNode* a, TypeNode* b) {
    std::unordered_map<std::string, TypeNode*> replacements;
    ObjectTypeNode& ob = b->object();
    if (a->kind == Kind::OBJECT) {
        ObjectTypeNode& oa = a->object();
        if (oa.identifier.size() == 1 && islower(oa.identifier[0])) {
            replacements[oa.identifier] = b;
        }
        for (int i = 0; i < oa.type_parameters.size(); i++) {
            if (is_generic(*oa.type_parameters[i])) {
                std::unordered_map<std::string, TypeNode*> rep = make_replacements(
                        oa.type_parameters[i],
                        ob.type_parameters[i]
                );
                replacements.insert(rep.begin(), rep.end());
            }
        }
    } else {
        if (a->kind == Kind::FUNCTION && b->kind == Kind::FUNCTION) {
            FunctionTypeNode& fa = a->function();
            FunctionTypeNode& fb = b->function();
            for (int i = 0; i < fa.parameter_types.size(); i++) {
                if (is_generic(*fa.parameter_types[i])) {
                    std::unordered_map<std::string, TypeNode*> rep = make_replacements(
                            fa.parameter_types[i],
                            fb.parameter_types[i]
                    );
                    replacements.insert(rep.begin(), rep.end());
                }
            }
            if (is_generic(*fa.return_type)) {
                std::unordered_map<std::string, TypeNode*> rep = make_replacements(fa.return_type, fb.return_type);
                replacements.insert(rep.begin(), rep.end());
            }
        }
    }
    return replacements;
}

bool type_matches(TypeNode* aa, TypeNode* bb) {
    auto& a = *aa;
    auto& b = *bb;
    if (a.kind != Kind::OBJECT && b.kind == Kind::OBJECT) {
        return false;
    } else if (a.kind == Kind::FUNCTION && b.kind == Kind::FUNCTION) {
        // both are functions
        FunctionTypeNode& fa = a.function();
        FunctionTypeNode& fb = b.function();
        FunctionTypeNode& new_f = fa;
        std::unordered_map<std::string, TypeNode*> replacements;
        if (fa.parameter_types.size() != fb.parameter_types.size()) {
            return false;
        }
        VectorOfTypes param_types = fa.parameter_types;
        for (int i = 0; i < param_types.size(); i++) {
            if (type_matches(param_types[i], fb.parameter_types[i])) {
                std::unordered_map<std::string, TypeNode*> rep = make_replacements(
                        fa.parameter_types[i],
                        fb.parameter_types[i]
                );
                replacements.insert(rep.begin(), rep.end());
                for (int j = 0; j < param_types.size(); j++) {
                    param_types[j] = make_type(*param_types[j], replacements);
                }
            } else {
                return false;
            }
        }
        return type_matches(make_type(*fa.return_type, replacements), fb.return_type);
    } else {
        // both are objects
        ObjectTypeNode& oa = a.object();
        ObjectTypeNode& ob = b.object();
        if (!is_generic(a)) {
            return a == (b);
        }
        // a is generic
        if (oa.type_parameters.size() == 0) {
            return true;
        }
        if (b.kind != Kind::FUNCTION) {
            return false;
        }
        if (oa.identifier != ob.identifier) {
            return false;
        }
        if (oa.type_parameters.size() != ob.type_parameters.size()) {
            return false;
        }
        for (int i = 0; i < oa.type_parameters.size(); i++) {
            if (!type_matches(oa.type_parameters[i], ob.type_parameters[i])) {
                return false;
            }
        }
        return true;
    }
    return false;
}

std::unordered_map<std::string, TypeNode*>
make_generic_replacements(TypeNode& t_generic_type, TypeNode& t_matching_type) {
    ObjectTypeNode& generic_type = (t_generic_type).object();
    ObjectTypeNode& matching_type = (t_matching_type).object();
    std::unordered_map<std::string, TypeNode*> replacements;
    if (generic_type.type_parameters.size() == 0) {
        replacements[generic_type.identifier] = &matching_type;
    } else {
        for (int i = 0; i < generic_type.type_parameters.size(); i++) {
            std::unordered_map<std::string, TypeNode*> param_replacements = make_generic_replacements(
                    *generic_type.type_parameters[i],
                    *matching_type.type_parameters[i]
            );
            replacements.insert(param_replacements.begin(), param_replacements.end());
        }
    }
    return replacements;
}

SymbolInfo
Checker::match_arguments_to_generic_function(const FunctionTypeNode& function_type, VectorOfTypes arg_types) {
    std::unordered_map<std::string, TypeNode*> generic_replacements;
    for (int i = 0; i < function_type.parameter_types.size(); i++) {
        TypeNode& param_type = *function_type.parameter_types[i];
        ObjectTypeNode& otn = param_type.object();
        if (is_generic(param_type)) {
            if (type_matches(&param_type, arg_types[i])) {
                std::unordered_map<std::string, TypeNode*> param_generic_replacements = make_replacements(
                        &param_type,
                        arg_types[i]
                );
                for (auto gtr: param_generic_replacements) {
                    if (generic_replacements.find(gtr.first) != generic_replacements.end()) {
                        // this type has already been replaced, see if it matches
                        if (gtr.second != (generic_replacements[gtr.first])) {
                            throw std::runtime_error(
                                    "Type has already been replacen by something that doesn't match!"
                            );
                        }
                    }
                }
                generic_replacements.insert(param_generic_replacements.begin(), param_generic_replacements.end());
            } else {
                throw std::runtime_error("Argument type error!");
            }
        } else {
            const TypeNode& arg_type = *arg_types[i];
            const TypeNode& param_type = *function_type.parameter_types[i];
            if (arg_type != param_type) {
                throw std::runtime_error(
                        "Function call type mismatch! Expected " +
                        param_type.to_string() + " but got " +
                        arg_type.to_string() + "  instead"
                );
            }
        }
    }

    SymbolInfo rv;
    const TypeNode& ret_type = *function_type.return_type;
    if (is_generic(ret_type)) {
        const ObjectTypeNode& rtn = ret_type.object();
        rv.set_type(*make_type(rtn, generic_replacements));
    } else {
        rv.set_type(ret_type);
    }
    return rv;

}


USymbolInfo Checker::visit(CallNode& n) {
    USymbolInfo fun_info_p = this->dispatch(n.function);
    SymbolInfo& fun_info = *fun_info_p;
    bool is_a_method = false;
    Node* object_node;
    SymbolInfo retv;
    if (fun_info.is_method) {
        // Since it's a method, we have to transform it and prepare it for the translation step,
        // where instead of calling object.method(args), we call <class>.method(object, args)

        if (n.function->ntype != NodeType::MEMBER) {
            throw std::runtime_error("Expected it to be a member node!");
        }
        MemberNode& member_node = n.function->member();
        n.function = new IdNode(fun_info.class_info->class_name + "." + member_node.s_child);
        this->replace_me = false;
        object_node = member_node.parent;
        is_a_method = true;
    } else if (fun_info.is_class_method) {
        if (n.function->ntype != NodeType::MEMBER) {
            throw std::runtime_error("Expected it to be a member node!");
        }
        MemberNode& member_node = n.function->member();
        n.function = new IdNode(fun_info.class_info->class_name + "." + member_node.s_child);
        this->replace_me = false;
        const FunctionTypeNode& ftn = fun_info.type().function();
        FunctionTypeNode& copy_ftn = ftn.clone()->function();
        copy_ftn.parameter_types.insert(copy_ftn.parameter_types.begin(), TYPE(fun_info.class_info->class_name, {}));
        retv.set_type(*copy_ftn.clone());
        object_node = member_node.parent;
    }
    if (fun_info.is_function || fun_info.is_method || fun_info.is_class_method) {
        // ok
        const FunctionTypeNode& function_type = fun_info.type().function();
        if (n.arguments.size() != function_type.parameter_types.size()) {
            throw std::runtime_error("Calling function with wrong number of arguments");
        }
        VectorOfTypes arg_types;
        for (auto& arg: n.arguments) {
            USymbolInfo arg_type_p = this->dispatch(arg);
            const TypeNode& arg_type = arg_type_p->type();
            if (this->replace_me) {
                arg = replacement;
                this->replace_me = false;
            }
            arg_types.push_back(arg_type.clone());
        }
        if (function_is_generic(function_type)) {
            retv = match_arguments_to_generic_function(function_type, arg_types);
        } else {
            for (int i = 0; i < n.arguments.size(); i++) {
                const TypeNode& arg_type = *arg_types[i];
                const TypeNode& param_type = *function_type.parameter_types[i];
                if (arg_type != param_type) {
                    throw std::runtime_error(
                            "At line " + std::to_string(n.line) + " column " + std::to_string(n.column) +
                            ": ERROR, Function call type mismatch!\n\tExpected: \n\t\t" +
                            param_type.to_string() + "\n\tbut got:\n\t\t" +
                            arg_type.to_string() + ""
                    );
                }
            }
            retv.set_type(*function_type.return_type);
        }
    } else {
        throw std::runtime_error("calling something that's not a function!");
    }
    if (is_a_method) {
        // prepend the "this" argument (the object on which the method is being called)
        n.arguments.insert(n.arguments.begin(), object_node);
    }
    return std::make_unique<SymbolInfo>(retv);
}

bool Checker::type_exists(TypeNode& type) {
    return false;
//    auto function_type = TO_FUNCTION_TYPE(type);
//    if (function_type != nullptr) {
//        for (auto pt: function_type->parameter_types) {
//            if (!this->type_exists(pt)) return false;
//        }
//        if (!this->type_exists(function_type->return_type)) return false;
//    } else {
//        auto object_type = (type)->object();
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

USymbolInfo Checker::visit(BlockNode& program) {
    for (auto n: program.nodes) {
        USymbolInfo sinfo_p = this->dispatch(n);
        SymbolInfo& sinfo = *sinfo_p;
        if (n->ntype == NodeType::CALL) {
            // it's a function call
            // if return value != NoneType, then force the return value
            if (sinfo.type() != ObjectTypeNode(".None", {})) {
                throw std::runtime_error("You should use the return value of this function call!");
            }
        }
    }
    return nullptr;
}

USymbolInfo Checker::visit(ClassLiteralExpressionNode& node) {
    ObjectTypeNode& object_type = *node.type;
    std::string& object_type_id = object_type.identifier;
    const std::string& object_type_str = object_type.to_string();

    if (!this->class_table->declared(object_type_id)) {
        throw std::runtime_error("No struct named " + object_type_id);
    }
    ClassInfo* class_info = this->class_table->get(object_type_id);
    unsigned long num_required_type_params = class_info->type_parameters.size();
    unsigned long num_actual_type_params = object_type.type_parameters.size();
    if (num_required_type_params != 0) {
        // it's a generic class
        if (num_required_type_params != num_actual_type_params) {
            throw std::runtime_error(
                    "Error: generic class requires " + std::to_string(num_required_type_params) +
                    " type parameters, but " + std::to_string(num_actual_type_params) + " given"
            );
        }
        if (this->class_table->declared(object_type_str)) {
            class_info = this->class_table->get(object_type_str);
        } else {
            class_info = instantiate_generic(class_info, object_type);
            this->class_table->set(object_type_str, class_info);
        }
    } else if (num_actual_type_params != 0) {
        throw std::runtime_error(
                "At line " + std::to_string(node.line) + " column " + std::to_string(node.column) + ": Error, class " +
                object_type_id + " is not generic, but given " +
                std::to_string(num_actual_type_params) + " type parameter(s)!"
        );
    }
    auto class_field_types_ordered = class_info->member_types;
    auto class_field_names_ordered = class_info->member_names;

    unsigned long num_required_init = class_field_names_ordered.size();
    unsigned long num_actual_init = node.init.size();
    if (num_required_init != num_actual_init) {
        throw std::runtime_error(
                "In struct \"" + object_type_id + "\" initialization: " + "Expected " +
                std::to_string(num_required_init) + " initializers but got " +
                std::to_string(num_actual_init));
    }

    for (int i = 0; i < num_actual_init; i++) {
        Node* exp = node.init[i];
        USymbolInfo semanticInfo_p = this->dispatch(exp);
        SymbolInfo& semanticInfo = *semanticInfo_p;
        if (this->replace_me) {
            node.init[i] = this->replacement;
            this->replace_me = false;
        }
        TypeNode& field_type = *class_field_types_ordered[i];
        if (!this->can_assign(semanticInfo.type(), field_type)) {
            throw std::runtime_error(
                    "In struct \"" + object_type_id + "\" initialization: " + "field \"" +
                    class_field_names_ordered[i] +
                    "\" is of type " +
                    field_type.to_string() +
                    " but got " + semanticInfo.type().to_string());
        }
    }
    node.names = class_field_names_ordered;
    SymbolInfo rv;
    rv.set_type(object_type);
    return std::make_unique<SymbolInfo>(rv);
}


bool Checker::can_assign(const TypeNode& from, const TypeNode& to) {
    auto to_object = (to).object();
    if (to_object.identifier == "Option") {
        if (*to_object.type_parameters[0] != from) {
            auto foo = from.object();
            if (foo.identifier != "NoneType") {
                return false;
            }
        }
        return true;
    } else if (to_object.identifier == "Union") {
        for (auto type_param: to_object.type_parameters) {
            if (*type_param == from) {
                return true;
            }
        }
        return false;
    }
    return to == from;
}

bool Checker::can_assign_generic(TypeNode& from, TypeNode& to, std::vector<std::string> type_params) {
    auto to_object = (to).object();
    if (to_object.type_parameters.size() == 0) {
        for (auto tp:type_params) {
            if (to_object.identifier == tp) {
                return true;
            }
        }
    }
    if (to_object.identifier == "Option") {
        if (*to_object.type_parameters[0] != from) {
            auto foo = from.object();
            if (foo.identifier != "NoneType") {
                return false;
            }
        }
        return true;
    } else if (to_object.identifier == "Union") {
        for (auto type_param: to_object.type_parameters) {
            if (*type_param == from) {
                return true;
            }
        }
        return false;
    }
    return to == (from);
}

TypeNode*
make_type_from_object_pattern(const ObjectTypeNode& object_type,
                              const std::unordered_map<std::string, TypeNode*>& replacements) {
    std::string type_identifier = object_type.identifier;
    for (auto r: replacements) {
        if (type_identifier == r.first) {
            if (object_type.type_parameters.size() != 0) {
                throw std::runtime_error(
                        "Trying to make a type for a template for exmaple struct Foo[T]{foo:T[Integer];}!"
                );
            }
            return r.second;
        }
    }
    // It's not the top level type
    VectorOfTypes new_type_params;
    for (auto tp: object_type.type_parameters) {
        TypeNode* new_tp = make_type(*tp, replacements);
        new_type_params.push_back(new_tp);
    }
    return TYPE(type_identifier, new_type_params);
}

TypeNode* make_type_from_function_pattern(const FunctionTypeNode& ftn,
                                          const std::unordered_map<std::string, TypeNode*>& replacements) {
    VectorOfTypes new_param_types;
    for (auto pt: ftn.parameter_types) {
        TypeNode* new_pt = make_type(*pt, replacements);
        new_param_types.push_back(new_pt);
    }
    TypeNode* new_return_type = make_type(*ftn.return_type, replacements);
    return FUNCTION_TYPE(new_param_types, new_return_type);
//        throw std::runtime_error("Making non object concrete type template!");
}

TypeNode* make_type(const TypeNode& original, const std::unordered_map<std::string, TypeNode*>& replacements) {
    if (original.kind == Kind::OBJECT) {
        return make_type_from_object_pattern(original.object(), replacements);
    } else {
        return make_type_from_function_pattern(original.function(), replacements);
    }
}

ClassInfo* Checker::instantiate_generic(ClassInfo* generic, const ObjectTypeNode& instance) {
    std::unordered_map<std::string, TypeNode*> replacements;
    for (int i = 0; i < generic->type_parameters.size(); i++) {
        std::string tp = generic->type_parameters[i];
        TypeNode& type_replacement = *instance.type_parameters[i];
        replacements[tp] = &type_replacement;
    }
    auto field_names = generic->member_names;
    VectorOfTypes concrete_field_types;
    for (auto f: generic->member_types) {
        TypeNode& concrete_type = *make_type(*f, replacements);
        concrete_field_types.push_back(&concrete_type);
    }

    std::unordered_map<std::string, FunctionTypeNode*> concrete_methods;
    for (auto m: generic->methods) {
        TypeNode& concrete_type = *make_type(*(m.second), replacements);
        concrete_methods.insert(std::make_pair(m.first, &concrete_type.function()));
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

USymbolInfo Checker::visit(ClassLiteralFieldNode& node) {
    ObjectTypeNode* object_type = node.type;
    std::string& object_type_id = object_type->identifier;
    const std::string& object_type_str = object_type->to_string();

    if (!this->class_table->declared(object_type_id)) {
        throw std::runtime_error("No struct named " + object_type_id);
    }

    if (!this->class_table->declared(object_type_id)) {
        throw std::runtime_error("No struct named " + object_type_id);
    }
    ClassInfo* class_info = this->class_table->get(object_type_id);
    unsigned long num_required_type_params = class_info->type_parameters.size();
    unsigned long num_actual_type_params = object_type->type_parameters.size();
    if (num_required_type_params != 0) {
        // it's a generic class
        if (num_required_type_params != num_actual_type_params) {
            throw std::runtime_error(
                    "Error: generic class requires " + std::to_string(num_required_type_params) +
                    " type parameters, but " + std::to_string(num_actual_type_params) + " given"
            );
        }
        if (this->class_table->declared(object_type_str)) {
            class_info = this->class_table->get(object_type_str);
        } else {
            class_info = instantiate_generic(class_info, *object_type);
            this->class_table->set(object_type_str, class_info);
        }
    }
    auto class_fields = class_info->members;

    for (auto f: node.init) {
        if (class_fields.find(f.first) == class_fields.end()) {
            throw std::runtime_error("No field named " + f.first);
        }
    }
    if (class_fields.size() != node.init.size()) {
        throw std::runtime_error(
                "In struct \"" + object_type_id + "\" initialization: " + "Expected " +
                std::to_string(class_fields.size()) + " initializers but got " +
                std::to_string(node.init.size()));
    }
    for (auto f: node.init) {
        Node* exp = f.second;
        USymbolInfo semanticInfo_p = this->dispatch(exp);
        SymbolInfo& semanticInfo = *semanticInfo_p;
        if (this->replace_me) {
            node.init[f.first] = this->replacement;
            this->replace_me = false;
        }
        TypeNode& field_type = *class_fields[f.first];
        if (!this->can_assign(semanticInfo.type(), field_type)) {
            throw std::runtime_error(
                    "In struct \"" + object_type_id + "\" initialization: " + "field \"" + f.first +
                    "\" is of type " +
                    field_type.to_string() +
                    " but got " + semanticInfo.type().to_string());
        }
    }
    SymbolInfo rv;
    rv.set_type(*object_type);
    return std::make_unique<SymbolInfo>(rv);
}

USymbolInfo Checker::visit(ForNode& node) {
    USymbolInfo symbol_info_p = this->dispatch(node.exp);
    SymbolInfo& symbol_info = *symbol_info_p;
    if (symbol_info.type().kind != Kind::OBJECT) {
        throw std::runtime_error("Iterating over something bad!");
    }

    const ObjectTypeNode& obj = symbol_info.type().object();
    if (obj.identifier != "List") {
        throw std::runtime_error(
                "At line " + std::to_string(node.line) + " column " + std::to_string(node.column) +
                ": For loop should have a List[t] after @ but got " + obj.to_string());
    }

    TypeNode& var_type = *obj.type_parameters[0];
    this->enter_scope("for");
    this->scope->set(node.var, var_type);
    this->visit(*node.body);
    this->leave_scope();
    return nullptr;
}

USymbolInfo Checker::visit(ListNode& node) {
    USymbolInfo element_type_p = this->dispatch(node.elements[0]);
    const TypeNode& element_type = element_type_p->type();
    if (this->replace_me) {
        node.elements[0] = this->replacement;
        this->replace_me = false;
    }
    for (int i = 1; i < node.elements.size(); i++) {
        USymbolInfo current_type_p = this->dispatch(node.elements[i]);
        const TypeNode& current_type = current_type_p->type();
        if (this->replace_me) {
            node.elements[i] = this->replacement;
            this->replace_me = false;
        }
        if (current_type != element_type) {
            throw std::runtime_error(
                    "List literal with more than one element type, first element has type: " +
                    element_type.to_string() + " but at index " + std::to_string(i) +
                    " got type " +
                    current_type.to_string());
        }
    }
    SymbolInfo return_info;
    return_info.is_function = false;
    return_info.set_type(ObjectTypeNode("List", {element_type.clone()}));
    return std::make_unique<SymbolInfo>(return_info);
}

USymbolInfo Checker::visit(BooleanNode& node) {
    SymbolInfo symbol_info;
    symbol_info.set_type(ObjectTypeNode("Boolean", {}));
    symbol_info.is_function = false;
    return std::make_unique<SymbolInfo>(symbol_info);
}

USymbolInfo Checker::visit(WhileNode& node) {
    USymbolInfo condition_p = this->dispatch(node.condition);
    SymbolInfo& condition = *condition_p;
    if (condition.type() != ObjectTypeNode("Boolean", {})) {
        std::string str = condition.type().to_string();
//        throw std::runtime_error("At line " +
//                                 std::to_string(node.condition->line + 1) + " column " +
//                                 std::to_string(node.condition->column + 1) +
//                                 ": Expected Boolean expression as while loop condition, got " +
//                                 condition.type->to_string());
        throw std::runtime_error(
                "At line " + std::to_string(node.line) + " column " + std::to_string(node.column) +
                ": Expected Boolean expression as while loop condition, got " + str
        );
    }
    this->enter_scope("while");
    this->visit(*node.body);
    this->leave_scope();
    return nullptr;
}

USymbolInfo Checker::visit(NumberNode& node) {
    SymbolInfo semanticInfo;
    semanticInfo.set_type(ObjectTypeNode("Integer", {}));
    semanticInfo.is_function = false;
    return std::make_unique<SymbolInfo>(semanticInfo);
}

USymbolInfo Checker::visit(StringNode& node) {
    SymbolInfo semanticInfo;
    semanticInfo.set_type(ObjectTypeNode("String", {}));
    semanticInfo.is_function = false;
    return std::make_unique<SymbolInfo>(semanticInfo);
}

USymbolInfo Checker::visit(SubscriptNode& node) {
    USymbolInfo parent_p = this->dispatch(node.parent);
    SymbolInfo& parent = *parent_p;
    if (parent.type().kind != Kind::OBJECT) {
        throw std::runtime_error("Accessing subscript of non object!");
    }
    SymbolInfo symbol_info;
    const ObjectTypeNode& object_type = parent.type().object();

    VectorOfTypes children;
    bool not_integer = false;
    if (node.child.size() > 1) {
        throw std::runtime_error("Error subscript with more than one child!");
    }
    for (auto& c: node.child) {
        USymbolInfo ct = this->dispatch(c);
        if (ct->type() != T_INT) {
            not_integer = true;
        }
        children.emplace_back(ct->type().clone());
    }

    if (object_type.identifier == "List") {
        if (not_integer) {
            throw std::runtime_error("Access not number subscript of List!");
        }
        symbol_info.set_type(*object_type.type_parameters[0]);
    } else if (object_type.identifier == "String") {
        if (not_integer) {
            throw std::runtime_error("Access not number subscript of String!");
        }
        symbol_info.set_type(object_type);
    }

    symbol_info.is_function = false;

    return std::make_unique<SymbolInfo>(symbol_info);
}

USymbolInfo Checker::visit(BreakNode& node) {
    return nullptr;
}

USymbolInfo Checker::visit(TernaryNode& node) {
    USymbolInfo expression_info_p = this->dispatch(node.expression);
    SymbolInfo& expression_info = *expression_info_p;
    if (expression_info.type().kind != Kind::OBJECT) {
        throw std::runtime_error("Unexpected non-object");
    }
    auto expression_type = expression_info.type().object();

    if (expression_type.identifier != "Option") {
        throw std::runtime_error("Expected an Option[T], got: " + expression_type.to_string());
    }
    SymbolInfo semanticInfo;
    TypeNode& type = *expression_type.type_parameters[0];
    semanticInfo.set_type(type);
    this->enter_scope("true_case");
    this->scope->set("it", type);
    USymbolInfo true_case_p = this->dispatch(node.true_case);
    SymbolInfo& true_case = *true_case_p;
    if (this->replace_me) {
        node.true_case = this->replacement;
        this->replace_me = false;
    }
    this->leave_scope();
    USymbolInfo false_case_p = this->dispatch(node.false_case);
    SymbolInfo& false_case = *false_case_p;
    if (this->replace_me) {
        node.false_case = this->replacement;
        this->replace_me = false;
    }
    if (false_case.type() != true_case.type()) {
        throw std::runtime_error(
                "True case and false case type don't match: " + true_case.type().to_string() + " != " +
                false_case.type().to_string());
    } else {
        semanticInfo.set_type(true_case.type());
    }
    return std::make_unique<SymbolInfo>(semanticInfo);
}

USymbolInfo Checker::visit(NoneNode& node) {
    SymbolInfo semanticInfo;
    semanticInfo.set_type(ObjectTypeNode("NoneType", {}));
    semanticInfo.is_function = false;
    return std::make_unique<SymbolInfo>(semanticInfo);
}

USymbolInfo Checker::visit(EmptyListNode& node) {
    SymbolInfo semanticInfo;
    semanticInfo.set_type(T_LIST(node.type->clone()));
    semanticInfo.is_function = false;
    return std::make_unique<SymbolInfo>(semanticInfo);
}

USymbolInfo Checker::visit(ClassNode& node) {
    this->add_this = true;
    VectorOfTypes tp;
    for (auto type_param: node.type_parameters) {
        tp.push_back(TYPE(type_param, {}));
    }
    this->this_type = new ObjectTypeNode(node.class_name, tp);
    for (auto method: node.methods) {
        this->visit(*method.second);
    }
    this->add_this = false;
    delete this_type;
    this->this_type = nullptr;
    return nullptr;
}

USymbolInfo Checker::visit(ContinueNode& node) {
    return nullptr;
}

USymbolInfo Checker::dispatch(Node* nod) {
    auto& n = *nod;
    switch (n.ntype) {
        case NodeType::ASSIGN:
            return this->visit(n.assign());
        case NodeType::BINOP:
            return this->visit(n.binop());
            break;
        case NodeType::BLOCK:
            return this->visit(n.block());
            break;
        case NodeType::BOOLEAN:
            return this->visit(n.boolean());
            break;
        case NodeType::BRK:
            return this->visit(n.brk());
            break;
        case NodeType::CALL:
            return this->visit(n.call());
            break;
        case NodeType::CLSEXP:
            return this->visit(n.clsexp());
            break;
        case NodeType::CLSFLD:
            return this->visit(n.clsfld());
            break;
        case NodeType::CLS:
            return this->visit(n.cls());
            break;
        case NodeType::CNTINUE:
            return this->visit(n.cntinue());
            break;
        case NodeType::DECL:
            return this->visit(n.decl());
            break;
        case NodeType::EMPTYLST:
            return this->visit(n.emptylst());
            break;
        case NodeType::FORLOOP:
            return this->visit(n.forloop());
            break;
        case NodeType::FUNC:
            return this->visit(n.func());
            break;
        case NodeType::ID:
            return this->visit(n.id());
            break;
        case NodeType::IFF:
            return this->visit(n.iff());
            break;
        case NodeType::LST:
            return this->visit(n.lst());
            break;
        case NodeType::MEMBER:
            return this->visit(n.member());
            break;
        case NodeType::NONE:
            return this->visit(n.none());
            break;
        case NodeType::NUMBER:
            return this->visit(n.number());
            break;
        case NodeType::RETRN:
            return this->visit(n.retrn());
            break;
        case NodeType::STRNG:
            return this->visit(n.strng());
            break;
        case NodeType::SUB:
            return this->visit(n.sub());
            break;
        case NodeType::TERNARY:
            return this->visit(n.ternary());
            break;
        case NodeType::TUPLE:
            return this->visit(n.tuple());
            break;
        case NodeType::WHIL:
            return this->visit(n.whil());
            break;
        case NodeType::UNINITIALIZED:
            break;
        default:
            throw std::runtime_error("Don't know what to do!");
    }
    return nullptr;
}

TypeClassInfo* Checker::get_typeclass_for_function(std::string function_name) {
    for (auto typeclass: this->typeclasses) {
        if (typeclass.second->has_function(function_name)) {
            return typeclass.second;
        }
    }
    return nullptr;
}

USymbolInfo Checker::visit(TupleNode& node) {
    VectorOfTypes types;
    for (auto n: node.values) {
        USymbolInfo vtype = this->dispatch(n);
        types.emplace_back(vtype->type().clone());
    }
    ObjectTypeNode tuple_type("Tuple", types);
    SymbolInfo sinfo;
    sinfo.set_type(tuple_type);
    return std::make_unique<SymbolInfo>(sinfo);
}
