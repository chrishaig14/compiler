//
// Created by chris on 11/10/21.
//

#include <log/log.h>
#include "PackagePrechecker.h"

bool PackagePrechecker::preprocess_module(Module& module) {
    ModulePrechecker gp(module, this->instances, this->all_classes);
    LOG_INFO("precheck", "Pre checking module '" + module.name + "' at path: " + module.abs_path);
    gp.visit_root();
    return gp.error_reporter.ok();
}

bool PackagePrechecker::preprocess_package(Package& package) {
    bool ok = true;
    LOG_INFO("precheck", "Pre checking package '" + package.name + "' at path: " + package.abs_path);
    for (const auto& ep: package.units) {
        Unit* uvalue = ep.second.get();
        if (uvalue->is_package()) {
            Package& subpackage = uvalue->package();
            if (not this->preprocess_package(subpackage)) {
                std::cout << E_HLT("Error global processing package ") << E_INFO(subpackage.abs_path) << std::endl;
                ok = false;
            }
        } else if (uvalue->is_module()) {
            Module& module = uvalue->module();
            if (not this->preprocess_module(module)) {
                std::cout << E_HLT("Error global processing module ") << E_INFO(module.abs_path) << std::endl;
                ok = false;
            }
        }
    }
    return ok;
}

PackagePrechecker::PackagePrechecker(std::vector<std::reference_wrapper<ConcreteClass>>& all_classes) : all_classes(all_classes) {
}
