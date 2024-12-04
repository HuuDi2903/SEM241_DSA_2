/*
 * File:   DLinkedList.h
 */

#ifndef DLINKEDLIST_H
#define DLINKEDLIST_H

#include <iostream>
#include <sstream>
#include <type_traits>

#include "list/IList.h"
using namespace std;

template <class T>
class DLinkedList : public IList<T> {
 public:
  class Node;         // Forward declaration
  class Iterator;     // Forward declaration
  class BWDIterator;  // Forward declaration

 protected:
  Node *head;
  Node *tail;
  int count;
  bool (*itemEqual)(T &lhs, T &rhs);
  void (*deleteUserData)(DLinkedList<T> *);

 public:
  DLinkedList(void (*deleteUserData)(DLinkedList<T> *) = 0,
              bool (*itemEqual)(T &, T &) = 0);
  DLinkedList(const DLinkedList<T> &list);
  DLinkedList<T> &operator=(const DLinkedList<T> &list);
  ~DLinkedList();

  // Inherit from IList: BEGIN
  void add(T e);
  void add(int index, T e);
  T removeAt(int index);
  bool removeItem(T item, void (*removeItemData)(T) = 0);
  bool empty();
  int size();
  void clear();
  T &get(int index);
  int indexOf(T item);
  bool contains(T item);
  string toString(string (*item2str)(T &) = 0);
  // Inherit from IList: END

  void println(string (*item2str)(T &) = 0) {
    cout << toString(item2str) << endl;
  }
  void setDeleteUserDataPtr(void (*deleteUserData)(DLinkedList<T> *) = 0) {
    this->deleteUserData = deleteUserData;
  }

  bool contains(T array[], int size) {
    int idx = 0;
    for (DLinkedList<T>::Iterator it = begin(); it != end(); it++) {
      if (!equals(*it, array[idx++], this->itemEqual)) return false;
    }
    return true;
  }

  Iterator begin() { return Iterator(this, true); }
  Iterator end() { return Iterator(this, false); }

  BWDIterator bbegin() { return BWDIterator(this, true); }
  BWDIterator bend() { return BWDIterator(this, false); }

 protected:
  void copyFrom(const DLinkedList<T> &list);
  void removeInternalData();
  Node *getPreviousNodeOf(int index);

  //! FUNCTION STATIC
 public:
  static void free(DLinkedList<T> *list) {
    typename DLinkedList<T>::Iterator it = list->begin();
    while (it != list->end()) {
      delete *it;
      it++;
    }
  }

 protected:
  static bool equals(T &lhs, T &rhs, bool (*itemEqual)(T &, T &)) {
    if (itemEqual == 0)
      return lhs == rhs;
    else
      return itemEqual(lhs, rhs);
  }

 public:
  class Node {
   public:
    T data;
    Node *next;
    Node *prev;
    friend class DLinkedList<T>;

   public:
    Node(Node *next = 0, Node *prev = 0) {
      this->next = next;
      this->prev = prev;
    }
    Node(T data, Node *next = 0, Node *prev = 0) {
      this->data = data;
      this->next = next;
      this->prev = prev;
    }
  };

 public:
  class Iterator {
   private:
    DLinkedList<T> *pList;
    Node *pNode;

   public:
    Iterator(DLinkedList<T> *pList = 0, bool begin = true) {
      if (begin) {
        if (pList != 0)
          this->pNode = pList->head->next;
        else
          pNode = 0;
      } else {
        if (pList != 0)
          this->pNode = pList->tail;
        else
          pNode = 0;
      }
      this->pList = pList;
    }

    Iterator &operator=(const Iterator &iterator) {
      this->pNode = iterator.pNode;
      this->pList = iterator.pList;
      return *this;
    }
    void remove(void (*removeItemData)(T) = 0) {
      pNode->prev->next = pNode->next;
      pNode->next->prev = pNode->prev;
      Node *pNext = pNode->prev;  // MUST prev, so iterator++ will go to end
      if (removeItemData != 0) removeItemData(pNode->data);
      delete pNode;
      pNode = pNext;
      pList->count -= 1;
    }

