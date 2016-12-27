/****************************************************************************
  FileName     [ myMinHeap.h ]
  PackageName  [ util ]
  Synopsis     [ Define MinHeap ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_MIN_HEAP_H
#define MY_MIN_HEAP_H

#include <algorithm>
#include <vector>

template <class Data>
class MinHeap
{
public:
   MinHeap(size_t s = 0) { if (s != 0) _data.reserve(s); }
   ~MinHeap() {}

   void clear() { _data.clear(); }

   // For the following member functions,
   // We don't respond for the case vector "_data" is empty!
   const Data& operator [] (size_t i) const { return _data[i]; }
   Data& operator [] (size_t i) { return _data[i]; }

   size_t size() const { return _data.size(); }

   // TODO
   const Data& min() const { return _data[0]; }
   void insert(const Data& d) {
     size_t idx = _data.size();
     _data.push_back(d);
     sortForward(idx);

   }
   void sortForward(size_t idx){
     while(idx>0&&_data[idx]<_data[(idx-1)/2]){
       Data temp = _data[idx];
       _data[idx] = _data[(idx-1)/2];
       _data[(idx-1)/2] = temp;
       idx = (idx-1)/2;
     }
   }
   void sortBackward(size_t idx){
     size_t t_size = _data.size();
     while(true){
       size_t min_idx = idx;
       if(idx*2+1<t_size&&_data[idx*2+1]<_data[idx]){
         min_idx = idx*2+1;
         if(idx*2+2<t_size&&_data[idx*2+2]<_data[min_idx]){
           min_idx = idx*2+2;
         }
       }else if(idx*2+2<t_size&&_data[idx*2+2]<_data[idx]){
         min_idx = idx*2+2;
       }else{ return; }
       Data temp = _data[idx];
       _data[idx] = _data[min_idx];
       _data[min_idx] = temp;
       idx = min_idx;
     }
   }
   size_t goButton(size_t idx){
     size_t t_size = _data.size();
     while(true){
       size_t o_idx = idx;
       if(2*idx+1>=t_size){
         break;
       }
       if (2*idx+2>=t_size) {
         idx = 2*idx+1;
       }else if(_data[2*idx+2]<_data[2*idx+1]){
         idx = 2*idx+2;
       }else{
         idx = 2*idx+1;
       }
       Data temp = _data[o_idx];
       _data[o_idx] = _data[idx];
       _data[idx] = temp;
     }
     return idx;
   }
   void delMin() {
     delData(0);
   }
   void delData(size_t i) {

     size_t f_idx = goButton(i);
     size_t last = _data.size()-1;
     if(f_idx < last){
       Data temp = _data[last];
       _data[last] = _data[f_idx];
       _data[f_idx] = temp;
     }
     _data.pop_back();
   }

private:
   // DO NOT add or change data members
   vector<Data>   _data;
};

#endif // MY_MIN_HEAP_H
