# Functionally-Reduced-And-Inverter-Graph-Project

## Summary
This project focuses on the design and implementation of data structures and algorithms using C++ for managing digital circuits and performing various optimization and simulation operations.

## Table of contents
* [Design of Data Structure](Design-of-Data-Structure)
* [Algorithms of Each Parts](Algorithms-of-Each-Parts)


## I. Design of Data Structure
### 1. class `CirMgr`
> Used to manage functions, including `optimize()`, `sweep()`, `fraig()`, etc.

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

#### Helper function

Besides, I define some helper functions, can be categorized as below:


##### i. DFS_helper [void genDFSList(), void addDFS(CirGate* root)]: 
> Used to generate ```private member vector<CirGate*> _dfsList```
- It calls `genDFSList()` in the order of PO, and recusively call `addDFS()` to connect nodes under root with `_fanin`, then traverse nodes and stores node in `_dfsList`

##### ii. void simulateHelper(int idx): 
> Used to generate ```private member HashMap<SignalKey, vector<size_t>> *_simList```

##### iii. void optHelper(CirGate* input):
> Used to optimize gates performance

### 2. class `CirGate`
> Used to manage local attributes and functions in gates, including Gate details (ID、 line、Type), marked status, connection details (fanin, fanout). Child class (AigGate, PoGate, PiGate, ConstGate) will also inherit those common attributes.

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

##### i. Please notice that `marked()` is used to see if eqals to currently tested `cur_mark`, to prevent process overload：

```bool marked() const { return (_mark == cur_mark); }```
    
##### ii. `_signalList` is used to store the results in every simulation. 

##### iii. `_baseSignal` is the simulation status when PI are all 0 (`_baseSignal` served as padding when outputing gate value).


## II. Algorithms of Each Parts

### 1. CirMgr::sweep()

- First using `genDFSList()` to run DFS, the node that is not traversed and also not PI need to be removed.
- To remove the node, we also need to update the below things, in case other gates pointed to the removed gate, and occurs segmentation fault.
  - the `fanoutList` in all its `faninGate`
  - the `faninList` in all its `fanoutGate` 


### 2. CirMgr::optimize()

#### Helper function

##### i. CirMgr::optHelper(CirGate* input)
> Since we need to recursively simplifying from PO, I write another helper function to do the recursive. And `optimize()` iteratively call all the PO to execute helper function.

- I simplify gate optimization into three forms (`a` stands as one of the fanin gate):

#####	i.   to_zero
#####	ii.  to_a
#####	ii.  to_invert_a


- Categorized the gate simplification as the course mentioned, and implement the simplication.
- Meanwhile, simplify operation involves removing gate, need to handle the removed gate connection with other gates. (same as `sweep()`)
- Besides, I add a variables `_changed` in gate to check if its child node is simplified, in order to implement recursively changing from PO.


### 3. CirMgr::strash()

> Used to check if two AIG has the same `fanin` (also the same `invert` order)

- I define a new DataType：

 ```template <class HashKey, class HashData>class HashMap```
 
with two member element `in0, in1` to store two faninGate ID in AIG

- First, using operator `overloading()` to generate HashMap key with two fanin gate ID.
- By inserting `<key, gateID>` to hashTable, check if there are gates with same key.
- If the key is the same, this means that two AIG can be merged. The merge operaion involves removing gate.
- Meanwhile, merge operation involves removing gate, need to move all the `fanoutGate` in merged gate to the other one.


### 4. CirMgr::randomSim() / CirMgr::fileSim()
> These two functions are both simulation. `fileSim()` uses the assigned pattern to categorize gate signal, while `randomSim()` uses random pattern.

Since the algorithm is mostly the same, I will illustrate the implementation of `fileSim()`.

- First, input the assigned PI signal in pattern file to `_signalList`, and use `simulateHelper()` to construct `_signalList` in other gates. And then uses `_signalList` as the key to put results into HashTable.

- If there are two gates with the same `_signalList`, we views them as FECPairs. And this HashTable is stored in `HashMap<SignalKey, vector<size_t>> *_simList` in `CirMgr()`, to make it accessible when gates outputing FECs, and don't need to re-simulate when outputing `FECPairs()`.

- Please notice that the key stored in `_signalList` can be extracted as gate value. I use `string` to store keys so that there is no bit count limit, and then casts the first 64 bits to `long long int` as hash index.
