/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

#include "cirDef.h"
#include "cirGate.h"

extern CirMgr *cirMgr;

// TODO: Define your own data members and member functions
class CirMgr
{
friend class CirGate;

public:
   CirMgr() {}
   ~CirMgr() {
      clear();
      /*for(size_t i = 0; i < _gates.size(); i++)
          if(_gates[i] != NULL)
              delete _gates[i];
      for(size_t i = 0; i < _pos.size(); i++)
          delete _pos[i];*/
   }

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const {
        //AIG or PI or CONST0
        if(gid < _gates.size())
            return _gates[gid];
        else if(gid - _gates.size() < _pos.size())
            return _pos[gid - _gates.size()];
        return 0;
   }
   // Member functions about circuit construction
   bool readCircuit(const string&);

   // Member functions about circuit optimization
   void sweep();
   void optimize();

   // Member functions about simulation
   void randomSim();
   void fileSim(ifstream&);
   void setSimLog(ofstream *logFile) { _simLog = logFile; }
   void simulateHelper(int idx);

   // Member functions about fraig
   void strash();
   void printFEC() const;
   void fraig();

   void genDFSList();
   void addDFS(CirGate* root);

   

   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void printFECPairs() const;
   void writeAag(ostream&) const;
   void writeGate(ostream&, CirGate*);

private:
    ofstream           *_simLog;

    vector<CirGate*>   _gates; //_gates[0] is CONST0
    vector<PoGate*>    _pos;
    vector<unsigned>   _pis_idx;

    vector<CirGate*> _dfsList;
    HashMap<SignalKey, vector<size_t>> *_simList;
    vector<unsigned>   gateIDs;
    int simulationCnt;


    //private functions
    int valid_aig() const;
    void print_valid_aig(ostream& outfile) const;
    int valid_aig_from(CirGate *g) const;
    void print_valid_aig_from(ostream& outfile, CirGate *g) const;
    void addChild(CirGate* root);
    void clear();
    void optHelper(CirGate* input);
};

#endif // CIR_MGR_H
