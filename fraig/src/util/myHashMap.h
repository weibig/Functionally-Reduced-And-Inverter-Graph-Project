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

using namespace std;

// TODO: (Optionally) Implement your own HashMap and Cache classes.

//-----------------------
// Define HashMap classes
//-----------------------
// To use HashMap ADT, you should define your own HashKey class.
// It should at least overload the "()" and "==" operators.
//
class SignalKey
{
public:
   SignalKey(string a = "", char b = '\0'):key(a), base(b){}
   long long int operator() () const { // as hash function
      string str = getKey();
      if(getKey().length() > 64)
         str = (str).substr(0,63); 
      return stoll(str,0,2);
   }  
   int length() const {return getKey().length();}
   bool operator == (const SignalKey& k) const { return (this->getKey() == k.getKey()); }
   string getKey() const {return (base == '\0')? key:base+key;}

private:
   string key;
   char base;
};


class HashKey
{
public:
   HashKey(int a = 0, int b = 0):in0(a), in1(b) {}
   size_t operator() () const { // as hash function
      auto hash1 = hash<size_t>{}(in0); 
      auto hash2 = hash<size_t>{}(in1);
      return hash1 ^ hash2; 
   }  

   bool operator == (const HashKey& k) const { return ((*this)() == k()); }

private:
   size_t in0, in1; // use LSB for inverted phase (literal)
};

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
      iterator( vector<HashNode>* b,vector<HashNode>* end,size_t i): _thisBuckets(b), _end(end), idx(i) {}
      ~iterator(){}
      const HashNode& operator * () const { 
         return _thisBuckets->at(idx);
      }
      iterator& operator ++ () 
      {
         idx++;
         if(_thisBuckets->size() == idx)
         {
            idx = 0;
            _thisBuckets++;
         }
         while(_thisBuckets->empty())
         {
            if(_thisBuckets == _end)
               return(*this);
            _thisBuckets++;
         }
         return (*this);
             
      }
      iterator& operator ++ (int) 
      {
         iterator temp = *this;
         idx++;
         if(_thisBuckets->size() == idx)
         {
            idx = 0;
            _thisBuckets++;
         }
         while(_thisBuckets->empty())
         {
            if(_thisBuckets == _end)
               return(temp);
            
            _thisBuckets++;
          }
         return temp;
      }
      bool operator != (const iterator& i) const 
      { 
         return !(i._thisBuckets==_thisBuckets && i.idx==idx);
      }


      bool operator == (const iterator& i) const 
      {
         return (i._thisBuckets==_thisBuckets && i.idx==idx);
      }


      iterator& operator = (const iterator& i) 
      {
         idx = i.idx;
         _thisBuckets = i._thisBuckets;
         return(*this);
      }
   private:
      size_t idx;
      vector<HashNode>* _end;
      vector<HashNode>* _thisBuckets;
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
      for(vector<HashNode>* ptr = _buckets; ptr < _buckets + _numBuckets; ptr++){
         if(!ptr->empty()){
            return iterator(ptr,_buckets+_numBuckets,0);
         }
      }
      return end(); 
   }
   // Pass the end
   iterator end() const { 
      return iterator(_buckets+_numBuckets,_buckets+_numBuckets,0);
   }


   bool empty() const { return (begin() == end()); }
   // number of valid data
   size_t size() const 
   { 
      size_t s = 0;
      for(iterator it = begin() ; it != end(); ++it)
          s++;
      return s; 
   }

   // check if k is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const HashKey& k) const 
   { 
      typename vector<HashNode>::iterator it;
      for(it = _buckets[bucketNum(k)].begin() ; it != _buckets[bucketNum(k)].end(); ++it)
      {
         if(it->first == k)
            return true; 
      }
      return false; 
   }

   // query if k is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(const HashKey& k, HashData& d) const 
   {  
      if(check(k))
      {
         typename vector<HashNode>::iterator it;
         for(it = _buckets[bucketNum(k)].begin() ; it != _buckets[bucketNum(k)].end(); ++it)
         {
            if(it->first == k)
            {
               d = it->second;   ///////
               return true;
            }
         }
      }
      return false; 
   }

   // update the entry in hash that is equal to k (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const HashKey& k, HashData& d) 
   { 
      typename vector<HashNode>::iterator it;
      for(it = _buckets[bucketNum(k)].begin() ; it != _buckets[bucketNum(k)].end(); ++it)
      {
         if(it->first == k)
         {
            it->second = d;
            return true;
         }
      } 
      insert(k,d);
      return false;
   }

   // return true if inserted d successfully (i.e. k is not in the hash)
   // return false is k is already in the hash ==> will not insert
   bool insert(const HashKey& k, const HashData& d) 
   { 
      if(!check(k))
      {
         HashNode tmp(k,d);
         _buckets[bucketNum(k)].push_back(tmp);
         return true; 
      }
      return false;
   }

   // return true if removed successfully (i.e. k is in the hash)
   // return fasle otherwise (i.e. nothing is removed)
   bool remove(const HashKey& k) 
   {
      if(check(k))
      {
         typename vector<HashNode>::iterator it;
         for(it = _buckets[bucketNum(k)].begin() ; it != _buckets[bucketNum(k)].end(); ++it)
         {
            if(it->first == k)
            {
               _buckets[bucketNum(k)].erase(it); 
               break;
            }
         }
         return true; 
      }
      return false;
   }


   bool add(const HashKey& k, const size_t d) 
   {

      // cout << "_numBuckets = " << _numBuckets << endl;
      // cout << "_buckets.size() = " << _buckets->size() << endl;
      

      string str = k.getKey();
      string invert_str;


      for(int i = 0; i < k.length(); i++)
      {
         int result = 1 - (str[i]=='1'? 1 : 0);
         invert_str += (result == 1? '1' : '0');
      }

      HashKey invert_k(invert_str,'\0');
      // cout << "Gate " << d << " with key = " << k.getKey() << " and invert_key = " << invert_k.getKey() << endl;

      typename vector<HashNode>::iterator it;
      for(it = _buckets[bucketNum(k)].begin() ; it != _buckets[bucketNum(k)].end(); ++it)
      {
         if(it->first == k)
         {
            it->second.push_back(2*d);
            return false;
         }
      } 

      for(it = _buckets[bucketNum(invert_k)].begin() ; it != _buckets[bucketNum(invert_k)].end(); ++it)
      {
         if(it->first == invert_k)
         {
            it->second.push_back(2*d+1);
            return false;
         }
      } 

      // cout << "new pattern!! k = " << k() << endl;
      vector<size_t> ds;
      ds.push_back(2*d);

      HashNode tmp(k,ds);
      _buckets[bucketNum(k)].push_back(tmp);
      return true; // add new pattern
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
class CacheKey
{
public:
   CacheKey():in0(0), in1(0) {}

   size_t operator() () const { // as hash function
      auto hash1 = hash<size_t>{}(in0); 
      auto hash2 = hash<size_t>{}(in1);
      return hash1 ^ hash2; 
   }  

   bool operator == (const CacheKey& k) const { return ((*this)() == k()); }

private:
   size_t  in0, in1; // use LSB for inverted phase (literal)
}; 

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
