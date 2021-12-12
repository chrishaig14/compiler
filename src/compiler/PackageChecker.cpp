//
// Created by chris on 11/10/21.
//

#include "PackageChecker.h"

bool PackageChecker::check_module(Module& module) {
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

PackageChecker::PackageChecker(Package& top_package, std::map<std::string, std::set<std::string>>& instances)
        : instances(instances), top_package(top_package) {
}
