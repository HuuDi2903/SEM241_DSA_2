/*
 * File:   XArrayList.h
 */

#ifndef XARRAYLIST_H
#define XARRAYLIST_H
#include <memory.h>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <type_traits>

#include "list/IList.h"
using namespace std;

template <class T>
class XArrayList : public IList<T> {
 public:
  class Iterator;  // forward declaration

 protected:
  T *data;
  int capacity;
  int count;
  bool (*itemEqual)(T &lhs, T &rhs);
  void (*deleteUserData)(XArrayList<T> *);

 public:
  XArrayList(void (*deleteUserData)(XArrayList<T> *) = 0,
             bool (*itemEqual)(T &, T &) = 0, int capacity = 10);
  XArrayList(const XArrayList<T> &list);
  XArrayList<T> &operator=(const XArrayList<T> &list);
  ~XArrayList();

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
  // Inherit from IList: BEGIN

  void println(string (*item2str)(T &) = 0) {
    cout << toString(item2str) << endl;
  }
  void setDeleteUserDataPtr(void (*deleteUserData)(XArrayList<T> *) = 0) {
    this->deleteUserData = deleteUserData;
  }

  Iterator begin() { return Iterator(this, 0); }
  Iterator end() { return Iterator(this, count); }

 protected:
  void checkIndex(int index);      // check validity of index for accessing
  void ensureCapacity(int index);  // auto-allocate if needed
  void copyFrom(const XArrayList<T> &list);
  void removeInternalData();

  //! FUNCTION STATIC
 protected:
  static bool equals(T &lhs, T &rhs, bool (*itemEqual)(T &, T &)) {
    if (itemEqual == 0)
      return lhs == rhs;
    else
      return itemEqual(lhs, rhs);
  }

 public:
  static void free(XArrayList<T> *list) {
    typename XArrayList<T>::Iterator it = list->begin();
    while (it != list->end()) {
      delete *it;
      it++;
    }
  }

 public:
  class Iterator {
   private:
    int cursor;
    XArrayList<T> *pList;

   public:
    Iterator(XArrayList<T> *pList = 0, int index = 0) {
      this->pList = pList;
      this->cursor = index;
    }
    Iterator &operator=(const Iterator &iterator) {
      cursor = iterator.cursor;
      pList = iterator.pList;
      return *this;
    }
    void remove(void (*removeItemData)(T) = 0) {
      T item = pList->removeAt(cursor);
      if (removeItemData != 0) removeItemData(item);
      cursor -= 1;  // MUST keep index of previous, for ++ later
    }

    T &operator*() { return pList->data[cursor]; }
    bool operator!=(const Iterator &iterator) {
      return cursor != iterator.cursor;
    }
    // Prefix ++ overload
    Iterator &operator++() {
      this->cursor++;
      return *this;
    }
    // Postfix ++ overload
    Iterator operator++(int) {
      Iterator iterator = *this;
      ++*this;
      return iterator;
    }
  };
};

//! ////////////////////////////////////////////////////////////////////
//! //////////////////////     METHOD DEFINITION      ///////////////////
//! ////////////////////////////////////////////////////////////////////

template <class T>
XArrayList<T>::XArrayList(void (*deleteUserData)(XArrayList<T> *),
                          bool (*itemEqual)(T &, T &), int capacity) {
  // TODO implement
  this->capacity = capacity;
  this->count = 0;
  this->data = new T[capacity];

  this->deleteUserData = deleteUserData;
  this->itemEqual = itemEqual;
}

template <class T>
XArrayList<T>::XArrayList(const XArrayList<T> &list) {
  // TODO implement
  this->capacity = list.capacity;
  this->count = list.count;
  this->data = new T[this->capacity];

  for (int i = 0; i < this->count; i++) {
    this->data[i] = list.data[i];
  }

  this->deleteUserData = list.deleteUserData;
  this->itemEqual = list.itemEqual;
}

template <class T>
XArrayList<T> &XArrayList<T>::operator=(const XArrayList<T> &list) {
  // TODO implement
  // Check for self-assignment
  if (this == &list) {
    return *this; 
  }
  
  // Remove old data
  delete[] this->data;

  // Copy new data
  this->capacity = list.capacity;
  this->count = list.count;
  this->data = new T[this->capacity];

  for (int i = 0; i < this->count; i++) {
    this->data[i] = list.data[i];
  }

  this->deleteUserData = list.deleteUserData;
  this->itemEqual = list.itemEqual;

  return *this;
}

template <class T>
XArrayList<T>::~XArrayList() {
  // TODO implement
    if (this->deleteUserData) {
      this->deleteUserData(this); 
    }

    delete[] data;
}

template <class T>
void XArrayList<T>::add(T e) {
  // TODO implement
  if (count == capacity) {
    capacity = capacity * 2 + 1;
    
    // Allocate a new array with the updated capacity
    T* newData = new T[capacity];
    
    // Copy elements from the old array to the new array
    for (int i = 0; i < this->count; i++) {
      newData[i] = data[i];
    }
    
    delete[] data;
    this->data = newData;
  }

  this->data[count++] = e;
}

