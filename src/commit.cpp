#include "commit.hpp"
#include "utils.hpp"
#include <iostream>
#include <chrono>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include <functional>

namespace fs = std::filesystem;

std::string build_tree_from_index(const std::unordered_map<std::string, std::string>& index) {
    struct TreeNode
    {
        std::unordered_map<std::string, TreeNode> children;
        std::unordered_map<std::string, std::string> files;
    } root;

    for(const auto& [path, hash] : index) {
        fs::path p(path);
        TreeNode* node = &root;
        auto end = p.end();
        --end;
        for(auto it = p.begin(); it != end; it++) {
            auto &child = node->children[it->string()];
            node = &child;
        }
        node->files[p.filename().string()] = hash;
    }

    std::function<std::string(const TreeNode&)> write_tree = [&](const TreeNode& node) -> std::string {
        std::stringstream tree_data;
        for(const auto& [name, hash] : node.files) {
            tree_data << "blob " << hash << " " << name << "\n";
        }

        for(const auto& [name, child] : node.children) {
            std::string child_hash = write_tree(child);
            tree_data << "tree " << child_hash << " " << name << "\n";
        }

        std::string tree_str = tree_data.str();
        std::string tree_hash = hash_string(tree_str);
        std::ofstream out(".vcs/objects/" + tree_hash);
        out << tree_str;
        return tree_hash;
    };

    return write_tree(root);   
}

std::string generate_commit_id(const std::string& message) {
    auto now = std::chrono::system_clock::now();
    auto now_time = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::to_string(now_time) << message;
    return hash_string(ss.str());
}

void vcs_commit(const std::string& message) {
    if(!ensure_vcs_initialized()) return;

    std::unordered_map<std::string, std::string> index = load_index();
    if(index.empty()) {
        std::cout << "Nothing to commit.\n";
        return;
    }

    fs::create_directories(".vcs/commits");

    std::string commit_id = generate_commit_id(message);
    std::string commit_path = ".vcs/commits/" + commit_id;
    std::string parent_commit_id;

    std::ifstream head(".vcs/HEAD");
    std::string ref;
    std::getline(head, ref);
    head.close();

    std::string branch_path = ".vcs/" + ref;
    if(fs::exists(branch_path)) {
        std::ifstream branchFile(branch_path);
        std::getline(branchFile, parent_commit_id);
        branchFile.close();
    }

    std::string tree_hash = build_tree_from_index(index);

    std::ofstream commit_file(commit_path);
    if(!commit_file) {
        std::cerr << "Failed to write commit.\n";
        return;
    }

    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    commit_file << "# Commit: " << commit_id << "\n";
    commit_file << "# Time: " << std::ctime(&now);
    commit_file << "# Message: " << message << "\n";
    commit_file << "# Parent: " << parent_commit_id << "\n";
    commit_file << "# Tree: " << tree_hash << "\n";

    commit_file.close();

    std::ofstream branchFile(".vcs/" + ref);
    branchFile << commit_id;

    std::cout << "Commited as " << commit_id << "\n";
}