//
// Created by chris on 25/5/21.
//

#include "Compiler.h"
#include "../semantic/Checker.h"

void analyze_module(Module& module, Package& top_package) {
    for (const auto& path: module.imported_paths_no_alias_v) {
        add_path_to_module(module, path.second, top_package);
    }
    // std::vector<Path> default_imports = {Path("core.core.String")};
    // for (const auto& path:default_imports) {
    //     this->add_path_to_module(module, path, top_package);
    // }
    for (const auto& i: module.imported_paths_with_alias_v) {
        add_path_with_alias_to_module(module, i.first, i.second, top_package);
    }
    Checker checker(top_package, module);
    USemanticInfoBlock check_info = checker.visit_root(*module.ast);
    module.sast = std::move(check_info->snode);
    if (checker.error_reporter.failed) {
        // global_fail = true;
        throw std::runtime_error("Semantic analysis failed for module " + module.abs_path);
    }
}

void analyze_module_result(Module& module, Package& top_package) {
    for (const auto& path: module.imported_paths_no_alias_v) {
        add_path_to_module(module, path.second, top_package);
    }
    // std::vector<Path> default_imports = {Path("core.core.String")};
    // for (const auto& path:default_imports) {
    //     this->add_path_to_module(module, path, top_package);
    // }
    for (const auto& i: module.imported_paths_with_alias_v) {
        add_path_with_alias_to_module(module, i.first, i.second, top_package);
    }
}

void analyze_all_modules(Package& package, Package& top_package) {
    // std::cout << "Analyzing package " << package->name << std::endl;
    for (const auto& ep: package.units) {
        if (ep.second.type == U_TYPE::PACKAGE) {
            Package& subpackage = *ep.second.package;
            analyze_all_modules(subpackage, top_package);
        } else if (ep.second.type == U_TYPE::MODULE) {
            Module& module = *ep.second.module;
            analyze_module(module, top_package);
        }
    }
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
    auto current_flirpin = Flirpin{.type=F_TYPE::PACKAGE, .package=&top_package};
    std::string path_so_far = "global";
    Flirpin last_flirpin;

    for (const auto& path_part: path.as_vec()) {
        if (current_flirpin.type == F_TYPE::PACKAGE) {
            Package* package = current_flirpin.package;
            auto unit = package->units.find(path_part);
            if (unit == package->units.end()) {
                throw std::runtime_error("Error '" + path_part + "' not found in package '" + path_so_far + "'");
            }
            current_flirpin = map_unit_to_flirpin(unit->second);
            last_flirpin = current_flirpin;
        } else if (current_flirpin.type == F_TYPE::MODULE) {
            auto flirpin = current_flirpin.module->flirpins.find(path_part);
            if (flirpin == current_flirpin.module->flirpins.end()) {
                throw std::runtime_error("Error '" + path_part + "' not found in module '" + path_so_far + "'");
            }
            current_flirpin = flirpin->second;
        }
        path_so_far += "." + path_part;
    }

    std::string included_header;
    if (last_flirpin.type == F_TYPE::PACKAGE) {
        included_header = last_flirpin.package->full_header_path;
        module.included_module_paths[last_flirpin.package->name] = included_header;
    } else {
        included_header = last_flirpin.module->full_header_path;
        module.included_module_paths[last_flirpin.module->name] = included_header;
    }
    module.flirpins[path.as_vec().back()] = current_flirpin;
}

void Compiler::process_global_all_modules(Package& package) {
    for (const auto& ep: package.units) {
        if (ep.second.type == U_TYPE::PACKAGE) {
            Package* subpackage = ep.second.package;
            process_global_all_modules(*subpackage);
        } else if (ep.second.type == U_TYPE::MODULE) {
            Module* module = ep.second.module;
            GlobalProcessor gp(*module);
            std::cout << "Global-processing module " << module->name << " at path: " << module->abs_path << std::endl;
            gp.visit_root();
        }
    }
}
