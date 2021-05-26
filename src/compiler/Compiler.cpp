//
// Created by chris on 25/5/21.
//

#include <dirent.h>
#include "Compiler.h"
#include "../logging/logging.h"

void write_cmakelists(const std::string& cmake_output_path, const std::string& output_name, const std::string& all_files,
                      const std::string& all_libraries, bool is_lib) {
    std::string cmakelists = "cmake_minimum_required(VERSION 3.16)\n"
                             "project(xlang)\n"
                             "set(CMAKE_CXX_STANDARD 14)\n"
                             "include_directories(.)\n"
                             "set(CMAKE_CXX_FLAGS \"${CMAKE_CXX_FLAGS} -Werror -O0 -fverbose-asm -Winline\")\n";

    cmakelists += "include_directories(/home/chris/CLionProjects/compiler/lib)\n";
    if (is_lib) {
        cmakelists += "add_library(" + output_name + " " + all_files + ")\n";
    } else {
        cmakelists += "add_executable(" + output_name + " " + all_files + ")\n";
    }
    cmakelists += "target_link_directories(" + output_name + " PUBLIC /home/chris/CLionProjects/compiler/lib/build)\n";
    cmakelists += "target_link_libraries(" + output_name + " " + all_libraries + ")\n";


    std::ofstream cmakelists_file(cmake_output_path);
    cmakelists_file << cmakelists;
}

std::map<std::string, std::string> read_requirements(const std::string& filepath) {
    std::string line;
    std::map<std::string, std::string> requirements;
    std::ifstream infile(filepath);
    if (!infile.is_open()) {
        std::cerr << "No requirements file at " << filepath << std::endl;
        return requirements;
    }
    while (std::getline(infile, line)) {
        std::string op;
        std::string first;
        std::string second;
        for (char i : line) {
            if (i == '=') {
                op += '=';
            } else {
                if (op == "") {
                    first += i;
                } else {
                    second += i;
                }
            }
        }
        requirements[first] = second;
    }
    return requirements;
}

Compiler::Compiler(const std::string& project_dir, const std::string& project_output_dir, const std::string& output_name,
                   const std::string& lib_path, bool is_lib, const std::string& version)
        : project_dir(project_dir), project_output_dir(project_output_dir), output_name(output_name),
          lib_path(lib_path), is_lib(is_lib), version(version) {
    this->root_package = nullptr;
    this->top_package = nullptr;
}

void Compiler::main() {
    this->root_package = new Package(Path(this->output_name), project_dir, "", false, "", "");

    std::string req_file_path = path_join(this->project_dir, REQUIREMENTS_FILE);
    this->top_package = new Package(Path("global"), "", "", false, "", "");
    this->top_package->units[this->output_name] = Unit{.type=U_TYPE::PACKAGE, .package=root_package};

    VectorOfStrings requirements = this->load_requirements(req_file_path);

    this->load_package(*root_package, 1);
    this->parse_all_modules(*root_package);

    this->process_global_all_modules(*root_package);

    try {
        this->analyze_all_modules(*root_package, top_package);
    } catch (const std::runtime_error& e) {
        std::cout << "ERROR: " << e.what() << std::endl;
        exit(0);
    }

    this->transpile_all_modules(*root_package, project_output_dir, true);
    std::string all_files;
    for (auto f: this->all_modules) {
        if (f == "core.xl") {
            continue;
        }
        f = path_join(this->output_name, f.substr(0, f.size() - 3) + ".cpp");
        all_files += f + " ";
    }

    std::string cmake_output_path = project_output_dir + "/CMakeLists.txt";

    std::string all_libraries;
    for (const auto& req: requirements) {
        all_libraries += req + " ";
    }
    std::string final_output_name = this->output_name;
    if (is_lib) {
        final_output_name += "-" + this->version;
    }
    write_cmakelists(cmake_output_path, final_output_name, all_files, all_libraries, is_lib);
}

VectorOfStrings Compiler::load_requirements(const std::string& filepath) {
    std::cout << "Loading requirements from file " << filepath << std::endl;

    auto requirements = read_requirements(filepath);

    bool has_error = false;

    VectorOfStrings reqs;
    for (const auto& r: requirements) {
        load_library(r.first, r.second);
        reqs.push_back(r.first + "-" + r.second);
    }
    if (has_error) {
        std::cerr << "Error loading requirements" << std::endl;
        exit(1);
    }
    return reqs;
}

void Compiler::load_module(Package& package, const std::string& d_name) {
    std::string module_abs_path = path_join(package.abs_path, d_name);
    std::string module_rel_path = path_join(package.rel_path, d_name);
    if (!package.is_lib) {
        all_modules.push_back(module_rel_path);
    }
    std::string module_name = d_name.substr(0, d_name.size() - 3);
    auto* module = new Module(Path(package.path, module_name),
                              module_abs_path,
                              module_rel_path,
                              package.is_lib,
                              path_join(package.header_parent_path, module_name + ".h"));
    package.units[module_name] = Unit{.type=U_TYPE::MODULE, .module=module};
}

