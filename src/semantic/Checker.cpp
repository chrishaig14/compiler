#include <iostream>
#include <set>
#include "Checker.h"
#include "TypeClassInfo.h"
#include "../macros.h"
#include "unify.h"
#include "../logging/logging.h"

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

ClassInfo* make_boolean_class_info() {
    auto int_class_info = new ClassInfo();
    int_class_info->class_name = "Boolean";
    int_class_info->methods.insert(std::make_pair("str", new FunctionTypeNode({}, new T_STRING)));
    return int_class_info;
}

ClassInfo* make_float_class_info() {
    auto float_class_info = new ClassInfo();
    float_class_info->class_name = "Float";
    float_class_info->methods.insert(std::make_pair("str", new FunctionTypeNode({}, new T_STRING)));
    return float_class_info;
}

ClassInfo* make_string_class_info() {
    auto string_class_info = new ClassInfo();
    string_class_info->class_name = "String";
    string_class_info->methods.insert(std::make_pair("len", new FunctionTypeNode({}, new T_INT)));
    return string_class_info;
}

Checker::Checker(SymbolTable* globals, ClassTable* class_table, FunctionTable* function_table) {
    this->is_lvalue = false;
    this->function_table = function_table;
    this->failed = false;
    this->class_table = class_table;
    this->scope = globals;
    this->scopes["global"] = this->scope;

    this->add_this = false;
    this->this_type = nullptr;

    this->class_table->set("Float", make_float_class_info());
    this->class_table->set("Integer", make_int_class_info());
    this->class_table->set("List", make_list_class_info());
    this->class_table->set("Boolean", make_boolean_class_info());
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

void Checker::assert_type_exists(TypeNode& type, TextPosition pos) {
    if (type.kind == Kind::OBJECT) {
        if (type.object().type_parameters.size() == 0) {
            if (!is_generic(type)) {
                if (!this->class_table->declared(type.object().identifier)) {
                    std::string msg;
                    msg = E_FMT(text_pos_to_string(this->__file__, pos));
                    msg += E_FMT(" type ") + E_HLT(type.object().identifier) + E_FMT(" doesn't exist");
                    std::cout << msg << std::endl;
                    // exit(1);
                }
            }
            return;
        }
        if (!this->class_table->declared(type.object().identifier)) {
            std::string msg;
            msg = E_FMT(text_pos_to_string(this->__file__, pos));
            msg += E_FMT(" type ") + E_HLT(type.object().identifier) + E_FMT(" doesn't exist");
            std::cout << msg << std::endl;
            // exit(1);
        } else {
            for (auto t: type.object().type_parameters) {
                this->assert_type_exists(*t, pos);
            }
        }
    } else {
        for (auto t: type.function().parameter_types) {
            this->assert_type_exists(*t, pos);
        }
        this->assert_type_exists(*type.function().return_type, pos);
    }
}

USymbolInfo Checker::visit(FunctionNode& n) {
    this->current_function = n.identifier;
    this->enter_scope(n.identifier);
    if (this->add_this) {
        this->scope->set("this", *this->this_type);
    }
    for (int i = 0; i < n.parameter_names.size(); i++) {
        TypeNode& type = *n.parameter_types[i];
        if (type.kind == Kind::OBJECT) {
            this->assert_type_exists(type, n.start);
        }
        this->scope->set(n.parameter_names[i], type);
    }
    TypeNode& returnType = *n.return_type;
    this->assert_type_exists(returnType, n.start);
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

void Checker::error_variable_not_declared(const std::string& name, TextPosition pos) {
    std::string msg;
    msg = this->context_string(pos) +
          E_FMT("Variable ") +
          E_HLT("'" + name + "'") +
          E_FMT(" not declared") +
          this->code_context_string(pos);
    std::cout << msg << std::endl;
    this->failed = true;
}

USymbolInfo Checker::visit(IdNode& n) {
    SymbolInfo symbol_info;
    symbol_info.is_function = false;
    symbol_info.is_method = false;
    if (!this->scope->has(n.identifier)) {
        // it might be a function name
        if (this->function_table->has_function(n.identifier)) {
            n.is_global_function = true;
            symbol_info.is_function = true;
            n.location = VariableLocation(-2, -1);
            symbol_info.set_type(this->function_table->get(n.identifier));
        } else {
            this->error_variable_not_declared(n.identifier, n.start);
            return std::make_unique<SymbolInfo>(ErrorStub());
        }
    } else {
        symbol_info.set_type(this->scope->get(n.identifier));
        if (symbol_info.type().kind == Kind::UNKNOWN) {
            symbol_info.is_error = true;
        } else {
            n.location = this->scope->find(n.identifier);
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
    }
    return std::make_unique<SymbolInfo>(symbol_info);
}

void Checker::error_redeclared(const std::string& name, TextPosition pos) {
    std::string msg;
    msg = this->context_string(pos) + E_FMT("Variable ") + E_HLT(name) + E_FMT(" already declared ") +
          this->code_context_string(pos);
    std::cout << msg << std::endl;
}

USymbolInfo Checker::visit(DeclarationNode& n) {
    if (this->scope->declared(n.identifier)) {
        this->error_redeclared(n.identifier, n.start);
        this->failed = true;
    }
    SymbolInfo symbol_info;
    symbol_info.is_function = false;
    if (n.expression->ntype != NodeType::UNINITIALIZED and n.type != nullptr) {
        this->assert_type_exists(*n.type, n.start);
        USymbolInfo exp_info_p = this->dispatch(n.expression);
        SymbolInfo& exp_info = *exp_info_p;
        if (exp_info.is_error) {
            this->scope->set(n.identifier, *n.type);
            return std::make_unique<SymbolInfo>(symbol_info);
        }
        if (this->replace_me) {
            n.expression = replacement;
            this->replace_me = false;
        }
        if (n.type->kind == Kind::FUNCTION) {
            // it's a function
            if (*n.type != exp_info.type()) {
                this->error_assignment(*n.type, exp_info.type(), n.start);
                // exit(1);
            }
        } else {
            SymbolInfo expression_info = exp_info;
            const ObjectTypeNode& actual_type = n.type->object();
            if (actual_type.identifier == "Option") {
                if (*actual_type.type_parameters[0] != expression_info.type()) {
                    auto foo = expression_info.type().object();
                    if (foo.identifier != "NoneType") {
                        this->error_assignment(*n.type, expression_info.type(), n.start);
                        // exit(1);
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
                    this->error_assignment(*n.type, expression_info.type(), n.start);
                    // exit(1);
                }
            } else {
                if (*n.type != expression_info.type()) {
                    this->error_assignment(*n.type, expression_info.type(), n.start);
                    // exit(1);
                }
            }
        }
        symbol_info.set_type(*n.type);

    } else if (n.expression->ntype != NodeType::UNINITIALIZED) {
        USymbolInfo exp_info_p = this->dispatch(n.expression);
        n.type = exp_info_p->type().clone();
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

std::string Checker::context_string(TextPosition position) {
    std::string msg = E_HLT(text_pos_to_string(this->__file__, position)) +
                      E_FMT(" In function ") +
                      E_HLT((this->current_class == "" ? "" : this->current_class + ".") + this->current_function) +
                      E_FMT(": ");
    return msg;
}


std::string Checker::code_context_string(TextPosition position) {
    std::string str = "\n" + this->code_lines.get_line(position.line) + "\n";
    str += fmt::format(fmt::fg(fmt::color::orange_red), std::string(position.column, ' ') + std::string(1, '^'));
    return str;
}

std::string Checker::code_error_string(TextPosition start, TextPosition end) {
    int length = end.column - start.column + 1;
    std::string str = "\n" + this->code_lines.get_line(start.line) + "\n";
    str += fmt::format(fmt::fg(fmt::color::orange_red), std::string(start.column, ' ') + std::string(length, '^'));
    return str;
}


void Checker::error_binop(const TypeNode& left, const TypeNode& right, TextPosition position) {
    this->failed = true;
    std::string msg;
    msg = context_string(position) +
          E_FMT("Cannot perform binary op between types ") + E_HLT(left.to_string()) +
          E_FMT(" and ") +
          E_HLT(right.to_string()) + this->code_context_string(position);
    std::cout << msg << std::endl;
}

void Checker::error_no_member(const TypeNode& t, const std::string& member, TextPosition position) {
    this->failed = true;
    std::string msg;
    msg =
            this->context_string(position) +
            E_FMT("Type ") + E_HLT(t.to_string()) +
            E_FMT(" has no member ") +
            E_HLT("'" + member + "'") +
            this->code_context_string(position);
    std::cout << msg << std::endl;
}

void Checker::error_bool_op(const TypeNode& left, const TypeNode& right, TextPosition position) {
    this->failed = true;
    std::string msg;
    msg = context_string(position) +
          E_FMT("Cannot perform bool op between types ") + E_HLT(left.to_string()) +
          E_FMT(" and ") +
          E_HLT(right.to_string());
    std::cout << msg << std::endl;
}

void Checker::error_assignment(const TypeNode& expected, const TypeNode& actual, TextPosition position) {
    this->failed = true;
    std::string msg;
    msg = context_string(position) +
          E_FMT("Expected ") +
          E_HLT(expected.to_string()) +
          E_FMT(", got ") +
          E_HLT(actual.to_string()) +
          this->code_error_string(position, position);
    std::cout << msg << std::endl;
}

void Checker::error_condition(const TypeNode& t, TextPosition position, const std::string& st) {
    this->failed = true;
    std::string msg;
    msg = context_string(position) +
          E_FMT(" Expected ") + E_HLT("Boolean ") +
          E_FMT("as condition for " + st + " statement, got ") +
          E_HLT(t.to_string());
    std::cout << msg << std::endl;
}

void Checker::error_no_return(const TypeNode& t, TextPosition position) {
    this->failed = true;
    std::string msg;
    msg = context_string(position) + E_FMT(" Expected to return ") +
          E_HLT(t.to_string()) +
          E_FMT(" but not returning anything");
    std::cout << msg << std::endl;
}

void Checker::error_bad_return(TextPosition position) {
    this->failed = true;
    std::string msg;
    msg = E_HLT(text_pos_to_string(this->__file__, position)) +
          E_FMT(" Returning a value from a function returning no value ");
    std::cout << msg << std::endl;
}

void Checker::error_return_mismatch(const TypeNode& expected, const TypeNode& actual, TextPosition position) {
    this->failed = true;
    std::string msg;
    msg = E_HLT(text_pos_to_string(this->__file__, position)) +
          E_FMT(" In function ") +
          E_HLT((this->current_class == "" ? "" : this->current_class + ".") + this->current_function) + E_FMT(": ") +
          E_FMT(" Expected to return ") + E_HLT(expected.to_string()) + E_FMT(" but got ") + E_HLT(actual.to_string());
    std::cout << msg << std::endl;
}

void Checker::error_tuple_assign(TextPosition pos) {
    std::string msg =
            this->context_string(pos) +
            E_FMT("Error: can't reassign a member of a tuple!") +
            this->code_context_string(pos);
    std::cout << msg << std::endl;
}


USymbolInfo Checker::visit(AssignmentNode& n) {
    if (n.lvalue->ntype == NodeType::ID) {
        if (n.lvalue->id().identifier == "_") {
            this->dispatch(n.rvalue);
            return nullptr;
        }
    }
    this->is_lvalue = true;
    USymbolInfo linfo_p = this->dispatch(n.lvalue);
    if (linfo_p->is_error) {
        return nullptr;
    }
    this->is_lvalue = false;
    if (n.lvalue->ntype == MEMBER && n.lvalue->member().type == MemberType::NUM) {
        this->error_tuple_assign(n.start);
        this->failed = true;
    }
    USymbolInfo expression_type_p = this->dispatch(n.rvalue);
    if (expression_type_p->is_error) {
        return nullptr;
    }
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
                auto& foo = expression_type.type().object();
                if (foo.identifier != "NoneType") {
                    this->error_assignment(linfo.type(), expression_type.type(), n.start);
                    // exit(1);
                }
                // assigning none, ok
            }
            // type matches exactly, no proble
            std::cout << "p may be none" << std::endl;
            n.type = linfo.type().clone();
            this->scope->set_not_none(n.lvalue->id().identifier, false);
        }
    } else {
        if (linfo.type() != expression_type.type()) {
            if (actual_type.identifier == "Option") {
                // if type doesn't match exactly, we may be assigning to an Option[t]
                if (*actual_type.type_parameters[0] != expression_type.type()) {
                    auto& foo = expression_type.type().object();
                    if (foo.identifier != "NoneType") {
                        this->error_assignment(
                                linfo.type(),
                                expression_type.type(),
                                n.start
                        );
                        // exit(1);
                    }
                }
            } else {
                // if it's not Option[t], then it's an error
                this->error_assignment(linfo.type(), expression_type.type(), n.start);
                // exit(1);
            }
        }
        // else, type matches don't do anything
        n.type = linfo.type().clone();
    }
    return nullptr;
}

void Checker::error_member_no_object(TextPosition pos) {
    std::string msg;
    msg = this->context_string(pos) + E_FMT("Accessing member of non object ") + this->code_context_string(pos);
    std::cout << msg << std::endl;
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
                IdNode* idn = new IdNode(class_info->class_name + "." + n.s_child);
                idn->is_global_function = true;
                this->replacement = idn;
                return std::make_unique<SymbolInfo>(rv);
            } else {
                throw std::runtime_error("Class " + class_info->class_name + " has no method " + n.s_child);
            }
        }
    }
    bool old_lvalue = this->is_lvalue;
    this->is_lvalue = false;
    USymbolInfo symbol_info_p = this->dispatch(n.parent);
    this->is_lvalue = old_lvalue;
    SymbolInfo& symbol_info = *symbol_info_p;
    if (symbol_info.type().kind != Kind::OBJECT) {
        this->error_member_no_object(n.start);
        return std::make_unique<SymbolInfo>(ErrorStub());
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
                        "Error: line " + text_pos_to_string(this->__file__, idn.start) + " -> " + idn.identifier +
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
            this->error_no_member(object, n.s_child, n.start);
            return std::make_unique<SymbolInfo>(ErrorStub());
            // exit(1);
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
        this->error_condition(condition_info.type(), n.start, "if");
        // exit(1);
    }

    this->enter_scope("if");
    this->visit(*n.then);
    this->leave_scope();

    for (int i = 0; i < n.elifs.size(); i++) {
        condition_info_p = this->dispatch(n.elifs[i].first);
        SymbolInfo& condition_info = *condition_info_p;
        if (condition_info.type() != T_BOOL) {
            this->error_condition(condition_info.type(), n.start, "elif");
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

USymbolInfo Checker::visit(BoolOpNode& n) {
    USymbolInfo left_info_p = this->dispatch(n.left);
    USymbolInfo right_info_p = this->dispatch(n.right);

    SymbolInfo& left_info = *left_info_p;
    SymbolInfo& right_info = *right_info_p;

    SymbolInfo symbol_info;
    bool ok = false;
    if (left_info.type().kind == Kind::OBJECT) {
        auto& left = left_info.type().object();
        if (right_info.type().kind == Kind::OBJECT) {
            auto& right = right_info.type().object();
            if (left.identifier == "Option" && right.identifier == "NoneType") {
                symbol_info.set_type(ObjectTypeNode("Boolean", {}));
                ok = true;
            }
        }
    }
    if (!ok && left_info.type() != right_info.type()) {
        this->error_bool_op(left_info.type(), right_info.type(), n.start);
        // exit(1);
    }

    symbol_info.set_type(ObjectTypeNode("Boolean", {}));

    return std::make_unique<SymbolInfo>(symbol_info);
}

USymbolInfo Checker::visit(BinopNode& n) {
    USymbolInfo left_info_p = this->dispatch(n.left);
    if (this->replace_me) {
        n.left = this->replacement;
        this->replace_me = false;
    }

    USymbolInfo right_info_p = this->dispatch(n.right);
    if (this->replace_me) {
        n.right = this->replacement;
        this->replace_me = false;
    }

    SymbolInfo& left_info = *left_info_p;
    SymbolInfo& right_info = *right_info_p;

    if (left_info.is_error || right_info.is_error) {
        auto e = ErrorStub();
        auto s = std::make_unique<SymbolInfo>(e);
        return s;
    }

    SymbolInfo symbol_info;
    auto& left = left_info.type().object();
    auto& right = right_info.type().object();
    auto ltype = left.identifier;
    auto rtype = right.identifier;
    bool ok = false;
    if (ltype == "Integer" && rtype == "Integer") {
        symbol_info.set_type(ObjectTypeNode("Integer", {}));
        symbol_info.is_function = false;
        ok = true;
    } else if (ltype == "Float" && rtype == "Float") {
        symbol_info.set_type(ObjectTypeNode("Float", {}));
        symbol_info.is_function = false;
        ok = true;
    } else if (ltype == "Float" && rtype == "Integer" || ltype == "Integer" && rtype == "Float") {
        symbol_info.set_type(ObjectTypeNode("Float", {}));
        symbol_info.is_function = false;
        ok = true;
    } else if (ltype == "String" && rtype == "String") {
        if (n.op == OpType::ADD) {
            symbol_info.set_type(ObjectTypeNode("String", {}));
            symbol_info.is_function = false;
            IdNode* idn = new IdNode("String_add");
            idn->is_global_function = true;
            this->replace_me = true;
            this->replacement = new CallNode(idn, VectorOfNodes({n.left, n.right}));
            ok = true;
        }
    } else if (ltype == "List" && rtype == "List" && left == (right)) {
        if (n.op == OpType::ADD) {
            symbol_info.set_type(left);
            symbol_info.is_function = false;
            IdNode* idn = new IdNode("List_add");
            idn->is_global_function = true;
            this->replace_me = true;
            this->replacement = new CallNode(idn, VectorOfNodes({n.left, n.right}));
            ok = true;
        }
    }

    if (!ok) {
        this->error_binop(left, right, n.op_pos);
        return std::make_unique<ErrorStub>(ErrorStub());
        // exit(1);
    }

    return std::make_unique<SymbolInfo>(symbol_info);
}


USymbolInfo Checker::visit(ReturnNode& n) {
    const TypeNode& return_type = this->scope->get("__return__");
    if (return_type == ObjectTypeNode(".None", {})) {
        if (n.expression != nullptr) {
            this->error_bad_return(n.start);
            // exit(1);
        }
        return nullptr;
    } else if (n.expression == nullptr) {
        this->error_no_return(return_type, n.start);
        // exit(1);
    }
    USymbolInfo expression_info_p = this->dispatch(n.expression);
    SymbolInfo& expression_info = *expression_info_p;
    if (expression_info.is_error) {
        return nullptr;
    }
    if (this->replace_me) {
        n.expression = replacement;
        this->replace_me = false;
    }
    if (!this->can_assign(expression_info.type(), return_type)) {
        this->error_return_mismatch(return_type, expression_info.type(), n.start);
        this->failed = true;
        return std::make_unique<SymbolInfo>(ErrorStub());
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
    if (a->kind == Kind::OBJECT) {
        ObjectTypeNode& oa = a->object();
        ObjectTypeNode& ob = b->object();
        if (oa.identifier.size() == 1 && islower(oa.identifier[0])) {
            replacements[oa.identifier] = b->clone();
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

std::vector<TypeNode*> make_replacements_in_order(TypeNode* a, TypeNode* b) {
    std::vector<TypeNode*> replacements;
    if (a->kind == Kind::OBJECT) {
        ObjectTypeNode& oa = a->object();
        ObjectTypeNode& ob = b->object();
        if (oa.identifier.size() == 1 && islower(oa.identifier[0])) {
            replacements.push_back(b->clone());
        } else {
            for (int i = 0; i < oa.type_parameters.size(); i++) {
                if (is_generic(*oa.type_parameters[i])) {
                    std::vector<TypeNode*> rep = make_replacements_in_order(
                            oa.type_parameters[i],
                            ob.type_parameters[i]
                    );
                    replacements.insert(replacements.end(), rep.begin(), rep.end());
                }
            }
        }
    } else {
        if (a->kind == Kind::FUNCTION && b->kind == Kind::FUNCTION) {
            FunctionTypeNode& fa = a->function();
            FunctionTypeNode& fb = b->function();
            for (int i = 0; i < fa.parameter_types.size(); i++) {
                if (is_generic(*fa.parameter_types[i])) {
                    std::vector<TypeNode*> rep = make_replacements_in_order(
                            fa.parameter_types[i],
                            fb.parameter_types[i]
                    );
                    replacements.insert(replacements.end(), rep.begin(), rep.end());
                }
            }
            if (is_generic(*fa.return_type)) {
                std::vector<TypeNode*> rep = make_replacements_in_order(fa.return_type, fb.return_type);
                replacements.insert(replacements.end(), rep.begin(), rep.end());
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
make_generic_replacements(TypeNode& t_generic_type, TypeNode& t_matching_type);

std::unordered_map<std::string, TypeNode*>
make_function_generic_replacements(FunctionTypeNode& t_generic_type, FunctionTypeNode& t_matching_type) {
    std::unordered_map<std::string, TypeNode*> repl;

    if (t_generic_type.parameter_types.size() != t_matching_type.parameter_types.size()) {
        throw std::runtime_error("Error parameter_types size mismatch!");
    }
    for (int i = 0; i < t_generic_type.parameter_types.size(); i++) {
        if (is_generic(*t_generic_type.parameter_types[i])) {
            auto r = make_generic_replacements(
                    *t_generic_type.parameter_types[i],
                    *t_matching_type.parameter_types[i]
            );
            for (auto x: r) {
                if (repl.count(x.first) != 0 && *repl[x.first] != *x.second) {
                    throw std::runtime_error("Error type already replaced by something else");
                }
            }
            repl.insert(r.begin(), r.end());
        } else {
            if (*t_generic_type.parameter_types[i] != *t_matching_type.parameter_types[i]) {
                throw std::runtime_error("Error: parameter type mismatch!");
            }
        }
    }

    if (is_generic(*t_generic_type.return_type)) {
        auto r = make_generic_replacements(
                *t_generic_type.return_type,
                *t_matching_type.return_type
        );
        for (auto x: r) {
            if (repl.count(x.first) != 0 && *repl[x.first] != *x.second) {
                throw std::runtime_error("Error type already replaced by something else");
            }
        }
        repl.insert(r.begin(), r.end());
    }

    return repl;
}

std::unordered_map<std::string, TypeNode*>
make_object_generic_replacements(ObjectTypeNode& t_generic_type, ObjectTypeNode& t_matching_type) {
    std::unordered_map<std::string, TypeNode*> repl;
    if (t_generic_type.type_parameters.size() == 0) {
        repl[t_generic_type.identifier] = t_matching_type.clone();
    } else {
        if (t_generic_type.type_parameters.size() != t_matching_type.type_parameters.size()) {
            throw std::runtime_error("Error type parameter size mismatch!");
        }
        for (int i = 0; i < t_generic_type.type_parameters.size(); i++) {
            if (is_generic(*t_generic_type.type_parameters[i])) {
                auto r = make_generic_replacements(
                        *t_generic_type.type_parameters[i],
                        *t_matching_type.type_parameters[i]
                );
                for (auto x: r) {
                    if (repl.count(x.first) != 0 && *repl[x.first] != *x.second) {
                        throw std::runtime_error("Error type already replaced by something else");
                    }
                }
                repl.insert(r.begin(), r.end());
            }
        }
    }
    return repl;
}

std::unordered_map<std::string, TypeNode*>
make_generic_replacements(TypeNode& t_generic_type, TypeNode& t_matching_type) {
    if (t_generic_type.kind == Kind::FUNCTION && t_matching_type.kind == Kind::FUNCTION) {
        return make_function_generic_replacements(t_generic_type.function(), t_matching_type.function());
    } else if (t_generic_type.kind == Kind::OBJECT && t_matching_type.kind == Kind::OBJECT) {
        return make_object_generic_replacements(t_generic_type.object(), t_matching_type.object());
    } else {
        if (t_generic_type.kind == Kind::OBJECT && t_matching_type.kind == Kind::FUNCTION) {
            if (t_generic_type.object().type_parameters.size() == 0) {
                return std::unordered_map<std::string, TypeNode*>({{t_generic_type.object().identifier, t_matching_type.clone()}});
            }
        }
        throw std::runtime_error("Error: making generic replacements for mismatching types!");
    }
}

std::unordered_map<std::string, TypeNode*>
make_generic_to_generic_replacements(TypeNode& t_generic_type, TypeNode& t_matching_type) {
    std::unordered_map<std::string, TypeNode*> replacements;
    if (t_generic_type.kind == Kind::FUNCTION && t_matching_type.kind == Kind::FUNCTION) {

    } else if (t_generic_type.kind == Kind::OBJECT && t_matching_type.kind == Kind::OBJECT) {
        return make_object_generic_replacements(t_generic_type.object(), t_matching_type.object());
    } else {
        if (t_generic_type.kind == Kind::OBJECT && t_matching_type.kind == Kind::FUNCTION) {
            if (t_generic_type.object().type_parameters.size() == 0) {
                return std::unordered_map<std::string, TypeNode*>({{t_generic_type.object().identifier, t_matching_type.clone()}});
            }
        }
    }
    throw std::runtime_error("Error: making generic replacements for mismatching types!");
}

SymbolInfo Checker::match_arguments_to_generic_function(const FunctionTypeNode& ft, VectorOfTypes arg_types) {
    FunctionTypeNode& function_type = ft.clone()->function();
    FunctionTypeNode* f = ft.clone();
    unify_function_call(*f, arg_types);
    SymbolInfo rv;
    const TypeNode& ret_type = *f->return_type;
    rv.set_type(ret_type);
    return rv;

}

VectorOfTypes
Checker::get_replacements_in_order(const FunctionTypeNode& function_type, VectorOfTypes arg_types) {
    VectorOfTypes generic_replacements;
    for (int i = 0; i < function_type.parameter_types.size(); i++) {
        TypeNode& param_type = *function_type.parameter_types[i];
        if (is_generic(param_type)) {
            if (type_matches(&param_type, arg_types[i])) {
                VectorOfTypes param_generic_replacements = make_replacements_in_order(
                        &param_type,
                        arg_types[i]
                );
                generic_replacements.insert(
                        generic_replacements.end(),
                        param_generic_replacements.begin(),
                        param_generic_replacements.end());
            } else {
                throw std::runtime_error("Argument type error!");
            }
        }
    }

    return generic_replacements;
}

void
Checker::error_function_call_type_mismatch(const TypeNode& expected, const TypeNode& actual, TextPosition start,
                                           TextPosition end) {
    this->failed = true;
    std::string msg;
    msg = context_string(start) +
          E_FMT(" Function call type mismatch") +
          E_FMT(" expected ") + E_HLT(expected.to_string()) + E_FMT(" but got ") + E_HLT(actual.to_string()) +
          this->code_error_string(start, end);
    std::cout << msg << std::endl;
}

void Checker::error_function_call_num_args(TextPosition position) {
    this->failed = true;
    std::string msg;
    msg = context_string(position) +
          E_FMT("Calling function with wrong number of arguments ");
    std::cout << msg << std::endl;
}

void Checker::error_call_not_a_function(TextPosition position) {
    this->failed = true;
    std::string msg;
    msg = E_HLT(text_pos_to_string(this->__file__, position)) + E_FMT("Calling something that's not a function");
    std::cout << msg << std::endl;
}


USymbolInfo Checker::visit(CallNode& n) {
    USymbolInfo fun_info_p = this->dispatch(n.function);
    if (fun_info_p->is_error) {
        return std::make_unique<SymbolInfo>(ErrorStub());
    }
    SymbolInfo& fun_info = *fun_info_p;
    bool is_a_method = false;
    Node* object_node;
    SymbolInfo retv;
    if (fun_info.is_method) {
        // Since it's a method, we have to transform it and prepare it for the translation step,
        // where instead of calling object.method(args), we call <class>.method(object, args)

        // if (n.function->ntype != NodeType::MEMBER) {
        //     throw std::runtime_error("Expected it to be a member node!");
        // }
        MemberNode& member_node = n.function->member();
        IdNode* pNode = new IdNode(fun_info.class_info->class_name + "." + member_node.s_child);
        pNode->location = VariableLocation(-2, -1);
        n.function = pNode;
        pNode->is_global_function = true;
        this->replace_me = false;
        object_node = member_node.parent;
        is_a_method = true;
    } else if (fun_info.is_class_method) {
        // if (n.function->ntype != NodeType::MEMBER) {
        //     throw std::runtime_error("Expected it to be a member node!");
        // }
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
            this->error_function_call_num_args(n.start);
            return std::make_unique<SymbolInfo>(ErrorStub());
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
            retv.set_type(*function_type.return_type);
            for (int i = 0; i < n.arguments.size(); i++) {
                const TypeNode& arg_type = *arg_types[i];
                const TypeNode& param_type = *function_type.parameter_types[i];
                if (arg_type != param_type) {
                    this->error_function_call_type_mismatch(
                            param_type,
                            arg_type,
                            n.arguments[i]->start,
                            n.arguments[i]->end
                    );
                    return std::make_unique<SymbolInfo>(retv);
                }
            }
        }
    } else {
        this->error_call_not_a_function(n.start);
        // exit(1);
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

void Checker::error_unused_return_value(TextPosition pos) {
    std::string msg;
    msg = this->context_string(pos) +
          E_FMT("Unused return value of function call") + this->code_context_string(pos);
    std::cout << msg << std::endl;
    this->failed = true;
}

USymbolInfo Checker::visit(BlockNode& program) {
    for (auto& n: program.nodes) {
        USymbolInfo sinfo_p = this->dispatch(n);
        if (this->replace_me) {
            n = this->replacement;
            this->replace_me = false;
            this->replacement = nullptr;
        }
        SymbolInfo& sinfo = *sinfo_p;
        if (n->ntype == NodeType::CALL) {
            // it's a function call
            // if return value != NoneType, then force the return value
            if (!sinfo.is_error && sinfo.type() != ObjectTypeNode(".None", {})) {
                this->error_unused_return_value(n->start);
                // throw std::runtime_error("You should use the return value of this function call!");
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
                "At " + text_pos_to_string(this->__file__, node.start) + ": Error, class " +
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
    auto& to_object = (to).object();
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

    for (int i = 0; i < node.init_names.size(); i++) {
        if (class_fields.find(node.init_names[i]) == class_fields.end()) {
            throw std::runtime_error("No field named " + node.init_names[i]);
        }
    }
    if (class_fields.size() != node.init_names.size()) {
        throw std::runtime_error(
                "In struct \"" + object_type_id + "\" initialization: " + "Expected " +
                std::to_string(class_fields.size()) + " initializers but got " +
                std::to_string(node.init_names.size()));
    }
    for (int i = 0; i < node.init_names.size(); i++) {
        Node* exp = node.init_values[i];
        USymbolInfo semanticInfo_p = this->dispatch(exp);
        SymbolInfo& semanticInfo = *semanticInfo_p;
        if (this->replace_me) {
            node.init_values[i] = this->replacement;
            this->replace_me = false;
        }
        TypeNode& field_type = *class_fields[node.init_names[i]];
        if (!this->can_assign(semanticInfo.type(), field_type)) {
            throw std::runtime_error(
                    "In struct \"" + object_type_id + "\" initialization: " + "field \"" + node.init_names[i] +
                    "\" is of type " +
                    field_type.to_string() +
                    " but got " + semanticInfo.type().to_string());
        }
    }
    SymbolInfo rv;
    rv.set_type(*object_type);
    return std::make_unique<SymbolInfo>(rv);
}

void Checker::error_for(const TypeNode& t, TextPosition position) {
    this->failed = true;
    std::string msg;
    msg = E_HLT(text_pos_to_string(this->__file__, position)) +
          E_FMT("Expected") + E_HLT(" List[t] ") +
          E_FMT("in loop, but got ") +
          E_HLT(t.to_string());
    std::cout << msg << std::endl;
}

USymbolInfo Checker::visit(ForNode& node) {
    USymbolInfo symbol_info_p = this->dispatch(node.exp);
    SymbolInfo& symbol_info = *symbol_info_p;
    if (symbol_info.type().kind != Kind::OBJECT) {
        throw std::runtime_error("Iterating over something bad!");
    }

    const ObjectTypeNode& obj = symbol_info.type().object();
    if (obj.identifier != "List") {
        this->error_for(obj, node.start);
        // exit(1);
    }
    this->scope->set(".index0", T_INT);
    this->scope->set(".list0", T_LIST(new T_INT));
    IdNode* lid = new IdNode("List.len");
    lid->is_global_function = true;
    Node* new_condition = new BoolOpNode(
            BoolOp::LT,
            new IdNode(".index0"),
            new CallNode(lid, {new IdNode(".list0")}));

    BlockNode* new_body = new BlockNode({});
    new_body->nodes.push_back(
            new DeclarationNode(
                    node.var,
                    obj.type_parameters[0]->clone(),
                    new SubscriptNode(new IdNode(".list0"), {new IdNode(".index0")}))
    );
    new_body->nodes.insert(new_body->nodes.end(), node.body->nodes.begin(), node.body->nodes.end());
    AssignmentNode* asn = new AssignmentNode(
            new IdNode(".index0"),
            new BinopNode(OpType::ADD, new IdNode(".index0"), new NumberNode(1)));
    asn->type = new T_INT;
    new_body->nodes.push_back(
            asn
    );

    TypeNode& var_type = *obj.type_parameters[0];
    this->enter_scope("for");
    this->scope->set(node.var, var_type);
    this->visit(*node.body);
    this->leave_scope();
    this->replace_me = true;
    this->replacement = new BlockNode(
            {new DeclarationNode(".index0", new T_INT, new NumberNode(0)),
             new DeclarationNode(".list0", obj.clone(), node.exp),
             new WhileNode(new_condition, new_body)}
    );
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
    node.type = element_type.clone();
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
        this->error_condition(condition.type(), node.start, "elif");
        // exit(1);
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

void Checker::error_subscript_non_object(TextPosition pos) {
    std::string msg;
    msg = this->context_string(pos) + E_FMT("Accessing subscript of non object") + this->code_context_string(pos);
    std::cout << msg << std::endl;
    this->failed = true;
}

void Checker::error_string_immutable(TextPosition pos) {
    std::string msg;
    msg = this->context_string(pos) + E_FMT("Strings are immutable") + this->code_context_string(pos);
    std::cout << msg << std::endl;
    this->failed = true;
}

USymbolInfo Checker::visit(SubscriptNode& node) {
    USymbolInfo parent_p = this->dispatch(node.parent);
    SymbolInfo& parent = *parent_p;
    if (parent.type().kind != Kind::OBJECT) {
        this->error_subscript_non_object(node.start);
        return std::make_unique<SymbolInfo>(ErrorStub());
    }
    SymbolInfo symbol_info;
    const ObjectTypeNode& object_type = parent.type().object();

    if (object_type.identifier == "String") {
        if (this->is_lvalue) {
            this->error_string_immutable(node.start);
            return std::make_unique<SymbolInfo>(ErrorStub());
        }
    }

    VectorOfTypes children;
    bool not_integer = false;
    if (node.child.size() > 1) {
        throw std::runtime_error("Error subscript with more than one child!");
    }
    bool old_lvalue = this->is_lvalue;
    this->is_lvalue = false;
    for (auto& c: node.child) {
        USymbolInfo ct = this->dispatch(c);
        if (ct->type() != T_INT) {
            not_integer = true;
        }
        children.emplace_back(ct->type().clone());
    }
    this->is_lvalue = old_lvalue;

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
    auto& expression_type = expression_info.type().object();

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
    this->current_class = node.class_name;
    this->add_this = true;
    VectorOfTypes tp;
    for (auto type_param: node.type_parameters) {
        tp.push_back(TYPE(type_param, {}));
    }
    for (auto mt: node.members_ordered) {
        TypeNode& t = *node.members[mt];
        this->assert_type_exists(t, node.start);
    }

    this->this_type = new ObjectTypeNode(node.class_name, tp);
    for (auto method: node.methods) {
        this->visit(*method.second);
    }
    this->add_this = false;
    delete this_type;
    this->this_type = nullptr;
    this->current_class = "";
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
        case NodeType::BINOP: {
            auto r = this->visit(n.binop());
            return r;
        }
            break;
        case NodeType::BOOLOP:
            return this->visit(n.boolop());
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
        case NodeType::FLOT:
            return this->visit(n.flot());
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
        case PARTIAL:
            return this->visit(n.partial());
            break;
        case OTYPE:
            break;
        case FTYPE:
            break;
        case IMPORT:
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

bool Checker::is_immutable(const TypeNode& node) {
    if (node == T_STRING) {
        return true;
    }
    if (node == T_INT) {
        return true;
    }
    if (node == T_BOOL) {
        return true;
    }
    if (node.kind == Kind::OBJECT && node.object().identifier == "Tuple") {
        return true;
    }
    return false;
}

USymbolInfo Checker::visit(TupleNode& node) {
    VectorOfTypes types;
    int i = 0;
    for (auto n: node.values) {
        i++;
        USymbolInfo vtype = this->dispatch(n);
        types.emplace_back(vtype->type().clone());
        if (!this->is_immutable(vtype->type())) {
            throw std::runtime_error(
                    "All tuple member types must be immutable, at position " + std::to_string(i) + " got " +
                    vtype->type().to_string() + " which is not"
            );
        }
    }
    ObjectTypeNode tuple_type("Tuple", types);
    SymbolInfo sinfo;
    sinfo.set_type(tuple_type);
    return std::make_unique<SymbolInfo>(sinfo);
}

USymbolInfo Checker::visit(FloatNode& node) {
    SymbolInfo s;
    s.set_type(T_FLOAT);
    return std::make_unique<SymbolInfo>(s);
}

USymbolInfo Checker::visit(PartialApplication& node) {
    USymbolInfo func = this->dispatch(node.function);
    VectorOfTypes partial_args;
    if (node.args.size() != func->type().function().parameter_types.size()) {
        throw std::runtime_error("Error: wrong number of arguments for partial function");
    }
    for (int i = 0; i < node.args.size(); i++) {
        if (node.args[i] != nullptr) {
            USymbolInfo arg = this->dispatch(node.args[i]);
            if (arg->type() != *func->type().function().parameter_types[i]) {
                throw std::runtime_error(
                        "Error in partial function: type of arg " + std::to_string(i + 1) + " (" +
                        arg->type().to_string() + ") doesn't match expected type " +
                        func->type().function().parameter_types[i]->to_string());
            }
        } else {
            partial_args.push_back(func->type().function().parameter_types[i]->clone());
        }
    }
    node.complete_type = &func->type().clone()->function();
    SymbolInfo s;
    s.set_type(FunctionTypeNode(partial_args, func->type().function().return_type->clone()));
    return std::make_unique<SymbolInfo>(s);
}

Checker::~Checker() {
    for (auto s: this->scopes) {
        if (s.first == "global") {
            continue;
        }
        delete s.second;
    }
}


std::pair<std::string, TypeNode*>*
Checker::get_first_substitution_object(ObjectTypeNode& a, ObjectTypeNode& b, bool is_top_level_arg) {
    if (is_variable(a) && is_variable(b) && a.object().identifier == b.object().identifier) {
        return nullptr;
    }
    if (is_variable(a)) {
        return new std::pair<std::string, TypeNode*>(a.object().identifier, b.clone());
    }
    if (is_variable(b)) {
        if (is_top_level_arg) {
            throw std::runtime_error("trying to replace var with concrete type at top level!");
        }
        return new std::pair<std::string, TypeNode*>(b.object().identifier, a.clone());
    }
    if (a.identifier != b.identifier) {
        throw std::runtime_error("Error trying to unify object types " + a.to_string() + " and " + b.to_string());
    }
    if (a.type_parameters.size() != b.type_parameters.size()) {
        throw std::runtime_error("Error trying to unify object types " + a.to_string() + " and " + b.to_string());
    }
    for (int i = 0; i < a.type_parameters.size(); i++) {
        std::pair<std::string, TypeNode*>* u = get_first_substitution(
                *a.type_parameters[i],
                *b.type_parameters[i],
                is_top_level_arg
        );
        if (u != nullptr) {
            return u;
        }
    }
    return nullptr;
}

TypeNode* Checker::substitute(TypeNode* t, std::string var, TypeNode* replacement) {
    if (t->kind == Kind::OBJECT) {
        if (is_variable(t->object()) && t->object().identifier == var) {
            return replacement;
        } else {
            TypeNode* c = t->clone();
            for (int i = 0; i < t->object().type_parameters.size(); i++) {
                c->object().type_parameters[i] = substitute(t->object().type_parameters[i], var, replacement);
            }
            return c;
        }
    } else {
        TypeNode* c = t->clone();
        for (int i = 0; i < t->function().parameter_types.size(); i++) {
            c->function().parameter_types[i] = substitute(t->function().parameter_types[i], var, replacement);
        }
        c->function().return_type = substitute(c->function().return_type, var, replacement);
        return c;
    }
}


std::pair<std::string, TypeNode*>*
Checker::get_first_substitution_function(FunctionTypeNode& a, FunctionTypeNode& b, bool is_top_level_arg) {
    if (a.parameter_types.size() != b.parameter_types.size()) {
        throw std::runtime_error(
                "Error: trying to unify two functions with different parameter count: " + a.to_string() + " and " +
                b.to_string());
    }
    for (int i = 0; i < a.parameter_types.size(); i++) {
        std::pair<std::string, TypeNode*>* u = get_first_substitution(
                *a.parameter_types[i],
                *b.parameter_types[i],
                false
        );
        if (u != nullptr) {
            return u;
        }
    }
    std::pair<std::string, TypeNode*>* u = get_first_substitution(*a.return_type, *b.return_type, false);
    if (u != nullptr) {
        return u;
    }
    return nullptr;
}

void Checker::error_generic_call_mismatch(const TypeNode& expected, const TypeNode& actual, int i) {
    std::string msg =
            E_FMT("Error matching argument number " + std::to_string(i) + " expected ") + E_HLT(expected.to_string()) +
            E_FMT(" got ") + E_HLT(actual.to_string()) + "\n";
    std::cout << msg << std::endl;
}

void Checker::error_call_bad_num_args() {
    std::string msg = "Function call with wrong number of arguments!";
    std::cout << msg << std::endl;
}

void Checker::unify_function_call(FunctionTypeNode& fun, VectorOfTypes& args) {
    if (args.size() != fun.parameter_types.size()) {
        this->error_call_bad_num_args();
        this->failed = true;
        return;
    }

    for (int i = 0; i < args.size(); i++) {
        auto param = fun.parameter_types[i];
        auto arg = args[i];
        try {
            std::pair<std::string, TypeNode*>* substitution = get_first_substitution(*param, *arg, true);
            while (substitution != nullptr) {
                for (int j = 0; j < args.size(); j++) {
                    // if (j == i) {
                    //     continue;
                    // }
                    fun.parameter_types[j] = substitute(
                            fun.parameter_types[j],
                            substitution->first,
                            substitution->second
                    );
                    args[j] = substitute(args[j], substitution->first, substitution->second);
                }
                fun.return_type = substitute(fun.return_type, substitution->first, substitution->second);
                std::cout << "Simple substitution: " << fun.to_string() << std::endl;
                param = fun.parameter_types[i];
                arg = args[i];
                substitution = get_first_substitution(*param, *arg, true);
            }
        } catch (...) {
            error_generic_call_mismatch(*param, *arg, i);
            this->failed = true;
            // exit(1);
        }
    }
}

std::pair<std::string, TypeNode*>* Checker::get_first_substitution(TypeNode& a, TypeNode& b, bool is_top_level_arg) {
    if (a.kind != b.kind) {
        throw std::runtime_error(
                "Error trying to unify types of different kind" + a.to_string() + " and " + b.to_string());
    }
    if (a.kind == Kind::FUNCTION) {
        return get_first_substitution_function(a.function(), b.function(), is_top_level_arg);
    } else {
        return get_first_substitution_object(a.object(), b.object(), is_top_level_arg);
    }
}

bool Checker::is_variable(const ObjectTypeNode& a) {
    return a.type_parameters.size() == 0 && islower(a.identifier[0]);
}

