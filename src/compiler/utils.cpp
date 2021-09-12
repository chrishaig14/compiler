//
// Created by chris on 25/5/21.
//

#include <iostream>
#include <getopt.h>
#include <sys/stat.h>
#include <fstream>
#include "utils.h"

std::vector<Requirement> read_requirements(const std::string& filepath) {
    std::string line;
    std::vector<Requirement> requirements;
    std::ifstream in_file(filepath);
    if (!in_file.is_open()) {
        std::cerr << "No requirements file at " << filepath << std::endl;
        return requirements;
    }
    while (std::getline(in_file, line)) {
        std::string op;
        std::string package_name;
        std::string package_version;
        for (char i : line) {
            if (i == '=') {
                op += '=';
            } else {
                if (op == "") {
                    package_name += i;
                } else {
                    package_version += i;
                }
            }
        }
        requirements.push_back(Requirement{package_name, package_version});
    }
    return requirements;
}


void parse_args(int argc, char** argv, bool& is_lib, std::string& project_dir, std::string& project_output_dir,
                std::string& global_name, std::string& version) {
    int opt;
    while ((opt = getopt(argc, argv, "i:o:n:v:l")) != -1) {
        switch (opt) {
            case 'i':
                project_dir = optarg;
                break;
            case 'l':
                is_lib = true;
                break;
            case 'o':
                project_output_dir = optarg;
                break;
            case 'v':
                version = optarg;
                break;
            case 'n':
                global_name = optarg;
                break;
            case ':':
                printf("option needs a value\n");
                exit(1);
            case '?':
                printf("unknown option: %c\n", optopt);
                exit(1);
        }
    }
    for (; optind < argc; optind++) {
        printf("extra arguments: %s\n", argv[optind]);
    }
}

void init(int argc, char** argv, bool& is_lib, std::string& project_dir, std::string& project_output_dir,
          std::string& global_name, std::string& version) {
    parse_args(argc, argv, is_lib, project_dir, project_output_dir, global_name, version);

    if (is_lib && version == "") {
        std::cout << "Library without version: using default version 1.0.0" << std::endl;
        version = "1.0.0";
    } else if (!is_lib && version != "") {
        std::cout << "Ignoring version for executable" << std::endl;
    }

    std::string command = "rm -rf " + project_output_dir;
    system(command.c_str());

    int x = mkdir(project_output_dir.c_str(), 0700);
    if (x != 0) {
        std::cout << "failed to create application dir" << std::endl;
        exit(0);
    }
}
