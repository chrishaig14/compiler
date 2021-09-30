//
// Created by chris on 25/5/21.
//

#include "Compiler.h"
#include "../semantic/Checker.h"


void resolve_module_imports(Module& module, Package& top_package) {
    for (const auto& path: module.imported_paths_no_alias_v) {
        add_path_to_module(module, path.second, top_package);
    }
    for (const auto& i: module.imported_paths_with_alias_v) {
        add_path_with_alias_to_module(module, i.first, i.second, top_package);
    }
}

bool check_module(Module& module, Package& top_package) {
    resolve_module_imports(module, top_package);
    Checker checker(top_package, module);
    module.sast = checker.visit_root(*module.ast);
    return checker.error_reporter.ok();
}

bool check_package(Package& package, Package& top_package) {
    // std::cout << "Analyzing package " << package->name << std::endl;
    bool ok = true;
    for (const auto& ep: package.units) {
        Unit* uvalue = ep.second.get();
        if (uvalue->is_package()) {
            Package& subpackage = uvalue->package();
            if (not check_package(subpackage, top_package)) {
                ok = false;
                std::cout << E_HLT("Failed checking package ") << E_INFO(subpackage.name) << std::endl;
            }
        } else if (uvalue->is_module()) {
            Module& module = uvalue->module();
            if (not check_module(module, top_package)) {
                std::cout << E_HLT("Failed checking module ") << E_INFO(module.name) << std::endl;
                ok = false;
            }
        }
    }
    return ok;
}

void add_path_with_alias_to_module(Module& module, const std::string& alias, Path path, Package& root_package) {
    // auto current_flirpin = Flirpin{.type=F_TYPE::PACKAGE, .package=root_package};
    // std::string path_so_far;
    // std::string last_include;
    // std::string lib_path = "lib";
    // for (const auto& path_part: path.as_vec()) {
    //     if (current_flirpin.type == F_TYPE::PACKAGE) {
    //         Package* package = current_flirpin.package;
    //         auto unit = package->units.find(path_part);
    //         if (unit == package->units.end()) {
    //             throw std::runtime_error("Error '" + path_part + "' not found in package " + path_so_far);
    //         }
    //         if (unit->second.type == U_TYPE::MODULE) {
    //             if (unit->second.module->is_lib) {
    //                 last_include = path_join(lib_path, unit->second.module->rel_path);
    //             } else {
    //                 last_include = unit->second.module->rel_path;
    //             }
    //         } else {
    //             if (unit->second.package->is_lib) {
    //                 last_include = path_join(lib_path, unit->second.package->rel_path);
    //             } else {
    //                 last_include = unit->second.package->rel_path;
    //             }
    //         }
    //         current_flirpin = map_unit_to_flirpin(unit->second);
    //     } else if (current_flirpin.type == F_TYPE::MODULE) {
    //         auto flirpin = current_flirpin.module->flirpins.find(path_part);
    //         if (flirpin == current_flirpin.module->flirpins.end()) {
    //             throw std::runtime_error("Error '" + path_part + "' not found in module " + path_so_far);
    //         }
    //         current_flirpin = flirpin->second;
    //     }
    //     path_so_far += "." + path_part;
    // }
    // if (current_flirpin.type == F_TYPE::CLASS) {
    //     throw std::runtime_error(
    //             "Cannot import class " + current_flirpin.clazz->path.as_str() + " aliased with " + alias);
    // }
    // std::string included_module_header_basename = last_include.substr(0, last_include.size() - 3);
    // std::string included_module_name = current_flirpin.module->name;
    // module.included_module_paths[included_module_name] = included_module_header_basename + ".h";
    // module.flirpins[alias] = current_flirpin;
}

void add_path_to_module(Module& module, Path path, Package& top_package) {
    VectorOfStrings pv = path.as_vec();
    if (pv[0] == "root") {
        VectorOfStrings path_vec = VectorOfStrings(pv.begin(), pv.end());
        add_local_path_to_module(module, Path(path_vec), top_package);
    } else {
        VectorOfStrings path_vec = VectorOfStrings(pv.begin(), pv.end());
        add_local_path_to_module(module, Path(path_vec), top_package);
        // add_global_path_to_module(module, path);
    }
}

void Compiler::add_global_path_to_module(Module& module, Path path) {

}

std::unique_ptr<ModuleMember> find(Path path, Package& top_package) {
    std::unique_ptr<ModuleMember> current_member = std::make_unique<PackageModuleMember>(&top_package);
    std::string path_so_far = "global";

    for (const auto& path_part: path.as_vec()) {
        if (current_member->is_package()) {
            Package& package = current_member->package();
            auto unit = package.units.find(path_part);
            if (unit == package.units.end()) {
                throw std::runtime_error("Error '" + path_part + "' not found in package '" + path_so_far + "'");
            }
            current_member = std::unique_ptr<ModuleMember>(map_unit_to_module_member(*unit->second));
            // last_member = current_member;
        } else if (current_member->is_module()) {
            Module& module_ = current_member->module();
            auto member = module_.members.find(path_part);
            if (member == module_.members.end()) {
                throw std::runtime_error("Error '" + path_part + "' not found in module '" + path_so_far + "'");
            }
            current_member = member->second.get()->clone();
        }
        path_so_far += "." + path_part;
    }
    return current_member;
}

void add_local_path_to_module(Module& module, Path path, Package& top_package) {
    auto current_member = find(path, top_package);
    module.members[path.as_vec().back()] = std::move(current_member);
}

bool preprocess_module(Module& module) {
    GlobalProcessor gp(module);
    std::cout << "** Global-processing module " << E_INFO(module.name) << " at path: " << E_INFO(module.abs_path)
              << std::endl;
    gp.visit_root();
    return gp.error_reporter.ok();
}

bool preprocess_package(Package& package) {
    bool ok = true;
    std::cout << "* Global-processing package " << E_INFO(package.name) << " at path: " << E_INFO(package.abs_path)
              << std::endl;
    for (const auto& ep: package.units) {
        Unit* uvalue = ep.second.get();
        if (uvalue->is_package()) {
            Package& subpackage = uvalue->package();
            if (not preprocess_package(subpackage)) {
                std::cout << E_HLT("Error global processing package ") << E_INFO(subpackage.abs_path) << std::endl;
                ok = false;
            }
        } else if (uvalue->is_module()) {
            Module& module = uvalue->module();
            if (not preprocess_module(module)) {
                std::cout << E_HLT("Error global processing module ") << E_INFO(module.abs_path) << std::endl;
                ok = false;
            }
        }
    }
    return ok;
}
