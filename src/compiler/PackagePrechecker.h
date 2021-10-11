//
// Created by chris on 11/10/21.
//

#ifndef XLANG_PACKAGEPRECHECKER_H
#define XLANG_PACKAGEPRECHECKER_H

#include <string>
#include <fstream>
#include "../units/infos/Package.h"
#include "../units/infos/Module.h"
#include "../util/utils.h"
#include "../semantic/ModulePrechecker.h"

class PackagePrechecker {
public:
    bool preprocess_module(Module& module) {
        ModulePrechecker gp(module);
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

};


#endif //XLANG_PACKAGEPRECHECKER_H
