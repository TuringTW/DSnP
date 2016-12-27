/****************************************************************************
  FileName     [ myHashSet.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashSet ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_SET_H
#define MY_HASH_SET_H

#include <vector>

using namespace std;

//---------------------
// Define HashSet class
//---------------------
// To use HashSet ADT,
// the class "Data" should at least overload the "()" and "==" operators.
//
// "operator ()" is to generate the hash key (size_t)
// that will be % by _numBuckets to get the bucket number.
// ==> See "bucketNum()"
//
// "operator ==" is to check whether there has already been
// an equivalent "Data" object in the HashSet.
// Note that HashSet does not allow equivalent nodes to be inserted
//
template <class Data>
class HashSet
{
public:
   HashSet(size_t b = 0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashSet() { reset(); }

   // TODO: implement the HashSet<Data>::iterator
   // o An iterator should be able to go through all the valid Data
   //   in the Hash
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class HashSet<Data>;

   public:
     iterator(const HashSet* container, size_t b_id, size_t s_id, bool isEnd):_container(container), _bId(b_id), _sId(s_id), _isEnd(isEnd){};
     ~iterator(){};
     bool operator == (const iterator itr) const {
      if (_isEnd&&itr._isEnd){ return true; }
      if(!_isEnd&&!itr._isEnd){ return (itr._container == _container)&&(itr._sId==_sId)&&(itr._bId==_bId); }
      return false;
     }
     bool operator != (const iterator itr) const {
      if (_isEnd&&itr._isEnd){ return false; }
      if(!_isEnd&&!itr._isEnd){ return !((itr._container == _container)&&(itr._sId==_sId)&&(itr._bId==_bId)); }
      return true;
     }
     iterator& operator = (const iterator & itr){
       _container = itr._container;
       _sId = itr._sId;
       _bId = itr._bId;
       _isEnd = itr._isEnd;
       return *this;
     }
     //++i
     iterator& operator ++ () {
       size_t t_bId = _bId;
       size_t t_sId = _sId;
       if(_isEnd){return *this;}
       if(t_sId+1 < (*_container)[t_bId].size()){ _bId = t_bId; _sId = t_sId+1;_isEnd = false;}
       else{
         t_bId++;
         while(t_bId!=_container->_numBuckets&&(*_container)[t_bId].size()==0){
           t_bId++;
         }
         if(t_bId==_container->_numBuckets){_isEnd = true; return *this;}
         _isEnd = false;
         _bId = t_bId;
         _sId = 0;
       }
       return *this;
     }
     //i++
     iterator operator ++ (int) {
       iterator temp = *this;
       ++(*this);
       return temp;
     }
     //++i
     iterator& operator -- () {
       size_t t_bId = _bId;
       size_t t_sId = _sId;
       if(_isEnd){
         t_bId = _container->_numBuckets-1;
         t_sId = (*_container)[t_bId].size();
       }
       if(t_sId-1 >= 0){ _bId = t_bId; _sId = t_sId-1;_isEnd = false;}
       else{
         t_bId--;
         while(t_bId != -1&&(*_container)[t_bId].size()==0){
           t_bId--;
         }
         if(t_bId == -1){return *this;}
         _isEnd = false;
         _bId = t_bId;
         _sId = (*_container)[_bId].size()-1;
       }
       return *this;
     }
     //i++
     iterator operator -- (int) {
       iterator temp = *this;
       --(*this);
       return temp;
     }

     const Data& operator * () const {
       return (*_container)[_bId].at(_sId);
     }
   private:
     const HashSet* _container;
     size_t _bId;
     size_t _sId;
     bool _isEnd;
   };

   void init(size_t b) { _numBuckets = b; _buckets = new vector<Data>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<Data>& operator [] (size_t i) { return _buckets[i]; }
   const vector<Data>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions
   //
   // Point to the first valid data
   iterator begin() const {
     size_t i = 0;
     while(_buckets[i].size()==0){
        i++;
       if(i==_numBuckets){
         return end();
       }
     }
    return iterator(this, i, 0, false);

   }
   // Pass the end
   iterator end() const { return iterator(this, 0, 0, true); }
   // return true if no valid data
   bool empty() const {
     for (size_t i = 0; i < _numBuckets; i++) {
       if(!_buckets[i].empty()){ return false; }
     }
     return true;
   }
   // number of valid data
   size_t size() const {
     size_t s = 0;
     for (size_t i = 0; i < _numBuckets; i++) {
       s+=_buckets[i].size();
     }
     return s;
   }

   // check if d is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const Data& d) const {
     size_t b_num = bucketNum(d);
     for (typename vector<Data>::const_iterator itr = _buckets[b_num].begin(); itr != _buckets[b_num].end(); itr++) {
       if(*itr==d){ return true; }
     }
     return false;
   }

   // query if d is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(Data& d) const {
     size_t b_num = bucketNum(d);
     for (typename vector<Data>::const_iterator itr = _buckets[b_num].begin(); itr != _buckets[b_num].end(); itr++) {
       if(*itr==d){ d = *itr; return true; }
     }
     return false;
   }

   // update the entry in hash that is equal to d (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const Data& d) {
     size_t b_num = bucketNum(d);
     for (typename vector<Data>::iterator itr = _buckets[b_num].begin(); itr != _buckets[b_num].end(); itr++) {
       if(*itr==d){ *itr = d; return true; }
     }
     _buckets[b_num].push_back(d);
     return false;
   }

   // return true if inserted successfully (i.e. d is not in the hash)
   // return false is d is already in the hash ==> will not insert
   bool insert(const Data& d) {
     size_t b_num = bucketNum(d);
     for (typename vector<Data>::iterator itr = _buckets[b_num].begin(); itr != _buckets[b_num].end(); itr++) {
       if(*itr==d){ return false; }
     }
     _buckets[b_num].push_back(d);
     return true;
   }

   // return true if removed successfully (i.e. d is in the hash)
   // return fasle otherwise (i.e. nothing is removed)
   bool remove(const Data& d) {
     size_t b_num = bucketNum(d);
     for (typename vector<Data>::iterator itr = _buckets[b_num].begin(); itr != _buckets[b_num].end(); itr++) {
       if(*itr==d){ _buckets[b_num].erase(itr); return true; }
     }
     return false;
   }

private:
   // Do not add any extra data member
   size_t            _numBuckets;
   vector<Data>*     _buckets;

   size_t bucketNum(const Data& d) const {
      return (d() % _numBuckets); }
};

#endif // MY_HASH_SET_H
