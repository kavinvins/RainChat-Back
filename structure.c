/**
  @file  structure.h
  @brief A header file to apply common interface and language between
         the client and a server sides.
*/

#include "structure.h"

Node * create(void *data) {
    Node *new_node = (Node*)malloc(sizeof(Node));
    if (new_node == NULL) {
        printLog("Error creating a new node.\n");
        return NULL;
    }
    new_node->data = data;
    new_node->next = NULL;
    new_node->prev = NULL;
    new_node->attached = 0;
    pthread_mutex_init(&new_node->lock, NULL);
    return new_node;
}

Node * append(List *list, Node *this) {
    pthread_mutex_lock(&list->lock);
    if (list->count == 0) {
        // link to self
        this->next = this;
        this->prev = this;
        this->attached = 1;
        list->count++;
        printLog("node count: %d\n", list->count);
        list->head = this;
        pthread_mutex_unlock(&list->lock);
        return this;
    }
    printLog("locking\n");
    pthread_mutex_lock(&list->head->lock);
    Node *first = list->head;
    Node *last = first->prev;
    if (list->count > 1) pthread_mutex_lock(&last->lock);
    printLog("locked\n");
    pthread_mutex_unlock(&list->lock);

    this->next = first;
    printLog("next linked\n");
    this->prev = last;
    printLog("prev linked\n");
    first->prev = this;
    printLog("first attached\n");
    last->next = this;
    printLog("last attached\n");
    list->count++;
    // increment count
    this->attached = 1;
    pthread_mutex_unlock(&first->lock);
    if (list->count > 2) pthread_mutex_unlock(&last->lock);
    printLog("unlocked\n");
    printLog("node count: %d\n", list->count);
    return this;
}

Node * delete(List *list, Node *this) {
    if (!this->attached) {
        // node detached is in detached state
        pthread_mutex_destroy(&this->lock);
        free(this);
        return NULL;
    }
    pthread_mutex_lock(&list->lock);
    if (this == list->head) {
        // switch head before delete
        list->head = this->next;
    }
    printLog("locking\n");
    pthread_mutex_lock(&this->lock);
    Node *prev = this->prev;
    Node *next = this->next;
    if (list->count > 1) pthread_mutex_lock(&next->lock);
    if (list->count > 2) pthread_mutex_lock(&prev->lock);
    printLog("locked\n");
    pthread_mutex_unlock(&list->lock);
    // assigned
    prev->next = next;
    next->prev = prev;
    printLog("unlinked\n");
    this->next = NULL;
    this->prev = NULL;
    printLog("detached\n");
    list->count--;
    // decrement count
    if (list->count == 0) {
        // assign null pointer to head
        list->head = NULL;
    }
    // assign null pointer to head
    pthread_mutex_unlock(&this->lock);
    if (list->count > 0) pthread_mutex_unlock(&next->lock);
    if (list->count > 1) pthread_mutex_unlock(&prev->lock);
    printLog("unlocked\n");
    pthread_mutex_destroy(&this->lock);
    // destroy mutex
    free(this);
    printLog("freed\n");
    printLog("node count: %d\n", list->count);
    return next;
}

void map(Node *this, callback function, void *argument) {
    Node *cursor = this->next;
    User *user = (User*)cursor->data;
    while (cursor != this && cursor != NULL) {
        function(cursor, argument);
        cursor = cursor->next;
    }
}
