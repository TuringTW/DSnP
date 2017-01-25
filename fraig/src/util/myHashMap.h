/****************************************************************************
  FileName     [ myHashMap.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashMap and Cache ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2009-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_MAP_H
#define MY_HASH_MAP_H

#include <vector>
#include <climits>

using namespace std;

// TODO: (Optionally) Implement your own HashMap and Cache classes.

//-----------------------
// Define HashMap classes
//-----------------------
// To use HashMap ADT, you should define your own HashKey class.
// It should at least overload the "()" and "==" operators.
//
class HashKey
{
public:
   HashKey(unsigned fin1, unsigned fin2):_fanin1(fin1), _fanin2(fin2){}

   size_t operator() () const {
     return (_fanin1+_fanin2)*2654435761 % INT_MAX;
   }
   bool operator == (const HashKey& k) const {
     return (k._fanin1==_fanin1&&k._fanin2==_fanin2)
          ||(k._fanin1==_fanin2&&k._fanin2==_fanin1);
   }

private:
  unsigned _fanin1, _fanin2;
};
class HashKey4Ptn
{
public:
   HashKey4Ptn(unsigned pattern):_pattern(pattern){};

   size_t operator() () const {
     return (_pattern)*2654435761 % INT_MAX;
   }
   bool operator == (const HashKey4Ptn& k) const {
     return (k._pattern==_pattern);
   }
   void set_pattern(unsigned pattern){
     _pattern = pattern;
   }
private:
  unsigned _pattern;
};
//
template <class HashKey, class HashData>
class HashMap
{
typedef pair<HashKey, HashData> HashNode;

public:
   HashMap(size_t b=0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashMap() { reset(); }

   // [Optional] TODO: implement the HashMap<HashKey, HashData>::iterator
   // o An iterator should be able to go through all the valid HashNodes
   //   in the HashMap
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class HashMap<HashKey, HashData>;

   public:
     iterator(const HashMap* container, size_t b_id, size_t s_id, bool isEnd):_container(container), _bId(b_id), _sId(s_id), _isEnd(isEnd){};
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

     const HashNode& operator * () const {
       return (*_container)[_bId].at(_sId);
     }
   private:
     const HashMap* _container;
     size_t _bId;
     size_t _sId;
     bool _isEnd;
   };

   void init(size_t b) {
      reset(); _numBuckets = b; _buckets = new vector<HashNode>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<HashNode>& operator [] (size_t i) { return _buckets[i]; }
   const vector<HashNode>& operator [](size_t i) const { return _buckets[i]; }

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

   // check if k is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const HashKey& k) const {
     size_t b_num = bucketNum(k);
     for (typename vector<HashNode>::const_iterator itr = _buckets[b_num].begin(); itr != _buckets[b_num].end(); itr++) {
       if((*itr).first==k){ return true; }
     }
     return false;
   }

   // query if k is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(const HashKey& k, HashData& d) const {
     size_t b_num = bucketNum(k);
     for (typename vector<HashNode>::const_iterator itr = _buckets[b_num].begin(); itr != _buckets[b_num].end(); itr++) {
       if((*itr).first==k){ d = (*itr).second; return true; }
     }
     return false;
   }

   // update the entry in hash that is equal to k (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const HashKey& k, HashData& d) {
     size_t b_num = bucketNum(k);
     for (typename vector<HashNode>::iterator itr = _buckets[b_num].begin(); itr != _buckets[b_num].end(); itr++) {
       if((*itr).first==k){ (*itr).second = d; return true; }
     }
     HashNode new_node(k, d);
     _buckets[b_num].push_back(new_node);
     return false;
   }

   // return true if inserted d successfully (i.e. k is not in the hash)
   // return false is k is already in the hash ==> will not insert
   bool insert(const HashKey& k, const HashData& d) {
     size_t b_num = bucketNum(k);
     for (typename vector<HashNode>::iterator itr = _buckets[b_num].begin(); itr != _buckets[b_num].end(); itr++) {
       if((*itr).first==k){ return false; }
     }
     HashNode new_node(k, d);
     _buckets[b_num].push_back(new_node);
     return true;
   }

   bool force_insert(const HashKey& k, const HashData& d) {
     size_t b_num = bucketNum(k);
     HashNode new_node(k, d);
     _buckets[b_num].push_back(new_node);
     return true;
   }

   // return true if removed successfully (i.e. k is in the hash)
   // return fasle otherwise (i.e. nothing is removed)
   bool remove(const HashKey& k) {
     size_t b_num = bucketNum(k);
     for (typename vector<HashNode>::iterator itr = _buckets[b_num].begin(); itr != _buckets[b_num].end(); itr++) {
       if((*itr).first==k){ _buckets[b_num].erase(itr); return true; }
     }
     return false;
   }

private:
   // Do not add any extra data member
   size_t                   _numBuckets;
   vector<HashNode>*        _buckets;

   size_t bucketNum(const HashKey& k) const {
      return (k() % _numBuckets); }

};


//---------------------
// Define Cache classes
//---------------------
// To use Cache ADT, you should define your own HashKey class.
// It should at least overload the "()" and "==" operators.
//
// class CacheKey
// {
// public:
//    CacheKey() {}
//
//    size_t operator() () const { return 0; }
//
//    bool operator == (const CacheKey&) const { return true; }
//
// private:
// };
//
template <class CacheKey, class CacheData>
class Cache
{
typedef pair<CacheKey, CacheData> CacheNode;

public:
   Cache() : _size(0), _cache(0) {}
   Cache(size_t s) : _size(0), _cache(0) { init(s); }
   ~Cache() { reset(); }

   // NO NEED to implement Cache::iterator class

   // TODO: implement these functions
   //
   // Initialize _cache with size s
   void init(size_t s) { reset(); _size = s; _cache = new CacheNode[s]; }
   void reset() {  _size = 0; if (_cache) { delete [] _cache; _cache = 0; } }

   size_t size() const { return _size; }

   CacheNode& operator [] (size_t i) { return _cache[i]; }
   const CacheNode& operator [](size_t i) const { return _cache[i]; }

   // return false if cache miss
   bool read(const CacheKey& k, CacheData& d) const {
      size_t i = k() % _size;
      if (k == _cache[i].first) {
         d = _cache[i].second;
         return true;
      }
      return false;
   }
   // If k is already in the Cache, overwrite the CacheData
   void write(const CacheKey& k, const CacheData& d) {
      size_t i = k() % _size;
      _cache[i].first = k;
      _cache[i].second = d;
   }

private:
   // Do not add any extra data member
   size_t         _size;
   CacheNode*     _cache;
};


#endif // MY_HASH_H
