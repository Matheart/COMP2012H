#include <iostream>
#include <filesystem>
#include <string>
#include <vector>

#include "Repository.h"
#include "Tester.h"

using std::cout;
using std::endl;
using path = std::filesystem::path;

// Clean the testing directory for further testing
void clean_environment() {
    for (auto &entry : std::filesystem::directory_iterator(Repository::CWD)) {
        if (entry.is_regular_file()) {
            if (entry.path().filename() != "gitlite" && entry.path().filename() != "gitlite.exe") {
                // Remove all the files other than the program itself
                std::filesystem::remove(entry.path());
            }
        } else if (entry.is_directory() && entry.path().filename() == ".gitlite") {
            // Remove the whole .gitlite directory recursively
            std::filesystem::remove_all(entry.path());
        }
    }
}

bool test_handler(const std::string &filename, bool verbose) {
    try {
        Tester tester(filename, verbose);
        clean_environment();
        if (!tester.run()) {
            cout << "Test FAILED" << endl;
            return false;
        }
    } catch (const std::exception &e) {
        cout << e.what() << endl << "Test FAILED" << endl;
        return false;
    }
    return true;
}

void test_all(const path &dir, bool verbose) {
    for (auto &entry : std::filesystem::directory_iterator(dir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".in") {
            path relative = std::filesystem::relative(entry.path());
            if (!test_handler(relative.string(), verbose)) {
                Repository::close();
                return;
            }
            Repository::close();
            Repository::reset_states();
        }
    }
    std::cout << "All tests PASSED" << std::endl;
}

int run_test(const std::string &filename, bool verbose) {
    path dir = Repository::CWD / path(filename);
    if (std::filesystem::is_directory(dir)) {
        test_all(dir, verbose);
        return 0;
    }
    test_handler(filename, verbose);
    Repository::close();
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        // You may write code here to test linked list operations or other stuff
        // This if branch is executed when Gitlite is executed without arguments
        return 0;
    }

    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.emplace_back(argv[i]);
    }

    if (args.size() == 2 && args[0] == "-t") {
        return run_test(args[1], false);
    }

    if (args.size() == 2 && (args[0] == "-tv" || args[0] == "-vt")) {
        return run_test(args[1], true);
    }

    if (!validate_args(args)) {
        return 0;
    }
    if (Repository::check_file_structure()) {
        try {
            Repository::load_repository();
        } catch (...) {
            std::cout << ".gitlite directory detected, but failed to load." << std::endl;
            std::cout << "File structures may be corrupted. Please delete .gitlite and retry." << std::endl;
            return 0;
        }
    }
    parse_args(args);
    Repository::close();

    return 0;
}
