//
// Created by chris on 25/5/21.
//

#include <dirent.h>
#include "Compiler.h"
#include <logging/logging.h>
#include "utils.h"
#include "PackagePrechecker.h"
#include "Loader.h"
#include "PackageChecker.h"

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
    Loader l;
    l.load_package(root_package, 1);

    if (not parse_package(root_package)) {
        throw std::runtime_error("Parse Error");
    }
    PackagePrechecker pp;
    if (not pp.preprocess_package(root_package)) {
        throw std::runtime_error("Preprocess Error");
    }
    this->instances = pp.instances;
    return true;
}

bool Compiler::main() {
    PackageChecker pc(top_package, this->instances);
    return pc.check_package(root_package);
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


void Compiler::load_library(const std::string& name, const std::string& lib_version) {
    std::string lib_rel_out_path = path_join(path_join(path_join(name, lib_version), "out"), name);
    std::string lib_rel_top_unit_path = path_join(path_join(name, lib_version), "src");
    std::string abs_top_unit_path = path_join(lib_path, lib_rel_top_unit_path);
    if (loaded_top_units.count(lib_rel_top_unit_path) != 0) {
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

    auto library_top_package = std::make_unique<Package>(Path(name), abs_top_unit_path, true);
    Loader l;
    l.load_package(*library_top_package, 1);
    parse_package(*library_top_package);
    PackagePrechecker pp;
    pp.preprocess_package(*library_top_package);
    top_package.units[name] = std::make_unique<SubpackageUnit>(library_top_package.get());
    top_package.subpackages.push_back(std::move(library_top_package));
    std::cout << "Finished loading top unit: " << E_INFO(lib_rel_top_unit_path) << std::endl;
    this->loaded_top_units[lib_rel_top_unit_path] = true;
}

void Compiler::load_top_unit(const std::string& name, const std::string& m_version, bool m_is_lib) {
    std::string lib_rel_top_unit_path = path_join(name, m_version);
    std::string abs_top_unit_path = path_join(lib_path, lib_rel_top_unit_path);
    if (loaded_top_units.count(lib_rel_top_unit_path) != 0) {
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
    Loader l;
    l.load_package(*top_unit_package, 1);
    parse_package(*top_unit_package);
    PackagePrechecker pp;
    pp.preprocess_package(*top_unit_package);
    top_package.units[name] = std::make_unique<SubpackageUnit>(top_unit_package);
    std::cout << "Finished loading top unit: " << E_INFO(lib_rel_top_unit_path) << std::endl;
    this->loaded_top_units[lib_rel_top_unit_path] = true;
}

void Compiler::load_project() {
    std::string req_file_path = path_join(this->project_dir, REQUIREMENTS_FILE);
    VectorOfStrings requirements = this->load_requirements(req_file_path);

    Loader l;
    l.load_package(root_package, 1);
    parse_package(root_package);
    PackagePrechecker pp;
    bool global_ok = pp.preprocess_package(root_package);
    if (!global_ok) {
        this->ok = false;
        return;
    }
    PackageChecker pc(top_package, this->instances);
    bool check_ok = pc.check_package(root_package);
    if (!check_ok) {
        this->ok = false;
        return;
    }
}