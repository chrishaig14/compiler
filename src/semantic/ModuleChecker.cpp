#include <iostream>
#include <set>
#include <cassert>
#include "ModuleChecker.h"
#include <util/macros.h>
#include <logging/logging.h>
#include <simple_nodes/common/include/TypeObject.h>
#include <simple_nodes/common/include/TypeFunction.h>
#include "util.h"

UExpressionInfo exp_error_stub() {
    return std::make_unique<ExpErrorStub>();
}


bool function_is_generic(const sem::TypeFunction& ft) {
    for (auto& param_type: ft.param_types) {
        if (is_generic(*param_type)) {
            return true;
            break;
        }
    }
    return false;
}

ModuleChecker::ModuleChecker(Package& top_package, Module& module, std::map<std::string, std::string>& instances)
        : instances(instances), module(module), error_reporter(module.code_lines), top_package(top_package) {
    this->scope = new SymbolTable(nullptr);
    this->add_this = false;
}

void ModuleChecker::enter_scope() {
    this->scope = new SymbolTable(this->scope);
}

void ModuleChecker::leave_scope() {
    this->scope = this->scope->parent;
}

bool ModuleChecker::assert_type_exists(const ast::Type& type, TextPosition pos) {
    if (type.kind == Kind::OBJECT) {
        if (type.object().id == ".None") {
            return true;
        }
        if (type.object().type_params.empty()) {
            sem::UType p_type(type.to_sem());
            if (!is_generic(*p_type)) {
            }
            return true;
        }
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

bool is_generic(const sem::Type& t) {
    if (t.kind == sem::Kind::OBJECT) {
        const sem::TypeObject& o = t.object();
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
        const sem::TypeFunction& fo = t.function();
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

UExpressionInfo
ModuleChecker::match_arguments_to_generic_function(const ast::FunctionType& ft, ast::VectorOfTypes arg_types,
                                                   std::map<std::string, ast::Type*>& all_substitutions,
                                                   std::unordered_map<std::string, std::string> constraints) {
    std::unique_ptr<ast::FunctionType> f;
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
        return exp_error_stub();
    }

    for (auto& c: constraints) {
        auto s = all_substitutions.at(c.first);
        sem::Type* p_type = s->to_sem();
        this->module.fill_actual(*p_type);
        // std::unique_ptr<EntityValue> v = this->make_value(p_type);
        // auto& clazz = *v->clazz;
        // bool ok = false;
        // for (auto& it: clazz.implemented_typeclasses) {
        //     if (it == c.second) {
        //         ok = true;
        //         break;
        //     }
        // }
        // if (p_type->is_generic()) {
        std::cout << this->instances.size() << std::endl;
        std::string x = p_type->object().data.actual_base_path.as_str();
        if (this->instances.count(x) != 0) {
            auto instance = this->instances.at(p_type->object().data.actual_base_path.as_str());
            if (instance != c.second) {
                throw std::runtime_error(
                        "function call with type substitution " + c.first + " -> " + p_type->to_string() +
                        " which doesn't implement required typeclass " + c.second);
            }
        } else {
            throw std::runtime_error(
                    "function call with type substitution " + c.first + " -> " + p_type->to_string() +
                    " which doesn't implement required typeclass " + c.second);
        }
        // }
    }


    for (auto* at: arg_types) {
        delete at;
    }
    UExpressionInfo rv_p = std::make_unique<ExpressionInfo>();
    auto& rv = *rv_p;
    rv.set_entity(this->make_value(f->return_type->to_sem()));
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

std::unique_ptr<ConcreteClass>
ModuleChecker::instantiate_generic(const TemplateClassInfo& generic, const ast::ObjectType& instance) {
    std::cout << "gonna instantiate generic: " << instance.actual_to_string() << std::endl;
    assert(not generic.type_params.empty());
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
        // this->module.fill_actual(concrete_type);
    }


    std::unordered_map<std::string, std::unique_ptr<ConstFunction>> concrete_methods;
    for (const auto& method_cf: generic.methods) {
        ast::UTypeNode t((method_cf.second)->const_function_ft.to_ast());
        ast::UTypeNode concrete_type = make_type(*t, replacements);
        auto tf = (sem::TypeFunction*) concrete_type->to_sem();
        this->module.fill_actual(*tf);
        auto cf = std::make_unique<ConstFunction>(method_cf.second->path, sem::UTypeFunction(tf));
        concrete_methods[method_cf.first] = std::move(cf);
    }

    std::unordered_map<std::string, std::unique_ptr<ConstFunction>> concrete_static_methods;
    for (const auto& m: generic.static_methods) {
        ast::UTypeNode t((m.second)->const_function_ft.to_ast());
        ast::UTypeNode concrete_type(make_type(*t, replacements));
        sem::Type* p_type = concrete_type->to_sem();
        this->module.fill_actual(*p_type);
        concrete_static_methods[m.first] = std::make_unique<ConstFunction>(m.second->path,
                                                                           sem::UTypeFunction((sem::TypeFunction*) p_type));
    }

    auto concrete = std::make_unique<ConcreteClass>(generic.class_name, generic.path);
    concrete->methods = std::move(concrete_methods);
    concrete->static_methods = std::move(concrete_static_methods);
    concrete->member_names = generic.member_names;
    concrete->member_types = concrete_field_types;
    for (size_t i = 0; i < generic.member_names.size(); i++) {
        std::string mn = generic.member_names[i];
        concrete->members[mn] = concrete_field_types[i];
        sem::Type* u = concrete_field_types[i]->to_sem();
        this->module.fill_actual(*u);
        concrete->member_entities[mn] = this->make_value(u);
        // std::make_unique<EntityNothing>();
    }
    return concrete;
}

ModuleMember* map_unit_to_module_member(Unit& u) {
    if (u.is_package()) {
        return new PackageModuleMember(&u.package());
    } else if (u.is_module()) {
        return new ModuleModuleMember(&u.module());
    }
    return nullptr;
}

bool ModuleChecker::is_immutable(const ast::Type& node) {
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

bool ModuleChecker::is_variable(const ast::ObjectType& a) {
    return a.type_params.empty() && (islower(a.id[0]) != 0);
}

UExpressionInfo ModuleChecker::dispatch_rvalue(const ast::ExpNode& n) {
    switch (n.ntype) {
        case ExpNodeType::BINOP: {
            return this->visit_binop((ast::BinaryOp&) n);
        }
        case ExpNodeType::BOOLEAN:
            return this->visit_boolean((ast::Boolean&) n);
        case ExpNodeType::CALL:
            return this->visit_callexp((ast::CallExp&) n, true);
        case ExpNodeType::EMPTYLST:
            return this->visit_emptylist((ast::EmptyList&) n);
        case ExpNodeType::ID:
            return this->visit_id((ast::Id&) n);
        case ExpNodeType::LST:
            return this->visit_list((ast::List&) n);
        case ExpNodeType::MEMBER:
            return this->visit_member((ast::Member&) n);
        case ExpNodeType::NONE:
            return this->visit_none((ast::None&) n);
        case ExpNodeType::NUMBER:
            return this->visit_number((ast::Number&) n);
        case ExpNodeType::STRNG:
            return this->visit_string((ast::String&) n);
        case ExpNodeType::SUB:
            return this->visit_subscript((ast::Subscript&) n);
        case ExpNodeType::TERNARY:
            return this->visit_ternary((ast::Ternary&) n);
        case ExpNodeType::TUPLE:
            return this->visit_tuple((ast::Tuple&) n);
        case ExpNodeType::UNARY:
            return this->visit_unary((ast::UnaryOp&) n);
        case ExpNodeType::PARTIAL:
            return this->visit_partial((ast::PartialApplication&) n);
        case ExpNodeType::DICT:
            return this->visit_dict((ast::DictNode&) n);
        case ExpNodeType::EMPTYDICT:
            return this->visit_emptydict((ast::EmptyDict&) n);
        case ExpNodeType::DEF_CONST:
            return this->visit_defconst((ast::DefaultConstructor&) n);
        default:
            this->error_reporter.fail("Don't know what to do!");
    }
    __builtin_unreachable();
}

sem::UCommon ModuleChecker::dispatch(const ast::Statement& nod) {
    return this->dispatch_statement(nod, false);
}

std::unique_ptr<sem::Top> ModuleChecker::dispatch_top(const ast::TopNode& n) {
    switch (n.ntype) {
        case TopNodeType::FUNC:
            return this->visit_function((ast::Function&) n);
        case TopNodeType::ENUM:
            return this->visit_enum((ast::EnumNode&) n);
        case TopNodeType::IMPORT:
            return nullptr;
        case TopNodeType::TYPECLASS:
            return this->visit_typeclass((ast::TypeclassAst&) n);
        case TopNodeType::CONCRETE_CLS:
            return this->visit_class((ast::ConcreteClassDef&) n);
            break;
        case TopNodeType::TEMPLATE_CLS:
            return this->visit_template_class((ast::TemplateClassDef&) n);
            break;
        case TopNodeType::INSTANCE:
            break;
    }
    __builtin_unreachable();
}

sem::UCommon ModuleChecker::dispatch_statement(const ast::Statement& n, bool is_rvalue) {
    switch (n.ntype) {
        case StatementType::ASSIGN:
            return this->visit_assignment((ast::Assignment&) n);
        case StatementType::BRK:
            return this->visit_break((ast::Break&) n);
        case StatementType::CALL:
            return this->visit_call((ast::Call&) n);
        case StatementType::CNTINUE:
            return this->visit_continue((ast::Continue&) n);
        case StatementType::DECL:
            return this->visit_declaration((ast::Declaration&) n);
        case StatementType::FORLOOP:
            return this->visit_for((ast::For&) n);
        case StatementType::CAST:
            return this->visit_cast((ast::Cast&) n);
        case StatementType::IFF:
            return this->visit_if((ast::If&) n);
        case StatementType::THROW:
            return nullptr;
        case StatementType::RETRN:
            return this->visit_return((ast::Return&) n);
        case StatementType::WHIL:
            return this->visit_while((ast::While&) n);
        case StatementType::ALIAS:
            return this->visit_alias((ast::Alias&) n);
        case StatementType::MATCH_EXP:
            return this->visit_match((ast::Match&) n);
        default:
            this->error_reporter.fail("Don't know what to do!");
    }
    return nullptr;
}