void Compiler::load_package(Package& package, int level) {
    std::string abs_path = package.abs_path;
    DIR* dir = opendir(abs_path.c_str());
    if (dir == nullptr) {
        std::cerr << "No such dir for package '" << package.name << "': " << "'" << abs_path << "'" << std::endl;
        return;
    }

    std::vector<std::string> modules;
    std::vector<std::string> subpackages;

    std::cout << std::string(level, '-') << " Loading package " << package.name << " at path " << package.abs_path
              << std::endl;

    dirent* ent = readdir(dir);
    while (ent != nullptr) {
        std::string d_name = ent->d_name;
        if (d_name != "." && d_name != "..") {
            unsigned char d_type = ent->d_type;
            if (d_type == DT_REG) {
                std::string ext = d_name.substr(d_name.size() - 3, 3);
                if (ext == ".xl") {
                    modules.emplace_back(d_name);
                }
            } else if (d_type == DT_DIR) {
                subpackages.emplace_back(d_name);
            }
        }
        ent = readdir(dir);
    }

    if (modules.empty() && subpackages.empty()) {
        std::cerr << "Package " << package.name << " is empty" << std::endl;
        exit(1);
    }

    for (const auto& module_name: modules) {
        std::cout << std::string(level + 1, '-') << " Found module " << module_name << std::endl;

        load_module(package, module_name);
    }

    for (const auto& subpackage_name:subpackages) {
        std::cout << std::string(level + 1, '-') << " Found subpackage " << subpackage_name << std::endl;

        std::string subpackage_abs_path = path_join(package.abs_path, subpackage_name);
        std::string subpackage_rel_path = path_join(package.rel_path, subpackage_name);
        const std::string& subpackage_header_parent_path = path_join(package.header_parent_path, subpackage_name);
        const std::string& subpackage_full_header_path = path_join(subpackage_header_parent_path, "__package__.h");

        auto* subpackage = new Package(Path(package.path, subpackage_name),
                                       subpackage_abs_path,
                                       subpackage_rel_path,
                                       package.is_lib,
                                       subpackage_full_header_path,
                                       subpackage_header_parent_path);
        load_package(*subpackage, level + 1);
        package.units[subpackage_name] = Unit{.type=U_TYPE::PACKAGE, .package=subpackage};
    }
}

void Compiler::load_library(const std::string& name, const std::string& version) {
    std::string lib_rel_out_path = path_join(path_join(path_join(name, version), "out"), name);
    std::string lib_rel_top_unit_path = path_join(path_join(name, version), "src");
    std::string abs_top_unit_path = path_join(lib_path, lib_rel_top_unit_path);
    if (loaded_top_units.count(lib_rel_top_unit_path) != 0) {
        // skip, already loaded
        return;
    }
    DIR* dir = opendir(abs_top_unit_path.c_str());
    if (dir == nullptr) {
        std::cout << "Top unit " << name + "==" + version << " NOT FOUND" << std::endl;
        return;
    }
    std::cout << "Loading top unit: " << E_HLT(name) << " at path: " << E_HLT(abs_top_unit_path) << std::endl;

    std::string library_requirements_file = path_join(abs_top_unit_path, REQUIREMENTS_FILE);
    load_requirements(library_requirements_file);

    auto* library_top_package = new Package(Path(name),
                                            abs_top_unit_path,
                                            lib_rel_top_unit_path,
                                            true,
                                            path_join(lib_rel_out_path, "__package__.h"),
                                            lib_rel_out_path);
    load_package(*library_top_package, 1);
    this->top_package_name = library_top_package->name;
    parse_all_modules(*library_top_package);
    process_global_all_modules(*library_top_package);
    top_package->units[name] = Unit{.type=U_TYPE::PACKAGE, .package=library_top_package};
    std::cout << "Finished loading top unit: " << E_HLT(lib_rel_top_unit_path) << std::endl;
    this->loaded_top_units[lib_rel_top_unit_path] = true;
}

void Compiler::load_top_unit(const std::string& name, const std::string& version, bool is_lib) {
    std::string lib_rel_top_unit_path = path_join(name, version);
    std::string abs_top_unit_path = path_join(lib_path, lib_rel_top_unit_path);
    if (loaded_top_units.count(lib_rel_top_unit_path) != 0) {
        // skip, already loaded
        return;
    }
    DIR* dir = opendir(abs_top_unit_path.c_str());
    if (dir == nullptr) {
        std::cout << "Top unit " << name + "==" + version << " NOT FOUND" << std::endl;
        return;
    }
    std::cout << "Loading top unit: " << E_HLT(name) << " at path: " << E_HLT(abs_top_unit_path) << std::endl;
    std::string unit_requirements_file = path_join(abs_top_unit_path, REQUIREMENTS_FILE);
    auto requirements = read_requirements(unit_requirements_file);
    for (const auto& req: requirements) {
        load_top_unit(req.first, req.second, true);
    }

    auto* top_unit_package = new Package(Path(name),
                                         abs_top_unit_path,
                                         lib_rel_top_unit_path,
                                         is_lib,
                                         path_join(path_join(path_join(lib_rel_top_unit_path, "out"), name),
                                                   "__package__.h"),
                                         path_join(path_join(lib_rel_top_unit_path, "out"), name));
    load_package(*top_unit_package, 1);
    this->top_package_name = top_unit_package->name;
    parse_all_modules(*top_unit_package);
    process_global_all_modules(*top_unit_package);
    top_package->units[name] = Unit{.type=U_TYPE::PACKAGE, .package=top_unit_package};
    std::cout << "Finished loading top unit: " << E_HLT(lib_rel_top_unit_path) << std::endl;
    this->loaded_top_units[lib_rel_top_unit_path] = true;
}
