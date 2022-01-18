//
// You don't need to modify any part of this file.
//

#include <fstream>
#include <unordered_set>
#include <regex>

#include "Repository.h"
#include "Utils.h"
#include "gitlite.h"

using namespace std;

using path = std::filesystem::path;
const path Repository::CWD = filesystem::current_path();
const path Repository::GITLITE = Repository::CWD / path(".gitlite");
const path Repository::REFS = Repository::GITLITE / path("refs");
const path Repository::INDEX = Repository::GITLITE / path("index");
const path Repository::COMMITS = Repository::GITLITE / path("commits");
const path Repository::BLOBS = Repository::GITLITE / path("blobs");
const path Repository::HEAD = Repository::GITLITE / path("HEAD");
const path Repository::TREE = Repository::GITLITE / path("TREE");
const path Repository::STAGE = Repository::GITLITE / path("STAGE");

std::unordered_map<std::string, Commit *> Repository::commits;

Commit *Repository::head_commit = nullptr;
List *Repository::tracked_files = nullptr;
List *Repository::branches = nullptr;
List *Repository::staged_files = nullptr;
Blob *Repository::current_branch = nullptr;

void Repository::make_file_structure() {
    if (!filesystem::create_directories(GITLITE))
        throw std::runtime_error("failed to create .gitlite directory");
    if (!filesystem::create_directories(REFS) || !filesystem::create_directories(INDEX)
        || !filesystem::create_directories(COMMITS) || !filesystem::create_directories(BLOBS))
        throw std::runtime_error("failed to create file structures for Gitlite");
}

void Repository::load_repository() {
    // Load list of tracked files
    ifstream is(TREE.string(), ios::in | ios::binary);
    if (!is.is_open()) {
        throw std::invalid_argument("failed to read .gitlite/TREE");
    }
    PersistentList tree;

    {
        cereal::BinaryInputArchive iarchive(is);
        iarchive(tree);
    }   // ensure the deserialization will finish before closing the stream

    tracked_files = tree.to_list();
    is.close();

    // Load staging record
    is.open(STAGE.string(), ios::in | ios::binary);
    if (!is.is_open()) {
        throw std::invalid_argument("failed to read .gitlite/STAGE");
    }
    PersistentList stage;

    {
        cereal::BinaryInputArchive iarchive(is);
        iarchive(stage);
    }

    staged_files = stage.to_list();
    is.close();

    // Reconstruct the hashmap of commits
    unordered_map<string, pair<string, string>> parent_refs;
    for (auto &dir : filesystem::directory_iterator(COMMITS)) {
        if (dir.is_directory()) {
            for (auto &commitFile : filesystem::directory_iterator(dir.path())) {
                PersistentCommit new_commit = PersistentCommit::from_path(commitFile.path());
                commits.insert({new_commit.commit_id, new_commit.to_commit()});
                parent_refs.insert({new_commit.commit_id, {new_commit.parent_ref, new_commit.second_parent_ref}});
            }
        }
    }

    // Reconstruct the DAG of commits used in tasks
    for (auto &entry : commits) {
        auto parent_ref = parent_refs.find(entry.first)->second;
        if (!parent_ref.first.empty()) {
            entry.second->parent = commits.find(parent_ref.first)->second;
        }
        if (!parent_ref.second.empty()) {
            entry.second->second_parent = commits.find(parent_ref.second)->second;
        }
    }

    // Load head commit
    string current_branch_name = read_content(HEAD);
    path branch = REFS / path(current_branch_name);
    auto entry = commits.find(read_content(branch));
    if (entry == commits.end()) {
        throw std::runtime_error("failed to find the commit corresponding to HEAD");
    }
    head_commit = entry->second;

    // Reconstruct the list of branches
    branches = list_new();
    for (auto &ref : filesystem::directory_iterator(REFS)) {
        string commit_id = read_content(ref.path());
        auto iter = commits.find(commit_id);
        if (iter == commits.end()) {
            throw std::runtime_error("failed to find the commit corresponding to the head of the branch");
        }
        list_put(branches, ref.path().filename().string(), iter->second);
    }

    // Load pointer to current branch
    current_branch = list_find_name(branches, current_branch_name);
}

bool Repository::init() {
    if (filesystem::exists(GITLITE)) {
        cout << "A Gitlite version-control system already exists in the current directory." << endl;
        return false;
    }

    try {
        make_file_structure();
    } catch (const std::runtime_error &err) {
        cout << err.what() << endl;
        return false;
    }

    ::init(current_branch, branches, staged_files, tracked_files, head_commit);
    PersistentCommit(head_commit).commit();
    write_content(HEAD, current_branch->name);
    write_content(REFS / path(current_branch->name), current_branch->commit->commit_id);
    commits.insert({head_commit->commit_id, head_commit});
    return true;
}

