#include "utils.hpp"
#include "add.hpp"
#include <iostream>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

void vcs_add(const std::vector<std::string>& paths) {
    if(!ensure_vcs_initialized()) return;

    std::unordered_map<std::string, std::string> index = load_index();

    for(const auto& path_str : paths) {
        fs::path path = fs::path(path_str);

        if(!fs::exists(path)) {
            std::cerr << "File not found " << path_str << '\n';
            continue;
        }

        if(fs::is_regular_file(path))
        {
            std::string relPath = fs::relative(path, fs::current_path()).string();
            std::string hash = hash_file(path.string());

            if(index.find(relPath) != index.end() && index[relPath] == hash) continue;

            std::string blob_path = ".vcs/objects/" + hash;
            if(!fs::exists(blob_path)) {
                std::ifstream in(path, std::ios::binary);
                std::ofstream out(blob_path, std::ios::binary);
                out << in.rdbuf();
            }

            index[relPath] = hash;  
            std:: cout << "Added: " << relPath << '\n';
        } else if(fs::is_directory(path)) {
            for (const auto& entry : fs::recursive_directory_iterator(path)) {
                if(!entry.is_regular_file()) continue;

                std::string relPath = fs::relative(entry.path(), fs::current_path()).string();
                std::string hash = hash_file(entry.path().string());

                if(index.find(relPath) != index.end() && index[relPath] == hash) continue;

                std::string blob_path = ".vcs/objects/" + hash;
                if(!fs::exists(blob_path)) {
                    std::ifstream in(entry.path(), std::ios::binary);
                    std::ofstream out(blob_path, std::ios::binary);
                    out << in.rdbuf();
                }

                index[relPath] = hash;  
                std:: cout << "Added: " << relPath << '\n';
            }
        }
    }

    save_index(index);
}