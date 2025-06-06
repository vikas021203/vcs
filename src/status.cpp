#include "status.hpp"
#include "utils.hpp"
#include <iostream>
#include <filesystem>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <utility>

namespace fs = std::filesystem;

void vcs_status() {
    if(!ensure_vcs_initialized()) return;

    auto ignored_patterns = load_ignore_patterns();
    auto index = load_index();
    std::unordered_map<std::string, std::string> current;
    std::ifstream headFile(".vcs/HEAD");
    std::string refPath, headCommit, tree_hash;

    if(headFile && std::getline(headFile, refPath)) {
        std::ifstream refFile(".vcs/" + refPath);
        if(refFile && std::getline(refFile, headCommit))
        {
            std::ifstream commitFile(".vcs/commits/" + headCommit);
            std::string line;
            while(std::getline(commitFile, line)) {
                if(line.rfind("# Tree: ", 0) == 0) {
                    tree_hash = line.substr(8);
                    break;
                }
            }
        }
    }

    std::unordered_map<std::string, std::string> lastCommit = load_tree_recursive(tree_hash);

    for(const auto& entry : fs::recursive_directory_iterator(fs::current_path())) {
        if(entry.is_regular_file()) {
            std::string relPath = fs::relative(entry.path(), fs::current_path()).string();

            if(relPath.find(".vcs", 0) != std::string::npos) continue;
            if(is_ignored(relPath, ignored_patterns)) continue;

            std::cout << relPath << "\n";

            std::string hash = hash_file(entry.path().string());
            current[relPath] = hash;
        }
    }

    std::cout << "\033[1mChanges to be committed:\033[0m\n";
    for(const auto& [file, hash] : index) {
        if(lastCommit.find(file) == lastCommit.end()) {
            std::cout << "\033[32m  New file:    " << file << "\033[0m\n";
        } else if(lastCommit[file] != hash) {
            std::cout << "\033[36m  Modified:    " << file << "\033[0m\n";
        }
    }

    for(const auto& [file, hash] : lastCommit) {
        if(index.find(file) == index.end()) {
            std::cout << "\033[31m  Deleted:    " << file << "\033[0m\n";
        }
    }

    std::cout << "\n\033[1mChanges not staged for commit:\033[0m\n";
    for (const auto& [file, hash] : index) {
        if (current.find(file) == current.end()) {
            std::cout << "\033[31m  Deleted:    " << file << "\033[0m\n";
        } else if (current.find(file) != current.end() && current[file] != hash) {
            std::cout << "\033[33m  Modified:   " << file << "\033[0m\n";
        }
    }

    std::cout << "\n\033[1mUntracked files:\033[0m\n";
    for (const auto& [file, hash] : current) {
        if(lastCommit.find(file) == lastCommit.end() && index.find(file) == index.end()) {
            std::cout << "\033[91m  " << file << "\033[0m\n";
        }
    }
}