    T &operator*() { return pNode->data; }
    bool operator!=(const Iterator &iterator) {
      return pNode != iterator.pNode;
    }
    // Prefix ++ overload
    Iterator &operator++() {
      pNode = pNode->next;
      return *this;
    }
    // Postfix ++ overload
    Iterator operator++(int) {
      Iterator iterator = *this;
      ++*this;
      return iterator;
    }
  };

  class BWDIterator {
    // TODO implement
   private:
    DLinkedList<T> *pList;
    Node *pNode;

   public:
    BWDIterator(DLinkedList<T> *pList, bool begin) {
      if (begin) {
        if (pList != 0)
          this->pNode = pList->tail->prev;
        else
          this->pNode = 0;
      } else {
        if (pList != 0)
          this->pNode = pList->head;
        else
          this->pNode = 0;
      }
      this->pList = pList;
    }

    BWDIterator &operator=(const BWDIterator &bwditerator) {
      this->pNode = bwditerator.pNode;
      this->pList = bwditerator.pList;
      return *this;
    }
    void remove(void (*removeItemData)(T) = 0) {
      pNode->prev->next = pNode->next;
      pNode->next->prev = pNode->prev;
      Node *pNext = pNode->next;  // MUST next, so iterator-- will go to start
      if (removeItemData != 0) removeItemData(pNode->data);
      delete pNode;
      pNode = pNext;
      pList->count -= 1;
    }

    T &operator*() { return pNode->data; }

    bool operator!=(const BWDIterator &bwditerator) {
      return pNode != bwditerator.pNode;
    }

    // Prefix -- overload: move bwditerator to the previous node
    BWDIterator &operator--() {
      if (pNode == nullptr) {
        pNode = pList->tail;
      } else {
        pNode = pNode->prev;
      }

      return *this;
    }
      
    // Postfix -- overload: move bwditerator to the previous node, but return the node before
    BWDIterator operator--(int) {
      BWDIterator bwditerator = *this;
      --*this;
      return bwditerator;
    }
  };
};

template <class T>
using List = DLinkedList<T>;

//! ////////////////////////////////////////////////////////////////////
//! //////////////////////     METHOD DEFINTION      ///////////////////
//! ////////////////////////////////////////////////////////////////////

// We have a head and a tail dummy node

template <class T>
DLinkedList<T>::DLinkedList(void (*deleteUserData)(DLinkedList<T> *),
                            bool (*itemEqual)(T &, T &)) {
  // TODO implement
  // Initialize a new list with dummy nodes
  this->head = new Node();               
  this->tail = new Node(); 
  this->head->next = this->tail;
  this->tail->prev = this->head;               
  this->count = 0;                    

  this->deleteUserData = deleteUserData;
  this->itemEqual = itemEqual; 
}

template <class T>
DLinkedList<T>::DLinkedList(const DLinkedList<T> &list) {
  // TODO implement
  // Initialize a new list
  this->head = new Node();               
  this->tail = new Node(); 
  this->head->next = this->tail;
  this->tail->prev = this->head;               
  this->count = 0;                    

  this->deleteUserData = deleteUserData;
  this->itemEqual = itemEqual; 

  // // Deep copy from list
  // // copyFrom() doesn't initialize first
  // this->copyFrom(list);
  Node *current = list.head->next;
  for (int i = 0; i < list.count; ++i) {
    this->add(current->data);
    current = current->next;
  }
}

template <class T>
DLinkedList<T> &DLinkedList<T>::operator=(const DLinkedList<T> &list) {
  // TODO implement
    // Check for self-assignment
    if (this == &list) {
      return *this;
    }

    // Clear current list's data
    this->clear();

    // Copy elements from the list
    // copyFrom doesn't initialize first
    // this->copyFrom(list);

    // Copy the function pointers
    this->deleteUserData = list.deleteUserData;
    this->itemEqual = list.itemEqual;

    Node *current = list.head->next;
    for (int i = 0; i < list.count; ++i) {
      this->add(current->data);
      current = current->next;
    }

    // Return the current object
    return *this;
}