template <class T>
void XArrayList<T>::add(int index, T e) {
  // TODO implement
  if (index < 0 || index > count) { throw out_of_range("Index is out of range!"); }

  if (count == capacity) {
    capacity = capacity * 2 + 1;
    
    // Allocate a new array with the updated capacity
    T* newData = new T[capacity];
    
    // Copy elements from the old array to the new array
    for (int i = 0; i < count; i++) {
      newData[i] = data[i];
    }
    
    delete[] data;
    this->data = newData;
  }

  // Shift elements to the right to make space for the new element
  for (int i = count; i > index; --i) {
      data[i] = data[i - 1];
  }

  // Insert the new element
  this->data[index] = e;
  this->count += 1;
}

template <class T>
T XArrayList<T>::removeAt(int index) {
  // TODO implement
  if (index < 0 || index >= count) { throw std::out_of_range("Index is out of range!"); }
  T removedElement = this->data[index];
  for (int i = index; i < count - 1; ++i) {
    data[i] = data[i + 1];
  }
  this->count -= 1;
  return removedElement;
}

template <class T>
bool XArrayList<T>::removeItem(T item, void (*removeItemData)(T)) {
  // TODO implement
  for (int i = 0; i < count; ++i) {
    if (equals(data[i], item, this->itemEqual)) {
        if (removeItemData) {
          removeItemData(data[i]);
        }

        for (int j = i; j < count - 1; ++j) {
          data[j] = data[j + 1];
        }
        this->count -= 1;

        return true; // Item removed successfully
      }
  }

  return false;
}

template <class T>
bool XArrayList<T>::empty() {
  // TODO implement
  return this->count == 0;
}

template <class T>
int XArrayList<T>::size() {
  // TODO implement
  return this->count;
}

template <class T>
void XArrayList<T>::clear() {
  // TODO implement
  // Removes all elements in the list and resets the list to its initial state.
  removeInternalData();
  this->data = new T[10];  
}

template <class T>
T &XArrayList<T>::get(int index) {
  // TODO implement
  if (index < 0 || index >= count) { throw std::out_of_range("Index is out of range!"); }
  return this->data[index];
}

template <class T>
int XArrayList<T>::indexOf(T item) {
  // TODO implement
  for (int i = 0; i < count; ++i) {
    if (equals(data[i], item, this->itemEqual)) {
      return i;
    }
  }
  return -1;
}

template <class T>
bool XArrayList<T>::contains(T item) {
  // TODO implement
  for (int i = 0; i < count; i++) {
    if (equals(data[i], item, this->itemEqual)) {
      return true;
    }
  }

  return false;
}

template <class T>
string XArrayList<T>::toString(string (*item2str)(T &)) {
  // TODO implement
  ostringstream oss;
  oss << "[";

  for (int i = 0; i < count; ++i) {
    if (item2str != nullptr) {
      oss << item2str(data[i]);
    } else {
      oss << data[i]; 
    }

    if (i != count - 1) { 
      oss << ", "; 
    }
  }

  oss << "]";
  return oss.str();
}


//! ////////////////////////////////////////////////////////////////////
//! ////////////////////// (private) METHOD DEFINTION //////////////////
//! ////////////////////////////////////////////////////////////////////
// template <class T>
// void XArrayList<T>::checkIndex(int index) {
//   /**
//    * Validates whether the given index is within the valid range of the list.
//    * Throws an std::out_of_range exception if the index is negative or exceeds
//    * the number of elements. Ensures safe access to the list's elements by
//    * preventing invalid index operations.
//    */
//   // TODO implement
// }
// template <class T>
// void XArrayList<T>::ensureCapacity(int index) {
//   /**
//    * Ensures that the list has enough capacity to accommodate the given index.
//    * If the index is out of range, it throws an std::out_of_range exception. If
//    * the index exceeds the current capacity, reallocates the internal array with
//    * increased capacity, copying the existing elements to the new array. In case
//    * of memory allocation failure, catches std::bad_alloc.
//    */
//   // TODO implement
// }

template <class T>
void XArrayList<T>::copyFrom(const XArrayList<T> &list) {
  /*
   * Copies the contents of another XArrayList into this list.
   * Initializes the list with the same capacity as the source list and copies
   * all elements. Also duplicates user-defined comparison and deletion
   * functions, if applicable.
   */
  // TODO implement
    removeInternalData();

    this->capacity = list.capacity;
    this->count = list.count;
    data = new T[capacity];

    for (int i = 0; i < count; i++) {
      data[i] = list.data[i]; 
    }
}

template <class T>
void XArrayList<T>::removeInternalData() {
  /*
   * Clears the internal data of the list by deleting the dynamic array and any
   * user-defined data. If a custom deletion function is provided, it is used to
   * free the stored elements. Finally, the dynamic array itself is deallocated
   * from memory.
   */
  // TODO implement
    if (this->deleteUserData) {
        this->deleteUserData(this); 
    }
    else if constexpr (std::is_pointer<T>::value) {
      // Delete the pointer if T is a pointer type
      for (int i = 0; i < count; i++) {
        delete data[i];
      }
    }

    delete[] data;

    this->data = nullptr;
    this->count = 0;
    this->capacity = 10;
}

#endif /* XARRAYLIST_H */
