#include <sstream>
#include <dirent.h>
#include "../scanner/Scanner.h"
#include "../parser/Parser.h"
#include "../semantic/GlobalProcessor.h"
#include "../semantic/Checker.h"
#include "../transpiler/STranspiler.h"
#include "utils.h"
#include "parse.h"
#include "analyze.h"
#include "transpile.h"
#include <unistd.h>

#define REQUIREMENTS_FILE "requirements.txt"
static std::string lib_path;
static bool global_fail = false;
static std::map<std::string, std::string> function_builtins;
static std::map<std::string, std::map<std::string, std::string>> class_builtins;
static Package* root_package;
static std::string static_initializations;
static std::string static_cleanups;

static Package* top_package;


VectorOfStrings all_modules;

void load_package(Package* package, int level) {
    DIR* dir = opendir(package->abs_path.c_str());
    if (dir == nullptr) {
        std::cerr << "No such dir for package '" << package->name << "': " << "'" << package->abs_path << "'"
                  << std::endl;
        return;
    }
    dirent* ent = readdir(dir);
    while (ent != nullptr) {
        std::string d_name = ent->d_name;
        if (d_name != "." && d_name != "..") {
            if (ent->d_type == DT_REG) {
                std::string file_extension = d_name.substr(d_name.size() - 3, 3);
                if (file_extension == ".xl") {
                    std::string module_abs_path = path_join(package->abs_path, d_name);
                    std::string module_rel_path = path_join(package->rel_path, d_name);
                    all_modules.push_back(module_rel_path);
                    std::string module_name = d_name.substr(0, d_name.size() - 3);
                    auto* module = new Module(Path(package->path, module_name), module_abs_path, module_rel_path);
                    package->units[module_name] = Unit{.type=U_TYPE::MODULE, .module=module};
                } else {
                    // std::cout << std::string(level + 1, '\t') << "OTHER: " << d_name << std::endl;
                }
            } else if (ent->d_type == DT_DIR) {
                std::string subpackage_abs_path = path_join(package->abs_path, d_name);
                std::string subpackage_rel_path = path_join(package->rel_path, d_name);
                const std::string& subpackage_name = d_name;
                auto* subpackage = new Package(Path(package->path, d_name), subpackage_abs_path, subpackage_rel_path);
                load_package(subpackage, level + 1);
                package->units[subpackage_name] = Unit{.type=U_TYPE::PACKAGE, .package=subpackage};
            }
        }
        ent = readdir(dir);
    }
}


void process_global_all_modules(Package* package) {
    for (const auto& ep: package->units) {
        if (ep.second.type == U_TYPE::PACKAGE) {
            Package* subpackage = ep.second.package;
            process_global_all_modules(subpackage);
        } else if (ep.second.type == U_TYPE::MODULE) {
            Module* module = ep.second.module;
            GlobalProcessor gp;
            gp.module = module;
            gp.__file__ = module->abs_path;
            std::cout << "Global-processing module " << module->name << " at path: " << module->abs_path << std::endl;
            gp.visit_root(*module->ast);
        }
    }
}

VectorOfStrings make_path(const std::string& s) {
    VectorOfStrings path;
    path.push_back("");
    for (char c : s) {
        if (c != '.') {
            path.back() += c;
        } else {
            path.push_back("");
        }
    }
    return path;
}


std::string project_output_dir;

std::map<std::string, bool> loaded_top_units;

void load_top_unit(const std::string& name, const std::string& top_unit_path) {
    if (loaded_top_units.count(top_unit_path) != 0) {
        // skip, already loaded
        return;
    }
    std::cout << "Loading top unit: " << E_HLT(name) << " at path: " << E_HLT(top_unit_path) << std::endl;
    std::string unit_requirements_file = path_join(top_unit_path, REQUIREMENTS_FILE);
    auto requirements = read_requirements(unit_requirements_file);
    for (const auto& req: requirements) {
        std::string req_top_unit_path = path_join(path_join(lib_path, req.first), req.second);
        load_top_unit(req.first, req_top_unit_path);
    }

    auto* top_unit_package = new Package(Path(name), path_join(top_unit_path, "src"), "");
    load_package(top_unit_package, 0);
    parse_all_modules(*top_unit_package);
    process_global_all_modules(top_unit_package);
    top_package->units[name] = Unit{.type=U_TYPE::PACKAGE, .package=top_unit_package};
    std::cout << "Finished loading top unit: " << E_HLT(top_unit_path) << std::endl;
    loaded_top_units[top_unit_path] = true;
}

