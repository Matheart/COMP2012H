//
// You don't need to modify any part of this file.
//

#ifndef COMP2012H_FA21_PA2_REPOSITORY_H
#define COMP2012H_FA21_PA2_REPOSITORY_H

#include <string>
#include <filesystem>
#include <unordered_map>
#include <cereal/archives/binary.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>

#include "Commit.h"

class PersistentBlob;
class PersistentList;
class PersistentCommit;

// This class works as a wrapper for all the tasks
// It handles the persistence and part of the filesystem operations
class Repository {
    using path = std::filesystem::path;

public:
    static const path CWD;          // current working directory
    static const path GITLITE;      // CWD/.gitlite - main directory for Gitlite
    static const path REFS;         // .gitlite/refs - stores branch references
    static const path INDEX;        // .gitlite/index - stores staged files
    static const path COMMITS;      // .gitlite/commits - stores persisted commits
    static const path BLOBS;        // .gitlite/blobs - stores blobs from the commits
    static const path HEAD;         // .gitlite/HEAD - stores the name of the current branch
    static const path TREE;         // .gitlite/TREE - stores the persisted list of currently tracked files
    static const path STAGE;        // .gitlite/STAGE - stores the persisted list of staged files, just for convenience

    static void make_file_structure();
    static void load_repository();
    static void close();
    static void reset_states();
    static bool check_file_structure();

    // Wrappers for all the tasks
    static bool init();
    static bool add(const std::string &filename);
    static bool commit(const std::string &message);
    static bool remove(const std::string &filename);
    static void log();
    static void global_log();        // implemented for you to facilitate debugging
    static bool find(const std::string &message);   // implemented for you to facilitate debugging
    static void status();
    static bool checkout_file(const std::string &filename);
    static bool checkout_file(const std::string &commit_id, const std::string &filename);
    static bool checkout_branch(const std::string &branch_name);
    static bool branch(const std::string &branch_name);
    static bool remove_branch(const std::string &branch_name);
    static bool reset(const std::string &commit_id);
    static bool merge(const std::string &branch_name);

private:
    static void flush_track_records();
    static void flush_staged_changes();
    static void clear_staging_area();
    static List *get_cwd_files();
    static std::string resolve_commit_id(const std::string &commit_id);

    static std::unordered_map<std::string, Commit *> commits;   // hashmap from commit id to pointers, used only internally

    static Commit *head_commit;      // current head commit
    static List *tracked_files;      // currently tracked files
    static List *staged_files;       // a linked list recording the state of the staging area
    static List *branches;           // a linked list of all the branches, the blobs has pointers to Commit
    static Blob *current_branch;     // current branch we are on
};

// Persistent version of the Blob class
class PersistentBlob {
public:
    PersistentBlob() = default;
    explicit PersistentBlob(Blob *blob);

    Blob *to_blob() const;

    template <class Archive>
    void serialize(Archive &archive) {
        archive(name, ref);
    }

private:
    std::string name;
    std::string ref;
};

// Persistent version of class List
class PersistentList {
public:
    PersistentList() = default;
    explicit PersistentList(List *list);

    List *to_list() const;

    template <class Archive>
    void serialize(Archive &archive) {
        archive(list);
    }

private:
    std::vector<PersistentBlob> list;
};

// Persistent version of class Commit
class PersistentCommit {
    friend class Repository;

public:
    PersistentCommit() = default;
    explicit PersistentCommit(Commit *commit);

    Commit *to_commit() const;

    void commit() const;

    template <class Archive>
    void serialize(Archive &archive) {
        archive(message, time, commit_id, parent_ref, second_parent_ref, tracked_files);
    }

    static PersistentCommit from_path(const std::filesystem::path &path);
    static PersistentCommit from_id(const std::string &commit_id);

private:
    std::string message;
    std::string time;
    std::string commit_id;
    std::string parent_ref, second_parent_ref;
    PersistentList tracked_files;
};

bool validate_args(const std::vector<std::string> &args);

bool parse_args(const std::vector<std::string> &args);

std::vector<std::string> split_args(std::string input);

#endif //COMP2012H_FA21_PA2_REPOSITORY_H
