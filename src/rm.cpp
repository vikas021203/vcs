#include "rm.hpp"
#include "utils.hpp"
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

void vcs_rm(std::vector<std::string>& files) {
    if(!ensure_vcs_initialized()) return;

    std::unordered_map<std::string, std::string> index = load_index();

    for (const auto& file : files) {
        fs::path filepath = fs::path(file);
        std::string relPath = fs::relative(filepath, fs::current_path()).string();

        if(index.erase(relPath)) {
            std::cout << "Removed: " << relPath << "\n";
        } else {
            std::cout << "Not staged: " << relPath << "\n";
        }
    }

    save_index(index);
}