bool Repository::add(const string &filename) {
    path file = CWD / path(filename);
    if (!filesystem::is_regular_file(file)) {
        cout << "File does not exist." << endl;
        return false;
    }

    if (::add(filename, staged_files, tracked_files, head_commit)) {
        // Stage the files to .gitlite/index
        path staged_file = INDEX / path(filename);
        copy_file_overwrite(file, staged_file);
    } else {
        // Remove from staged files
        path staged_file = INDEX / path(filename);
        filesystem::remove(staged_file);
    }

    flush_track_records();
    return true;
}

bool Repository::commit(const string &message) {
    if (::commit(message, current_branch, staged_files, tracked_files, head_commit)) {
        PersistentCommit newCommit(head_commit);
        newCommit.commit();
        write_content(REFS / path(current_branch->name), newCommit.commit_id);
        commits.insert({newCommit.commit_id, head_commit});
        flush_staged_changes();
        return true;
    }
    return false;
}

bool Repository::remove(const string &filename) {
    if (::remove(filename, staged_files, tracked_files, head_commit)) {
        flush_track_records();
        path staged_file = INDEX / path(filename);
        filesystem::remove(staged_file);
        return true;
    }
    return false;
}

void Repository::log() {
    ::log(head_commit);
}

void Repository::global_log() {
    for (auto &entry : commits) {
        cout << "===" << endl;
        commit_print(entry.second);
        cout << endl << endl;
    }
}

bool Repository::find(const string &message) {
    bool found = false;
    for (auto &entry : commits) {
        if (entry.second->message == message) {
            cout << entry.second->commit_id << endl;
            found = true;
        }
    }
    if (!found) {
        cout << "Found no commit with that message." << endl;
    }
    return found;
}

void Repository::status() {
    List *files = get_cwd_files();
    ::status(current_branch, branches, staged_files, tracked_files, files, head_commit);
    list_delete(files);
}

bool Repository::checkout_file(const string &filename) {
    return ::checkout(filename, head_commit);
}

bool Repository::checkout_file(const string &commit_id, const string &filename) {
    string full_id = resolve_commit_id(commit_id);
    auto commit = commits.find(full_id);
    if (commit == commits.end()) {
        return ::checkout(filename, nullptr);
    } else {
        return ::checkout(filename, commit->second);
    }
}

bool Repository::checkout_branch(const string &branchName) {
    List *filenames = get_cwd_files();
    if (::checkout(branchName, current_branch, branches, staged_files, tracked_files, filenames, head_commit)) {
        clear_staging_area();
        write_content(HEAD, branchName);
        flush_track_records();
        list_delete(filenames);
        return true;
    }
    list_delete(filenames);
    return false;
}

bool Repository::branch(const string &branch_name) {
    if (::branch(branch_name, branches, head_commit)) {
        write_content(REFS / path(branch_name), head_commit->commit_id);
        return true;
    }
    return false;
}

bool Repository::remove_branch(const string &branch_name) {
    if (::remove_branch(branch_name, current_branch, branches)) {
        path branch = REFS / path(branch_name);
        filesystem::remove(branch);
        return true;
    }
    return false;
}

bool Repository::reset(const std::string &commit_id) {
    string full_id = resolve_commit_id(commit_id);
    List *filenames = get_cwd_files();
    auto commit = commits.find(full_id);
    if (commit == commits.end()) {
        ::reset(nullptr, current_branch, staged_files, tracked_files, filenames, head_commit);
        list_delete(filenames);
        return false;
    } else {
        if (::reset(commit->second, current_branch, staged_files, tracked_files, filenames, head_commit)) {
            clear_staging_area();
            write_content(REFS / path(current_branch->name), head_commit->commit_id);
            flush_track_records();
            list_delete(filenames);
            return true;
        }
        list_delete(filenames);
        return false;
    }
}

bool Repository::merge(const std::string &branch_name) {
    List *filenames = get_cwd_files();
    Commit *prev_head_commit = head_commit;
    if (::merge(branch_name, current_branch, branches, staged_files, tracked_files, filenames, head_commit)) {
        write_content(HEAD, current_branch->name);
        flush_track_records();
        list_delete(filenames);

        if (prev_head_commit != head_commit) {
            PersistentCommit new_commit(head_commit);
            new_commit.commit();
            write_content(REFS / path(current_branch->name), new_commit.commit_id);
            commits.insert({new_commit.commit_id, head_commit});
            flush_staged_changes();
        }
        return true;
    }
    list_delete(filenames);
    return false;
}

