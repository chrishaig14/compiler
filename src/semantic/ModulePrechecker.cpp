//
// Created by chris on 28/6/20.
//

#include <iostream>
#include "ModulePrechecker.h"
#include <simple_nodes/common/include/TypeObject.h>
#include <simple_nodes/common/include/TypeFunction.h>
#include <simple_nodes/common/include/Type.h>
#include "errors/include/Redeclared.h"
#include <units/entities/EntityNothing.h>
#include "errors/include/GlobalRedeclared.h"
#include <ast/top/Alias.h>
#include <ast/top/Import.h>
#include <ast/top/EnumNode.h>
#include <ast/top/TemplateClassDef.h>
#include <ast/top/ConcreteClassDef.h>
#include <ast/top/TypeclassAst.h>
#include <ast/top/Instance.h>
#include <units/infos/TemplateClass.h>
#include <semantic/my_error_formatter/MyErrorFormatter.h>
#include <log/log.h>

const VectorOfStrings default_imports = {"libcore.libcore.String", "libcore.libcore.Integer", "libcore.libcore.List",
                                         "libcore.libcore.Double", "libcore.libcore.Boolean", "libcore.libcore.Float",
                                         "libcore.libcore.Option", "libcore.libcore.print", "libcore.libcore.Dict",
                                         "libcore.libcore.input", "libcore.libcore.File"};


void ModulePrechecker::visit_import(ast::Import& node) {
    std::string name;
    if (node.has_alias) {
        name = node.alias;
    } else {
        name = node.path.back().str;
    }
    VectorOfStrings pp;
    for (auto& p: node.path) {
        pp.emplace_back(p.str);
    }
    // this->module.imported_paths[name] = Path(pp);
}

void ModulePrechecker::add_default_imports() {
    for (auto& path_str: default_imports) {
        Path path(path_str);
        this->module.imported_paths[path.as_vec().back()] = path;
    }
}


void ModulePrechecker::visit_function(ast::Function& node) {
    sem::VectorOfTypes x;
    for (ast::Type& type_node: node.parameter_types) {
        sem::Type* p_type = this->make_sem_type(type_node);
        x.emplace_back(p_type);
    }
    auto rt = sem::UType(this->make_sem_type(*node.return_type));
    auto function_info = sem::TypeFunction(std::move(x), std::move(rt));
    std::string function_name = node.identifier.str;
    Path function_path = Path(this->module.path, function_name);
    ConstFunction const_function(Path(this->module.path, function_name), function_info);
    for (auto& c: node.constraints) {
        for (auto& t: c.second) {
            const_function.constraints[c.first].insert(Path(this->module.path, t).as_str());
        }
    }
    node.path = const_function.path;
    this->module.add_func_definition(const_function);
}

std::unique_ptr<Enum> make_enum(ast::EnumNode& n, Path module_path) {
    auto enumm = std::make_unique<Enum>(n.id, Path(module_path, n.id), n.values);
    // enumm->functions["__eq__"] = std::make_unique<ConstFunction>(Path(enumm->path, "__eq__"), sem::F);
    // enumm->functions["__ne__"] = std::make_unique<ConstFunction>(Path(enumm->path, "__ne__"), nullptr);
    return enumm;
}

void ModulePrechecker::visit_root() {
    ast::Module& node = *this->module.ast;
    this->add_default_imports();
    this->check_duplicated_names(node);

    for (ast::Import& n: node.imports) {
        this->visit_import(n);
    }
    for (ast::ConcreteClassDef& n: node.classes) {
        auto class_info = std::make_unique<ConcreteClass>(n.class_name, Path(this->module.path, n.class_name));
        this->all_classes.emplace_back(*class_info);
        this->module.add_class_definition(std::move(class_info));
    }

    for (ast::TemplateClassDef& n: node.template_classes) {
        auto class_info = std::make_unique<TemplateClassInfo>(n.class_name,
                                                              Path(this->module.path, n.class_name),
                                                              n.type_parameters);
        this->module.add_template_class_definition(std::move(class_info));
    }

    for (ast::EnumNode& n: node.enums) {
        auto enumm = make_enum(n, this->module.path);
        this->module.add_enum_definition(std::move(enumm));
    }

    for (ast::ConcreteClassDef& n: node.classes) {
        this->visit_class(n);
    }

    for (ast::Instance& n: node.instances) {
        this->visit_instance(n);
    }

    for (ast::TemplateClassDef& n: node.template_classes) {
        this->visit_template_class(n);
    }


    for (ast::TypeclassAst& n: node.typeclasses) {
        this->visit_typeclass(n);
    }

    for (ast::Function& n: node.functions) {
        this->visit_function(n);
    }

}

