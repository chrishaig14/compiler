//
// Created by chris on 25/5/21.
//

#include <iostream>
#include <fstream>
#include <getopt.h>
#include "utils.h"

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

void parse_args(int argc, char** argv, bool& is_lib, std::string& project_dir, std::string& project_output_dir,
                std::string& global_name) {
    int opt;
    while ((opt = getopt(argc, argv, "i:o:n:l")) != -1) {
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
