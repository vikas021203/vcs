#include "branch.hpp"
#include "utils.hpp"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <sstream>

namespace fs = std::filesystem;

void vcs_branch(const std::string& branch_name) {
    if(!ensure_vcs_initialized()) return;

    std::ifstream head(".vcs/HEAD");
    std::string headRef;
    std::getline(head, headRef);

    std::ifstream currentBranchFile(".vcs/" + headRef);
    std::string currentCommit;
    std::getline(currentBranchFile, currentCommit);

    std::string newBranchPath = ".vcs/refs/heads/" + branch_name;
    std::ofstream out(newBranchPath);
    out << currentCommit;

    std::cout << "Branch " << branch_name << " created at commit " << currentCommit << "\n";
}