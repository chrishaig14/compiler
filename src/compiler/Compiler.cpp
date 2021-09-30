//
// Created by chris on 25/5/21.
//

#include <dirent.h>
#include "Compiler.h"
#include "../logging/logging.h"
#include "utils.h"
#include "../transpiler/PythonTranspiler.h"

Compiler::Compiler(const std::string& project_dir, const std::string& project_output_dir,
                   const std::string& output_name, const std::string& lib_path, bool is_lib, const std::string& version)
        : project_dir(project_dir), project_output_dir(project_output_dir), output_name(output_name),
          lib_path(lib_path), is_lib(is_lib), version(version),
          root_package(Path(this->output_name), project_dir, false), top_package(Path("global"), "", false) {
    this->top_package.units[this->output_name] = std::make_unique<SubpackageUnit>(&root_package);
}

bool Compiler::pre() {
    std::string req_file_path = path_join(this->project_dir, REQUIREMENTS_FILE);

    VectorOfStrings requirements = this->load_requirements(req_file_path);

    load_package(root_package, 1);

    if (not parse_package(root_package)) {
        throw std::runtime_error("Parse Error");
    }
    // for (auto& p: root_package.units) {
    //     Unit* uvalue = p.second;
    //     if (uvalue->is_module()) {
    //         // std::cout << p.second->module() << std::endl;
    //         Module& m = uvalue->module();
    //         std::cout << "Hello" << m.ast.get() << std::endl;
    //         assert(m.ast.get() != nullptr);
    //     }
    // }
    return preprocess_package(root_package);
}

bool Compiler::main() {
    return check_package(root_package, top_package);
}

VectorOfStrings Compiler::load_requirements(const std::string& filepath) {
    std::cout << "Loading requirements from file " << E_INFO(filepath) << std::endl;

    auto requirements = read_requirements(filepath);

    bool has_error = false;

    VectorOfStrings reqs;
    for (const auto& r: requirements) {
        this->load_library(r.package, r.version);
        reqs.push_back(r.package + "-" + r.version);
    }
    if (has_error) {
        std::cerr << "Error loading requirements" << std::endl;
        exit(1);
    }
    return reqs;
}

void load_module(Package& package, const std::string& module_name) {
    std::string module_abs_path = path_join(package.abs_path, module_name + ".xl");
    std::string module_rel_path = path_join(package.rel_path, module_name);
    // if (!package.is_lib) {
    //     all_modules.push_back(module_rel_path);
    // }
    auto module = std::make_unique<Module>(Path(package.path, module_name), module_abs_path, package.is_lib);
    // this->my_modules.push_back(std::unique_ptr<Module>(module));
    package.units[module_name] = std::make_unique<ModuleUnit>(module.get());
    package.modules.push_back(std::move(module));
}

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
                    std::cout << std::string(level + 1, '-') << " Found module " << E_INFO(module_name) << std::endl;
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

void Compiler::load_library(const std::string& name, const std::string& lib_version) {
    std::string lib_rel_out_path = path_join(path_join(path_join(name, lib_version), "out"), name);
    std::string lib_rel_top_unit_path = path_join(path_join(name, lib_version), "src");
    std::string abs_top_unit_path = path_join(lib_path, lib_rel_top_unit_path);
    if (loaded_top_units.count(lib_rel_top_unit_path) != 0) {
        // skip, already loaded
        return;
    }
    DIR* dir = opendir(abs_top_unit_path.c_str());
    if (dir == nullptr) {
        std::cout << "Top unit " << name + "==" + lib_version << " NOT FOUND" << std::endl;
        return;
    }
    closedir(dir);
    std::cout << "Loading top unit: " << E_INFO(name) << " at path: " << E_INFO(abs_top_unit_path) << std::endl;

    std::string library_requirements_file = path_join(abs_top_unit_path, REQUIREMENTS_FILE);
    load_requirements(library_requirements_file);

    auto* library_top_package = new Package(Path(name), abs_top_unit_path, true);
    load_package(*library_top_package, 1);
    parse_package(*library_top_package);
    preprocess_package(*library_top_package);
    top_package.units[name] = std::make_unique<SubpackageUnit>(library_top_package);
    std::cout << "Finished loading top unit: " << E_INFO(lib_rel_top_unit_path) << std::endl;
    this->loaded_top_units[lib_rel_top_unit_path] = true;
}

void Compiler::load_top_unit(const std::string& name, const std::string& m_version, bool m_is_lib) {
    std::string lib_rel_top_unit_path = path_join(name, m_version);
    std::string abs_top_unit_path = path_join(lib_path, lib_rel_top_unit_path);
    if (loaded_top_units.count(lib_rel_top_unit_path) != 0) {
        // skip, already loaded
        return;
    }
    DIR* dir = opendir(abs_top_unit_path.c_str());
    if (dir == nullptr) {
        std::cout << "Top unit " << name + "==" + m_version << " NOT FOUND" << std::endl;
        return;
    }
    std::cout << "Loading top unit: " << E_INFO(name) << " at path: " << E_INFO(abs_top_unit_path) << std::endl;
    std::string unit_requirements_file = path_join(abs_top_unit_path, REQUIREMENTS_FILE);
    auto requirements = read_requirements(unit_requirements_file);
    for (const auto& req: requirements) {
        load_top_unit(req.package, req.version, true);
    }

    auto* top_unit_package = new Package(Path(name), abs_top_unit_path, m_is_lib);
    load_package(*top_unit_package, 1);
    parse_package(*top_unit_package);
    preprocess_package(*top_unit_package);
    top_package.units[name] = std::make_unique<SubpackageUnit>(top_unit_package);
    std::cout << "Finished loading top unit: " << E_INFO(lib_rel_top_unit_path) << std::endl;
    this->loaded_top_units[lib_rel_top_unit_path] = true;
}