template <class T>
DLinkedList<T>::~DLinkedList() {
  // TODO implement
  // this->clear();

    if(this->deleteUserData){
      this->deleteUserData(this);
    }

    Node *current = this->head->next;
    while (current != this->tail){
        Node *nextNode = current->next;
        delete current;
        current = nextNode;
    }

    // Delete dummy nodes
    delete this->head;
    delete this->tail;
    count = 0;
}


template <class T>
void DLinkedList<T>::add(T e) { 
  // TODO implement
    // Node *newNode = new Node(e, this->tail, this->tail->prev);
    // this->tail->prev->next = newNode;
    // this->tail->prev = newNode;

    Node *newNode = new Node(e);

    if (this->count = 0) {
        // If the list is empty
        this->head->next = newNode;
        newNode->prev = this->head;
        newNode->next = this->tail;
        this->tail->prev = newNode;
    } else {
      // Find the last actual node (the one before tail)
      Node* lastNode = this->tail->prev;
      
      // Connect newNode to the list
      newNode->next = this->tail;        
      newNode->prev = lastNode;  
      lastNode->next = newNode;    
      this->tail->prev = newNode;      
    }
     
    this->count += 1;
}

template <class T>
void DLinkedList<T>::add(int index, T e) {
  // TODO implement
    if (index < 0 || index > count) { throw out_of_range("Index is out of range!"); }

    Node* newnode = new Node(e);
    
    // // Get the node that should be before our insertion point
    // Node* prevNode = getPreviousNodeOf(index);
    
    // newNode->next = prevNode->next;
    // newNode->prev = prevNode;
    // prevNode->next->prev = newNode;
    // prevNode->next = newNode; 

    if (index == 0) {
        newnode->next = head->next;
        head->next->prev = newnode;
        head->next = newnode;
        newnode->prev = head;
    }
    else if (index == count) {
        tail->prev->next = newnode;
        newnode->prev = tail->prev;
        tail->prev = newnode;
        newnode->next = tail;
    }
    else {
        Node* temp = head->next;
        for (int i = 0; i < index; i++) {
            temp = temp->next;
        }
        newnode->next = temp;
        newnode->prev = temp->prev;
        temp->prev->next = newnode;
        temp->prev = newnode;
    }

    this->count += 1;
}

template <class T>
T DLinkedList<T>::removeAt(int index) {
  // TODO implement
    if (index < 0 || index >= count) { throw out_of_range("Index is out of range!"); }

    Node *nodeToRemove = this->getPreviousNodeOf(index + 1);
    T removedData = nodeToRemove->data;

    nodeToRemove->prev->next = nodeToRemove->next;
    nodeToRemove->next->prev = nodeToRemove->prev;
    delete nodeToRemove;
    this->count -= 1;

    return removedData;
}

template <class T>
bool DLinkedList<T>::empty() {
  // TODO implement
  return this->count == 0;
}

template <class T>
int DLinkedList<T>::size() {
  // TODO implement
  return this->count;
}

template <class T>
void DLinkedList<T>::clear() {
  // // TODO implement
  // // Removes all elements in the list and resets the list to its initial state
  // this->removeInternalData();

  // // Initialize the list to its initial state
  // this->head->next = this->tail;
  // this->tail->prev = this->head;               
  // this->count = 0; 

   if(deleteUserData){
        deleteUserData(this);
    }
    Node*temp=head->next;
    while(temp!=tail){
        Node*current=temp->next;
        delete temp;
        temp=current;
    }
    head->next = tail;
    tail->prev = head;
    count = 0;
}

template <class T>
T &DLinkedList<T>::get(int index) {
  // TODO implement
    if (index < 0 || index >= count) { throw std::out_of_range("Index is out of range!"); }

    Node *current = getPreviousNodeOf(index + 1);

    return current->data;
}

template <class T>
int DLinkedList<T>::indexOf(T item) {
  // TODO implement
    Node *current = this->head->next; 
    int index = 0;

    while (current != this->tail) { 
        if (equals(current->data, item, this->itemEqual)) {
            return index; // Found the item
        }
        current = current->next; 
        ++index;
    }

    return -1;
}

