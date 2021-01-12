#include <iostream>
#include <set>
#include "Checker.h"
#include "TypeClassInfo.h"
#include "../macros.h"
#include "unify.h"
#include "../logging/logging.h"

bool function_is_generic(const FunctionType& ft) {
    for (auto param_type: ft.param_types) {
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
    list_class_info->methods.insert(std::make_pair("len", new FunctionType({}, new T_INT)));

    ObjectType generic_type_t("t", {});
    ObjectType generic_type_b("b", {});

    list_class_info->methods.insert(
            std::make_pair("push", new FunctionType({(generic_type_t.clone())}, TYPE(".None", {}))));
    list_class_info->methods.insert(std::make_pair("pop", new FunctionType({}, generic_type_t.clone())));
    list_class_info->methods.insert(
            std::make_pair(
                    "unordered_map",
                    new FunctionType(
                            {FUNCTION_TYPE({ generic_type_t.clone() },
                                           generic_type_b.clone())},
                            new T_LIST(generic_type_b.clone()))));
    list_class_info->type_params = {"t"};
    return list_class_info;
}

ClassInfo* make_int_class_info() {
    auto int_class_info = new ClassInfo();
    int_class_info->class_name = "Integer";
    int_class_info->methods.insert(std::make_pair("str", new FunctionType({}, new T_STRING)));
    return int_class_info;
}

ClassInfo* make_boolean_class_info() {
    auto int_class_info = new ClassInfo();
    int_class_info->class_name = "Boolean";
    int_class_info->methods.insert(std::make_pair("str", new FunctionType({}, new T_STRING)));
    return int_class_info;
}

ClassInfo* make_float_class_info() {
    auto float_class_info = new ClassInfo();
    float_class_info->class_name = "Float";
    float_class_info->methods.insert(std::make_pair("str", new FunctionType({}, new T_STRING)));
    return float_class_info;
}

ClassInfo* make_string_class_info() {
    auto string_class_info = new ClassInfo();
    string_class_info->class_name = "String";
    string_class_info->methods.insert(std::make_pair("len", new FunctionType({}, new T_INT)));
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

    this->class_table->set("Option", new ClassInfo("Option", VectorOfStrings(), {}, {"t"}));
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
        if (type.object().type_params.size() == 0) {
            if (!is_generic(type)) {
                if (!this->class_table->declared(type.object().id)) {
                    std::string msg;
                    msg = E_FMT(text_pos_to_string(this->__file__, pos));
                    msg += E_FMT(" type ") + E_HLT(type.object().id) + E_FMT(" doesn't exist");
                    std::cout << msg << std::endl;
                }
            }
            return;
        }
        if (!this->class_table->declared(type.object().id)) {
            std::string msg;
            msg = E_FMT(text_pos_to_string(this->__file__, pos));
            msg += E_FMT(" type ") + E_HLT(type.object().id) + E_FMT(" doesn't exist");
            std::cout << msg << std::endl;
        } else {
            for (auto t: type.object().type_params) {
                this->assert_type_exists(*t, pos);
            }
        }
    } else {
        for (auto t: type.function().param_types) {
            this->assert_type_exists(*t, pos);
        }
        this->assert_type_exists(*type.function().return_type, pos);
    }
}

