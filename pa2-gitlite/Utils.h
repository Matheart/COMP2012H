//
// This file contains all the utility functions. Only part of them are useful
// for your tasks.
//
// You don't need to modify any part of this file.
//

#ifndef COMP2012H_FA21_PA2_UTILS_H
#define COMP2012H_FA21_PA2_UTILS_H

#include <vector>
#include <string>
#include <filesystem>
#include <unordered_map>
#include <cereal/archives/binary.hpp>
#include <cereal/types/vector.hpp>

//=============================================================================
// These are the utility functions you will probably want to use in your code.
// They come along with docstrings to facilitate your invocation.
//=============================================================================

/**
 * Compute the SHA1 value with given string of commit message and time.
 * Note that this strategy is prone to collisions (consider making multiple commits with the same commit message
 * at the same time), a more reasonable choice is to include the files tracked for hashing. We chose the former
 * because it makes the tasks easier.
 * @param message the commit message
 * @param time the time of committing
 * @return a string containing the SHA1 value
 */
std::string get_sha1(const std::string &message, const std::string &time);


/**
 * Compute the SHA1 value of the file in CWD with given filename
 * @param filename the filename of the file
 * @return a string containing SHA1 value
 */
std::string get_sha1(const std::string &filename);


/**
 * Get the current time as a formatted string
 * @return a formatted string
 */
std::string get_time_string();


/**
 * Delete the file in CWD with given filename only if there exists a directory
 * named .gitlite in CWD
 * This function WILL ACTUALLY DELETE THE FILE in your filesystem!
 * @param filename the filename of the file to be deleted
 * @return true if successfully deleted, false otherwise
 */
bool restricted_delete(const std::string &filename);


/**
 * Add conflict resolution marker to the file in CWD based on the its contents
 * and the contents of the checked-out file with ref.
 * @param filename the filename of the file in CWD
 * @param ref the reference (SHA1 value) of the file compared to
 */
void add_conflict_marker(const std::string &filename, const std::string &ref);


/**
 * Replace the contents of a file in CWD with the contents of the blob specified
 * by the reference (SHA1 value)
 * @param filename the filename of the file to be overwritten
 * @param ref the reference (SHA1 value) to the blob
 * @return true if succeeded, false otherwise
 */
bool write_file(const std::string &filename, const std::string &ref);


/**
 * Check whether a file with given name exists in CWD
 * @param filename the name of the file
 * @return true if the file exists, false otherwise
 */
bool is_file_exist(const std::string &filename);


/**
 * Copy the file in CWD to the staging area
 * @param filename the name of the file to copy
 */
void stage_content(const std::string &filename);


//========================================================
// You can safely ignore any of the following functions.
//========================================================

std::string read_content(const std::filesystem::path &path);

std::vector<std::string> regular_files_in_path(const std::filesystem::path &path);

void write_content(const std::filesystem::path &path, const std::string &content);

std::string get_string_sha1(const std::string &str);

std::string get_sha1(const std::filesystem::path &path);

void copy_file_overwrite(const std::filesystem::path &from, const std::filesystem::path &to);

#endif //COMP2012H_FA21_PA2_UTILS_H
