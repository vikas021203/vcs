#pragma once 
#include <string>
#include <unordered_map>

bool starts_with(const std::string& input, const char* prefix);
bool ensure_vcs_initialized();
std::string hash_string(const std::string& input);
std::string hash_file(const std::string& path);
std::unordered_map<std::string, std::string> load_index();
void save_index(const std::unordered_map<std::string, std::string>& index);
void clean_working_directory();
std::unordered_map<std::string, std::string> load_tree_recursive(const std::string& tree_hash, const std::string& base_path = "");