void ModulePrechecker::check_duplicated_names(ast::Module& node) {
    std::set<std::string> names;
    for (auto& np: node.all) {
        auto& n = *np;
        std::string name;
        Token idtok;
        switch (n.ntype) {
            case TopNodeType::IMPORT:
                if (((ast::Import&) n).has_alias) {
                    name = ((ast::Import&) n).alias;
                } else {
                    name = ((ast::Import&) n).path.back().str;
                }
                break;
            case TopNodeType::FUNC:
                idtok = ((ast::Function&) n).identifier;
                break;
            case TopNodeType::ENUM:
                name = ((ast::EnumNode&) (n)).id;
                break;
            case TopNodeType::TYPECLASS:
                name = ((ast::TypeclassAst&) (n)).id;
                break;
            case TopNodeType::CONCRETE_CLS:
                name = ((ast::ConcreteClassDef&) n).class_name;
                break;
            case TopNodeType::TEMPLATE_CLS:
                name = ((ast::TemplateClassDef&) n).class_name;
                break;
            case TopNodeType::INSTANCE:
                continue;
        }
        // assert(not name.empty());
        if (idtok.str.empty()) {
            continue;
        }
        if (not names.contains(idtok.str)) {
            names.insert(idtok.str);
        } else {
            this->error_reporter.error(std::make_unique<error::GlobalRedeclared>(idtok));
        }
    }
}

void ModulePrechecker::visit_class(ast::ConcreteClassDef& node) {
    ConcreteClass* class_info = &this->module.members.at(node.class_name)->klass();
    for (const auto& attr: node.attributes) {
        // this->module.fill_actual(*mt.second);
        class_info->attribute_names.push_back(attr.id);
        sem::UType attr_type(this->make_sem_type(*attr.type));
        class_info->attribute_types.emplace_back(attr_type->clone());
        class_info->attributes[attr.id] = std::move(attr_type);
        class_info->attribute_entities[attr.id] = std::make_unique<EntityNothing>();
        class_info->all_members[attr.id] = ClassMemberCategory::attribute;
    }
    for (const auto& attr: node.static_attributes) {
        class_info->static_attributes[attr.first] = std::make_pair(attr.second.first->clone(), attr.second.second);
        class_info->all_members[attr.first] = ClassMemberCategory::static_attribute;
    }
    for (const auto& ast_meth: node.methods) {
        ast::Function& method = *ast_meth->func;
        std::string method_name = method.identifier.str;


        sem::VectorOfTypes x;
        for (ast::Type& p: method.parameter_types) {
            sem::Type* args = this->make_sem_type(p);
            x.emplace_back(args);
        }
        sem::Type* p_type = this->make_sem_type(*method.return_type);
        ConstFunction cf(Path(class_info->path, method_name), sem::TypeFunction(x, sem::UType(p_type)));
        method.path = cf.path;
        class_info->methods.insert(make_pair(method_name,
                                             InstanceMethod(Path(""), BaseMethod(ast_meth->is_static, cf))));
        class_info->all_members[method_name] = ClassMemberCategory::method;
    }

}

sem::Type* ModulePrechecker::make_sem_type(const ast::Type& t) {
    auto s = t.to_sem();
    this->module.fill_actual(*s);
    return s;
}