void Repository::flush_track_records() {
    PersistentList tree(tracked_files);
    ofstream os(TREE, ios::out | ios::binary);

    {
        cereal::BinaryOutputArchive oarchive(os);
        oarchive(tree);
    }

    os.close();
}

List *Repository::get_cwd_files() {
    vector<string> filenames = regular_files_in_path(CWD);
    List *tree = list_new();
    for (auto &filename : filenames) {
        ::list_put(tree, filename, string());
    }
    return tree;
}

void Repository::clear_staging_area() {
    for (auto &entry : filesystem::directory_iterator(INDEX)) {
        if (entry.is_regular_file()) {
            filesystem::remove(entry.path());
        }
    }
}

std::string Repository::resolve_commit_id(const string &commit_id) {
    if (commit_id.size() < 2 || commit_id.size() > 40) {
        return string();    // return empty string if no match
    }

    string prefix = commit_id.substr(0, 2);
    path dir = Repository::COMMITS / path(prefix);
    if (!filesystem::is_directory(dir)) {
        return string();
    }
    auto candidates = regular_files_in_path(dir);

    if (commit_id.size() == 2) {
        return candidates.size() == 1 ? candidates[0] : string();
    }

    string result;
    for (auto &candidate : candidates) {
        auto res = mismatch(commit_id.begin(), commit_id.end(), candidate.begin());
        if (res.first == commit_id.end()) {
            if (!result.empty()) {
                return string();    // multiple matches
            }
            result = candidate;
        }
    }
    return result;
}

void Repository::close() {
    if (!check_file_structure()) {
        return;
    }

    // Store the list of tracked files
    ofstream os(TREE.string(), ios::out | ios::binary);
    if (!os.is_open()) {
        throw std::invalid_argument("failed to write .gitlite/TREE");
    }
    PersistentList tree(tracked_files);

    {
        cereal::BinaryOutputArchive oarchive(os);
        oarchive(tree);
    }

    os.close();

    // Store staging record
    os.open(STAGE.string(), ios::out | ios::binary);
    if (!os.is_open()) {
        throw std::invalid_argument("failed to write .gitlite/STAGE");
    }
    PersistentList stage(staged_files);

    {
        cereal::BinaryOutputArchive oarchive(os);
        oarchive(stage);
    }

    os.close();

    // Free all pointers
    list_delete(tracked_files);
    list_delete(staged_files);
    list_delete(branches);
    for (auto &entry : commits) {
        list_delete(entry.second->tracked_files);
        delete entry.second;
    }
}

bool Repository::check_file_structure() {
    return filesystem::is_directory(GITLITE);
}

void Repository::flush_staged_changes() {
    for (auto &entry : filesystem::directory_iterator(INDEX)) {
        if (entry.is_regular_file()) {
            string hash(get_sha1(entry.path()));
            path file = BLOBS / path(hash);
            copy_file_overwrite(entry.path(), file);
            filesystem::remove(entry.path());
        }
    }
}

// Reset all the in-memory states. Used for the tester when running multiple tests.
void Repository::reset_states() {
    head_commit = nullptr;
    tracked_files = staged_files = branches = nullptr;
    current_branch = nullptr;
    commits.clear();
}


PersistentBlob::PersistentBlob(Blob *blob) {
    // Make sure to catch wrong implementation from students
    if (blob == nullptr)
        throw std::runtime_error("tracked_files contains nullptr");

    name = blob->name;
    ref = blob->ref;
}

Blob *PersistentBlob::to_blob() const {
    Blob *blob = new Blob;
    blob->name = name;
    blob->ref = ref;
    return blob;
}

PersistentCommit::PersistentCommit(Commit *commit) {
    if (commit == nullptr)
        throw std::runtime_error("failed to convert to PersistentCommit: commit is nullptr");

    message = commit->message;
    time = commit->time;
    commit_id = commit->commit_id;

    if (commit->parent)
        parent_ref = commit->parent->commit_id;
    if (commit->second_parent)
        second_parent_ref = commit->second_parent->commit_id;

    tracked_files = PersistentList(commit->tracked_files);
}

Commit *PersistentCommit::to_commit() const {
    auto *commit = new Commit;
    commit->message = message;
    commit->commit_id = commit_id;
    commit->time = time;
    commit->tracked_files = tracked_files.to_list();
    return commit;
}

PersistentCommit PersistentCommit::from_path(const path &path) {
    ifstream is(path.string(), ios::in | ios::binary);
    if (!is.is_open())
        throw std::invalid_argument("failed to read the commit");
    PersistentCommit commit;

    {
        cereal::BinaryInputArchive iarchive(is);
        iarchive(commit);
    }   // ensure the deserialization will finish before leaving the function

    is.close();
    return commit;
}

