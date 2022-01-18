//
// You don't need to modify any part of this file.
//

#include <TinySHA1.hpp>
#include <string>
#include <filesystem>
#include <fstream>
#include <ctime>

#include "Utils.h"

using namespace std;
using path = std::filesystem::path;

std::string get_sha1(const std::string &message, const std::string &time) {
    return get_string_sha1(message + time);
}

std::string get_sha1(const std::string &filename) {
    path file = filesystem::current_path() / path(filename);
    return get_sha1(file);
}

std::string get_time_string() {
    std::time_t now = std::time(nullptr);
    return std::ctime(&now);
}

bool restricted_delete(const std::string &filename) {
    path gitlite = filesystem::current_path() / path(".gitlite");
    if (!filesystem::is_directory(gitlite)) {
        // Only remove the file if it is in a directory with .gitlite
        return false;
    }

    path file = filesystem::current_path() / path(filename);
    return filesystem::remove(file);
}

void add_conflict_marker(const std::string &filename, const std::string &ref) {
    string header = "<<<<<<< HEAD\n";
    string separator = "=======\n";
    string footer = ">>>>>>>\n";

    path file = filesystem::current_path() / path(filename);
    if (ref.empty()) {
        if (!filesystem::is_regular_file(file)) {
            return;
        }
        string head_content = read_content(file);
        ofstream os(file);
        os << header << head_content << separator << footer;
        os.close();
        return;
    }

    path other = filesystem::current_path() / path(".gitlite/blobs") / path(ref);
    if (!filesystem::is_regular_file(file) && !filesystem::is_regular_file((other))) {
        return;
    }

    string head_content = filesystem::is_regular_file(file) ? read_content(file) : string();
    string other_content = filesystem::is_regular_file(other) ? read_content(other) : string();
    ofstream os(file);
    os << header << head_content << separator << other_content << footer;
    os.close();
}

bool write_file(const std::string &filename, const std::string &ref) {
    path gitlite = filesystem::current_path() / path(".gitlite");
    path src = gitlite / path("blobs") / path(ref);
    path dst = filesystem::current_path() / path(filename);
    if (!filesystem::is_regular_file(src)) {
        return false;
    }
    copy_file_overwrite(src, dst);
    return true;
}

bool is_file_exist(const std::string &filename) {
    path file = filesystem::current_path() / path(filename);
    return filesystem::is_regular_file(file);
}

void stage_content(const std::string &filename) {
    path staged = filesystem::current_path() / path(".gitlite/index") / filename;
    path src = filesystem::current_path() / path(filename);
    copy_file_overwrite(src, staged);
}

std::string read_content(const std::filesystem::path &path) {
    if (!filesystem::is_regular_file(path))
        throw std::invalid_argument("failed to read " + path.string());

    ifstream is(path);
    if (!is.is_open())
        throw std::invalid_argument("failed to read " + path.string());

    istreambuf_iterator<char> begin(is), end;
    string content(begin, end);
    is.close();
    return content;
}

std::vector<std::string> regular_files_in_path(const std::filesystem::path &path) {
    vector<string> filenames;
    for (auto &entry : filesystem::directory_iterator(path)) {
        if (entry.is_regular_file()) {
            filenames.push_back(entry.path().filename().string());
        }
    }
    return filenames;
}

void write_content(const std::filesystem::path &path, const std::string &content) {
    ofstream os(path);
    if (!os.is_open())
        throw std::runtime_error("failed to write to " + path.string());
    os << content;
    os.close();
}

std::string get_string_sha1(const string &str) {
    sha1::SHA1 s;
    s.processBytes(str.c_str(), str.size());
    uint32_t digest[5];
    s.getDigest(digest);
    char buf[48];
    snprintf(buf, 42, "%08x%08x%08x%08x%08x",
             digest[0], digest[1], digest[2], digest[3], digest[4]);
    return buf;
}

std::string get_sha1(const filesystem::path &path) {
    if (!filesystem::is_regular_file(path))
        throw invalid_argument(path.string() + " does not represent a regular file");

    ifstream is(path, ios::in | ios::binary);
    if (!is.is_open())
        throw runtime_error("failed to open " + path.string());

    ostringstream buf;
    buf << is.rdbuf();
    is.close();
    return get_string_sha1(buf.str());
}

void copy_file_overwrite(const std::filesystem::path &from, const std::filesystem::path &to) {
    // MinGW (up to gcc 10.3.0) has a peculiar bug that even after copy_options::overwrite_existing
    // is specified, copy_file() will still throw a exception saying that file already exists
    // So have to use a workaround
    if (!filesystem::is_regular_file(from)) {
        throw std::invalid_argument(from.string() + " does not exist");
    }
    if (filesystem::is_regular_file(to)) {
        filesystem::remove(to);
    }
    // overwrite_existing does not work here
    filesystem::copy_file(from, to, filesystem::copy_options::overwrite_existing);
}
