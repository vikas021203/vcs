#pragma once

#include <string>
#include <filesystem>

std::string write_tree(const std::filesystem::path& dir);
std::string generate_commit_id(const std::string& message);
void vcs_commit(const std::string& message);