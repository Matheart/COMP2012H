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
    Blob *last_blob = list->head->prev;

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
    if(find_blob == nullptr){ //no blob with the same name exists in the linked list 
        Blob *new_node = new Blob;
        new_node->name = name;
        new_node->ref = ref;

        if(name < list->head->name){
            // insert new_node to the front of the linked list
            Blob *last_node = list->head->prev;
            last_node->next = new_node;
            new_node->next = list->head;
            new_node->prev = last_node;
            list->head->prev = new_node;
            list->head = new_node;
        } else if(name > list->head->prev->name){
            // insert new_node to the back of the linked list
            list_push_back(list, new_node);
        } else{
            for(Blob *blob = list->head->next; blob != list->head; blob = blob->next){
                if(blob->name <= name && name <= blob->next->name){
                    // insert the node
                    new_node->next = blob->next;
                    new_node->prev = blob;
                    blob->next->prev = new_node;
                    blob->next = new_node;  
                    break;
                }
            }
        }
        return new_node;
        
    } else{
        find_blob->ref = ref; // update the content
        return find_blob;
    }
}

Blob *list_put(List *list, const string &name, Commit *commit) {
    Blob *find_blob = list_find_name(list, name);
    if(find_blob == nullptr){ //no blob with the same name exists in the linked list 
        Blob *new_node = new Blob;
        new_node->name = name;
        new_node->commit = commit;

        if(name < list->head->name){
            // insert new_node to the front of the linked list
            Blob *last_node = list->head->prev;
            last_node->next = new_node;
            new_node->next = list->head;
            new_node->prev = last_node;
            list->head->prev = new_node;
            list->head = new_node;
        } else if(name > list->head->prev->name){
            // insert new_node to the back of the linked list
            list_push_back(list, new_node);
        }  else{
            for(Blob *blob = list->head->next; blob != list->head; blob = blob->next){
                if(blob->name <= name && name <= blob->next->name){
                    // insert the node
                    new_node->next = blob->next;
                    new_node->prev = blob;
                    blob->next->prev = new_node;
                    blob->next = new_node;  
                    break;
                }
            }
        }
        return new_node;
        
    } else{
        find_blob->commit = commit; // update the content
        return find_blob;
    }
}

bool list_remove(List *list, const string &target) {
    Blob *find_blob = list_find_name(list, target);
    if(find_blob == nullptr) return false;
    if(find_blob == find_blob->next){ // only one node left
        delete find_blob;
        list->head = nullptr;
        return true;
    }

    // update head
    if(find_blob == list->head) list->head = find_blob->next;
    // we need to delete find_blob
    find_blob->prev->next = find_blob->next;
    find_blob->next->prev = find_blob->prev;
    delete find_blob;
    return true;
}

int list_size(const List *list) {
    if(list->head == nullptr) return 0;
    if(list->head == list->head->next) return 1;

    int lngth = 1;
    Blob *blob = list->head->next;
    while(blob != list->head){
        ++ lngth;
        blob = blob->next;
    }
    return lngth;
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

/*  ------------ For testing ---------- */
void forward_transverse(List *list){
    cout << "Array size:" << list_size(list) << endl;
    cout << "Forward:" << endl;
    cout << list->head->name << " " << list->head->ref << endl;
    for(Blob *blob = list->head->next; blob != list->head; blob = blob->next){
        cout << blob->name << " " << blob->ref << endl;
    }
    cout << endl;
}

void backward_transverse(List *list){
    cout << "Backward:" << endl;
    for(Blob *blob = list->head->prev; blob != list->head; blob = blob->prev){
        cout << blob->name << " " << blob->ref << endl;
    }
    cout << list->head->name << " " << list->head->ref << endl;
    cout << endl;
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
    cout << "Task 3" << endl;
    for(int i = 3; i <= 7; ++ i){
        Blob *blob = new Blob;
        string no = "";
        no += (char)(i+'0');
        blob->name = "#" + no + " node";
        list_push_back(list, blob);
    }

    forward_transverse(list);
    backward_transverse(list);

    /* Task 4, 5: ListFindNameFound */
    cout << "==============" << endl;
    cout << "Task 4, 5" << endl;
    Blob *name_blob_four = list_find_name(list, "#4 node");
    cout << name_blob_four->next->name << endl;
    Blob *name_blob_seven = list_find_name(list, "#7 node");
    cout << name_blob_seven->next->name << endl; 
    Blob *name_blob_one = list_find_name(list, "#1 node");
    cout << name_blob_one->prev->name << endl;
    Blob *name_blob_eight = list_find_name(list, "#8 node");
    if(name_blob_eight == nullptr){
        cout << "nullptr" << endl;
    } else{
        cout << name_blob_eight->prev->name << endl; 
    }        
    
    /* Task 6, 7, 8: ListPutNew */
    cout << "==============" << endl;
    cout << "Task 6, 7, 8" << endl;
    Blob *task6_node = list_put(list, "#5 node", "Reference");
    //cout << task6_node->prev->name << " " << task6_node->next->name << endl;
    Blob *task6_node_two = list_put(list, "#62 node", "Reference2");

    //cout << task6_node->prev->name << " " << task6_node->next->name << endl;
    cout << "Commit 1" << endl;
    Commit *task6_commit = new Commit; task6_commit->message = "First Commit";
    cout << "Commit 2" << endl;
    Blob *task6_node_three = list_put(list, "#0 node", task6_commit);
    cout << task6_node_three->commit->message << endl;
    Blob *task6_node_four = list_put(list, "#8 node", "Reference3");
    Blob *task6_node_five = list_put(list, "#63 node", "Reference4");
    
    forward_transverse(list);
    backward_transverse(list);

    /* Task 12, 13, 14: ListRemove */
    cout << "==============" << endl;
    cout << "Task 12, 13, 14" << endl;

    cout << list_remove(list, "haha") << endl;
    forward_transverse(list);
    backward_transverse(list);

    cout << list_remove(list, "#2 node") << endl;
    forward_transverse(list);
    backward_transverse(list); 

    blob = list->head;
    while(blob->name != "#8 node"){
        list_remove(list, blob->name);
        blob = blob->next;
    }
    list_remove(list, "#8 node");
    cout << (list->head == nullptr) << endl;  

    /* Task 15: ListSize */  
}