# Functionally-Reduced-And-Inverter-Graph-Project

## Table of contents
* [Design of Data Structure](#Design-of-Data-Structure)
* [Algorithms of Each Parts](#Algorithms-of-Each-Parts)


## I. Design of Data Structure
### class CirMgr
主要管理各種函式，包括這次所需要寫optimize, sweep, fraig 等
  
  
  ```
   bool readCircuit(const string&);

   // Member functions about circuit optimization
   void sweep();
   void optimize();

   // Member functions about simulation
   void randomSim();
   void fileSim(ifstream&);

   // Member functions about fraig
   void strash();
   void printFEC() const;
   void fraig();
   
   ```
另外也新增一些function，功能區分大概為：
#### i. DFS_helper [void genDFSList(), void addDFS(CirGate* root)]: 
用來生成 ```private member vector<CirGate*> _dfsList```
  genDFSList 依照 PO 順序，recusively call addDFS()，把 root 以下的點透過_fanin 連結，traverse node 並存在 _dfsList

#### ii. void simulateHelper(int idx): 
用來生成 ```private member HashMap<SignalKey, vector<size_t>> *_simList```

#### iii. void optHelper(CirGate* input):
用來 optimize gate

### class CirGate
主要管理 gate 的 local 資訊及函式，包括 Gate details (ID、 line、Type)、是否 marked、連結資訊（fanin、fanout），這些共通資訊也會繼承給child class (AigGate、PoGate、PiGate、ConstGate)。

```
   //  some protected member
   unsigned  _id;
   unsigned  _mark;
   unsigned  _fanin_idx[2];
   bool      _fanin_invert[2];
   CirGate*  _fanin[2];
   vector<CirGate*>  _fanout;
   bool _changed;
   string _signalList;
   char _baseSignal;

```

i. 另外比較特別的是，用```marked```判斷是否```==```目前檢查的```cur_mark```值，避免過多的 process overload：

```bool marked() const { return (_mark == cur_mark); }```
    
ii. ```_signalList```用來存放每次 simulation 的測試結果，```_baseSignal```是在PI 皆為 0 時的 simulation 狀態（輸出 gate value 的時候會需要_baseSignal 來當 padding）。
## II. Algorithms of Each Parts
### CirMgr::sweep() 
先用 genDFSList() 跑一次 DFS，沒被 traverse 到、且不為 PI 的就需要被 removed，處理 remove 有個麻煩點是要把那個gate的所有 faninGate 的 fanoutList, fanoutGate 的 faninList 都更改，以免其他 gate 指到 removed gate 而發生 segmentation fault。


### CirMgr::optimize() 
因為需要 recursively simplifying from PO，所以我又寫了一個helper function 來做 recursive，而原本的 optimize() 則用迴圈呼叫每個 PO 進入helper function CirMgr::optHelper(CirGate* input)
我把 optimize gate 的簡化形式方為三種（a 為其中一個 fanin gate）：
####	i.   to_zero
####	ii.  to_a
####	ii.  to_invert_a
先依照上課的簡化情形歸納做 gate 關係的判斷，分別找出簡化的形式，最後再實作簡化。同時簡化可能需要 remove gate，所以也要做 remove gate 的連結處理（同 sweep() 所以省略）。
另外我在每個 gate 中多設一個 _changed 判斷其 child node 是否做了簡化的更動，藉此來實作 recursively changed from PO。




### CirMgr::strash()
這個 function 要判斷 gate 的 ```fanin``` 是否完全相同（invert 順序也要一致），所以需要用到一個新的 dataType：
```template <class HashKey, class HashData>class HashMap```
而 class HashKey 有兩個 member element ```in0, in1```，用來存放 AIG 兩個 faninGate 的 ID

首先 operator overloading() 將兩個 fanin 的 gateID 生成 HashMap 的 key。透過 insert <key, gateID> 到 hashTable 裡，判斷是否有相同 key 的 gate。

如果 key 相同代表兩個 AIG 可以合併。合併的動作也會牽涉到 remove gate，另外需要把被合併的 gate 所有的 fanoutGate 也合併到另一 gate。


### CirMgr::randomSim() / CirMgr::fileSim()
這兩個 function 都是 simulation，```fileSim()``` 用指定的 pattern 去歸類 gate signal，```randomSim()``` 用隨機數字來做 pattern。因演算法大致相同，僅細說 fileSim() 的實作。

一開始先將 pattern file 指定的 PI signal分次輸入至 ```_signalList```，再利用 ```simulateHelper()``` 來做其他 gate 相對應的 ```_signalList``` 結果。之後再將所有 gate 的 ```_signalList``` 當作 key 放到 HashTable。

如果有兩個 gate 有一樣的 ```_signalList```，就判斷為 FECPairs，而此 HashTable 被存在 CirMgr() 的 ```HashMap<SignalKey, vector<size_t>> *_simList```，以利 gate 在輸出 FECs 時可以 access，還有輸出 ```FECPairs()``` 比較方便不用重新 simulate。

另外存進去當 key 的 ```_signalList``` 經過處理就是 gate value。我用 string 來存 key，不會有字元限制，而用前 64 bits 轉 long long int 當作 hash 的 index。
