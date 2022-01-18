#include "gitlite.h"
#include "Utils.h"

using namespace std;

const string msg_initial_commit = "initial commit";

const string msg_no_changes_added = "No changes added to the commit.";

const string msg_no_reason_remove = "No reason to remove the file.";

const string msg_commit_does_not_exist = "No commit with that id exists.";

const string msg_file_does_not_exist = "File does not exist in that commit.";

const string msg_untracked_file = "There is an untracked file in the way; delete it, or add and commit it first.";

const string msg_branch_does_not_exist = "A branch with that name does not exist.";

const string msg_checkout_current = "No need to checkout the current branch.";

const string msg_branch_exists = "A branch with that name already exists.";

const string msg_remove_current = "Cannot remove the current branch.";

const string msg_merge_current = "Cannot merge a branch with itself.";

const string msg_exists_uncommitted_changes = "You have uncommitted changes.";

const string msg_given_is_ancestor_of_current = "Given branch is an ancestor of the current branch.";

const string msg_fast_forward = "Current branch fast-forwarded.";

const string msg_encountered_merge_conflict = "Encountered a merge conflict.";

const string status_branches_header = "=== Branches ===";

const string status_staged_files_header = "=== Staged Files ===";

const string status_removed_files_header = "=== Removed Files ===";

const string status_modifications_not_staged_header = "=== Modifications Not Staged For Commit ===";

const string msg_status_deleted = " (deleted)";

const string msg_status_modified = " (modified)";

const string status_untracked_files_header = "=== Untracked Files ===";

string get_merge_commit_message(const Blob *given_branch, const Blob *current_branch) {
    string message("Merged " + given_branch->name + " into " + current_branch->name + ".");
    return message;
}

void init(Blob *&current_branch, List *&branches, List *&staged_files, List *&tracked_files, Commit *&head_commit) {

}

bool add(const string &filename, List *staged_files, List *tracked_files, const Commit *head_commit) {
    return false;
}

bool commit(const string &message, Blob *current_branch, List *staged_files, List *tracked_files, Commit *&head_commit) {
    return false;
}

bool remove(const string &filename, List* staged_files, List *tracked_files, const Commit *head_commit) {
    return false;
}

void log(const Commit *head_commit) {

}

void status(const Blob *current_branch, const List *branches, const List *staged_files, const List *tracked_files,
            const List *cwd_files, const Commit *head_commit) {

}

bool checkout(const string &filename, Commit *commit) {
    return false;
}

bool checkout(const string &branch_name, Blob *&current_branch, const List *branches, List *staged_files,
              List *tracked_files, const List *cwd_files, Commit *&head_commit) {
    return false;
}

bool reset(Commit *commit, Blob *current_branch, List *staged_files, List *tracked_files, const List *cwd_files,
           Commit *&head_commit) {
    return false;
}

Blob *branch(const string &branch_name, List *branches, Commit *head_commit) {
    return nullptr;
}

bool remove_branch(const string &branch_name, Blob *current_branch, List *branches) {
    return false;
}

bool merge(const string &branch_name, Blob *&current_branch, List *branches, List *staged_files, List *tracked_files,
           const List *cwd_files, Commit *&head_commit) {
    return false;
}
