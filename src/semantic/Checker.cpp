#include <iostream>
#include <set>
#include <cassert>
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

Checker::Checker() {
    this->is_call = false;
    this->scope = new SymbolTable("global", nullptr);
    this->scopes["global"] = this->scope;
    this->add_this = false;
    this->loop_count = 0;
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
                // if (this->imported_paths.count(type.object().id) == 0) {
                //     this->error_class_not_found(type, {1, 1});
                //     return false;
                // }
            }
            return true;
        }
        // if (this->imported_paths.count(type.object().id) == 0) {
        //     this->error_class_not_found(type, pos);
        //     return false;
        // } else {
        //     bool error = false;
        //     for (auto t: type.object().type_params) {
        //         if (!this->assert_type_exists(*t, pos)) {
        //             error = true;
        //         }
        //     }
        //     return !error;
        // }
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
    return false;
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

std::unique_ptr<SemanticInfo> Checker::match_arguments_to_generic_function(const FunctionType& ft, VectorOfTypes arg_types) {
    FunctionType* f = ft.clone();
    unify_function_call(*f, arg_types);
    for (auto at: arg_types) {
        delete at;
    }
    SemanticInfo rv;
    rv.entity = Entity(new Value(f->return_type->clone()));
    delete f;
    return std::make_unique<SemanticInfo>(rv);
}

bool Checker::can_assign(const TypeNode& from, const TypeNode& to) {
    if (from.object().is_generic_param || to.object().is_generic_param) {
        return from.object().id == to.object().id;
    } else if (from.object().actual_base_path.as_str() != to.object().actual_base_path.as_str()) {
        return false;
    }
    return true;
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
    ObjectType* ot = TYPE(type_identifier, new_type_params);
    return ot;
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

Class* Checker::instantiate_generic(Class* generic, const ObjectType& instance) {
    MapStringType replacements;
    for (size_t i = 0; i < generic->type_params.size(); i++) {
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

    std::unordered_map<std::string, ConstFunction*> concrete_methods;
    for (auto m: generic->methods) {
        TypeNode* t = (m.second)->ft;
        TypeNode& concrete_type = *make_type(*t, replacements);
        this->module->fill_actual(&concrete_type);
        ConstFunction* cf = new ConstFunction();
        cf->path = m.second->path;
        cf->ft = (FunctionType*) concrete_type.clone();
        concrete_methods[m.first] = cf;
    }

    Class* concrete = new Class();
    concrete->class_name = generic->class_name;
    concrete->methods = concrete_methods;
    concrete->member_names = generic->member_names;
    concrete->member_types = concrete_field_types;
    concrete->path = generic->path;
    for (size_t i = 0; i < generic->member_names.size(); i++) {
        concrete->members[generic->member_names[i]] = concrete_field_types[i];
    }
    return concrete;
}

USemanticInfo error_stub() {
    return std::make_unique<SemanticInfo>(ErrorStub());
}

Flirpin map_unit_to_flirpin(Unit u) {
    switch (u.type) {
        case U_TYPE::PACKAGE:
            return Flirpin{.type=F_TYPE::PACKAGE, .package=u.package};
        case U_TYPE::MODULE:
            return Flirpin{.type=F_TYPE::MODULE, .module=u.module};
    }
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
        delete s.second;
    }
}

bool Checker::is_variable(const ObjectType& a) {
    return a.type_params.size() == 0 && islower(a.id[0]);
}

USemanticInfo Checker::dispatch_rvalue(Node* nod) {
    return this->dispatch_any(nod, true);
}
USemanticInfo Checker::dispatch(Node* nod) {
    return this->dispatch_any(nod, false);
}

USemanticInfo Checker::dispatch_any(Node* nod, bool is_rvalue) {
    auto& n = *nod;
    switch (n.ntype) {
        case NodeType::ASSIGN:
            return this->visit_assignment(n.assign());
        case NodeType::ENUM:
            return this->visit_enum((EnumNode&) *nod);
        case NodeType::BINOP: {
            auto r = this->visit_binop(n.binop());
            return r;
        }
        case NodeType::BOOLOP:
            return this->visit_boolop(n.boolop());
        case NodeType::BLOCK:
            return this->visit_block(n.block());
        case NodeType::BOOLEAN:
            return this->visit_boolean(n.boolean());
        case NodeType::BRK:
            return this->visit_break(n.brk());
        case NodeType::CALL:
            return this->visit_call(n.call(), is_rvalue);
        case NodeType::CLS:
            return this->visit_class(n.cls());
        case NodeType::CNTINUE:
            return this->visit_continue(n.cntinue());
        case NodeType::DECL:
            return this->visit_declaration(n.decl());
        case NodeType::EMPTYLST:
            return this->visit_emptylist(n.emptylst());
        case NodeType::FLOT:
            return this->visit_float(n.flot());
        case NodeType::FORLOOP:
            return this->visit_for(n.forloop());
        case NodeType::FUNC:
            return this->visit_function(n.func());
        case NodeType::ID:
            return this->visit_id(n.id());
        case NodeType::CAST:
            return this->visit_cast(n.cast());
        case NodeType::IFF:
            return this->visit_if(n.iff());
        case NodeType::LST:
            return this->visit_list(n.lst());
        case NodeType::MEMBER:
            return this->visit_member(n.member());
        case NodeType::NONE:
            return this->visit_none(n.none());
        case NodeType::NUMBER:
            return this->visit_number(n.number());
        case NodeType::RETRN:
            return this->visit_return(n.retrn());
        case NodeType::STRNG:
            return this->visit_string(n.strng());
        case NodeType::SUB:
            return this->visit_subscript(n.sub());
        case NodeType::TERNARY:
            return this->visit_ternary(n.ternary());
        case NodeType::TUPLE:
            return this->visit_tuple(n.tuple());
        case NodeType::UNARY:
            return this->visit_unary((UnaryOpNode&) n);
        case NodeType::WHIL:
            return this->visit_while(n.whil());
        case NodeType::PARTIAL:
            return this->visit_partial(n.partial());
        case NodeType::DICT:
            return this->visit_dict(n.dict());
        case NodeType::EMPTYDICT:
            return this->visit_emptydict(n.emptydict());
        case NodeType::DEF_CONST:
            return this->visit_defconst(n.defconst());
        case NodeType::IMPORT:
            return this->visit_import(n.import());
        case NodeType::ALIAS:
            return this->visit_alias((AliasNode*) nod);
        case NodeType::MATCH_EXP:
            return this->visit_match((MatchExpressionNode*) nod);
        default:
            this->error_reporter.fail("Don't know what to do!");
    }
    return nullptr;
}