void ModulePrechecker::visit_template_class(ast::TemplateClassDef& node) {
    TemplateClassInfo* class_info = &this->module.members.at(node.class_name)->template_klass();
    for (const auto& mt: node.attributes) {
        class_info->member_names.push_back(mt.id);
        class_info->member_types.push_back(mt.type->clone());
        class_info->members[mt.id] = mt.type->clone();
        class_info->member_entities[mt.id] = std::make_unique<EntityNothing>();
    }
    for (const auto& mn: node.static_attributes) {
        class_info->static_members[mn.first] = std::make_pair(mn.second.first->clone(), mn.second.second);
    }
    for (const auto& ast_meth: node.methods) {
        ast::Function& method = *ast_meth->func;
        std::string method_name = method.identifier.str;

        sem::VectorOfTypes x;
        for (ast::Type& p: method.parameter_types) {
            sem::Type* args = this->make_sem_type(p);
            x.emplace_back(args);
        }
        sem::Type* p_type = this->make_sem_type(*method.return_type);
        auto cf = std::make_unique<ConstFunction>(Path(class_info->path, method_name),
                                                  sem::TypeFunction(x, sem::UType(p_type)));
        method.path = cf->path;
        if (ast_meth->is_static) {
            class_info->static_methods.insert(make_pair(method_name, std::move(cf)));
        } else {
            class_info->methods.insert(make_pair(method_name, std::move(cf)));
        }
    }

    // for (const auto& f: node.static_methods) {
    //     ast::Function& method = *f.second;
    //     sem::VectorOfTypes x;
    //     for (ast::Type& p: method.parameter_types) {
    //         sem::Type* args = p.to_sem();
    //         this->module.fill_actual(*args);
    //         x.emplace_back(args);
    //     }
    //     sem::Type* p_type = method.return_type->to_sem();
    //     this->module.fill_actual(*p_type);
    //     auto cf = std::make_unique<ConstFunction>(Path(class_info->path, f.first),
    //                                               sem::TypeFunction(x, sem::UType(p_type)));
    //     method.path = cf->path;
    //     class_info->static_methods.insert(make_pair(f.first, std::move(cf)));
    // }
}

void ModulePrechecker::visit_alias(ast::Alias& node) {
    // this->module.fill_actual(*node.aliased_type);
    // this->module.aliased_types[node.alias_id] = node.aliased_type;
}

void ModulePrechecker::visit_enum(ast::EnumNode& node) {

}

ModulePrechecker::ModulePrechecker(Module& module, std::map<std::string, std::set<std::string>>& instances,
                                   std::vector<std::reference_wrapper<ConcreteClass>>& all_classes)
        : module(module),
          error_reporter(module.code_lines, std::make_unique<MyErrorFormatter>(module.abs_path, module.code_lines)),
          all_classes(all_classes), instances(instances) {
}


void ModulePrechecker::visit_typeclass(ast::TypeclassAst& typeclass) {
    auto tc = std::make_unique<TypeclassFoo>(typeclass.id, typeclass.base_type, Path(this->module.path, typeclass.id));
    for (auto& m : typeclass.methods) {
        ast::FunctionType& method = *m.second;

        sem::VectorOfTypes x;
        for (auto& p: method.param_types) {
            sem::Type* args = this->make_sem_type(*p);
            x.emplace_back(args);
        }
        sem::Type* p_type = this->make_sem_type(*method.return_type);
        auto cf = std::make_unique<sem::TypeFunction>(x, sem::UType(p_type));
        tc->methods[m.first] = std::move(cf);
    }

    for (auto& m : typeclass.static_methods) {
        ast::FunctionType& method = *m.second;

        sem::VectorOfTypes x;
        for (auto& p: method.param_types) {
            sem::Type* args = this->make_sem_type(*p);
            x.emplace_back(args);
        }
        sem::Type* p_type = this->make_sem_type(*method.return_type);
        auto cf = std::make_unique<sem::TypeFunction>(x, sem::UType(p_type));
        tc->static_methods[m.first] = std::move(cf);
    }

    this->module.add_typeclass_definition(std::move(tc));
}

void ModulePrechecker::visit_instance(ast::Instance& instance) {
    auto ot = this->make_sem_type(*instance.base_type);
    Path typeclass_path(this->module.path, instance.id);
    const std::string& base_name = ot->object().actual_to_string();
    if (this->instances[base_name].contains(typeclass_path.as_str())) {
        throw std::runtime_error("typeclass already implemented!");
    }
    this->instances[base_name].insert(typeclass_path.as_str());
    LOG_INFO("precheck", "Added instance of '" + instance.id + "' to type " + base_name);
}
