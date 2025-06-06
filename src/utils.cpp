#include "utils.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <openssl/sha.h>
#include <iostream>

namespace fs = std::filesystem;

bool starts_with(const std::string& input, const char* prefix) {
    std::string str_prefix = prefix;
    return input.size() >= str_prefix.size() && std::equal(str_prefix.begin(), str_prefix.end(), input.begin());
}

bool ensure_vcs_initialized() {
    if (!std::filesystem::exists(".vcs")) {
        std::cerr << "fatal: Not a VCS repository (or any of the parent directories): .vcs\n";
        return false;
    }
    return true;
}

std::string hash_string(const std::string& input) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(input.c_str()), input.size(), hash);

    std::ostringstream hex;
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        hex << std::hex << (int)hash[i];
    
    return hex.str();
}

std::string hash_file(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if(!file) return "";

    std::ostringstream ss;
    ss << file.rdbuf();
    std::string content = ss.str();

    return hash_string(content);
}

std::unordered_map<std::string, std::string> load_index() {
    std::unordered_map<std::string, std::string> index;
    std::ifstream file(".vcs/index");
    if(!file) return index;

    std::string line;
    while(std::getline(file, line)) {
        std::istringstream iss(line);
        std::string filename, hash;
        if(iss >> filename >> hash) {
            index[filename] = hash;
        }
    }

    return index;
}

void save_index(const std::unordered_map<std::string, std::string>& index) {
    std::ofstream file(".vcs/index");
    for(const auto& [filename, hash] : index) {
        file << filename << ' ' << hash << '\n';
    }
}

void clean_working_directory() {
    for(const auto& entry : fs::directory_iterator(fs::current_path())) {
        if(entry.path().filename() == ".vcs") continue;
        fs::remove_all(entry.path());
    }
}

std::unordered_map<std::string, std::string> load_tree_recursive(const std::string& tree_hash, const std::string& base_path) {
    std::unordered_map<std::string, std::string> tree;
    std::ifstream tree_file(".vcs/objects/" + tree_hash);
    if (!tree_file) return tree;

    std::string line;
    while (std::getline(tree_file, line)) {
        std::istringstream iss(line);
        std::string type, hash, name;
        iss >> type >> hash >> name;

        std::string full_path = base_path.empty() ? name : base_path + "\\" + name;
        if (type == "blob") {
            tree[full_path] = hash;
        } else if (type == "tree") {
            std::unordered_map<std::string, std::string> subdir_tree = load_tree_recursive(hash, full_path);
            tree.insert(subdir_tree.begin(), subdir_tree.end());
        }
    }
    return tree;
}

std::vector<std::tuple<std::regex, bool, std::string>> load_ignore_patterns() {
    std::vector<std::tuple<std::regex, bool, std::string>> patterns;
    std::ifstream ignoreFile(".vcsignore");
    std::string line;
    while (std::getline(ignoreFile, line))
    {
        if(!line.empty() && line[0] != '#') {
            bool isNegated = false;
            if(line[0] == '!') {
                isNegated = true;
                line = line.substr(1);
            }
            std::string pattern = std::regex_replace(line, std::regex(R"(\.)"), R"(\.)");
            pattern = std::regex_replace(pattern, std::regex(R"(\*\*/?)"), R"((.*/)?))");
            pattern = std::regex_replace(pattern, std::regex(R"(\*)"), R"([^/]*)");
            if(!pattern.empty() && pattern.back() == '/') pattern += ".*";

            if(line.front() == '/') {
                pattern = "^" + pattern;
            } else {
                pattern = "(^|.*/)" + pattern;
            }

            patterns.push_back({std::regex(pattern), isNegated, pattern});
        }
    }
    return patterns;
}

bool is_ignored(const std::string& relPath, const std::vector<std::tuple<std::regex, bool, std::string>>& ignore_patterns) {
    std::string normalized = relPath;
    std::replace(normalized.begin(), normalized.end(), '\\', '/');
    for (const auto& pattern : ignore_patterns) {
        if(std::regex_match(normalized, std::get<0>(pattern))) {
            return !std::get<1>(pattern);
        }
    }
    return false;
}