//
// Created by chris on 2/5/21.
//

#include "CheckerUtils.h"
#include "../simple_nodes/common/include/TypeObject.h"

std::string binoptype_to_str(OpType op) {
    std::map<OpType, std::string> funs;
    funs[OpType::ADD] = "add";
    funs[OpType::SUB] = "sub";
    funs[OpType::MUL] = "mul";
    funs[OpType::DIV] = "div";
    funs[OpType::MOD] = "mod";

    funs[OpType::EQ] = "eq";
    funs[OpType::LT] = "lt";
    funs[OpType::GT] = "gt";
    funs[OpType::LE] = "le";
    funs[OpType::GE] = "ge";
    funs[OpType::AND] = "and";
    funs[OpType::OR] = "or";
    return "__" + funs.at(op) + "__";
}


int target_union_type(const sem::TypeObject& target, const sem::Type& source) {
    for (size_t ti = 0; ti < target.type_params.size(); ti++) {
        if (target.type_params[ti]->actual_to_string() == source.actual_to_string()) {
            return ti;
        }
    }
    return -1;
}

sem::FunctionDef* make_class_default_init(const std::string& class_path, const VectorOfStrings& members) {
    // auto nn = std::make_unique<sem::NewObject>();
    // nn->class_name = class_path;
    // for (const auto& m: members) {
    //     auto idn = std::make_unique<sem::Id>(m);
    //     nn->args.push_back(std::move(idn));
    // }
    // auto rn = std::make_unique<sem::Return>(std::move(nn));
    // auto bn = std::make_unique<sem::Block>();
    // bn->nodes.push_back(std::move(rn));
    // auto* fn = new sem::FunctionDef(class_path + ".__init__", members, std::move(bn));
    // return fn;
    return nullptr;
}


ast::Type* get_entity_type(Entity& e) {
    if (e.type == E_TYPE::CONST_FUNCTION) {
        return ((EntityConstFunction&) e).const_function.const_function_ft.to_ast();
    } else if (e.type == E_TYPE::VALUE) {
        return ((Value&) e).type.to_ast();
    }
    throw std::runtime_error("Get type of non function/object!");
}


void mangle_generic_names(ast::Type& t) {
    if (t.kind == Kind::OBJECT) {
        return mangle_generic_names(t.object());
    }
    return mangle_generic_names(t.function());
}

void mangle_generic_names(ast::FunctionType& t) {
    for (auto& pt: t.param_types) {
        mangle_generic_names(*pt);
    }
    mangle_generic_names(*t.return_type);
}

void mangle_generic_names(ast::ObjectType& t) {
    if (t.is_generic_param) {
        t.id = t.id + "0";
    } else {
        for (auto* tp: t.type_params) {
            mangle_generic_names(*tp);
        }
    }
}


void make_not_generic(ast::Type& t) {
    if (t.kind == Kind::FUNCTION) {
        return make_not_generic(t.function());
    }
    return make_not_generic(t.object());
}

void make_not_generic(ast::FunctionType& ft) {
    for (auto& pt: ft.param_types) {
        make_not_generic(*pt);
    }
    make_not_generic(*ft.return_type);
}

void make_not_generic(ast::ObjectType& ot) {
    ot.is_generic_param = false;
    for (auto* tp: ot.type_params) {
        make_not_generic(*tp);
    }
}

sem::UExp make_union_wrapper(int type_index, sem::UExp expression) {
    auto new_union = std::make_unique<sem::NewObject>();
    new_union->class_name = "core_D_core_D_Union";
    auto in = std::make_unique<sem::Integer>(std::string());
    in->str = std::to_string(type_index); // FIXME, use int directly
    new_union->args.push_back(std::move(expression));
    new_union->args.push_back(std::move(in));
    return new_union;
}


sem::UExp make_boolop_snode(ConstFunction* operator_fun, SemanticInfo& left_info, SemanticInfo& right_info) {
    // auto function_id = std::make_unique<sem::Id>(operator_fun->path.as_str());
    // std::vector<sem::UExp> v;
    // v.emplace_back(std::move(left_info.snode));
    // v.emplace_back(std::move(right_info.snode));
    // auto sn = std::make_unique<sem::Call>(std::move(function_id), std::move(v));
    // return sn;
    return nullptr;
}

