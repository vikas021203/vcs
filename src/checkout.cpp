#include "checkout.hpp"
#include "utils.hpp"
#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <unordered_map>

namespace fs = std::filesystem;

std::string extract_tree_from_commit(const std::string& commit_id) {
    std::ifstream commit_file(".vcs/commits/" + commit_id);
    std::string line;
    while(std::getline(commit_file, line)) {
        if(line.rfind("# Tree: ", 0) == 0) {
            return line.substr(8);
        }
    }
    return "";
}

void restore_tree(const std::string& tree_hash, const fs::path& dest_path, std::unordered_map<std::string, std::string>& index) {
    std::ifstream tree_file(".vcs/objects/" + tree_hash);
    if(!tree_file) {
        std::cerr << "Tree object not found: " << tree_hash << "\n";
        return;
    }

    fs::create_directories(dest_path);

    std::string line;
    while(std::getline(tree_file, line)) {
        std::istringstream iss(line);
        std::string type, hash, name;
        iss >> type >> hash >> name;

        fs::path full_path = dest_path / name;

        if(type == "blob") {
            std::ifstream in(".vcs/objects/" + hash, std::ios::binary);
            std::ofstream out(full_path, std::ios::binary);
            if(!in || !out) {
                std::cerr << "Failed to restore blob: " << name << "\n";
                continue;
            }
            out << in.rdbuf();
            index[fs::relative(full_path, fs::current_path()).string()] = hash;
        } else if(type == "tree") {
            restore_tree(hash, full_path, index);
        }
    }
}

void vcs_checkout(const std::string& branch_name) {
    std::string branch_path = ".vcs/refs/heads/" + branch_name;

    if(!fs::exists(branch_path)) {
        std::cerr << "Branch does not exist.\n";
        return;
    }

    std::ifstream branchFile(branch_path);
    std::string commitId;
    std::getline(branchFile, commitId);
    branchFile.close();

    std::ofstream head(".vcs/HEAD");
    head << "refs/heads/" << branch_name;
    head.close();

    std::string tree_hash = extract_tree_from_commit(commitId);
    if(tree_hash.empty()) {
        std::cerr << "Invalid commit in branch.\n";
        return;
    }

    clean_working_directory();

    std::unordered_map<std::string, std::string> new_index;
    restore_tree(tree_hash, fs::current_path(), new_index);

    save_index(new_index);

    std::cout << "Switched to branch: " << branch_name << "\n";
}