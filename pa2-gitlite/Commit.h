//
// This header file contains the necessary structures for the assignments.
// You should NOT modify any part of this file.
//

#ifndef COMP2012H_FA21_PA2_COMMIT_H
#define COMP2012H_FA21_PA2_COMMIT_H

#include <iostream>
#include <string>

using std::string;

struct Commit;


// This is mainly used to record the information about the blobs (binary large objects).
// It is also used a common data structure for the linked list for several other usages.
//
// After learning about STL and template classes, you will find a much more elegant way
// of implementing this.
struct Blob {
    // The filename of the blob, or the branch name.
    string name;

    // The SHA1 hash of the blob. This is the unique reference to a blob.
    // Remains empty when representing a branch.
    string ref;

    // Pointer to a Commit structure, ONLY USED WHEN REPRESENTING A BRANCH.
    Commit *commit = nullptr;

    Blob *prev = nullptr, *next = nullptr;

    // For testing
    bool operator==(const Blob &other) const {
        return name == other.name && ref == other.ref;
    }
};


// A struct of linked list.
struct List {
    Blob *head = nullptr;
};


// Abstraction of a commit.
struct Commit {
    string message;  // commit message

    string time;     // time of the commit

    string commit_id; // the commit id, generated as an SHA1 string of message and time

    Commit *parent = nullptr, *second_parent = nullptr;  // nullptr if parents do not exist

    List *tracked_files = nullptr;     // files being tracked in this commit
};


// Part 1: Linked List Operations

List *list_new();

void list_push_back(List *list, Blob *blob);

Blob *list_find_name(const List *list, const string &name);

Blob *list_put(List *list, const string &name, const string &ref);

Blob *list_put(List *list, const string &name, Commit *commit);

bool list_remove(List *list, const string &target);

int list_size(const List *list);

void list_clear(List *list);

void list_delete(List *list);

void list_replace(List *list, const List *another);

List *list_copy(const List *list);

// Part 2: Gitlite Commands

void commit_print(const Commit *commit);

Commit *get_lca(Commit *c1, Commit *c2);

#endif //COMP2012H_FA21_PA2_COMMIT_H
