//
// You should NOT modify any part of this file.
//

#ifndef COMP2012H_FA21_PA2_GITLITE_H
#define COMP2012H_FA21_PA2_GITLITE_H

#include "Commit.h"

using std::string;

void init(Blob *&current_branch, List *&branches, List *&staged_files, List *&tracked_files, Commit *&head_commit);

bool add(const string &filename, List *staged_files, List *tracked_files, const Commit *head_commit);

bool commit(const string &message, Blob *current_branch, List *staged_files, List *tracked_files, Commit *&head_commit);

bool remove(const string &filename, List* staged_files, List *tracked_files, const Commit *head_commit);

void log(const Commit *head_commit);

void status(const Blob *current_branch, const List *branches, const List *staged_files, const List *tracked_files,
            const List *cwd_files, const Commit *head_commit);

bool checkout(const string &filename, Commit *commit);

bool checkout(const string &branch_name, Blob *&current_branch, const List *branches, List *staged_files,
              List *tracked_files, const List *cwd_files, Commit *&head_commit);

bool reset(Commit *commit, Blob *current_branch, List *staged_files, List *tracked_files, const List *cwd_files,
           Commit *&head_commit);

Blob *branch(const string &branch_name, List *branches, Commit *head_commit);

bool remove_branch(const string &branch_name, Blob *current_branch, List *branches);

bool merge(const string &branch_name, Blob *&current_branch, List *branches, List *staged_files, List *tracked_files,
           const List *cwd_files, Commit *&head_commit);

#endif //COMP2012H_FA21_PA2_GITLITE_H
