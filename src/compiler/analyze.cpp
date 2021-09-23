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
    return not checker.error_reporter.failed;
}

bool check_package(Package& package, Package& top_package) {
    // std::cout << "Analyzing package " << package->name << std::endl;
    bool ok = true;
    for (const auto& ep: package.units) {
        Unit* uvalue = ep.second;
        if (uvalue->is_package()) {
            Package& subpackage = uvalue->package();
            if (not check_package(subpackage, top_package)) {
                ok = false;
            }
        } else if (uvalue->is_module()) {
            Module& module = uvalue->module();
            if (not check_module(module, top_package)) {
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

void add_local_path_to_module(Module& module, Path path, Package& top_package) {
    auto current_member = ModuleMember{.type=ModuleMemberType::PACKAGE, .package=&top_package};
    std::string path_so_far = "global";
    ModuleMember last_member;

    for (const auto& path_part: path.as_vec()) {
        if (current_member.type == ModuleMemberType::PACKAGE) {
            Package* package = current_member.package;
            auto unit = package->units.find(path_part);
            if (unit == package->units.end()) {
                throw std::runtime_error("Error '" + path_part + "' not found in package '" + path_so_far + "'");
            }
            current_member = map_unit_to_module_member(*unit->second);
            last_member = current_member;
        } else if (current_member.type == ModuleMemberType::MODULE) {
            auto member = current_member.module->members.find(path_part);
            if (member == current_member.module->members.end()) {
                throw std::runtime_error("Error '" + path_part + "' not found in module '" + path_so_far + "'");
            }
            current_member = member->second;
        }
        path_so_far += "." + path_part;
    }
    module.members[path.as_vec().back()] = current_member;
}

bool preprocess_module(Module& module) {
    GlobalProcessor gp(module);
    std::cout << "Global-processing module " << module.name << " at path: " << module.abs_path << std::endl;
    try {
        gp.visit_root();
    } catch (std::runtime_error& e) {
        std::cout << "Error global processing module " << module.abs_path << " : " << e.what() << std::endl;
        return false;
    }
    return true;
}

bool preprocess_package(Package& package) {
    bool ok = true;
    for (const auto& ep: package.units) {
        Unit* uvalue = ep.second;
        if (uvalue->is_package()) {
            Package& subpackage = uvalue->package();
            if (not preprocess_package(subpackage)) {
                ok = false;
            }
        } else if (uvalue->is_module()) {
            Module& module = uvalue->module();
            if (not preprocess_module(module)) {
                ok = false;
            }
        }
    }
    return ok;
}
