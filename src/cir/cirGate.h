/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include <iostream>
#include "cirDef.h"

using namespace std;

class CirGate;
class AigGate;
class PiGate;
class PoGate;
class ConstGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes
class CirGate
{
friend class AigGate;
friend class PiGate;
friend class PoGate;
friend class ConstGate;
friend class CirMgr;
public:
   static unsigned cur_mark;
   static unsigned cur_print_idx;
   CirGate() : _id(0), _mark(0), _changed(0) {
      for(int i = 0; i < 2; i++){
          _fanin_idx[i] = 0;
          _fanin_invert[i] = false;
          _fanin[i] = NULL;
          visited = false;
      }
   }
   CirGate(unsigned id) : _id(id), _mark(0), _changed(0) {
      for(int i = 0; i < 2; i++){
          _fanin_idx[i] = 0;
          _fanin_invert[i] = false;
          _fanin[i] = NULL;
      }
   }
   virtual ~CirGate() {}

   // Basic access methods
   virtual string getTypeStr() const = 0;
   virtual unsigned getLineNo() const { return 0; }
   void mark_node() { _mark = cur_mark; }
   bool marked() const { return (_mark == cur_mark); }
   void printFECs_Value() const;

   bool visited;

   // Printing functions
   virtual void printGate() const = 0;
   virtual void reportGate() const = 0;
   virtual void reportFanin(int level) const = 0;
   virtual void reportFanout(int level) const = 0;
   virtual bool isAig() = 0;

private:
protected:
    unsigned  _id;
    unsigned  _mark;
    unsigned  _fanin_idx[2];
    bool      _fanin_invert[2];
    CirGate*  _fanin[2];
    vector<CirGate*>  _fanout;
    bool _changed;

    string _signalList;
    char _baseSignal;

    virtual void printFanin(bool is_neg, int self_level, int rest_level) const = 0;
    virtual void printFanout(unsigned previous_id, int self_level, int rest_level) const = 0;
    virtual int aig_under() const = 0;
    virtual void print_aig_under(ostream& outfile) const = 0;
};

class AigGate : public CirGate
{
friend class PiGate;
friend class PoGate;
friend class ConstGate;
friend class CirMgr;
public:
    AigGate() : CirGate(), _line(0) {}
    AigGate(unsigned id, unsigned line) : CirGate(id), _line(line) {}
    ~AigGate() {}
    string getTypeStr() const { return "AIG"; }
    unsigned getLineNo() const { return _line; }
    void printGate() const;
    void reportGate() const;
    void reportFanin(int level) const { CirGate::cur_mark++; printFanin(false, 0, level); }
    void reportFanout(int level) const { CirGate::cur_mark++; printFanout(-1, 0, level); }
    int aig_under() const;
    void print_aig_under(ostream& outfile) const;
    bool isAig() {return true;}
private:
    unsigned _line;
    void printFanin(bool is_neg, int self_level, int rest_level) const;
    void printFanout(unsigned previous_id, int self_level, int rest_level) const;
};

class PiGate : public CirGate
{
friend class AigGate;
friend class PoGate;
friend class ConstGate;
friend class CirMgr;
public:
    PiGate() : CirGate(), _line(0) {}
    PiGate(unsigned id, unsigned line) : CirGate(id), _line(line) {}
    ~PiGate() {}
    string getTypeStr() const { return "PI"; }
    unsigned getLineNo() const { return _line; }
    void printGate() const;
    void reportGate() const;
    void reportFanin(int level) const{ CirGate::cur_mark++; printFanin(false, 0, level); }
    void reportFanout(int level) const { CirGate::cur_mark++; printFanout(-1, 0, level); }
    int aig_under() const { return 0; }
    void print_aig_under(ostream& outfile) const { return; }
    bool isAig() {return false;}
private:
    unsigned _line;
    string   _symbol;
    void printFanin(bool is_neg, int self_level, int rest_level) const;
    void printFanout(unsigned previous_id, int self_level, int rest_level) const;
};

class PoGate : public CirGate
{
friend class AigGate;
friend class PiGate;
friend class ConstGate;
friend class CirMgr;
public:
    PoGate() : CirGate(), _line(0) {}
    PoGate(unsigned id, unsigned line) : CirGate(id), _line(line) {}
    ~PoGate() {}
    string getTypeStr() const { return "PO"; }
    unsigned getLineNo() const { return _line; }
    void printGate() const;
    void reportGate() const;
    void reportFanin(int level) const { CirGate::cur_mark++; printFanin(false, 0, level); }
    void reportFanout(int level) const { CirGate::cur_mark++; printFanout(-1, 0, level); }
    int aig_under() const;
    void print_aig_under(ostream& outfile) const;
    bool isAig() {return false;}
private:
    unsigned  _line;
    string    _symbol;
    void printFanin(bool is_neg, int self_level, int rest_level) const;
    void printFanout(unsigned previous_id, int self_level, int rest_level) const;
};

class ConstGate : public CirGate
{
friend class AigGate;
friend class PiGate;
friend class PoGate;
friend class CirMgr;
public:
    ConstGate() : CirGate(0) {}
    ~ConstGate() {}
    string getTypeStr() const { return "CONST"; }
    void printGate() const;
    void reportGate() const;
    void reportFanin(int level) const { CirGate::cur_mark++; printFanin(false, 0, level); }
    void reportFanout(int level) const { CirGate::cur_mark++; printFanout(-1, 0, level); }
    int aig_under() const { return 0; }
    void print_aig_under(ostream& outfile) const { return; }
    bool isAig() {return false;}
private:
    void printFanin(bool is_neg, int self_level, int rest_level) const;
    void printFanout(unsigned previous_id, int self_level, int rest_level) const;
};

#endif // CIR_GATE_H