PersistentCommit PersistentCommit::from_id(const string &commit_id) {
    string prefix = commit_id.substr(0, 2);
    path dir = Repository::COMMITS / path(prefix);
    if (!filesystem::is_directory(dir))
        throw std::invalid_argument("commit " + commit_id + " does not exist in .gitlite/commits");

    path file = dir / path(commit_id);
    if (!filesystem::is_regular_file(file))
        throw std::invalid_argument("commit " + commit_id + " does not exist in .gitlite/commits");

    return from_path(file);
}

void PersistentCommit::commit() const {
    path dir = Repository::COMMITS / path(commit_id.substr(0, 2));
    filesystem::create_directory(dir);
    path file = dir / path(commit_id);

    ofstream os(file, ios::out | ios::binary);
    if (!os.is_open()) {
        throw std::runtime_error("failed to open " + file.string());
    }

    {
        cereal::BinaryOutputArchive oarchive(os);
        oarchive(*this);
    }

    os.close();
}

PersistentList::PersistentList(List *list) {
    if (list == nullptr || list->head == nullptr)
        throw std::runtime_error("tracked_files/tracked_files->head is nullptr");

    for (Blob *blob = list->head->next; blob != list->head; blob = blob->next) {
        this->list.emplace_back(blob);  // PersistentBlob ctor will handle nullptr
    }
}

List *PersistentList::to_list() const {
    List *tmp = list_new();
    for (const PersistentBlob &blob : list) {
        list_push_back(tmp, blob.to_blob());
    }
    return tmp;
}

bool validate_args(const std::vector<std::string> &args) {
    std::string command = args[0];
    if (command == "init" || command == "log" || command == "global-log" || command == "status") {
        if (args.size() != 1) {
            cout << "Incorrect operands." << endl;
            return false;
        }
        return true;
    }
    if (command == "add" || command == "rm" || command == "find"|| command == "branch" || command == "rm-branch"
        || command == "reset" || command == "merge") {
        if (args.size() != 2) {
            cout << "Incorrect operands." << endl;
            return false;
        }
        return true;
    }
    if (command == "checkout") {
        if (args.size() < 2 || args.size() > 4) {
            cout << "Incorrect operands." << endl;
            return false;
        }
        return true;
    }
    if (command == "commit") {
        if (args.size() < 2) {
            cout << "Please enter a commit message." << endl;
            return false;
        }
        if (args.size() > 2) {
            cout << "Incorrect operands." << endl;
            return false;
        }
        return true;
    }
    if (command == "quit") {
        if (args.size() != 1) {
            cout << "Incorrect operands." << endl;
            return false;
        }
        return true;
    }
    cout << "No command with that name exists." << endl;
    return false;
}

bool parse_args(const std::vector<std::string> &args) {
    std::string command = args[0];
    if (command == "init") {
        return Repository::init();
    } else {
        if (!Repository::check_file_structure()) {
            std::cout << "Not in an initialized Gitlite directory." << std::endl;
            return false;
        }
        if (command == "add") {
            return Repository::add(args[1]);
        }
        if (command == "commit") {
            return Repository::commit(args[1]);
        }
        if (command == "rm") {
            return Repository::remove(args[1]);
        }
        if (command == "log") {
            Repository::log();
            return true;
        }
        if (command == "global-log") {
            Repository::global_log();
            return true;
        }
        if (command == "find") {
            return Repository::find(args[1]);
        }
        if (command == "status") {
            Repository::status();
            return true;
        }
        if (command == "checkout") {
            if (args.size() == 2) {
                return Repository::checkout_branch(args[1]);
            }
            if (args[1] == "--") {
                return Repository::checkout_file(args[2]);
            }
            if (args[2] == "--") {
                return Repository::checkout_file(args[1], args[3]);
            }
            std::cout << "Incorrect operands." << std::endl;
            return false;
        }
        if (command == "branch") {
            return Repository::branch(args[1]);
        }
        if (command == "rm-branch") {
            return Repository::remove_branch(args[1]);
        }
        if (command == "reset") {
            return Repository::reset(args[1]);
        }
        if (command == "merge") {
            return Repository::merge(args[1]);
        }
    }
    return false;
}

std::vector<std::string> split_args(std::string input) {
    std::regex pattern(R"(("(?:\\.|[^"\\])*")|([^\s]+))");  // this captures escaped chars, but we do not actually escape them...
    std::vector<std::string> args;
    std::smatch match;
    while (std::regex_search(input, match, pattern)) {
        std::string arg(*match.begin());
        if (arg.length() > 2 && arg[0] == '"' && arg[arg.length() - 1] == '"') {
            args.emplace_back(arg.substr(1, arg.length() - 2));
        } else {
            args.emplace_back(arg);
        }
        input = match.suffix().str();
    }
    return args;
}

