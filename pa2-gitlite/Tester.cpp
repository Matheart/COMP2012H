#include "Tester.h"
#include "Repository.h"
#include "Utils.h"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <iterator>
#include <unordered_set>

using namespace std;
using path = std::filesystem::path;

const path Tester::SRC = filesystem::current_path() / path("src");

const regex Tester::DEF_PATTERN(R"#(^D\s*([a-zA-Z_][a-zA-Z_0-9]*)\s*"(.*)"\s*$)#");
const regex Tester::ADD_PATTERN(R"(\+\s*(\S+)\s+(\S+))");
const regex Tester::RM_PATTERN(R"(-\s*(\S+))");
const regex Tester::VAR_PATTERN(R"((\$\{.*?\}))");
const regex Tester::CMD_PATTERN(R"(>\s*(.*))");
const regex Tester::END_PATTERN(R"(<<<)");
const regex Tester::EXIST_PATTERN(R"(E\s*(\S+))");
const regex Tester::NE_PATTERN(R"(\*\s*(\S+))");
const regex Tester::INC_PATTERN(R"(I\s+(\S+))");
const regex Tester::CMP_PATTERN(R"(=\s*(\S+)\s+(\S+))");

static inline void trim(std::string &s);
static inline std::string trim_copy(std::string s);
static inline std::string rtrim_copy(std::string s);

Tester::Tester(const std::string &filename, bool verbose) : verbose(verbose) {
    ifstream is(filename);
    if (!is.is_open()) {
        throw std::runtime_error("failed to open the given test case");
    }

    cout << "Running test: " << filename << endl;
    copy(istreambuf_iterator<char>(is), istreambuf_iterator<char>(), ostreambuf_iterator<char>(script));
    is.close();
}

bool Tester::run() {
    string line;
    while (getline(script, line)) {
        canonicalize(line);
        if (verbose) {
            cout << line << endl;
        }
        if (trim_copy(line).empty()) {
            continue;
        }
        line = substitute(line);
        switch (line[0]) {
            case 'I':
                include_file(line);
                break;
            case 'D':
                add_definition(line);
                break;
            case 'E':
                if (!check_path_exists(line))
                    return false;
                break;
            case '+':
                copy_source(line);
                break;
            case '-':
                remove_file(line);
                break;
            case '*':
                if (!check_path_absent(line))
                    return false;
                break;
            case '>':
                if (!run_and_check_command(line))
                    return false;
                break;
            case '=':
                if (!compare_files(line))
                    return false;
                break;
            default:
                throw std::runtime_error("bad format: " + line);
        }
    }
    cout << "Test PASSED" << endl;
    return true;
}

bool Tester::run_and_check_command(const std::string &input) {
    // Parse
    smatch match;
    if (!regex_match(input, match, CMD_PATTERN)) {
        throw std::runtime_error("bad format: " + input);
    }

    string current_command = match[1];
    vector<string> expected;
    string line;
    while (true) {
        if (!getline(script, line)) {
            throw std::runtime_error("non-terminated command (missing <<<) : " + input);
        }
        canonicalize(line);
        if (verbose) {
            cout << line << endl;
        }
        line = substitute(line);
        if (regex_match(line, END_PATTERN)) {
            break;
        }
        expected.push_back(line);
    }

    // Run
    streambuf *original_output_buffer = cout.rdbuf();
    std::stringstream output;
    cout.rdbuf(output.rdbuf());
    auto args = split_args(current_command);
    if (validate_args(args)) {
        parse_args(args);
    }
    cout.rdbuf(original_output_buffer);

    // Check
    last_group.clear();
    string expected_output;
    string actual(std::istreambuf_iterator<char>(output), {});

    // Remove extra whitespaces at the end of each line
    canonicalize(actual);
    actual = regex_replace(actual, regex(R"([ \t]+\n)"), "\n");

    for (unsigned int i = 0; i < expected.size(); i++) {
        expected_output.append(expected[i]);
        if (i != expected.size() - 1) {
            expected_output.append("\n");
        }
    }

    canonicalize(expected_output);
    expected_output = regex_replace(expected_output, regex(R"([ \t]+\n)"), "\n");

    if (regex_match(actual, match, regex(expected_output))) {
        for (unsigned int i = 1; i < match.size(); i++) {
            last_group.insert({to_string(i), match[i].str()});
        }
    } else {
        string trimmed_actual(rtrim_copy(actual));
        string trimmed_expected(rtrim_copy(expected_output));
        if (regex_match(trimmed_actual, match, regex(trimmed_expected))) {
            for (unsigned int i = 1; i < match.size(); i++) {
                last_group.insert({to_string(i), match[i].str()});
            }
        } else {
            cout << "Wrong output for command: " << current_command << endl;
            cout << "Expected: " << endl << expected_output << endl << endl;
            cout << "Actual: " << endl << actual << endl;
            return false;
        }
    }
    return true;
}