void load_requirements(const std::string& filepath) {
    auto requirements = read_requirements(filepath);

    bool has_error = false;
    for (const auto& r: requirements) {
        std::string final_path = path_join(path_join(lib_path, r.first), r.second);
        DIR* dir = opendir(final_path.c_str());
        if (dir == nullptr) {
            std::cout << "REQUIREMENT " << r.first + "==" + r.second << " NOT FOUND" << std::endl;
            has_error = true;
            continue;
        }
        load_top_unit(r.first, final_path);
        // Package* req_package = new Package(r.first, final_path, "");
        // load_package(req_package, 0);
        // std::cout << "LOADED REQUIREMENT " << r.first + "==" + r.second << std::endl;
    }
    if (has_error) {
        std::cerr << "Error loading requirements" << std::endl;
        exit(1);
    }
}

std::string global_name;

void write_cmakelists(const std::string& all_files) {
    std::string cmakelists = "cmake_minimum_required(VERSION 3.16)\n"
                             "project(xlang)\n"
                             "set(CMAKE_CXX_STANDARD 14)\n"
                             "include_directories(.)\n"
                             "set(CMAKE_CXX_FLAGS \"${CMAKE_CXX_FLAGS} -Werror -O0 -fverbose-asm -Winline\")\n";
    // cmakelists += "add_executable(result " + all_files + ")\n";
    cmakelists += "include_directories(/home/chris/CLionProjects/compiler/lib/core/1.0.0/out)\n";
    cmakelists += "add_library(result " + all_files + ")\n";
    cmakelists += "target_link_libraries(result core)\n";


    std::string cmake_output_path = project_output_dir + "/CMakeLists.txt";
    std::ofstream cmakelists_file(cmake_output_path);
    cmakelists_file << cmakelists;
}

void write_top_cmakelists(char* const* argv) {
    std::string top_level_cmake = "cmake_minimum_required(VERSION 3.16)\n"
                                  "project(xlang)\n"
                                  "set(CMAKE_CXX_STANDARD 14)\n"
                                  "set(CMAKE_CXX_FLAGS \"${CMAKE_CXX_FLAGS} -Werror -O0 -fverbose-asm -Winline\")\n"
                                  "include_directories(/home/chris/CLionProjects/compiler/runtime)\n"
                                  "link_directories(/home/chris/CLionProjects/compiler/runtime/build)\n"
                                  "add_subdirectory(runtime)\n"
                                  "add_subdirectory(application)\n"
                                  "target_link_libraries(result core)\n";

    std::string top_cmake_output_path = std::string(argv[2]) + "/CMakeLists.txt";
    std::ofstream top_cmakelists_file(top_cmake_output_path);
    top_cmakelists_file << top_level_cmake;
}

int main(int argc, char* argv[]) {
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));
    std::cout << "working directory: " << cwd << std::endl;
    bool is_lib = false;
    std::string project_dir;
    parse_args(argc, argv, is_lib, project_dir, project_output_dir, global_name);

    lib_path = getenv("XLPATH");
    std::string top_project_dir = project_dir;
    project_dir = path_join(project_dir, "src");

    std::string command = "rm -rf " + project_output_dir;
    system(command.c_str());

    int x = mkdir(project_output_dir.c_str(), 0700);
    if (x != 0) {
        std::cout << "failed to create application dir" << std::endl;
        exit(0);
    }
    std::string __main_file__ = path_join(project_dir, u_basename(project_dir) + ".xl");
    // std::cout << style(BLUE, "Main file: ") << style(MAGENTA, __main_file__) << std::endl;
    root_package = new Package(Path("root"), project_dir, "");

    std::string req_file_path = path_join(top_project_dir, REQUIREMENTS_FILE);
    top_package = new Package(Path("global"), "", "");
    top_package->units["root"] = Unit{.type=U_TYPE::PACKAGE, .package=root_package};
    load_requirements(req_file_path);
    load_package(root_package, 0);
    parse_all_modules(*root_package);

    process_global_all_modules(root_package);

    try {
        analyze_all_modules(*root_package, top_package);
    } catch (const std::runtime_error& e) {
        std::cout << "ERROR: " << e.what() << std::endl;
        exit(0);
    }
    if (global_fail) {
        std::cout << "Failed to compile" << std::endl;
        exit(0);
    }

    transpile_all_modules(*root_package,
                          project_output_dir,
                          true,
                          global_name,
                          static_initializations,
                          static_cleanups);

    std::string all_files;
    for (auto f: all_modules) {
        if (f == "core.xl") {
            continue;
        }
        f = path_join("root", f.substr(0, f.size() - 3) + ".cpp");
        all_files += f + " ";
    }

    write_cmakelists(all_files);
    if (!is_lib) {
        write_top_cmakelists(argv);
    }
}