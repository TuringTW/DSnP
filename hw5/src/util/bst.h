/****************************************************************************
  FileName     [ bst.h ]
  PackageName  [ util ]
  Synopsis     [ Define binary search tree package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef BST_H
#define BST_H

#include <cassert>

using namespace std;

template <class T> class BSTree;
template <class T>class BSTTrace;

// BSTreeNode is supposed to be a private class. User don't need to see it.
// Only BSTree and BSTree::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class BSTreeNode
{
   // TODO: design your own class!!
   friend class BSTree<T>;
   friend class BSTree<T>::iterator;
 public:
   BSTreeNode(T x):_data(x), _left(0), _right(0){}
   BSTreeNode(): _left(0), _right(0){}

   ~BSTreeNode(){
     delete _left;
     delete _right;
   };
 private:
   T _data;
   BSTreeNode<T>* _left;
   BSTreeNode<T>* _right;
};

template <class T>
class traceNode{
  friend class BSTree<T>;
  friend class BSTree<T>::iterator;
  friend class BSTTrace<T>;
public:
  traceNode(BSTreeNode<T>* node):  _node(node), _prev(0), _isleft(false), _ismiddle(false),  _isright(false){};
  traceNode(traceNode<T>* i): _node(i->_node), _prev(0), _isleft(i->_isleft), _ismiddle(i->_ismiddle),  _isright(i->_isright){};
  ~traceNode(){
    if (_prev!=0) delete _prev;
  };
private:
  BSTreeNode<T>* _node;
  traceNode<T>* _prev;
  bool _isleft;
  bool _ismiddle;
  bool _isright;
};

template <class T>
class BSTTrace{
public:
  BSTTrace(BSTreeNode<T>* node){
    _last = new traceNode<T>(node);
  };
  BSTTrace(const BSTTrace<T>& i){
    _last = new traceNode<T>(i._last);
    traceNode<T>* temp = i._last;
    traceNode<T>* tempNew = _last;
    while(temp->_prev!=0){
      tempNew->_prev = new traceNode<T>(temp->_prev);
      temp = temp->_prev;
      tempNew = tempNew->_prev;
    }
  };
  ~BSTTrace(){
    delete _last;
  }
  void push(BSTreeNode<T>* node){
    traceNode<T>* temp = _last;
    _last = new traceNode<T>(node);
    _last->_prev = temp;
  }
  void pop(){
    if(empty()) return;
    traceNode<T>* temp = top();
    _last = _last->_prev;
    temp->_prev = 0;
    delete temp;
  }

  bool empty(){return (_last==0);}
  traceNode<T>* top(){return (empty())?0:_last;}
  BSTreeNode<T>* topNode(){return (empty())?0:_last->_node;}
  size_t count(){
    size_t count = 0;
    traceNode<T>* temp = _last;
    while(temp != 0){count++;temp = temp->_prev;}
    return count;
  }
private:
  traceNode<T>* _last;
};

template <class T>
class BSTree
{
   // TODO: design your own class!!
public:
  BSTree(): _root(0), _size(0){
    _root = new BSTreeNode<T>(T("a"));
  }
  ~BSTree(){ clear();};

  class iterator {
    friend class BSTree<T>;
  public:
    iterator(BSTreeNode<T>* n= 0):_node(n), _trace(BSTTrace<T>(n)) { };
    iterator(const iterator& i) : _node(i._node) , _trace(i._trace){};
    ~iterator() {} // Should NOT delete _node

    const T& operator * () const {return (_node->_data); }
    T& operator * () {return (_node->_data); }
    iterator& operator ++ () {
      while(true){
          if (!_trace.top()->_isleft) { //haven't go through left
            _trace.top()->_isleft = true;
            if(_node->_left != 0){
              _node = _node->_left;
              _trace.push(_node);
            }
          }else if (!_trace.top()->_ismiddle) { //haven't go through middle
            _trace.top()->_ismiddle = true;
            break;
          }else if(!_trace.top()->_isright){ //haven't go through left
            _trace.top()->_isright = true;
            if(_node->_right != 0){
              _node = _node->_right;
              _trace.push(_node);
            }
          }else{
            // back to previous
            _trace.pop();
            _node = _trace.topNode();
          }
      }

      return (*this);
    }

    iterator operator ++ (int) {
      iterator temp(*this);
      ++(*this);
      return (temp);
    }
    iterator& operator -- () {
      while(true){
        if (_trace.empty()) {
          _trace.push(_node);
        }
        if (!_trace.top()->_isright) { //haven't go through right
          _trace.top()->_isright = true;
          if(_node->_right != 0){
            _node = _node->_right;
            _trace.push(_node);
          }
        }else if (!_trace.top()->_ismiddle) { //haven't go through middle
          _trace.top()->_ismiddle = true;
          break;
        }else if(!_trace.top()->_isleft){ //haven't go through left
          _trace.top()->_isleft = true;
          if(_node->_left != 0){
            _node = _node->_left;
            _trace.push(_node);
          }
        }else{
          // back to previous

          _trace.pop();
          _node = _trace.topNode();
          if (_trace.count()==1&&_trace.top()->_isleft&&_trace.top()->_isright&&_trace.top()->_ismiddle) {
            _trace.top()->_isleft = _trace.top()->_isright = _trace.top()->_ismiddle = false;
            break;
          }
        }
      }
      return (*this);
    }
    iterator operator -- (int) {
      iterator temp(*this);
      --(*this);
      return (temp);
    }
    iterator& operator = (const iterator& i) { _trace = i._trace; _node = i._node;return (*this); }

    bool operator != (const iterator& i) const { return (_node!=i._node); }
    bool operator == (const iterator& i) const { return (_node==i._node); }

    iterator& next_node() {
      while(true){
          if (!_trace.top()->_ismiddle) { //haven't go through middle
            _trace.top()->_ismiddle = true;
            break;
          }else if (!_trace.top()->_isleft) { //haven't go through left
            _trace.top()->_isleft = true;
            if(_node->_left != 0){
              _node = _node->_left;
              _trace.push(_node);
            }else{
              break;
            }
          }else if(!_trace.top()->_isright){ //haven't go through left
            _trace.top()->_isright = true;
            if(_node->_right != 0){
              _node = _node->_right;
              _trace.push(_node);
            }else{
              break;
            }
          }else{
            // back to previous
            _trace.pop();
            _node = _trace.topNode();
          }
      }
      return (*this);
    }

  private:
    BSTreeNode<T>* _node;
    BSTTrace<T> _trace;
  };
  iterator begin() const {
    if(empty()) return iterator(_root);
    iterator ii = iterator(_root);
    while(ii._node->_left!=0) {ii++;}
    ii._trace.top()->_isleft = ii._trace.top()->_ismiddle = true;
    return ii;
  }
  iterator end() const {
    if(empty()) {return iterator(_root);}
    iterator ii = iterator(_root);
    while(ii._node->_right!=0) {ii--;}
    ii._trace.top()->_isright = ii._trace.top()->_ismiddle = true;
    return ii;
  }
  bool empty() const { return _size == 0; }
  size_t size() const { return _size; }
  void insert(const T& x) {
    if(empty()){
      _root->_data = x;
      _root->_right = new BSTreeNode<T>(T("a"));
      _size++;
      return;
    }
    BSTreeNode<T>* ii = _root;
    BSTreeNode<T>* end_node = end()._node;
    while (true) {
      if (ii == end_node) {
        ii->_data = x;
        ii->_right = new BSTreeNode<T>(T("a"));
        break;
      }
      if (ii->_data>x) {
        if (ii->_left==0) {ii->_left = new BSTreeNode<T>(x);break;}
        ii = ii->_left;
      }else{
        if (ii->_right==0) {ii->_right = new BSTreeNode<T>(x);break;}
        ii = ii->_right;
      }
    }
    _size++;
  }
  void pop_front() {
    if(empty()) return;
    erase(begin());
  }
  void pop_back() {
    if(empty()) return;
    erase(--end());
  }

  bool erase(iterator pos) {
    if(empty()) return false;
    BSTreeNode<T>* end_node = end()._node;
    iterator suc_node = successor(pos);
    if (suc_node._node == pos._node) {
      if(pos._node == _root) {
        if (_root->_right==end_node) {
          BSTreeNode<T>* last_elem = findMax(_root->_left);
          if (last_elem == 0) {
            _root = _root->_right;
          }else{
            last_elem->_right = _root->_right;
            _root->_right = pos._node->_left;
          }
        }
        if(_root->_left != 0) _root = pos._node->_left;

      }else if(pos._trace.top()->_prev->_node->_left == pos._node){
        pos._trace.top()->_prev->_node->_left = pos._node->_left;
      }else{
        if (pos._node->_right == end_node) {
          BSTreeNode<T>* temp = pos._node->_right;
          BSTreeNode<T>* last_elem = findMax(pos._node->_left);
          if (last_elem == 0) {
            pos._trace.top()->_prev->_node->_right = temp;
          }else{
            last_elem->_right = temp;
            pos._trace.top()->_prev->_node->_right = pos._node->_left;
          }
        }else{
          pos._trace.top()->_prev->_node->_right = pos._node->_left;
        }
      }
      pos._node->_left = pos._node->_right = 0;
    }else{
      if (suc_node._trace.top()->_prev->_node == pos._node) {
        if(suc_node._trace.top()->_prev->_node->_left == suc_node._node){
          suc_node._node->_right = pos._node->_right;
        }else{
          suc_node._node->_left = pos._node->_left;
        }
      }else{
        if(suc_node._trace.top()->_prev->_node->_left == suc_node._node){
          suc_node._trace.top()->_prev->_node->_left = suc_node._node->_right;
        }else{
          suc_node._trace.top()->_prev->_node->_right = suc_node._node->_right;
        }
        suc_node._node->_left = pos._node->_left;
        suc_node._node->_right = pos._node->_right;
      }
      if(pos._node == _root){
        _root = suc_node._node;
      }else{
        if(pos._trace.top()->_prev->_node->_left == pos._node){
          pos._trace.top()->_prev->_node->_left = suc_node._node;
        }else{
          pos._trace.top()->_prev->_node->_right = suc_node._node;
        }
      }
      pos._node->_left = pos._node->_right = 0;
    }
    _size--;
    if(pos._node != _root) delete pos._node;
    return true;
  }
  bool erase(const T& x) {
    if(empty()){
      return false;
    }
    BSTreeNode<T>* ii = _root;
    iterator itr(_root);
    BSTreeNode<T>* end_node = end()._node;
    while (true) {
      if (ii == end_node) {
        return false;
      }
      if (ii->_data>x) {
        if (ii->_left==0) {return false;}
        ii = ii->_left;
        itr._trace.push(ii);
        itr._node = ii;
      }else if(ii->_data<x){
        if (ii->_right==0) {return false;}
        ii = ii->_right;
        itr._trace.push(ii);
        itr._node = ii;
      }else{
        return erase(itr);
      }
    }
    return false;
  }

  void clear() {
    delete _root->_left;
    delete _root->_right;
    _root->_left = _root->_right = 0;
    _size = 0;
  }
  void print(){
    iterator ii = iterator(_root);
    ii._trace.top()->_ismiddle = true;
    while(true) {
      for (size_t i = 1; i < ii._trace.count(); i++) {
        cout << setw(3) << " ";
      }
      if((ii._trace.top()->_isright&&(ii._node->_right==0)) || (ii._trace.top()->_isleft&&(ii._node->_left==0))){
        cout << setw(3)<< " " <<setw(3) << "[0]";
      }else{
        if(ii._node==end()._node) {
          cout<<setw(3) <<"[0]" << endl;
          break;
        }else{
          cout <<setw(3)<< ii._node->_data;
        }
      }
      cout << endl;
      ii = ii.next_node();
    }
  }
  void sort() const {}

private:
    BSTreeNode<T>* _root;
    size_t _size;

    iterator successor(iterator ori_node){
      if (ori_node._node->_right==0||ori_node._node->_right == end()._node) return ori_node;
      ori_node._trace.push(ori_node._node->_right);
      ori_node._node = ori_node._node->_right;
      while(ori_node._node->_left!=0){
        ori_node._trace.push(ori_node._node->_left);
        ori_node._node = ori_node._node->_left;
      }
      return ori_node;
    }
    BSTreeNode<T>* findMax(BSTreeNode<T>* ori_node){
      BSTreeNode<T>* end_node = end()._node;
      if (ori_node==0) return 0;
      while(ori_node->_right!=0&&ori_node->_right!=end_node){ori_node = ori_node->_right;}
      return ori_node;
    }
};
#endif // BST_H