void Tester::add_definition(const std::string &input) {
    smatch results;
    if (!regex_match(input, results, DEF_PATTERN)) {
        throw std::runtime_error("bad format: " + input);
    }
    defs.insert({results[1], results[2]});
}

std::string Tester::substitute_once(const std::string &raw) {
    unordered_set<std::string> variables;
    // Find all the variables
    for (auto iter = sregex_iterator(raw.begin(), raw.end(), VAR_PATTERN); iter != sregex_iterator(); ++iter) {
        variables.insert((*iter)[1].str());
    }

    string substituted(raw);
    for (auto &var : variables) {
        // Replace each variable with corresponding definition
        string varname = var.substr(2, var.length() - 3);
        string result;
        auto iter = last_group.find(varname);
        if (iter == last_group.end()) {
            iter = defs.find(varname);
            if (iter == defs.end()) {
                continue;
            }
        }
        regex pattern(R"(\$\{)" + varname + R"(\})");
        substituted = regex_replace(substituted, pattern, iter->second);
    }
    return substituted;
}

// Check the result of command with the expected string from the given test case

void Tester::copy_source(const std::string &input) {
    smatch match;
    if (!regex_match(input, match, ADD_PATTERN)) {
        throw std::runtime_error("bad format: " + input);
    }

    path from = SRC / path(match[2].str()), to = filesystem::current_path() / path(match[1].str());
    copy_file_overwrite(from, to);
}

bool Tester::check_path_exists(const string &input) {
    smatch match;
    if (!regex_match(input, match, EXIST_PATTERN)) {
        throw std::runtime_error("bad format: " + input);
    }

    path file = filesystem::current_path() / path(match[1].str());
    if (!filesystem::exists(file)) {
        cout << "Failed at " << input << endl;
        cout << file << " does not exist" << endl;
        return false;
    }
    return true;
}

void Tester::remove_file(const string &input) {
    smatch match;
    if (!regex_match(input, match, RM_PATTERN)) {
        throw std::runtime_error("bad format: " + input);
    }

    path file = filesystem::current_path() / path(match[1].str());
    filesystem::remove(file);
}

bool Tester::check_path_absent(const string &input) {
    smatch match;
    if (!regex_match(input, match, NE_PATTERN)) {
        throw std::runtime_error("bad format: " + input);
    }

    path file = filesystem::current_path() / path(match[1].str());
    if (filesystem::exists(file)) {
        cout << "Failed at " << input << endl;
        cout << file << " exists" << endl;
        return false;
    }
    return true;
}

std::string Tester::substitute(const string &raw) {
    string result = substitute_once(raw), tmp;
    if (result == raw) {
        return raw;
    }

    int depth = 0;
    while ((tmp = substitute_once(result)) != result && depth < 3) {
        result = tmp;
        ++depth;
    }
    return result;
}

void Tester::include_file(const string &input) {
    smatch match;
    if (!regex_match(input, match, INC_PATTERN)) {
        throw std::runtime_error("bad format: " + input);
    }

    ifstream is(match[1].str());
    if (!is.is_open()) {
        throw std::runtime_error("failed to include the given file: " + match[1].str());
    }

    stringstream temp_stream;
    copy(istreambuf_iterator<char>(is), istreambuf_iterator<char>(), ostreambuf_iterator<char>(temp_stream));
    is.close();

    string remaining(script.str().substr(script.tellg()));
    temp_stream << remaining;
    script.str(temp_stream.str());
    script.clear();
}

bool Tester::compare_files(const string &input) {
    smatch match;
    if (!regex_match(input, match, CMP_PATTERN)) {
        throw std::runtime_error("bad format: " + input);
    }

    // https://stackoverflow.com/a/37575457
    path srcPath = SRC / path(match[2].str());
    path targetPath = filesystem::current_path() / path(match[1].str());
    ifstream src(srcPath, ifstream::binary);
    ifstream target(match[1].str(), ifstream::binary);

    if (src.fail()) {
        throw std::runtime_error("failed to open " + srcPath.string());
    }

    if (!filesystem::exists(targetPath)) {
        cout << "Failed at " << input << endl;
        cout << match[1].str() << " does not exist in the current working directory" << endl;
        return false;
    }

    if (target.fail()) {
        throw std::runtime_error("failed to open " + targetPath.string());
    }

    string src_str(std::istreambuf_iterator<char>(src), {});
    string target_str(std::istreambuf_iterator<char>(target), {});

    canonicalize(src_str);
    canonicalize(target_str);

    if (src_str != target_str) {
        cout << "Failed at " << input << endl;
        cout << match[1].str() << " does not contain correct content" << endl;
        return false;
    }
    return true;
}

void Tester::canonicalize(string &input) {
    input = regex_replace(input, regex(R"(\r)"), "");
}


// Helper functions from https://stackoverflow.com/a/217605
// trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

// trim from end (copying)
static inline std::string rtrim_copy(std::string s) {
    rtrim(s);
    return s;
}

// trim from both ends (copying)
static inline std::string trim_copy(std::string s) {
    trim(s);
    return s;
}
