#include "Commit.h"
#include "Utils.h"
#include <stdlib.h>

using namespace std;

// Part 1: Linked List Operations

List *list_new() {
    Blob *blob = new Blob; // create a new blob
    List *list = new List; 
    list->head = blob;
    list->head->next = blob;
    list->head->prev = blob;
    return list;
}

// pushing blob at the back of the list
void list_push_back(List *list, Blob *blob) {
    // the last node
    Blob *&last_blob = list->head->prev;
    last_blob->next = blob; blob->prev = last_blob;
    blob->next = list->head; list->head->prev = blob;
}

Blob *list_find_name(const List *list, const string &name) {
    Blob *head = list->head; 
    if(head->name == name) return head;

    for(Blob *blob = head->next; blob != head; blob = blob->next){
        if(blob->name == name)
            return blob;
    }
    return nullptr;
}

Blob *list_put(List *list, const string &name, const string &ref) {
    Blob *find_blob = list_find_name(list, name);
    if(find_blob == nullptr){
        //for(Blob *blob = list->head; blob 
        
    } else{
        find_blob->ref = ref;
    }
    Blob *blob = new Blob;
    blob->name = name; blob->ref = ref;
    return blob;
}

Blob *list_put(List *list, const string &name, Commit *commit) {
    return nullptr;
}

bool list_remove(List *list, const string &target) {
    return false;
}

int list_size(const List *list) {
    return 0;
}

void list_clear(List *list) {

}

void list_delete(List *list) {

}

void list_replace(List *list, const List *another) {

}

List *list_copy(const List *list) {
    return nullptr;
}

// Part 2: Gitlite Commands

// Print out the commit info. Used in log.
void commit_print(const Commit *commit) {
    cout << "commit " << commit->commit_id << endl;

    if (commit->second_parent != nullptr) {
        cout << "Merge: " << commit->parent->commit_id.substr(0, 7)
             << " " << commit->second_parent->commit_id.substr(0, 7) << endl;
    }

    cout << "Date: " << commit->time << endl << commit->message;
}

Commit *get_lca(Commit *c1, Commit *c2) {
    return nullptr;
}

// for linked list testing
int main(){
    /* Task 1: ListNew */
    List *list = list_new();
    list->head->name = "#1 node";
    cout << list->head->name << endl;

    /* Task 2: ListPushBackOne */
    Blob *blob = new Blob;
    blob->name = "#2 node";
    list_push_back(list, blob);
    cout << list->head->next->name << " " << list->head->prev->name << " " << list->head->prev->prev->name << " " << list->head->next->next->name << endl;

    /* Task 3: ListPushBackMultiple */
    cout << "===================" << endl;
    for(int i = 1; i <= 4; ++ i){
        Blob *blob = new Blob;
        string no = "";
        no += (char)(i+'0');
        blob->name = "#" + no + " node";
        list_push_back(list, blob);
    }
    cout << "forward" << endl;
    cout << list->head->name 300<< endl;
    int i = 1;
    for(Blob *blob = list->head->next; blob != list->head; blob = blob->next){
        cout << "Recursion: " << i << endl;
        cout << blob->name << endl;
        if(blob->name == "#2 node"){
            cout << blob << endl;
            cout <<"!!: " << blob->next->name << endl;
        }
        ++ i;
    }
}