#include "init.hpp"
#include <iostream>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

void vcs_init() {
    fs::create_directory(".vcs");
    fs::create_directory(".vcs/objects");
    fs::create_directories(".vcs/refs/heads");

    std::ofstream head(".vcs/HEAD");
    head << "refs/heads/master";
    head.close();

    std::cout << "Initialized empty VCS repository in .vcs/\n";
}