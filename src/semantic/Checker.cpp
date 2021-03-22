#include <iostream>
#include <set>
#include "Checker.h"
#include "../macros.h"
#include "../logging/logging.h"
#include "util.h"

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

    list_class_info->methods.insert(std::make_pair("push",
                                                   new FunctionType({(generic_type_t.clone())}, TYPE(".None", {}))));
    list_class_info->methods.insert(std::make_pair("pop", new FunctionType({}, generic_type_t.clone())));
    list_class_info->methods.insert(std::make_pair("unordered_map",
                                                   new FunctionType({FUNCTION_TYPE({ generic_type_t.clone() },
                                                                                   generic_type_b.clone())},
                                                                    new T_LIST(generic_type_b.clone()))));
    list_class_info->type_params = {"t"};
    return list_class_info;
}

ClassInfo* make_file_class_info() {
    auto int_class_info = new ClassInfo();
    int_class_info->class_name = "File";
    int_class_info->methods.insert(std::make_pair("read_line", parse_function_type("fun()->String")));
    return int_class_info;
}

ClassInfo* make_int_class_info() {
    auto int_class_info = new ClassInfo();
    int_class_info->class_name = "Integer";
    int_class_info->methods.insert(std::make_pair("str", parse_function_type("fun()->String")));
    return int_class_info;
}

ClassInfo* make_boolean_class_info() {
    auto int_class_info = new ClassInfo();
    int_class_info->class_name = "Boolean";
    int_class_info->methods.insert(std::make_pair("str", parse_function_type("fun()->String")));
    return int_class_info;
}

ClassInfo* make_float_class_info() {
    auto float_class_info = new ClassInfo();
    float_class_info->class_name = "Float";
    float_class_info->methods.insert(std::make_pair("str", parse_function_type("fun()->String")));
    return float_class_info;
}

ClassInfo* make_string_class_info() {
    auto string_class_info = new ClassInfo();
    string_class_info->class_name = "String";
    string_class_info->methods.insert(std::make_pair("len", parse_function_type("fun()->Integer")));
    return string_class_info;
}

Checker::Checker(SymbolTable* globals, ClassTable* class_table, FunctionTable* function_table) {
    this->is_lvalue = false;
    this->is_method = false;
    this->function_table = function_table;
    this->failed = false;
    this->class_table = class_table;
    this->scope = globals;
    this->scopes["global"] = this->scope;

    this->add_this = false;
    this->this_type = nullptr;

    this->class_table->set("Float", make_float_class_info());
    this->class_table->set("File", make_file_class_info());
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

bool Checker::assert_type_exists(TypeNode& type, TextPosition pos) {
    if (type.kind == Kind::OBJECT) {
        if (type.object().id == ".None") {
            return true;
        }
        if (type.object().type_params.size() == 0) {
            if (!is_generic(type)) {
                if (!this->class_table->declared(type.object().id)) {
                    this->error_class_not_found(type, pos);
                    return false;
                }
            }
            return true;
        }
        if (!this->class_table->declared(type.object().id)) {
            this->error_class_not_found(type, pos);
            return false;
        } else {
            bool error = false;
            for (auto t: type.object().type_params) {
                if (!this->assert_type_exists(*t, pos)) {
                    error = true;
                }
            }
            return !error;
        }
    } else {
        bool error = false;
        for (auto t: type.function().param_types) {
            if (!this->assert_type_exists(*t, pos)) {
                error = true;
            }
            return !error;
        }
        return this->assert_type_exists(*type.function().return_type, pos);
    }
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

SemanticInfo Checker::match_arguments_to_generic_function(const FunctionType& ft, VectorOfTypes arg_types) {
    FunctionType& function_type = ft.clone()->function();
    FunctionType* f = ft.clone();
    unify_function_call(*f, arg_types);
    SemanticInfo rv;
    const TypeNode& ret_type = *f->return_type;
    rv.set_type(ret_type);
    return rv;

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

TypeNode* make_type_from_object_pattern(const ObjectType& object_type, const MapStringType& replacements) {
    std::string type_identifier = object_type.id;
    for (auto r: replacements) {
        if (type_identifier == r.first) {
            if (object_type.type_params.size() != 0) {
                throw std::runtime_error(
                        "Trying to make a type for a template for exmaple struct Foo[T]{foo:T[Integer];}!");
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

TypeNode* make_type_from_function_pattern(const FunctionType& ftn, const MapStringType& replacements) {
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

USemanticInfo error_stub() {
    return std::make_unique<SemanticInfo>(ErrorStub());
}

Node* Checker::replace_if_necessary(Node* node) {
    if (this->replace_me) {
        this->replace_me = false;
        return this->replacement;
    }
    return node;
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

Checker::~Checker() {
    for (auto s: this->scopes) {
        if (s.first == "global") {
            continue;
        }
        delete s.second;
    }
}

bool Checker::is_variable(const ObjectType& a) {
    return a.type_params.size() == 0 && islower(a.id[0]);
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
        case DICT:
            return this->visit(n.dict());
            break;
        case EMPTYDICT:
            return this->visit(n.emptydict());
            break;
        default:
            throw std::runtime_error("Don't know what to do!");
    }
    return nullptr;
}