template <class T>
bool DLinkedList<T>::removeItem(T item, void (*removeItemData)(T)) {
  // TODO implement
    Node *current = this->head->next;
    while (current != this->tail) {
        if (equals(current->data, item, this->itemEqual)) {
            Node *nextNode = current->next;

            if (removeItemData) {
                removeItemData(current->data);
            }

            current->prev->next = current->next;
            current->next->prev = current->prev;
            delete current;
            count -= 1;

            current = nextNode;
            return true;
        }
        current = current->next;
    }
    return false;
}

template <class T>
bool DLinkedList<T>::contains(T item) {
  // TODO implement
    return this->indexOf(item) != -1;
}

template <class T>
string DLinkedList<T>::toString(string (*item2str)(T &)) {
  // TODO implement
    ostringstream oss;
    oss << "[";
    Node *current = this->head->next;

    while (current != this->tail) {
        if (item2str) {
            oss << item2str(current->data);
        } else {
            oss << current->data;
        }
        current = current->next;
        if (current != tail) {
            oss << ", ";
        }
    }

    oss << "]";
    return oss.str();
}

//! ////////////////////////////////////////////////////////////////////
//! ////////////////////// (private) METHOD DEFINITION //////////////////
//! ////////////////////////////////////////////////////////////////////
template <class T>
void DLinkedList<T>::copyFrom(const DLinkedList<T> &list) {
  /**
   * Copies the contents of another doubly linked list into this list.
   * Initializes the current list to an empty state and then duplicates all data
   * and pointers from the source list. Iterates through the source list and
   * adds each element, preserving the order of the nodes.
   */
  // TODO implement

  // 16:30 26/09
  // If the other list is empty
  // if (list.head->next == list.tail) {
  //   return;  // Already initialize dummy nodes in this list
  // }
  // else {
  //   // Use 2 pointers to traverse both lists at the same time
  //   Node *current1 = list.head->next;
  //   Node *current2 = this->head;

  //   while(current1->next != nullptr) {
  //     current2->next = new Node();
  //     current2->next->prev = current2;
  //     current2 = current2->next;

  //     current2->data = current1->data;
  //     current1 = current1->next;
  //   }

  //   current2->next = this->tail;
  //   this->tail->prev = current2;
  //   this->tail->next = nullptr;

  //   this->deleteUserData = list.deleteUserData;
  //   this->itemEqual = list.itemEqual;
  //   this->count = list.count;
  // }

    if (this == &list) {
      return;
    }

    this->clear();

    Node *current = list.head->next;
    this->count = list.count;
    while (current != list.tail) {
        add(current->data);
        current = current->next;
    }
}




template <class T>
void DLinkedList<T>::removeInternalData() {
  /**
   * Clears the internal data of the list by deleting all nodes and user-defined
   * data. If a custom deletion function is provided, it is used to free the
   * user's data stored in the nodes. Traverses and deletes each node between
   * the head and tail to release memory.
   */
  // TODO implement
    if (this->deleteUserData) {
        this->deleteUserData(this);
    }
    // Keep the dummy nodes
    Node *current = this->head->next;
    while (current != this->tail) {
        Node *nextNode = current->next;
        delete current; 
        current = nextNode; 
    }

    this->head->next = this->tail;
    this->tail->prev = this->head;
    this->count = 0;
}



template <class T>
typename DLinkedList<T>::Node *DLinkedList<T>::getPreviousNodeOf(int index) {
  /**
   * Returns the node preceding the specified index in the doubly linked list.
   * If the index is in the first half of the list, it traverses from the head;
   * otherwise, it traverses from the tail. Efficiently navigates to the node by
   * choosing the shorter path based on the index's position.
   */
  // TODO implement

    if (index == 0) { return this->head; }
    if (index == count) { return this->tail->prev; }

    // If index is in the first half
    if (index <= this->count / 2) {
        Node* current = this->head->next; 
        for (int i = 0; i < index - 1; ++i) {
            current = current->next; 
        }
        return current;
    } else {
        // If index is in the second half
        Node* current = this->tail->prev; 
        for (int i = count; i > index; --i) {
            current = current->prev; 
        }
        return current;
    }
}

#endif /* DLINKEDLIST_H */


