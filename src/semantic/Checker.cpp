#include <iostream>
#include <set>
#include <cassert>
#include "Checker.h"
#include "../macros.h"
#include "../logging/logging.h"
#include "util.h"

bool function_is_generic(const ast::FunctionType& ft) {
    for (auto& param_type: ft.param_types) {
        if (is_generic(*param_type)) {
            return true;
            break;
        }
    }
    return false;
}

Checker::Checker(Package& top_package, Module& module) : module(module), top_package(top_package) {
    this->is_call = false;
    this->scope = new SymbolTable("global", nullptr);
    this->scopes["global"] = this->scope;
    this->add_this = false;
    this->loop_count = 0;
}

void Checker::enter_scope(const std::string& name) {
    std::string new_scope_name = this->scope->s_name + "." + name;
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

bool Checker::assert_type_exists(const ast::Type& type, TextPosition pos) {
    if (type.kind == Kind::OBJECT) {
        if (type.object().id == ".None") {
            return true;
        }
        if (type.object().type_params.empty()) {
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
        for (auto& t: type.function().param_types) {
            if (!this->assert_type_exists(*t, pos)) {
                error = true;
            }
            return !error;
        }
        return this->assert_type_exists(*type.function().return_type, pos);
    }
    return false;
}

bool is_generic(const ast::Type& t) {
    if (t.kind == Kind::OBJECT) {
        const ast::ObjectType& o = t.object();
        if (o.id.size() == 1 && (islower(o.id[0]) != 0)) {
            // a is generic
            assert(o.type_params.empty());
            return true;
        }
        for (auto* type_param: o.type_params) {
            if (is_generic(*type_param)) {
                return true;
            }
        }
    } else {
        const ast::FunctionType& fo = t.function();
        for (auto& param_type: fo.param_types) {
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

std::unique_ptr<SemanticInfo>
Checker::match_arguments_to_generic_function(const ast::FunctionType& ft, ast::VectorOfTypes arg_types,
                                             std::map<std::string, ast::Type*>& all_substitutions) {
    std::unique_ptr<ast::FunctionType> f;
    // = ft.clone();
    try {
        f = unify_function_call(ft, arg_types, all_substitutions);
        if (f == nullptr) {
            throw std::runtime_error("unify error");
        }
    } catch (...) {
        std::string sss = "Error: cannot unify " + E_HLT(ft.to_string()) + " with args: ";
        std::string args_str;
        for (auto* at: arg_types) {
            args_str += at->to_string() + ", ";
        }
        if (!arg_types.empty()) {
            args_str = args_str.substr(0, args_str.size() - 2);
        }
        sss += E_HLT("(" + args_str + ")");
        this->error_reporter.fail(sss);
        return error_stub();
    }
    for (auto* at: arg_types) {
        delete at;
    }
    USemanticInfo rv_p = std::make_unique<SemanticInfo>();
    auto& rv = *rv_p;
    rv.set_entity(std::make_unique<Value>(f->return_type->to_sem()).release());
    // delete f;
    return rv_p;
}

ast::UTypeNode make_type_from_object_pattern(const ast::ObjectType& object_type, const MapStringType& replacements) {
    std::string type_identifier = object_type.id;
    for (const auto& r: replacements) {
        if (type_identifier == r.first) {
            if (!object_type.type_params.empty()) {
                throw std::runtime_error(
                        "Trying to make a type for a template for exmaple struct Foo[T]{foo:T[Integer];}!");
            }
            return ast::UTypeNode(r.second->clone());
        }
    }
    // It's not the top level type
    ast::VectorOfTypes new_type_params;
    for (auto* tp: object_type.type_params) {
        ast::Type* new_tp = make_type(*tp, replacements).release();
        new_type_params.push_back(new_tp);
    }
    for (auto* nt: new_type_params) {
        if (nt->kind == Kind::OBJECT && nt->object().id.size() == 1 && (islower(nt->object().id.c_str()[0]) != 0)) {
            nt->object().is_generic_param = true;
        }
    }
    auto ot = std::make_unique<ast::ObjectType>(type_identifier, new_type_params);
    if (type_identifier.size() == 1 && (islower(type_identifier.c_str()[0]) != 0)) {
        ot->object().is_generic_param = true;
    }
    return ot;

}

ast::UTypeNode make_type_from_function_pattern(const ast::FunctionType& ftn, const MapStringType& replacements) {
    ast::VectorOfTypes new_param_types;
    for (auto& pt: ftn.param_types) {
        ast::Type* new_pt = make_type(*pt, replacements).release();
        new_param_types.push_back(new_pt);
    }
    ast::UTypeNode new_return_type = make_type(*ftn.return_type, replacements);
    return std::make_unique<ast::FunctionType>(new_param_types, std::move(new_return_type));
}

ast::UTypeNode make_type(const ast::Type& original, const MapStringType& replacements) {
    if (original.kind == Kind::OBJECT) {
        return make_type_from_object_pattern(original.object(), replacements);
    } else {
        return make_type_from_function_pattern(original.function(), replacements);
    }
}

Class* Checker::instantiate_generic(const Class& generic, const ast::ObjectType& instance) {
    std::cout << "******* Instantiating type: " << instance.to_string() << std::endl;
    MapStringType replacements;
    for (size_t i = 0; i < generic.type_params.size(); i++) {
        std::string tp = generic.type_params[i];
        ast::Type& type_replacement = *instance.type_params[i];
        replacements[tp] = &type_replacement;
    }
    auto field_names = generic.member_names;
    ast::VectorOfTypes concrete_field_types;
    for (auto* f: generic.member_types) {
        ast::Type& concrete_type = *make_type(*f, replacements).release();
        concrete_field_types.push_back(&concrete_type);
        this->module.fill_actual(concrete_type);
    }


    std::unordered_map<std::string, ConstFunction*> concrete_methods;
    for (const auto& method_cf: generic.methods) {
        if (method_cf.second->implicit != nullptr) {
            Implicit* implicit = method_cf.second->implicit;
            if (implicit->type == generic.type_params[0]) {
                std::cout << "----------- Generic with implicit which is class parameter: " << method_cf.first
                          << std::endl;
                Value& e = *(Value*) entity_from_type(*instance.type_params[0]);
                this->fill_value(e);
                Class* clazz_t = e.clazz;
                auto meth = clazz_t->methods.find(implicit->method);
                if (meth == clazz_t->methods.end()) {
                    std::cout << "Not found in instance's type parameter, so skipping" << std::endl;
                } else {
                    std::cout << "Found implicit in instance's type parameter" << std::endl;
                    ast::Type* t = (method_cf.second)->const_function_ft_p->to_ast();
                    ast::Type& concrete_type = *make_type(*t, replacements).release();
                    this->module.fill_actual(concrete_type);
                    auto* cf = new ConstFunction(method_cf.second->path, (sem::TypeFunction*) concrete_type.to_sem());
                    std::cout << "Instantiated generic method " << method_cf.first << " : "
                              << cf->const_function_ft.to_string() << std::endl;
                    concrete_methods[method_cf.first] = cf;
                }

            } else {
                std::cout << "----------- Normal generic function: r" << method_cf.first << std::endl;
                ast::Type* t = (method_cf.second)->const_function_ft_p->to_ast();
                ast::Type& concrete_type = *make_type(*t, replacements).release();
                this->module.fill_actual(concrete_type);
                auto* cf = new ConstFunction(method_cf.second->path, (sem::TypeFunction*) concrete_type.to_sem());
                std::cout << "Instantiated generic method " << method_cf.first << " : "
                          << cf->const_function_ft.to_string() << std::endl;
                concrete_methods[method_cf.first] = cf;
            }
        } else {
            ast::Type* t = (method_cf.second)->const_function_ft_p->to_ast();
            ast::Type& concrete_type = *make_type(*t, replacements).release();
            this->module.fill_actual(concrete_type);
            auto* cf = new ConstFunction(method_cf.second->path, (sem::TypeFunction*) concrete_type.to_sem());
            std::cout << "Instantiated generic method " << method_cf.first << " : " << cf->const_function_ft.to_string()
                      << std::endl;
            concrete_methods[method_cf.first] = cf;
        }
    }

    std::unordered_map<std::string, ConstFunction*> concrete_static_methods;
    for (const auto& m: generic.static_methods) {
        ast::Type* t = (m.second)->const_function_ft_p->to_ast();
        ast::Type& concrete_type = *make_type(*t, replacements).release();
        this->module.fill_actual(concrete_type);
        auto* cf = new ConstFunction(m.second->path, (sem::TypeFunction*) concrete_type.to_sem());
        concrete_static_methods[m.first] = cf;
    }

    auto* concrete = new Class(generic.class_name, generic.path);
    // concrete->class_name = ;
    concrete->methods = concrete_methods;
    concrete->static_methods = concrete_static_methods;
    concrete->member_names = generic.member_names;
    concrete->member_types = concrete_field_types;
    // concrete->path = generic.path;
    for (size_t i = 0; i < generic.member_names.size(); i++) {
        std::string mn = generic.member_names[i];
        concrete->members[mn] = concrete_field_types[i];
        concrete->member_entities[mn] = new EntityNothing();
    }
    return concrete;
}

USemanticInfo error_stub() {
    return std::make_unique<ErrorStub>();
}

Flirpin map_unit_to_flirpin(Unit u) {
    switch (u.type) {
        case U_TYPE::PACKAGE:
            return Flirpin{.type=F_TYPE::PACKAGE, .package=u.package};
        case U_TYPE::MODULE:
            return Flirpin{.type=F_TYPE::MODULE, .module=u.module};
    }
    return Flirpin{};
}

bool Checker::is_immutable(const ast::Type& node) {
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
    for (const auto& s: this->scopes) {
        delete s.second;
    }
    // for (auto& e: this->entities) {
    //     delete e.second;
    // }
}

bool Checker::is_variable(const ast::ObjectType& a) {
    return a.type_params.empty() && (islower(a.id[0]) != 0);
}

USemanticInfo Checker::dispatch_rvalue(ast::Node& nod) {
    return this->dispatch_any(nod, true);
}

USemanticInfo Checker::dispatch(ast::Node& nod) {
    return this->dispatch_any(nod, false);
}

USemanticInfo Checker::dispatch_any(ast::Node& n, bool is_rvalue) {
    switch (n.ntype) {
        case NodeType::ASSIGN:
            return this->visit_assignment((ast::Assignment&) n);
        case NodeType::ENUM:
            return this->visit_enum((ast::EnumNode&) n);
        case NodeType::BINOP: {
            auto r = this->visit_binop((ast::BinaryOp&) n);
            return r;
        }
            // case NodeType::BLOCK:
            //     return this->visit_block((ast::Block&) n);
        case NodeType::BOOLEAN:
            return this->visit_boolean((ast::Boolean&) n);
        case NodeType::BRK:
            return this->visit_break((ast::Break&) n);
        case NodeType::CALL:
            return this->visit_call((ast::Call&) n, is_rvalue);
        case NodeType::CLS:
            return this->visit_class((ast::Klass&) n);
        case NodeType::CNTINUE:
            return this->visit_continue((ast::Continue&) n);
        case NodeType::DECL:
            return this->visit_declaration((ast::Declaration&) n);
        case NodeType::EMPTYLST:
            return this->visit_emptylist((ast::EmptyList&) n);
        case NodeType::FORLOOP:
            return this->visit_for((ast::For&) n);
        case NodeType::FUNC:
            return this->visit_function((ast::Function&) n);
        case NodeType::ID:
            return this->visit_id((ast::Id&) n);
        case NodeType::CAST:
            return this->visit_cast((ast::Cast&) n);
        case NodeType::IFF:
            return this->visit_if((ast::If&) n);
        case NodeType::LST:
            return this->visit_list((ast::List&) n);
        case NodeType::MEMBER:
            return this->visit_member((ast::Member&) n);
        case NodeType::NONE:
            return this->visit_none((ast::None&) n);
        case NodeType::NUMBER:
            return this->visit_number((ast::Number&) n);
        case NodeType::THROW:
            return nullptr;
            // return this->visit_throw((ThrowNode&) n);
        case NodeType::RETRN:
            return this->visit_return((ast::Return&) n);
        case NodeType::STRNG:
            return this->visit_string((ast::String&) n);
        case NodeType::SUB:
            return this->visit_subscript((ast::Subscript&) n);
        case NodeType::TERNARY:
            return this->visit_ternary((ast::Ternary&) n);
        case NodeType::TUPLE:
            return this->visit_tuple((ast::Tuple&) n);
        case NodeType::UNARY:
            return this->visit_unary((ast::UnaryOp&) n);
        case NodeType::WHIL:
            return this->visit_while((ast::While&) n);
        case NodeType::PARTIAL:
            return this->visit_partial((ast::PartialApplication&) n);
        case NodeType::DICT:
            return this->visit_dict((ast::DictNode&) n);
        case NodeType::EMPTYDICT:
            return this->visit_emptydict((ast::EmptyDict&) n);
        case NodeType::DEF_CONST:
            return this->visit_defconst((ast::DefaultConstructor&) n);
        case NodeType::IMPORT:
            return this->visit_import((ast::Import&) n);
        case NodeType::ALIAS:
            return this->visit_alias((ast::Alias&) n);
        case NodeType::MATCH_EXP:
            return this->visit_match((ast::Match&) n);
        default:
            this->error_reporter.fail("Don't know what to do!");
    }
    return nullptr;
}
