#include "log.hpp"
#include "utils.hpp"
#include <filesystem>
#include <iostream>
#include <fstream>

namespace fs = std::filesystem;

void vcs_log() {
    if(!ensure_vcs_initialized()) return;

    std::ifstream headFile(".vcs/HEAD");
    std::string ref;
    std::getline(headFile, ref);
    headFile.close();

    std::ifstream branchFile(".vcs/" + ref);
    std::string commitId;
    std::getline(branchFile, commitId);
    branchFile.close();

    while (!commitId.empty())
    {
        std::string commit_path = ".vcs/commits/" + commitId;

        if(!fs::exists(commit_path)) {
            std::cerr << "Commit object not found: " << commitId << "\n";
            break;
        }

        std::ifstream commitFile(commit_path);
        std::string line, message, timestamp, parent;
        while(std::getline(commitFile, line)) {
            if(starts_with(line, "# Message: ")) {
                message = line.substr(11);
            } else if(starts_with(line, "# Time: ")) {
                timestamp = line.substr(8);
            } else if(starts_with(line, "# Parent: ")) {
                parent = line.substr(10);
            }
        }
        commitFile.close();

        std::cout << "CommitId: " <<commitId << "\n";
        std::cout << "Time: " << timestamp << "\n";
        std::cout << "Message: " << message << "\n\n";

        commitId = parent;
    }
    
}