//
// Tester class. This is only used as a automated testing tool to facilitate you debugging,
// as we will not use it in our autograder.
//
// You may modify it if you understand the mechanisms and really want to add some other testing features.
//

#ifndef COMP2012H_FA21_PA2_TESTER_H
#define COMP2012H_FA21_PA2_TESTER_H

#include <string>
#include <sstream>
#include <unordered_map>
#include <filesystem>
#include <regex>

class Tester {
public:
    explicit Tester(const std::string &filename, bool verbose = false);

    bool run();

private:
    static void copy_source(const std::string &input);
    static void remove_file(const std::string &input);
    static bool check_path_exists(const std::string &input);
    static bool check_path_absent(const std::string &input);
    static bool compare_files(const std::string &input);
    static void canonicalize(std::string &input);

    std::string substitute_once(const std::string &raw);
    std::string substitute(const std::string &raw);
    bool run_and_check_command(const std::string &input);
    void add_definition(const std::string &input);
    void include_file(const std::string &input);

    static const std::regex DEF_PATTERN;
    static const std::regex ADD_PATTERN;
    static const std::regex RM_PATTERN;
    static const std::regex VAR_PATTERN;
    static const std::regex CMD_PATTERN;
    static const std::regex END_PATTERN;
    static const std::regex EXIST_PATTERN;
    static const std::regex NE_PATTERN;
    static const std::regex INC_PATTERN;
    static const std::regex CMP_PATTERN;

    static const std::filesystem::path SRC;

    bool verbose = false;
    std::stringstream script;
    std::unordered_map<std::string, std::string> defs;
    std::unordered_map<std::string, std::string> last_group;
};

#endif //COMP2012H_FA21_PA2_TESTER_H
