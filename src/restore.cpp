#include "restore.hpp"
#include "utils.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

void vcs_restore(const std::string& filename) {
    if(!ensure_vcs_initialized()) return;

    std::ifstream headFile(".vcs/HEAD");
    std::string commitId;
    if(!std::getline(headFile, commitId)) {
        std::cerr << "No commits to restore from.\n";
        return;
    }

    std::ifstream commitFile(".vcs/commits/" + commitId);
    std::string line, file, hash;
    bool found = false;

    while(std::getline(commitFile, line)) {
        std::istringstream iss(line);
        if(iss >> file >> hash && file == filename) {
            found = true;
            break;
        }
    }

    if(!found) {
        std::cerr << "File not found in last commit: " << filename << "\n";
        return;
    }

    std::ifstream objectFile(".vcs/objects/" + hash, std::ios::binary);
    if(!objectFile) {
        std::cerr << "Object data missing for file: " << filename << "\n";
        return;
    }

    std::ofstream outFile(filename, std::ios::binary);
    outFile << objectFile.rdbuf();
    std::cout << "Restored: " << filename << "\n";
}