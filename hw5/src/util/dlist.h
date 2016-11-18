/****************************************************************************
  FileName     [ dlist.h ]
  PackageName  [ util ]
  Synopsis     [ Define doubly linked list package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef DLIST_H
#define DLIST_H

#include <cassert>

template <class T> class DList;

// DListNode is supposed to be a private class. User don't need to see it.
// Only DList and DList::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class DListNode
{
   friend class DList<T>;
   friend class DList<T>::iterator;

   DListNode(const T& d, DListNode<T>* p = 0, DListNode<T>* n = 0):
      _data(d), _prev(p), _next(n) {}

   T              _data;
   DListNode<T>*  _prev;
   DListNode<T>*  _next;
};


template <class T>
class DList
{
public:
   // TODO: decide the initial value for _isSorted
   DList() {
      _head = new DListNode<T>(T());
      _head->_prev = _head->_next = _head; // _head is a dummy node
      _isSorted = false;
   }
   ~DList() { clear(); delete _head; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class DList;

   public:
      iterator(DListNode<T>* n= 0): _node(n) {}
      iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return _node->_data; }
      T& operator * () { return _node->_data; }
      iterator& operator ++ () { _node = _node->_next;return *(this); }
      iterator operator ++ (int) {
        iterator temp(*this);
        _node = _node->_next;
        return temp;
      }
      iterator& operator -- () { _node = _node->_prev;return *(this); }
      iterator operator -- (int) {
        iterator temp(*this);
        _node = _node->_prev;
        return temp;
      }

      iterator& operator = (const iterator& i) { _node = i._node; return *(this); }

      bool operator != (const iterator& i) const { return (this->_node != i._node); }
      bool operator == (const iterator& i) const { return (this->_node == i._node); }

   private:
      DListNode<T>* _node;
   };

   // TODO: implement these functions
   iterator begin() const { return iterator(_head->_next); }
   iterator end() const { return iterator(_head); }//dummy node
   bool empty() const { return (_head->_next==_head); }
   size_t size() const {
     size_t counter = 0;
     for (iterator ii = begin(); ii!=end(); ii++) {
       counter++;
     }
     return counter;
   }

   void push_back(const T& x) {
     insert_node(_head->_prev, new DListNode<T>(x));
   }
   void pop_front() {
     link_node(_head, _head->_next->_next);
   }
   void pop_back() {
     link_node(_head->_prev->_prev, _head);
   }

   // return false if nothing to erase
   bool erase(iterator pos) {
     if (!empty()) {
       link_node(pos._node->_prev, pos._node->_next);
       return true;
     }
     return false;
   }
   bool erase(const T& x) {
     DListNode<T>* result = find(x);
     if (result!=_head) {
       link_node(result->_prev, result->_next);
     }
     return false;
   }

   void clear() {
     link_node(_head, _head);
   }  // delete all nodes except for the dummy node

   void sort() const {
     for (iterator ii = begin(); ii!=end(); ii++) {
       for (iterator jj = ii; jj!=end(); jj++) {
         if (*ii>*jj) {
           T temp = *ii;
           *ii = *jj;
           *jj = temp;
          // exchange(ii, jj);
         }
       }
     }
   }

private:
   DListNode<T>*  _head;     // = dummy node if list is empty
   mutable bool   _isSorted; // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] helper functions; called by public member functions
   void link_node(DListNode<T>* a, DListNode<T>* b){
     a->_next = b;
     b->_prev = a;
   }

   void exchange(iterator& ii, iterator& jj){
     if(ii._node->_next == jj._node){
       erase(ii);
       insert_node(jj._node, ii._node);
     }else if(ii._node->_prev == jj._node){
       erase(jj);
       insert_node(ii._node, jj._node);
     }else{
       erase(ii);
       erase(jj);
       insert_node(jj._node->_prev, ii._node);
       insert_node(ii._node->_prev, jj._node);
     }
     DListNode<T>* temp = ii._node;
     ii._node = jj._node;
     jj._node = temp;
   }
   void insert_node(DListNode<T>* left, DListNode<T>* new_node){
     link_node(new_node, left->_next);
     link_node(left, new_node);
   }
   DListNode<T>* find(const T& x) {
     for(iterator ii = begin(); ii!=end(); ii++){
       if(*ii==x) return ii._node;
     }
     return _head;
   }
};

#endif // DLIST_H
