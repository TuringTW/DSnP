/****************************************************************************
  FileName     [ array.h ]
  PackageName  [ util ]
  Synopsis     [ Define dynamic array package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef ARRAY_H
#define ARRAY_H

#include <cassert>
#include <algorithm>
#include <math.h>
using namespace std;

// NO need to implement class ArrayNode
//
template <class T>
class Array
{
public:
   // TODO: decide the initial value for _isSorted
   Array() : _data(0), _size(0), _capacity(0), _isSorted(true) {}
   ~Array() { delete []_data; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class Array;

   public:
      iterator(T* n= 0): _node(n) {}
      iterator(const iterator& i): _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return (*this); }
      T& operator * () { return (*_node); }
      iterator& operator ++ () { _node++; return (*this); }
      iterator operator ++ (int) {
        iterator temp(*this);
        _node++;
        return (temp);
      }
      iterator& operator -- () { _node--; return (*this); }
      iterator operator -- (int) {
        iterator temp(*this);
        _node--;
        return (temp);
      }

      iterator operator + (int i) const {
        iterator temp(_node+i);
        return (temp);
      }
      iterator& operator += (int i) { _node = _node + i; return (*this); }

      iterator& operator = (const iterator& i) { return (*this); }

      bool operator != (const iterator& i) const { return (_node!=i._node); }
      bool operator == (const iterator& i) const { return (_node==i._node); }

   private:
      T*    _node;
   };

   // TODO: implement these functions
   iterator begin() const { return (_capacity==0)?0:iterator(_data); }
   iterator end() const { return (_capacity==0)?0:iterator(_data+_size); }
   bool empty() const { return _size==0; }
   size_t size() const { return _size; }

   T& operator [] (size_t i) { return *_data[i]; }
   const T& operator [] (size_t i) const { return *_data[i]; }

   void push_back(const T& x) {
     if(_isSorted&&!empty()&&*(_data+_size-1)>x) _isSorted = false;
     if (_size==_capacity) {
       _capacity = (_capacity==0)?1:(_capacity*2);
       T* temp = _data;
       _data = new T[_capacity];
       for (size_t i = 0; i < _capacity-1 && i<_size; i++) {
         *(_data+i) = *(temp+i);
       }
       delete [] temp;
     }
     *(_data+_size) = x;
     _size++;
   }
   void pop_front() {
     *(_data) = *(_data+_size-1);
     _size--;
     _isSorted = false;
   }
   void pop_back() {
     if (empty()) return;
     _size--;
   }

   bool erase(iterator pos) {
     if (empty()) return false;
     *pos = _data[_size-1];
     _size--;
     _isSorted = false;
     return true;
   }
   bool erase(const T& x) {
     for (iterator ii = begin(); ii!=end(); ii++) {
       if(*ii==x) {
         return erase(ii);
       }
     }
     return false;
   }

   void clear() {
     _size = 0;
     _isSorted = true;
   }

   // This is done. DO NOT change this one.
   void sort() const { if (!empty()&&!_isSorted) ::sort(_data, _data+_size); _isSorted = true;}

   // Nice to have, but not required in this homework...
   // void reserve(size_t n) { ... }
   // void resize(size_t n) { ... }

private:
   T*            _data;
   size_t        _size;       // number of valid elements
   size_t        _capacity;   // max number of elements
   mutable bool  _isSorted;   // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] Helper functions; called by public member functions
};

#endif // ARRAY_H
