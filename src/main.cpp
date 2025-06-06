#include "main.hpp"
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char* argv[]) {
    if(argc < 2) {
        std::cout << "Usage: vcs <command>\n";
        return 1;
    }

    std::string command = argv[1];
    if(command == "init") {
        vcs_init();
    } else if(command == "status") {
        vcs_status();
    } else if(command == "add") {
        std::vector<std::string> files;
        for(int i = 2; i < argc; i++) {
            files.push_back(argv[i]);
        }
        vcs_add(files);
    } else if(command == "rm") {
        std::vector<std::string> files;
        for (int i = 2; i < argc; i++) {
            files.push_back(argv[i]);
        }
        vcs_rm(files);
    } else if(command == "commit") {
        std::string message = argv[2];
        vcs_commit(message);
    } else if(command == "restore") {
        std::string filename = argv[2];
        vcs_restore(filename);
    } else if(command == "branch") {
        std::string branch_name = argv[2];
        vcs_branch(branch_name);
    } else if(command == "checkout") {
        std::string branch_name = argv[2];
        vcs_checkout(branch_name);
    } else if(command == "log") {
        vcs_log();
    } else {
        std::cout << "Unknown command: " << command << "\n";
    }

    return 0;
}