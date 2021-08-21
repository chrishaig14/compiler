//
// Created by chris on 25/5/21.
//

#include "transpile.h"
#include "Compiler.h"

const int NEW_DIR_MODE = 0777;

void Compiler::transpile_all_modules(Package& package, const std::string& output_dir, bool is_top) {
    std::string package_header;
    std::string output_package_dir = path_join(output_dir, package.name);
    if (package.name != "") {
        mkdir(output_package_dir.c_str(), NEW_DIR_MODE);
    }
    package_header += "#define LIBP(a, b, c, d, e) a ## _v ## b ## _ ## c ## _ ##  d ## _ ##  e\n"
                      "#define LIB(a, b, c, d, e)  LIBP(a,b,c,d,e)\n"
                      "#define LIBENT(x) LIB(LIBNAME, LIBMAJV, LIBMINV, LIBPATV, x)\n"
                      "#define LIBNAME " + this->output_name + "\n" + "#define LIBMAJV 1\n"
                                                                      "#define LIBMINV 0\n"
                                                                      "#define LIBPATV 0\n";

    Module* main_module = nullptr;
    for (const auto& u: package.units) {
        if (u.second.type == U_TYPE::PACKAGE) {
            Package& subpackage = *u.second.package;
            transpile_all_modules(subpackage, output_package_dir, false);
            package_header += "#include <" + subpackage.rel_path + "/__package__.h>\n";
        } else if (u.second.type == U_TYPE::MODULE) {
            Module& module = *u.second.module;
            if (module.flirpins.count("main") != 0) {
                main_module = &module;
                continue;
            }
            this->transpile_one_module(module, package_header, output_package_dir, package);
        }
    }
    if (is_top && main_module != nullptr) {
        this->transpile_one_module(*main_module, package_header, output_package_dir, package);
    }
    std::string output_package_header_path = path_join(output_package_dir, "__package__.h");
    std::ofstream output_package_header(output_package_header_path);
    if (output_package_header.is_open()) {
        output_package_header << package_header;
    } else {
        std::cerr << "Failed to open file " << output_package_header_path << std::endl;
    }
}

void Compiler::transpile_one_module(Module& module, std::string& package_header, const std::string& output_package_dir,
                                    Package& package) {
    if (module.name == "core") {
        return;
    }
    STranspiler t;
    t.transpile_program(*module.sast);
    this->static_initializations += t.static_initializations;
    this->static_cleanups += t.static_cleanups;

    std::string module_name = module.name;

    if (module.flirpins.count("main") != 0) {
        t.source += "\nint main(){\n";
        t.source += static_initializations;
        t.source += "TaggedObject* r = CALL0(" + mangle_path(module.path.as_str() + ".main") + ");\n";
        t.source += "if (has_tag(r,EXCEPTION_TAG)){std::cout << \"Exited with Exception: \" << std::endl; exit(1);}";
        t.source += static_cleanups;
        t.source += "return GET_INT(r);\n}";
    }
    std::string output_cpp_path = path_join(output_package_dir, module_name + ".cpp");
    std::ofstream output_cpp_file(output_cpp_path);
    output_cpp_file << "#include \"" << module_name << ".h\"\n";
    output_cpp_file << t.source;

    std::string output_h_path = path_join(output_package_dir, module_name + ".h");


    std::string module_define = module_name + "_H";
    std::string h_ifndef = "#ifndef " + module_define + "\n";
    std::string h_define = "#define " + module_define + "\n";
    std::string h_endif = "#endif //" + module_define + "\n";
    // std::string include_core = "#include <core/core.h>\n";
    std::string includes = "#include <core/1.0.0/out/core/xobject.h>\n";
    includes += "#include <core/1.0.0/out/core/Function.h>\n";
    for (const auto& m: module.included_module_paths) {
        includes += "#include <" + m.second + ">\n";
    }
    t.header = includes + t.header;

    t.header = h_ifndef + h_define + t.header + h_endif;
    std::ofstream output_h_file(output_h_path);
    output_h_file << t.header;
    package_header += "#include <" + package.rel_path + "/" + module_name + ".h>\n";
}

