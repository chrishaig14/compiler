//
// Created by chris on 11/10/21.
//

#include "PackageChecker.h"

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

void add_path_to_module(Module& module, Path path, Package& top_package) {
    auto current_member = find(path, top_package);
    module.members[path.basname()] = std::move(current_member);
}

void resolve_module_imports(Module& module, Package& top_package) {
    for (const auto& path: module.imported_paths_no_alias_v) {
        add_path_to_module(module, path.second, top_package);
    }
}

bool PackageChecker::check_module(Module& module) {
    resolve_module_imports(module, this->top_package);
    ModuleChecker checker(this->top_package, module, this->instances);
    module.sast = checker.check_module();
    return checker.error_reporter.ok();
}

bool PackageChecker::check_unit(Unit& uvalue) {
    bool ok = true;
    if (uvalue.is_package()) {
        Package& subpackage = uvalue.package();
        if (not this->check_package(subpackage)) {
            ok = false;
            std::cout << E_HLT("Failed checking package ") << E_INFO(subpackage.name) << std::endl;
        }
    } else if (uvalue.is_module()) {
        Module& module = uvalue.module();
        if (not this->check_module(module)) {
            std::cout << E_HLT("Failed checking module ") << E_INFO(module.name) << std::endl;
            ok = false;
        }
    }
    return ok;
}

bool PackageChecker::check_package(Package& package) {
    bool ok = true;
    for (const auto& ep: package.units) {
        if (not this->check_unit(*ep.second)) {
            ok = false;
        }
    }
    return ok;
}

PackageChecker::PackageChecker(Package& top_package, std::map<std::string, std::string>& instances) : instances(instances), top_package(
        top_package) {
}