USemanticInfo Checker::visit(FunctionNode& n) {
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
    if (returnType != ObjectType(".None", {})) {
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

USemanticInfo Checker::visit(IdNode& n) {
    SemanticInfo symbol_info;
    symbol_info.is_function = false;
    symbol_info.is_method = false;
    if (!this->scope->has(n._id)) {
        // it might be a function name
        if (this->function_table->has_function(n._id)) {
            n.is_global_function = true;
            symbol_info.is_function = true;
            n.location = VariableLocation(-2, -1);
            symbol_info.set_type(this->function_table->get(n._id));
        } else {
            this->error_variable_not_declared(n._id, n.start);
            return std::make_unique<SemanticInfo>(ErrorStub());
        }
    } else {
        symbol_info.set_type(this->scope->get(n._id));
        if (symbol_info.type().kind == Kind::UNKNOWN) {
            symbol_info.is_error = true;
        } else {
            n.location = this->scope->find(n._id);
            if (symbol_info.type().kind == Kind::OBJECT) {
                const ObjectType& otn = symbol_info.type().object();
                if (otn.id == "Option") {
                    if (this->scope->get_not_none(n._id)) {
                        symbol_info.set_type(*otn.type_params[0]);
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
    return std::make_unique<SemanticInfo>(symbol_info);
}

USemanticInfo Checker::visit(DeclarationNode& n) {
    if (this->scope->declared(n.identifier)) {
        this->error_redeclared(n.identifier, n.start);
        this->failed = true;
    }
    SemanticInfo symbol_info;
    symbol_info.is_function = false;
    if (n.expression->ntype != NodeType::UNINITIALIZED and n.type != nullptr) {
        this->assert_type_exists(*n.type, n.start);
        USemanticInfo exp_info_p = this->dispatch(n.expression);
        SemanticInfo& exp_info = *exp_info_p;
        if (exp_info.is_error) {
            this->scope->set(n.identifier, *n.type);
            return std::make_unique<SemanticInfo>(symbol_info);
        }
        if (this->replace_me) {
            n.expression = replacement;
            this->replace_me = false;
        }
        if (n.type->kind == Kind::FUNCTION) {
            // it's a function
            if (*n.type != exp_info.type()) {
                this->error_assignment(*n.type, exp_info.type(), n.start);
            }
        } else {
            SemanticInfo expression_info = exp_info;
            const ObjectType& actual_type = n.type->object();
            if (actual_type.id == "Option") {
                if (*actual_type.type_params[0] != expression_info.type()) {
                    auto foo = expression_info.type().object();
                    if (foo.id != "NoneType") {
                        this->error_assignment(*n.type, expression_info.type(), n.start);
                    }
                }
            } else if (actual_type.id == "Union") {
                bool ok = false;
                for (auto type_param: actual_type.type_params) {
                    if (*type_param != expression_info.type()) {
                        ok = true;
                        break;
                    }
                }
                if (!ok) {
                    this->error_assignment(*n.type, expression_info.type(), n.start);
                }
            } else {
                if (*n.type != expression_info.type()) {
                    this->error_assignment(*n.type, expression_info.type(), n.start);
                }
            }
        }
        symbol_info.set_type(*n.type);

    } else if (n.expression->ntype != NodeType::UNINITIALIZED) {
        USemanticInfo exp_info_p = this->dispatch(n.expression);
        n.type = exp_info_p->type().clone();
        SemanticInfo& exp_info = *exp_info_p;
        if (this->replace_me) {
            n.expression = replacement;
            this->replace_me = false;
        }
        symbol_info.set_type(exp_info.type());
    }
    this->scope->set(n.identifier, symbol_info.type());
    return std::make_unique<SemanticInfo>(symbol_info);
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

USemanticInfo Checker::visit(AssignmentNode& n) {
    if (n.lvalue->ntype == NodeType::ID) {
        if (n.lvalue->id()._id == "_") {
            this->dispatch(n.rvalue);
            return nullptr;
        }
    }
    this->is_lvalue = true;
    USemanticInfo linfo_p = this->dispatch(n.lvalue);
    if (linfo_p->is_error) {
        return nullptr;
    }
    this->is_lvalue = false;
    if (n.lvalue->ntype == MEMBER && n.lvalue->member().type == MemberType::NUM) {
        this->error_tuple_assign(n.start);
        this->failed = true;
    }
    USemanticInfo expression_type_p = this->dispatch(n.rvalue);
    if (expression_type_p->is_error) {
        return nullptr;
    }
    SemanticInfo& linfo = *linfo_p;
    SemanticInfo& expression_type = *expression_type_p;
    if (this->replace_me) {
        n.rvalue = replacement;
        this->replace_me = false;
    }

    const ObjectType& actual_type = linfo.type().object();
    if (n.lvalue->ntype == NodeType::ID && actual_type.id == "Option") {
        // special treatment if we are assigning to an id of a variable of type Option[t]
        if (expression_type.type() == (*actual_type.type_params[0])) {
            std::cout << "p cant be none" << std::endl;
            this->scope->set_not_none(n.lvalue->id()._id, true);
        } else {
            if (linfo.type() != (expression_type.type())) {
                auto& foo = expression_type.type().object();
                if (foo.id != "NoneType") {
                    this->error_assignment(linfo.type(), expression_type.type(), n.start);
                }
                // assigning none, ok
            }
            // type matches exactly, no proble
            std::cout << "p may be none" << std::endl;
            n.type = linfo.type().clone();
            this->scope->set_not_none(n.lvalue->id()._id, false);
        }
    } else {
        if (linfo.type() != expression_type.type()) {
            if (actual_type.id == "Option") {
                // if type doesn't match exactly, we may be assigning to an Option[t]
                if (*actual_type.type_params[0] != expression_type.type()) {
                    auto& foo = expression_type.type().object();
                    if (foo.id != "NoneType") {
                        this->error_assignment(
                                linfo.type(),
                                expression_type.type(),
                                n.start
                        );
                    }
                }
            } else {
                // if it's not Option[t], then it's an error
                this->error_assignment(linfo.type(), expression_type.type(), n.start);
            }
        }
        // else, type matches don't do anything
        n.type = linfo.type().clone();
    }
    return nullptr;
}

USemanticInfo Checker::visit(MemberNode& n) {
    SemanticInfo rv;
    if (n.parent->ntype == NodeType::ID) {
        IdNode& id_node = n.parent->id();
        // It might be something like <class>.<method>, so we need to handle this case differently
        if (this->class_table->declared(id_node._id)) {
            ClassInfo* class_info = this->class_table->get(id_node._id);
            if (class_info->methods.find(n.s_child) != class_info->methods.end()) {
                rv.set_type(*class_info->methods.find(n.s_child)->second);
                rv.class_info = class_info;

                const FunctionType& ftn = rv.type().function();
                FunctionType& copy_ftn = *ftn.clone();
                VectorOfTypes tp;
                for (auto tttp: rv.class_info->type_params) {
                    tp.push_back(new ObjectType(tttp, {}));
                }
                auto instance_type = new ObjectType(rv.class_info->class_name, tp);
                copy_ftn.param_types.insert(copy_ftn.param_types.begin(), instance_type);

                rv.set_type(copy_ftn);
                rv.is_method = false;
                rv.is_class_method = true;
                this->replace_me = true;
                IdNode* idn = new IdNode(class_info->class_name + "." + n.s_child);
                idn->is_global_function = true;
                this->replacement = idn;
                return std::make_unique<SemanticInfo>(rv);
            } else {
                this->error_class_no_method(class_info->class_name, n.s_child, n.start);
                return std::make_unique<SemanticInfo>(ErrorStub());
            }
        }
    }
    bool old_lvalue = this->is_lvalue;
    this->is_lvalue = false;
    USemanticInfo symbol_info_p = this->dispatch(n.parent);
    this->is_lvalue = old_lvalue;
    SemanticInfo& symbol_info = *symbol_info_p;
    if (symbol_info.type().kind != Kind::OBJECT) {
        this->error_member_no_object(n.start);
        return std::make_unique<SemanticInfo>(ErrorStub());
    }
    const ObjectType& object = symbol_info.type().object();
    const ObjectType* option_type = nullptr;
    if (n.parent->ntype == NodeType::ID) {
        IdNode& idn = n.parent->id();
        if (object.id == "Option") {
            if (this->scope->get_not_none(idn._id)) {
                // we can guarantee that it's not null, so we can access the members
                option_type = &(object.type_params[0])->object();
            } else {
                throw std::runtime_error(
                        "Error: line " + text_pos_to_string(this->__file__, idn.start) + " -> " + idn._id +
                        " might be none here, make sure to  this in a if XXX != none {...}!"
                );
            }
        }
    }
    const ObjectType& final_type = option_type != nullptr ? *option_type : object;
    if (final_type.id == "Tuple") {
        // special treatment for tuples
        if (n.type != MemberType::NUM) {
            throw std::runtime_error(
                    "Error can only access members " + std::to_string(1) + " to " +
                    std::to_string(final_type.type_params.size()) + " of " + final_type.to_string());
        }
        if (n.n_child < 1 || n.n_child > final_type.type_params.size()) {
            throw std::runtime_error(
                    "Error can only access members " + std::to_string(1) + " to " +
                    std::to_string(final_type.type_params.size()) + " of " + final_type.to_string());
        }
        SemanticInfo s;
        s.set_type(*final_type.type_params[n.n_child - 1]);
        return std::make_unique<SemanticInfo>(s);
    } else {
        if (n.type != MemberType::STR) {
            throw std::runtime_error(
                    "Error: can access number member for tuple types only, but got " + final_type.to_string());
        }

        ClassInfo* class_info;
        if (this->class_table->declared(final_type.to_string())) {
            class_info = this->class_table->get(final_type.to_string());
        } else {
            if (is_generic((final_type)) && final_type.type_params.size() == 0) {
                throw std::runtime_error(
                        "Cannot access member of totally generic value of generic type " + object.id + "!"
                );
            }
            class_info = this->class_table->get(object.id);
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
            return std::make_unique<SemanticInfo>(ErrorStub());
        }
    }

    return std::make_unique<SemanticInfo>(rv);

}

USemanticInfo Checker::visit(IfNode& n) {
    SemanticInfo symbol_info;
    USemanticInfo condition_info_p = this->dispatch(n.condition);
    SemanticInfo& condition_info = *condition_info_p;

    std::unordered_map<std::string, bool> not_null_vars;

    if (condition_info.type() != T_BOOL) {
        this->error_condition(condition_info.type(), n.start, "if");
    }

    this->enter_scope("if");
    this->visit(*n.then);
    this->leave_scope();

    for (int i = 0; i < n.elifs.size(); i++) {
        condition_info_p = this->dispatch(n.elifs[i].first);
        SemanticInfo& condition_info = *condition_info_p;
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
    return std::make_unique<SemanticInfo>(symbol_info);
}

USemanticInfo Checker::visit(BoolOpNode& n) {
    USemanticInfo left_info_p = this->dispatch(n.left);
    USemanticInfo right_info_p = this->dispatch(n.right);

    SemanticInfo& left_info = *left_info_p;
    SemanticInfo& right_info = *right_info_p;

    SemanticInfo symbol_info;
    bool ok = false;
    if (left_info.type().kind == Kind::OBJECT) {
        auto& left = left_info.type().object();
        if (right_info.type().kind == Kind::OBJECT) {
            auto& right = right_info.type().object();
            if (left.id == "Option" && right.id == "NoneType") {
                symbol_info.set_type(T_BOOL);
                ok = true;
            }
        }
    }
    if (!ok && left_info.type() != right_info.type()) {
        this->error_bool_op(left_info.type(), right_info.type(), n.start);
    }

    symbol_info.set_type(T_BOOL);

    return std::make_unique<SemanticInfo>(symbol_info);
}

USemanticInfo Checker::visit(BinopNode& n) {
    USemanticInfo left_info_p = this->dispatch(n.left);
    if (this->replace_me) {
        n.left = this->replacement;
        this->replace_me = false;
    }

    USemanticInfo right_info_p = this->dispatch(n.right);
    if (this->replace_me) {
        n.right = this->replacement;
        this->replace_me = false;
    }

    SemanticInfo& left_info = *left_info_p;
    SemanticInfo& right_info = *right_info_p;

    if (left_info.is_error || right_info.is_error) {
        auto e = ErrorStub();
        auto s = std::make_unique<SemanticInfo>(e);
        return s;
    }

    SemanticInfo symbol_info;
    auto& left = left_info.type().object();
    auto& right = right_info.type().object();
    auto ltype = left.id;
    auto rtype = right.id;
    bool ok = false;
    if (ltype == "Integer" && rtype == "Integer") {
        symbol_info.set_type(T_INT);
        symbol_info.is_function = false;
        ok = true;
    } else if (ltype == "Float" && rtype == "Float") {
        symbol_info.set_type(ObjectType("Float", {}));
        symbol_info.is_function = false;
        ok = true;
    } else if (ltype == "Float" && rtype == "Integer" || ltype == "Integer" && rtype == "Float") {
        symbol_info.set_type(ObjectType("Float", {}));
        symbol_info.is_function = false;
        ok = true;
    } else if (ltype == "String" && rtype == "String") {
        if (n.op == OpType::ADD) {
            symbol_info.set_type(T_STRING);
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
    }

    return std::make_unique<SemanticInfo>(symbol_info);
}

USemanticInfo Checker::visit(ReturnNode& n) {
    const TypeNode& return_type = this->scope->get("__return__");
    if (return_type == ObjectType(".None", {})) {
        if (n.expression != nullptr) {
            this->error_bad_return(n.start);
        }
        return nullptr;
    } else if (n.expression == nullptr) {
        this->error_no_return(return_type, n.start);
    }
    USemanticInfo expression_info_p = this->dispatch(n.expression);
    SemanticInfo& expression_info = *expression_info_p;
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
        return std::make_unique<SemanticInfo>(ErrorStub());
    }
    return nullptr;
}

bool is_generic(const TypeNode& t) {
    if (t.kind == Kind::OBJECT) {
        const ObjectType& o = t.object();
        if (o.id.size() == 1 && islower(o.id[0])) {
            // a is generic
            assert(o.type_params.size() == 0);
            return true;
        }
        for (auto type_param: o.type_params) {
            if (is_generic(*type_param)) {
                return true;
            }
        }
    } else {
        const FunctionType& fo = t.function();
        for (auto param_type: fo.param_types) {
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

MapStringType make_replacements(TypeNode* a, TypeNode* b) {
    MapStringType replacements;
    if (a->kind == Kind::OBJECT) {
        ObjectType& oa = a->object();
        ObjectType& ob = b->object();
        if (oa.id.size() == 1 && islower(oa.id[0])) {
            replacements[oa.id] = b->clone();
        }
        for (int i = 0; i < oa.type_params.size(); i++) {
            if (is_generic(*oa.type_params[i])) {
                MapStringType rep = make_replacements(
                        oa.type_params[i],
                        ob.type_params[i]
                );
                replacements.insert(rep.begin(), rep.end());
            }
        }
    } else {
        if (a->kind == Kind::FUNCTION && b->kind == Kind::FUNCTION) {
            FunctionType& fa = a->function();
            FunctionType& fb = b->function();
            for (int i = 0; i < fa.param_types.size(); i++) {
                if (is_generic(*fa.param_types[i])) {
                    MapStringType rep = make_replacements(
                            fa.param_types[i],
                            fb.param_types[i]
                    );
                    replacements.insert(rep.begin(), rep.end());
                }
            }
            if (is_generic(*fa.return_type)) {
                MapStringType rep = make_replacements(fa.return_type, fb.return_type);
                replacements.insert(rep.begin(), rep.end());
            }
        }
    }
    return replacements;
}

VectorOfTypes make_replacements_in_order(TypeNode* a, TypeNode* b) {
    VectorOfTypes replacements;
    if (a->kind == Kind::OBJECT) {
        ObjectType& oa = a->object();
        ObjectType& ob = b->object();
        if (oa.id.size() == 1 && islower(oa.id[0])) {
            replacements.push_back(b->clone());
        } else {
            for (int i = 0; i < oa.type_params.size(); i++) {
                if (is_generic(*oa.type_params[i])) {
                    VectorOfTypes rep = make_replacements_in_order(
                            oa.type_params[i],
                            ob.type_params[i]
                    );
                    replacements.insert(replacements.end(), rep.begin(), rep.end());
                }
            }
        }
    } else {
        if (a->kind == Kind::FUNCTION && b->kind == Kind::FUNCTION) {
            FunctionType& fa = a->function();
            FunctionType& fb = b->function();
            for (int i = 0; i < fa.param_types.size(); i++) {
                if (is_generic(*fa.param_types[i])) {
                    VectorOfTypes rep = make_replacements_in_order(
                            fa.param_types[i],
                            fb.param_types[i]
                    );
                    replacements.insert(replacements.end(), rep.begin(), rep.end());
                }
            }
            if (is_generic(*fa.return_type)) {
                VectorOfTypes rep = make_replacements_in_order(fa.return_type, fb.return_type);
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
        FunctionType& fa = a.function();
        FunctionType& fb = b.function();
        FunctionType& new_f = fa;
        MapStringType replacements;
        if (fa.param_types.size() != fb.param_types.size()) {
            return false;
        }
        VectorOfTypes param_types = fa.param_types;
        for (int i = 0; i < param_types.size(); i++) {
            if (type_matches(param_types[i], fb.param_types[i])) {
                MapStringType rep = make_replacements(
                        fa.param_types[i],
                        fb.param_types[i]
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
        ObjectType& oa = a.object();
        ObjectType& ob = b.object();
        if (!is_generic(a)) {
            return a == (b);
        }
        // a is generic
        if (oa.type_params.size() == 0) {
            return true;
        }
        if (oa.id != ob.id) {
            return false;
        }
        if (oa.type_params.size() != ob.type_params.size()) {
            return false;
        }
        for (int i = 0; i < oa.type_params.size(); i++) {
            if (!type_matches(oa.type_params[i], ob.type_params[i])) {
                return false;
            }
        }
        return true;
    }
    return false;
}

MapStringType
make_generic_replacements(TypeNode& t_generic_type, TypeNode& t_matching_type);

MapStringType
make_function_generic_replacements(FunctionType& t_generic_type, FunctionType& t_matching_type) {
    MapStringType repl;

    if (t_generic_type.param_types.size() != t_matching_type.param_types.size()) {
        throw std::runtime_error("Error parameter_types size mismatch!");
    }
    for (int i = 0; i < t_generic_type.param_types.size(); i++) {
        if (is_generic(*t_generic_type.param_types[i])) {
            auto r = make_generic_replacements(
                    *t_generic_type.param_types[i],
                    *t_matching_type.param_types[i]
            );
            for (auto x: r) {
                if (repl.count(x.first) != 0 && *repl[x.first] != *x.second) {
                    throw std::runtime_error("Error type already replaced by something else");
                }
            }
            repl.insert(r.begin(), r.end());
        } else {
            if (*t_generic_type.param_types[i] != *t_matching_type.param_types[i]) {
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

MapStringType
make_object_generic_replacements(ObjectType& t_generic_type, ObjectType& t_matching_type) {
    MapStringType repl;
    if (t_generic_type.type_params.size() == 0) {
        repl[t_generic_type.id] = t_matching_type.clone();
    } else {
        if (t_generic_type.type_params.size() != t_matching_type.type_params.size()) {
            throw std::runtime_error("Error type parameter size mismatch!");
        }
        for (int i = 0; i < t_generic_type.type_params.size(); i++) {
            if (is_generic(*t_generic_type.type_params[i])) {
                auto r = make_generic_replacements(
                        *t_generic_type.type_params[i],
                        *t_matching_type.type_params[i]
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

MapStringType
make_generic_replacements(TypeNode& t_generic_type, TypeNode& t_matching_type) {
    if (t_generic_type.kind == Kind::FUNCTION && t_matching_type.kind == Kind::FUNCTION) {
        return make_function_generic_replacements(t_generic_type.function(), t_matching_type.function());
    } else if (t_generic_type.kind == Kind::OBJECT && t_matching_type.kind == Kind::OBJECT) {
        return make_object_generic_replacements(t_generic_type.object(), t_matching_type.object());
    } else {
        if (t_generic_type.kind == Kind::OBJECT && t_matching_type.kind == Kind::FUNCTION) {
            if (t_generic_type.object().type_params.size() == 0) {
                return MapStringType({{t_generic_type.object().id, t_matching_type.clone()}});
            }
        }
        throw std::runtime_error("Error: making generic replacements for mismatching types!");
    }
}

MapStringType
make_generic_to_generic_replacements(TypeNode& t_generic_type, TypeNode& t_matching_type) {
    MapStringType replacements;
    if (t_generic_type.kind == Kind::FUNCTION && t_matching_type.kind == Kind::FUNCTION) {

    } else if (t_generic_type.kind == Kind::OBJECT && t_matching_type.kind == Kind::OBJECT) {
        return make_object_generic_replacements(t_generic_type.object(), t_matching_type.object());
    } else {
        if (t_generic_type.kind == Kind::OBJECT && t_matching_type.kind == Kind::FUNCTION) {
            if (t_generic_type.object().type_params.size() == 0) {
                return MapStringType({{t_generic_type.object().id, t_matching_type.clone()}});
            }
        }
    }
    throw std::runtime_error("Error: making generic replacements for mismatching types!");
}

SemanticInfo Checker::match_arguments_to_generic_function(const FunctionType& ft, VectorOfTypes arg_types) {
    FunctionType& function_type = ft.clone()->function();
    FunctionType* f = ft.clone();
    unify_function_call(*f, arg_types);
    SemanticInfo rv;
    const TypeNode& ret_type = *f->return_type;
    rv.set_type(ret_type);
    return rv;

}

VectorOfTypes
Checker::get_replacements_in_order(const FunctionType& function_type, VectorOfTypes arg_types) {
    VectorOfTypes generic_replacements;
    for (int i = 0; i < function_type.param_types.size(); i++) {
        TypeNode& param_type = *function_type.param_types[i];
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

USemanticInfo Checker::visit(CallNode& n) {
    USemanticInfo fun_info_p = this->dispatch(n.function);
    if (fun_info_p->is_error) {
        return std::make_unique<SemanticInfo>(ErrorStub());
    }
    SemanticInfo& fun_info = *fun_info_p;
    bool is_a_method = false;
    Node* object_node;
    SemanticInfo retv;
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
        const FunctionType& ftn = fun_info.type().function();
        FunctionType& copy_ftn = ftn.clone()->function();
        copy_ftn.param_types.insert(copy_ftn.param_types.begin(), TYPE(fun_info.class_info->class_name, {}));
        retv.set_type(*copy_ftn.clone());
        object_node = member_node.parent;
    }
    if (fun_info.is_function || fun_info.is_method || fun_info.is_class_method) {
        // ok
        const FunctionType& function_type = fun_info.type().function();
        if (n.arguments.size() != function_type.param_types.size()) {
            this->error_function_call_num_args(n.start);
            return std::make_unique<SemanticInfo>(ErrorStub());
        }
        VectorOfTypes arg_types;
        for (auto& arg: n.arguments) {
            USemanticInfo arg_type_p = this->dispatch(arg);
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
                const TypeNode& param_type = *function_type.param_types[i];
                if (arg_type != param_type) {
                    this->error_function_call_type_mismatch(
                            param_type,
                            arg_type,
                            n.arguments[i]->start,
                            n.arguments[i]->end
                    );
                    return std::make_unique<SemanticInfo>(retv);
                }
            }
        }
    } else {
        this->error_call_not_a_function(n.start);
    }
    if (is_a_method) {
        // prepend the "this" argument (the object on which the method is being called)
        n.arguments.insert(n.arguments.begin(), object_node);
    }
    return std::make_unique<SemanticInfo>(retv);
}

USemanticInfo Checker::visit(BlockNode& program) {
    for (auto& n: program.nodes) {
        USemanticInfo sinfo_p = this->dispatch(n);
        if (this->replace_me) {
            n = this->replacement;
            this->replace_me = false;
            this->replacement = nullptr;
        }
        SemanticInfo& sinfo = *sinfo_p;
        if (n->ntype == NodeType::CALL) {
            // it's a function call
            // if return value != NoneType, then force the return value
            if (!sinfo.is_error && sinfo.type() != ObjectType(".None", {})) {
                this->error_unused_return_value(n->start);
                // throw std::runtime_error("You should use the return value of this function call!");
            }
        }
    }
    return nullptr;
}

USemanticInfo Checker::visit(ClassLiteralExpressionNode& node) {
    ObjectType& object_type = *node.type;
    std::string& object_type_id = object_type.id;
    const std::string& object_type_str = object_type.to_string();

    if (!this->class_table->declared(object_type_id)) {
        throw std::runtime_error("No struct named " + object_type_id);
    }
    ClassInfo* class_info = this->class_table->get(object_type_id);
    unsigned long num_required_type_params = class_info->type_params.size();
    unsigned long num_actual_type_params = object_type.type_params.size();
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
        USemanticInfo semanticInfo_p = this->dispatch(exp);
        SemanticInfo& semanticInfo = *semanticInfo_p;
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
    SemanticInfo rv;
    rv.set_type(object_type);
    return std::make_unique<SemanticInfo>(rv);
}

bool Checker::can_assign(const TypeNode& from, const TypeNode& to) {
    auto& to_object = (to).object();
    if (to_object.id == "Option") {
        if (*to_object.type_params[0] != from) {
            auto foo = from.object();
            if (foo.id != "NoneType") {
                return false;
            }
        }
        return true;
    } else if (to_object.id == "Union") {
        for (auto type_param: to_object.type_params) {
            if (*type_param == from) {
                return true;
            }
        }
        return false;
    }
    return to == from;
}

bool Checker::can_assign_generic(TypeNode& from, TypeNode& to, VectorOfStrings type_params) {
    auto to_object = (to).object();
    if (to_object.type_params.size() == 0) {
        for (auto tp:type_params) {
            if (to_object.id == tp) {
                return true;
            }
        }
    }
    if (to_object.id == "Option") {
        if (*to_object.type_params[0] != from) {
            auto foo = from.object();
            if (foo.id != "NoneType") {
                return false;
            }
        }
        return true;
    } else if (to_object.id == "Union") {
        for (auto type_param: to_object.type_params) {
            if (*type_param == from) {
                return true;
            }
        }
        return false;
    }
    return to == (from);
}

TypeNode*
make_type_from_object_pattern(const ObjectType& object_type,
                              const MapStringType& replacements) {
    std::string type_identifier = object_type.id;
    for (auto r: replacements) {
        if (type_identifier == r.first) {
            if (object_type.type_params.size() != 0) {
                throw std::runtime_error(
                        "Trying to make a type for a template for exmaple struct Foo[T]{foo:T[Integer];}!"
                );
            }
            return r.second;
        }
    }
    // It's not the top level type
    VectorOfTypes new_type_params;
    for (auto tp: object_type.type_params) {
        TypeNode* new_tp = make_type(*tp, replacements);
        new_type_params.push_back(new_tp);
    }
    return TYPE(type_identifier, new_type_params);
}

TypeNode* make_type_from_function_pattern(const FunctionType& ftn,
                                          const MapStringType& replacements) {
    VectorOfTypes new_param_types;
    for (auto pt: ftn.param_types) {
        TypeNode* new_pt = make_type(*pt, replacements);
        new_param_types.push_back(new_pt);
    }
    TypeNode* new_return_type = make_type(*ftn.return_type, replacements);
    return FUNCTION_TYPE(new_param_types, new_return_type);
}

TypeNode* make_type(const TypeNode& original, const MapStringType& replacements) {
    if (original.kind == Kind::OBJECT) {
        return make_type_from_object_pattern(original.object(), replacements);
    } else {
        return make_type_from_function_pattern(original.function(), replacements);
    }
}

ClassInfo* Checker::instantiate_generic(ClassInfo* generic, const ObjectType& instance) {
    MapStringType replacements;
    for (int i = 0; i < generic->type_params.size(); i++) {
        std::string tp = generic->type_params[i];
        TypeNode& type_replacement = *instance.type_params[i];
        replacements[tp] = &type_replacement;
    }
    auto field_names = generic->member_names;
    VectorOfTypes concrete_field_types;
    for (auto f: generic->member_types) {
        TypeNode& concrete_type = *make_type(*f, replacements);
        concrete_field_types.push_back(&concrete_type);
    }

    std::unordered_map<std::string, FunctionType*> concrete_methods;
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

USemanticInfo Checker::visit(ClassLiteralFieldNode& node) {
    ObjectType* object_type = node.type;
    std::string& object_type_id = object_type->id;
    const std::string& object_type_str = object_type->to_string();

    if (!this->class_table->declared(object_type_id)) {
        this->error_class_not_found(*object_type, node.start);
        return std::make_unique<SemanticInfo>(ErrorStub());
    }

    ClassInfo* class_info = this->class_table->get(object_type_id);
    unsigned long num_required_type_params = class_info->type_params.size();
    unsigned long num_actual_type_params = object_type->type_params.size();
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
        USemanticInfo semanticInfo_p = this->dispatch(exp);
        SemanticInfo& semanticInfo = *semanticInfo_p;
        if (this->replace_me) {
            node.init_values[i] = this->replacement;
            this->replace_me = false;
        }
        TypeNode& field_type = *class_fields[node.init_names[i]];
        if (!this->can_assign(semanticInfo.type(), field_type)) {
            this->error_class_init_bad_member_type(*object_type, field_type, semanticInfo.type(), exp->start);
        }
    }
    SemanticInfo rv;
    rv.set_type(*object_type);
    return std::make_unique<SemanticInfo>(rv);
}

USemanticInfo Checker::visit(ForNode& node) {
    USemanticInfo symbol_info_p = this->dispatch(node.exp);
    SemanticInfo& symbol_info = *symbol_info_p;
    if (symbol_info.type().kind != Kind::OBJECT) {
        throw std::runtime_error("Iterating over something bad!");
    }

    const ObjectType& obj = symbol_info.type().object();
    if (obj.id != "List") {
        this->error_for(obj, node.start);
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
                    obj.type_params[0]->clone(),
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

    TypeNode& var_type = *obj.type_params[0];
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

USemanticInfo Checker::visit(ListNode& node) {
    USemanticInfo element_type_p = this->dispatch(node.elements[0]);
    const TypeNode& element_type = element_type_p->type();
    if (this->replace_me) {
        node.elements[0] = this->replacement;
        this->replace_me = false;
    }
    for (int i = 1; i < node.elements.size(); i++) {
        USemanticInfo current_type_p = this->dispatch(node.elements[i]);
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
    SemanticInfo return_info;
    return_info.is_function = false;
    return_info.set_type(ObjectType("List", {element_type.clone()}));
    return std::make_unique<SemanticInfo>(return_info);
}

USemanticInfo Checker::visit(BooleanNode& node) {
    SemanticInfo symbol_info;
    symbol_info.set_type(T_BOOL);
    symbol_info.is_function = false;
    return std::make_unique<SemanticInfo>(symbol_info);
}

USemanticInfo Checker::visit(WhileNode& node) {
    USemanticInfo condition_p = this->dispatch(node.condition);
    SemanticInfo& condition = *condition_p;
    if (condition.type() != T_BOOL) {
        this->error_condition(condition.type(), node.start, "elif");
    }
    this->enter_scope("while");
    this->visit(*node.body);
    this->leave_scope();
    return nullptr;
}

USemanticInfo Checker::visit(NumberNode& node) {
    SemanticInfo semanticInfo;
    semanticInfo.set_type(T_INT);
    semanticInfo.is_function = false;
    return std::make_unique<SemanticInfo>(semanticInfo);
}

USemanticInfo Checker::visit(StringNode& node) {
    SemanticInfo semanticInfo;
    semanticInfo.set_type(T_STRING);
    semanticInfo.is_function = false;
    return std::make_unique<SemanticInfo>(semanticInfo);
}

USemanticInfo Checker::visit(SubscriptNode& node) {
    USemanticInfo parent_p = this->dispatch(node.parent);
    SemanticInfo& parent = *parent_p;
    if (parent.type().kind != Kind::OBJECT) {
        this->error_subscript_non_object(node.start);
        return std::make_unique<SemanticInfo>(ErrorStub());
    }
    SemanticInfo symbol_info;
    const ObjectType& object_type = parent.type().object();

    if (object_type.id == "String") {
        if (this->is_lvalue) {
            this->error_string_immutable(node.start);
            return std::make_unique<SemanticInfo>(ErrorStub());
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
        USemanticInfo ct = this->dispatch(c);
        if (ct->type() != T_INT) {
            not_integer = true;
        }
        children.emplace_back(ct->type().clone());
    }
    this->is_lvalue = old_lvalue;

    if (object_type.id == "List") {
        if (not_integer) {
            throw std::runtime_error("Access not number subscript of List!");
        }
        symbol_info.set_type(*object_type.type_params[0]);
    } else if (object_type.id == "String") {
        if (not_integer) {
            throw std::runtime_error("Access not number subscript of String!");
        }
        symbol_info.set_type(object_type);
    }

    symbol_info.is_function = false;

    return std::make_unique<SemanticInfo>(symbol_info);
}

USemanticInfo Checker::visit(BreakNode& node) {
    return nullptr;
}

USemanticInfo Checker::visit(TernaryNode& node) {
    USemanticInfo expression_info_p = this->dispatch(node.expression);
    SemanticInfo& expression_info = *expression_info_p;
    if (expression_info.type().kind != Kind::OBJECT) {
        throw std::runtime_error("Unexpected non-object");
    }
    auto& expression_type = expression_info.type().object();

    if (expression_type.id != "Option") {
        throw std::runtime_error("Expected an Option[T], got: " + expression_type.to_string());
    }
    SemanticInfo semanticInfo;
    TypeNode& type = *expression_type.type_params[0];
    semanticInfo.set_type(type);
    this->enter_scope("true_case");
    this->scope->set("it", type);
    USemanticInfo true_case_p = this->dispatch(node.true_case);
    SemanticInfo& true_case = *true_case_p;
    if (this->replace_me) {
        node.true_case = this->replacement;
        this->replace_me = false;
    }
    this->leave_scope();
    USemanticInfo false_case_p = this->dispatch(node.false_case);
    SemanticInfo& false_case = *false_case_p;
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
    return std::make_unique<SemanticInfo>(semanticInfo);
}

USemanticInfo Checker::visit(NoneNode& node) {
    SemanticInfo semanticInfo;
    semanticInfo.set_type(ObjectType("NoneType", {}));
    semanticInfo.is_function = false;
    return std::make_unique<SemanticInfo>(semanticInfo);
}

USemanticInfo Checker::visit(EmptyListNode& node) {
    SemanticInfo semanticInfo;
    semanticInfo.set_type(T_LIST(node.type->clone()));
    semanticInfo.is_function = false;
    return std::make_unique<SemanticInfo>(semanticInfo);
}

USemanticInfo Checker::visit(ClassNode& node) {
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

    this->this_type = new ObjectType(node.class_name, tp);
    for (auto method: node.methods) {
        this->visit(*method.second);
    }
    this->add_this = false;
    delete this_type;
    this->this_type = nullptr;
    this->current_class = "";
    return nullptr;
}

USemanticInfo Checker::visit(ContinueNode& node) {
    return nullptr;
}

USemanticInfo Checker::dispatch(Node* nod) {
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
    if (node.kind == Kind::OBJECT && node.object().id == "Tuple") {
        return true;
    }
    return false;
}

USemanticInfo Checker::visit(TupleNode& node) {
    VectorOfTypes types;
    int i = 0;
    for (auto n: node.values) {
        i++;
        USemanticInfo vtype = this->dispatch(n);
        types.emplace_back(vtype->type().clone());
        if (!this->is_immutable(vtype->type())) {
            this->error_tuple_member_not_immutable(vtype->type(), node.start);
            return std::make_unique<SemanticInfo>(ErrorStub());
        }
    }
    ObjectType tuple_type("Tuple", types);
    SemanticInfo sinfo;
    sinfo.set_type(tuple_type);
    return std::make_unique<SemanticInfo>(sinfo);
}

USemanticInfo Checker::visit(FloatNode& node) {
    SemanticInfo s;
    s.set_type(T_FLOAT);
    return std::make_unique<SemanticInfo>(s);
}

USemanticInfo Checker::visit(PartialApplication& node) {
    USemanticInfo func = this->dispatch(node.function);
    VectorOfTypes partial_args;
    if (node.args.size() != func->type().function().param_types.size()) {
        throw std::runtime_error("Error: wrong number of arguments for partial function");
    }
    for (int i = 0; i < node.args.size(); i++) {
        if (node.args[i] != nullptr) {
            USemanticInfo arg = this->dispatch(node.args[i]);
            if (arg->type() != *func->type().function().param_types[i]) {
                throw std::runtime_error(
                        "Error in partial function: type of arg " + std::to_string(i + 1) + " (" +
                        arg->type().to_string() + ") doesn't match expected type " +
                        func->type().function().param_types[i]->to_string());
            }
        } else {
            partial_args.push_back(func->type().function().param_types[i]->clone());
        }
    }
    node.complete_type = &func->type().clone()->function();
    SemanticInfo s;
    s.set_type(FunctionType(partial_args, func->type().function().return_type->clone()));
    return std::make_unique<SemanticInfo>(s);
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
Checker::get_first_substitution_object(ObjectType& a, ObjectType& b, bool is_top_level_arg) {
    if (is_variable(a) && is_variable(b) && a.object().id == b.object().id) {
        return nullptr;
    }
    if (is_variable(a)) {
        return new std::pair<std::string, TypeNode*>(a.object().id, b.clone());
    }
    if (is_variable(b)) {
        if (is_top_level_arg) {
            throw std::runtime_error("trying to replace var with concrete type at top level!");
        }
        return new std::pair<std::string, TypeNode*>(b.object().id, a.clone());
    }
    if (a.id != b.id) {
        throw std::runtime_error("Error trying to unify object types " + a.to_string() + " and " + b.to_string());
    }
    if (a.type_params.size() != b.type_params.size()) {
        throw std::runtime_error("Error trying to unify object types " + a.to_string() + " and " + b.to_string());
    }
    for (int i = 0; i < a.type_params.size(); i++) {
        std::pair<std::string, TypeNode*>* u = get_first_substitution(
                *a.type_params[i],
                *b.type_params[i],
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
        if (is_variable(t->object()) && t->object().id == var) {
            return replacement;
        } else {
            TypeNode* c = t->clone();
            for (int i = 0; i < t->object().type_params.size(); i++) {
                c->object().type_params[i] = substitute(t->object().type_params[i], var, replacement);
            }
            return c;
        }
    } else {
        TypeNode* c = t->clone();
        for (int i = 0; i < t->function().param_types.size(); i++) {
            c->function().param_types[i] = substitute(t->function().param_types[i], var, replacement);
        }
        c->function().return_type = substitute(c->function().return_type, var, replacement);
        return c;
    }
}

std::pair<std::string, TypeNode*>*
Checker::get_first_substitution_function(FunctionType& a, FunctionType& b, bool is_top_level_arg) {
    if (a.param_types.size() != b.param_types.size()) {
        throw std::runtime_error(
                "Error: trying to unify two functions with different parameter count: " + a.to_string() + " and " +
                b.to_string());
    }
    for (int i = 0; i < a.param_types.size(); i++) {
        std::pair<std::string, TypeNode*>* u = get_first_substitution(
                *a.param_types[i],
                *b.param_types[i],
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

void Checker::unify_function_call(FunctionType& fun, VectorOfTypes& args) {
    if (args.size() != fun.param_types.size()) {
        this->error_call_bad_num_args();
        this->failed = true;
        return;
    }

    for (int i = 0; i < args.size(); i++) {
        auto param = fun.param_types[i];
        auto arg = args[i];
        try {
            std::pair<std::string, TypeNode*>* substitution = get_first_substitution(*param, *arg, true);
            while (substitution != nullptr) {
                for (int j = 0; j < args.size(); j++) {
                    // if (j == i) {
                    //     continue;
                    // }
                    fun.param_types[j] = substitute(
                            fun.param_types[j],
                            substitution->first,
                            substitution->second
                    );
                    args[j] = substitute(args[j], substitution->first, substitution->second);
                }
                fun.return_type = substitute(fun.return_type, substitution->first, substitution->second);
                std::cout << "Simple substitution: " << fun.to_string() << std::endl;
                param = fun.param_types[i];
                arg = args[i];
                substitution = get_first_substitution(*param, *arg, true);
            }
        } catch (...) {
            error_generic_call_mismatch(*param, *arg, i);
            this->failed = true;
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

bool Checker::is_variable(const ObjectType& a) {
    return a.type_params.size() == 0 && islower(a.id[0]);
}

