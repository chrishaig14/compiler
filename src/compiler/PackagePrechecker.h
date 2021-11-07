//
// Created by chris on 11/10/21.
//

#ifndef XLANG_PACKAGEPRECHECKER_H
#define XLANG_PACKAGEPRECHECKER_H

#include <string>
#include <fstream>
#include <units/infos/Package.h>
#include <units/infos/Module.h>
#include <util/utils.h>
#include <semantic/ModulePrechecker.h>

class PackagePrechecker {
public:
    std::map<std::string, std::set<std::string>> instances;
    std::vector<std::reference_wrapper<ConcreteClass>>& all_classes;

    PackagePrechecker(std::vector<std::reference_wrapper<ConcreteClass>>& all_classes);

    bool preprocess_module(Module& module);

    bool preprocess_package(Package& package);
};


#endif //XLANG_PACKAGEPRECHECKER_H
