//
// Created by chris on 11/10/21.
//

#ifndef XLANG_LOADER_H
#define XLANG_LOADER_H

#include <string>
#include <fstream>
#include "../units/infos/Package.h"
#include "../units/infos/Module.h"
#include "../util/utils.h"
#include <dirent.h>
#include "../logging/logging.h"
#include "utils.h"

class Loader {
public:
    void load_package(Package& package, int level) {
        std::string abs_path = package.abs_path;
        DIR* dir = opendir(abs_path.c_str());
        if (dir == nullptr) {
            std::cerr << "No such dir for package '" << package.name << "': " << "'" << abs_path << "'" << std::endl;
            return;
        }

        std::vector<std::string> modules;
        std::vector<std::string> subpackages;

        std::cout << std::string(level, '-') << " Loading package " << E_INFO(package.name) << " at path "
                  << E_INFO(package.abs_path) << std::endl;

        dirent* ent = readdir(dir);
        while (ent != nullptr) {
            std::string d_name = ent->d_name;
            if (d_name != "." && d_name != "..") {
                unsigned char d_type = ent->d_type;
                if (d_type == DT_REG) {
                    std::string ext = d_name.substr(d_name.size() - 3, 3);
                    if (ext == ".xl") {
                        std::string module_name = d_name.substr(0, d_name.size() - 3);
                        std::cout << std::string(level + 1, '-') << " Found module " << E_INFO(module_name)
                                  << std::endl;
                        load_module(package, module_name);
                        modules.emplace_back(module_name);
                    }
                } else if (d_type == DT_DIR) {
                    subpackages.emplace_back(d_name);
                }
            }
            ent = readdir(dir);
        }
        closedir(dir);

        if (modules.empty() && subpackages.empty()) {
            std::cerr << "Package " << package.name << " is empty" << std::endl;
            exit(1);
        }

        for (const auto& subpackage_name:subpackages) {
            std::cout << std::string(level + 1, '-') << " Found subpackage " << subpackage_name << std::endl;

            std::string subpackage_abs_path = path_join(package.abs_path, subpackage_name);
            std::string subpackage_rel_path = path_join(package.rel_path, subpackage_name);

            auto* subpackage = new Package(Path(package.path, subpackage_name), subpackage_abs_path, package.is_lib);
            load_package(*subpackage, level + 1);
            package.subpackages.push_back(std::unique_ptr<Package>(subpackage));
            package.units[subpackage_name] = std::make_unique<SubpackageUnit>(subpackage);
        }
    }


    void load_module(Package& package, const std::string& module_name) {
        std::string module_abs_path = path_join(package.abs_path, module_name + ".xl");
        std::string module_rel_path = path_join(package.rel_path, module_name);
        auto module = std::make_unique<Module>(Path(package.path, module_name), module_abs_path, package.is_lib);
        package.units[module_name] = std::make_unique<ModuleUnit>(module.get());
        package.modules.push_back(std::move(module));
    }
};


#endif //XLANG_LOADER_H
