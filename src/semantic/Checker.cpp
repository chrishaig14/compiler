#include <iostream>
#include <set>
#include <cassert>
#include "Checker.h"
#include "../macros.h"
#include "../logging/logging.h"
#include "util.h"
#include "../ast/TryCatchNode.h"

bool function_is_generic(const FunctionType& ft) {
    for (auto* param_type: ft.param_types) {
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

bool Checker::assert_type_exists(const TypeNode& type, TextPosition pos) {
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
        for (auto* t: type.function().param_types) {
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
        const FunctionType& fo = t.function();
        for (auto* param_type: fo.param_types) {
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
Checker::match_arguments_to_generic_function(const FunctionType& ft, VectorOfTypes arg_types,
                                             std::map<std::string, TypeNode*>& all_substitutions) {
    std::unique_ptr<FunctionType> f;
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
    SemanticInfo rv;
    rv.entity = *new EntityValue(std::make_unique<Value>(f->return_type->clone()));
    // delete f;
    return std::make_unique<SemanticInfo>(rv);
}

UTypeNode make_type_from_object_pattern(const ObjectType& object_type, const MapStringType& replacements) {
    std::string type_identifier = object_type.id;
    for (const auto& r: replacements) {
        if (type_identifier == r.first) {
            if (!object_type.type_params.empty()) {
                throw std::runtime_error(
                        "Trying to make a type for a template for exmaple struct Foo[T]{foo:T[Integer];}!");
            }
            return UTypeNode(r.second->clone());
        }
    }
    // It's not the top level type
    VectorOfTypes new_type_params;
    for (auto* tp: object_type.type_params) {
        TypeNode* new_tp = make_type(*tp, replacements).release();
        new_type_params.push_back(new_tp);
    }
    for (auto* nt: new_type_params) {
        if (nt->kind == Kind::OBJECT && nt->object().id.size() == 1 && (islower(nt->object().id.c_str()[0]) != 0)) {
            nt->object().is_generic_param = true;
        }
    }
    auto ot = std::make_unique<ObjectType>(type_identifier, new_type_params);
    if (type_identifier.size() == 1 && (islower(type_identifier.c_str()[0]) != 0)) {
        ot->object().is_generic_param = true;
    }
    return ot;

}

UTypeNode make_type_from_function_pattern(const FunctionType& ftn, const MapStringType& replacements) {
    VectorOfTypes new_param_types;
    for (auto* pt: ftn.param_types) {
        TypeNode* new_pt = make_type(*pt, replacements).release();
        new_param_types.push_back(new_pt);
    }
    TypeNode* new_return_type = make_type(*ftn.return_type, replacements).release();
    return std::make_unique<FunctionType>(new_param_types, new_return_type);
}

UTypeNode make_type(const TypeNode& original, const MapStringType& replacements) {
    if (original.kind == Kind::OBJECT) {
        return make_type_from_object_pattern(original.object(), replacements);
    } else {
        return make_type_from_function_pattern(original.function(), replacements);
    }
}

Class* Checker::instantiate_generic(Class* generic, const ObjectType& instance) {
    std::cout << "******* Instantiating type: " << instance.to_string() << std::endl;
    MapStringType replacements;
    for (size_t i = 0; i < generic->type_params.size(); i++) {
        std::string tp = generic->type_params[i];
        TypeNode& type_replacement = *instance.type_params[i];
        replacements[tp] = &type_replacement;
    }
    auto field_names = generic->member_names;
    VectorOfTypes concrete_field_types;
    for (auto* f: generic->member_types) {
        TypeNode& concrete_type = *make_type(*f, replacements).release();
        concrete_field_types.push_back(&concrete_type);
        this->module.fill_actual(&concrete_type);
    }


    std::unordered_map<std::string, ConstFunction*> concrete_methods;
    for (const auto& method_cf: generic->methods) {
        if (method_cf.second->implicit != nullptr) {
            Implicit* implicit = method_cf.second->implicit;
            if (implicit->type == generic->type_params[0]) {
                std::cout << "----------- Generic with implicit which is class parameter: " << method_cf.first
                          << std::endl;
                EntityValue& e = *(EntityValue*) entity_from_type(*instance.type_params[0]);
                this->fill_value(*e.value);
                Class* clazz_t = e.value->clazz;
                auto meth = clazz_t->methods.find(implicit->method);
                if (meth == clazz_t->methods.end()) {
                    std::cout << "Not found in instance's type parameter, so skipping" << std::endl;
                } else {
                    std::cout << "Found implicit in instance's type parameter" << std::endl;
                    TypeNode* t = (method_cf.second)->ft;
                    TypeNode& concrete_type = *make_type(*t, replacements).release();
                    this->module.fill_actual(&concrete_type);
                    auto* cf = new ConstFunction(method_cf.second->path, (FunctionType*) concrete_type.clone());
                    std::cout << "Instantiated generic method " << method_cf.first << " : " << cf->ft->to_string()
                              << std::endl;
                    concrete_methods[method_cf.first] = cf;
                }

            } else {
                std::cout << "----------- Normal generic function: r" << method_cf.first << std::endl;
                TypeNode* t = (method_cf.second)->ft;
                TypeNode& concrete_type = *make_type(*t, replacements).release();
                this->module.fill_actual(&concrete_type);
                auto* cf = new ConstFunction(method_cf.second->path, (FunctionType*) concrete_type.clone());
                std::cout << "Instantiated generic method " << method_cf.first << " : " << cf->ft->to_string()
                          << std::endl;
                concrete_methods[method_cf.first] = cf;
            }
        } else {
            TypeNode* t = (method_cf.second)->ft;
            TypeNode& concrete_type = *make_type(*t, replacements).release();
            this->module.fill_actual(&concrete_type);
            auto* cf = new ConstFunction(method_cf.second->path, (FunctionType*) concrete_type.clone());
            std::cout << "Instantiated generic method " << method_cf.first << " : " << cf->ft->to_string() << std::endl;
            concrete_methods[method_cf.first] = cf;
        }
    }

    std::unordered_map<std::string, ConstFunction*> concrete_static_methods;
    for (const auto& m: generic->static_methods) {
        TypeNode* t = (m.second)->ft;
        TypeNode& concrete_type = *make_type(*t, replacements).release();
        this->module.fill_actual(&concrete_type);
        auto* cf = new ConstFunction(m.second->path, (FunctionType*) concrete_type.clone());
        concrete_static_methods[m.first] = cf;
    }

    auto* concrete = new Class();
    concrete->class_name = generic->class_name;
    concrete->methods = concrete_methods;
    concrete->static_methods = concrete_static_methods;
    concrete->member_names = generic->member_names;
    concrete->member_types = concrete_field_types;
    concrete->path = generic->path;
    for (size_t i = 0; i < generic->member_names.size(); i++) {
        std::string mn = generic->member_names[i];
        concrete->members[mn] = concrete_field_types[i];
        concrete->member_entities[mn] = new EntityNothing();
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
    return Flirpin{};
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
    for (const auto& s: this->scopes) {
        delete s.second;
    }
}

bool Checker::is_variable(const ObjectType& a) {
    return a.type_params.empty() && (islower(a.id[0]) != 0);
}

USemanticInfo Checker::dispatch_rvalue(Node& nod) {
    return this->dispatch_any(nod, true);
}

USemanticInfo Checker::dispatch(Node& nod) {
    return this->dispatch_any(nod, false);
}

USemanticInfo Checker::dispatch_any(Node& n, bool is_rvalue) {
    switch (n.ntype) {
        case NodeType::ASSIGN:
            return this->visit_assignment((AssignmentNode&) n);
        case NodeType::ENUM:
            return this->visit_enum((EnumNode&) n);
        case NodeType::BINOP: {
            auto r = this->visit_binop((ast::BinopNode&) n);
            return r;
        }
        case NodeType::BOOLOP:
            return this->visit_boolop((BoolOpNode&) n);
        case NodeType::BLOCK:
            return this->visit_block((BlockNode&) n);
        case NodeType::BOOLEAN:
            return this->visit_boolean((BooleanNode&) n);
        case NodeType::BRK:
            return this->visit_break((BreakNode&) n);
        case NodeType::CALL:
            return this->visit_call((CallNode&) n, is_rvalue);
        case NodeType::CLS:
            return this->visit_class((ClassNode&) n);
        case NodeType::CNTINUE:
            return this->visit_continue((ContinueNode&) n);
        case NodeType::DECL:
            return this->visit_declaration((ast::DeclarationNode&) n);
        case NodeType::EMPTYLST:
            return this->visit_emptylist((EmptyListNode&) n);
        case NodeType::FORLOOP:
            return this->visit_for((ForNode&) n);
        case NodeType::FUNC:
            return this->visit_function((FunctionNode&) n);
        case NodeType::ID:
            return this->visit_id((IdNode&) n);
        case NodeType::CAST:
            return this->visit_cast((CastNode&) n);
        case NodeType::IFF:
            return this->visit_if((IfNode&) n);
        case NodeType::LST:
            return this->visit_list((ListNode&) n);
        case NodeType::MEMBER:
            return this->visit_member((MemberNode&) n);
        case NodeType::NONE:
            return this->visit_none((NoneNode&) n);
        case NodeType::NUMBER:
            return this->visit_number((NumberNode&) n);
        case NodeType::THROW:
            return nullptr;
            // return this->visit_throw((ThrowNode&) n);
        case NodeType::RETRN:
            return this->visit_return((ReturnNode&) n);
        case NodeType::TRY_CATCH:
            return this->visit_try_catch((TryCatchNode&) n);
        case NodeType::STRNG:
            return this->visit_string((StringNode&) n);
        case NodeType::SUB:
            return this->visit_subscript((SubscriptNode&) n);
        case NodeType::TERNARY:
            return this->visit_ternary((TernaryNode&) n);
        case NodeType::TUPLE:
            return this->visit_tuple((TupleNode&) n);
        case NodeType::UNARY:
            return this->visit_unary((UnaryOpNode&) n);
        case NodeType::WHIL:
            return this->visit_while((WhileNode&) n);
        case NodeType::PARTIAL:
            return this->visit_partial((PartialApplication&) n);
        case NodeType::DICT:
            return this->visit_dict((DictNode&) n);
        case NodeType::EMPTYDICT:
            return this->visit_emptydict((EmptyDictNode&) n);
        case NodeType::DEF_CONST:
            return this->visit_defconst((DefaultConstructorNode&) n);
        case NodeType::IMPORT:
            return this->visit_import((ImportNode&) n);
        case NodeType::ALIAS:
            return this->visit_alias((AliasNode&) n);
        case NodeType::MATCH_EXP:
            return this->visit_match((MatchExpressionNode&) n);
        default:
            this->error_reporter.fail("Don't know what to do!");
    }
    return nullptr;
}
