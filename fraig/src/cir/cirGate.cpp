/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

extern CirMgr *cirMgr;

unsigned CirGate::cur_mark = 0;
unsigned CirGate::cur_print_idx = 0;


void
AigGate::reportGate() const {
    cout << "================================================================================" << endl;
    string info = "= AIG(" + to_string(_id) + "), line " + to_string(_line);
    cout << info << endl;
    printFECs_Value();
    cout << "================================================================================" << endl;
}
void
PiGate::reportGate() const {
    cout << "================================================================================" << endl;
    string info = "= PI(" + to_string(_id) + ")" + (_symbol == "" ? "" : "\"" + _symbol + "\"") + ", line " + to_string(_line);
    cout << info << endl;
    printFECs_Value();
    cout << "================================================================================" << endl;
}
void
PoGate::reportGate() const {
    cout << "================================================================================" << endl;
    string info = "= PO(" + to_string(_id) + ")" + (_symbol == "" ? "" : "\"" + _symbol + "\"") + ", line " + to_string(_line);
    cout << info << endl;
    printFECs_Value();
    cout << "================================================================================" << endl;
}
void
ConstGate::reportGate() const  {
    cout << "================================================================================" << endl
         << "= " << "CONST(0), line 0" << endl
         << "= FECs:" << endl
         << "= Value: 00000000_00000000_00000000_00000000_00000000_00000000_00000000_00000000" << endl
         << "================================================================================" << endl;
}
void 
CirGate::printFECs_Value() const 
{
    // FECs
    if(_signalList.length() == 0)
    {
        cout << "= FECs:" << endl
             << "= Value: 00000000_00000000_00000000_00000000_00000000_00000000_00000000_00000000" << endl;

        return;
    }

    cout << "= FECs:";

    HashMap<SignalKey, vector<size_t>>::iterator it = cirMgr->_simList->begin();
    for(; it != cirMgr->_simList->end(); it++)
    {
      char base;
      if(cirMgr->simulationCnt % 64 == 0)
        base = '\0';
      else  
        base = _baseSignal;

      SignalKey tmp(_signalList,base);

      string invert_str;
      for(int i = 0; i < tmp.getKey().length(); i++)
      {
         int result = 1 - (tmp.getKey()[i]=='1'? 1 : 0);
         invert_str += (result == 1? '1' : '0');
      }

      SignalKey invert_tmp(invert_str, '\0');

      if((*it).first == tmp)
      {
        for(int j = 0; j < (*it).second.size(); j++)
        {
          if((*it).second[j]/2 != _id)
            cout << (((*it).second[j]%2 == 1)? " !" : " ") << (*it).second[j]/2;
        // MUST-TODO: new line setting !!!!!!!!!
        }
      }
      else if((*it).first == invert_tmp)
      {
        for(int j = 0; j < (*it).second.size(); j++)
        {
          if((*it).second[j]/2 != _id)
            cout << (((*it).second[j]%2 == 0)? " !" : " ") << (*it).second[j]/2;
        // MUST-TODO: new line setting !!!!!!!!!
        }
      }
    }
    cout << endl;

    // Value
    cout << "= Value: ";

    int print_num = _signalList.length() % 64;
    if(print_num == 0)
        print_num = 64;

    int cnt = 0;
    // cout << "print_num = " << print_num << endl;////
    for(int i = 0; i < 64-print_num; i++)
    {
        if(_baseSignal != NULL)
            cout << _baseSignal;
        else
            cout << '0';

        // cout << "!!" << endl;
        if((cnt+1) % 8 == 0 && cnt != 63)
            cout << "_";
        cnt++;
    }
    // cout << _signalList.length()-1 << ", " << print_num << endl;
    int end = _signalList.length()-print_num;
    for(int i = _signalList.length()-1; i >= end; i--) //128: 127-64
    {
        cout << _signalList[i];
        if((cnt+1) % 8 == 0 && cnt != 63)
            cout << "_";
        cnt++;
    } 
    cout << endl;
}

/*void
CirGate::reportFanin(int level) const
{
   assert (level >= 0);
}*/

/*===============================AIG GATE========================================================*/
void
AigGate::printGate() const {
    //fanins
    for(size_t i = 0; i < 2; i++){
        if(_fanin[i] != NULL && _fanin[i]->marked() == false){
            _fanin[i]->printGate();
            _fanin[i]->mark_node();
        }
    }
    //self
    cout << "[" << cur_print_idx++ << "] " << "AIG " << _id << " " << 
    (_fanin[0] == NULL? "*" : "") << (_fanin_invert[0]? "!" : "") << _fanin_idx[0] << " " <<
    (_fanin[1] == NULL? "*" : "") << (_fanin_invert[1]? "!" : "") << _fanin_idx[1] << endl;
}
void
AigGate::printFanin(bool is_neg, int self_level, int rest_level) const {
    if(rest_level < 0)
        return;
  
    for(int i = 0; i < self_level; i++)
        cout << "  ";
    cout << (is_neg? "!" : "") << "AIG " << _id << ((marked() && rest_level > 0)? " (*)" : "") << endl;

    if(!marked() && rest_level > 0)
        for(int i = 0; i < 2; i++){
            if(_fanin[i] == NULL){
                for(int i = 0; i < self_level + 1; i++)
                    cout << "  ";
                cout << (_fanin_invert[i]? "!" : "") << "UNDEF " << _fanin_idx[i] << endl;
            }
            else{
                _fanin[i]->printFanin(_fanin_invert[i], self_level + 1, rest_level - 1);
                if(rest_level > 1)
                    _fanin[i]->mark_node();
            }
        }


    cout << "done AigGate::printFanin()..." << endl;///

}
void
AigGate::printFanout(unsigned previous_id, int self_level, int rest_level) const {
    if(rest_level < 0)
        return;
        
    //print self
    for(int i = 0; i < self_level; i++)
        cout << "  ";
    bool is_invert = false;
    if((int)previous_id != -1)
        for(int i = 0; i < 2; i++)
            if(_fanin_idx[i] == previous_id)
                is_invert = _fanin_invert[i];
    cout << (is_invert? "!" : "") << "AIG " << _id << (marked() && rest_level > 0? " (*)" : "") << endl;
    
    //print fanouts
    if(rest_level > 0 && !marked())
        for(size_t i = 0; i < _fanout.size(); i++){
            _fanout[i]->printFanout(_id, self_level + 1, rest_level - 1);
            if(rest_level > 1)
                _fanout[i]->mark_node();
        }
}
int
AigGate::aig_under() const {
    if(marked())
        return 0;
    int sum = 1;
    for(int i = 0; i < 2; i++)
        if(_fanin[i] != NULL){
            sum += _fanin[i]->aig_under();
            _fanin[i]->mark_node();
        }
    return sum;
}
void
AigGate::print_aig_under(ostream& outfile) const {
    if(marked())
        return;

    for(int i = 0; i < 2; i++)
        if(_fanin[i] != NULL){
            _fanin[i]->print_aig_under(outfile);
            _fanin[i]->mark_node();
        }
    outfile << 2 * _id << " " << 2 * _fanin_idx[0] + (_fanin_invert[0]? 1 : 0) << " "
            << 2 * _fanin_idx[1] + (_fanin_invert[1]? 1 : 0) << endl;
}

/*======================================PI GATE================================================*/
void
PiGate::printGate() const {
    cout << "[" << cur_print_idx++ << "] " << "PI  " << _id;
    if(_symbol != "")
        cout << " (" << _symbol << ")";
    cout << endl;
}
void
PiGate::printFanin(bool is_neg, int self_level, int rest_level) const{

    for(int i = 0; i < self_level; i++)
        cout << "  ";
    cout << (is_neg? "!" : "") << "PI " << _id << endl;

    // cout << "done PiGate::printFanin()..." << endl;///
}
void
PiGate::printFanout(unsigned previous_id, int self_level, int rest_level) const {
    if(rest_level < 0)
        return;
    cout << "PI " << _id << endl;
    if(rest_level > 0){
        for(size_t i = 0; i < _fanout.size(); i++){
            _fanout[i]->printFanout(_id, 1, rest_level - 1);
            if(rest_level > 1)
                _fanout[i]->mark_node();
        }
    }
}

/*=============================================PO GATE====================================================*/
void
PoGate::printGate() const {
    //fanin
    if(_fanin[0] != NULL && _fanin[0]->marked() == false){
        _fanin[0]->printGate();
        _fanin[0]->mark_node();
    }
    //self
    cout << "[" << cur_print_idx++ << "] " << "PO  " << _id << " " << (_fanin[0] == NULL? "*" : "") << (_fanin_invert[0]? "!" : "") << (_fanin_idx[0]);
    if(_symbol != "")
        cout << " (" << _symbol << ")";
    cout << endl;
}
void
PoGate::printFanin(bool is_neg, int self_level, int rest_level) const{
    cout << "PO " << _id << endl;
    if(rest_level > 0){
        if(_fanin[0] == NULL)
            cout << (_fanin_invert[0]? "!" : "") << "  UNDEF " << _fanin_idx[0] << endl;
        else
        {
            _fanin[0]->printFanin(_fanin_invert[0], self_level + 1, rest_level - 1);

        }
        if(rest_level > 1)
            _fanin[0]->mark_node();
    }
    // cout << "done PoGate::printFanin()..." << endl;////
}
void
PoGate::printFanout(unsigned previous_id, int self_level, int rest_level) const {
    for(int i = 0; i < self_level; i++)
        cout << "  ";
    cout << (_fanin_invert[0] && (int)previous_id != -1 ? "!" : "") << "PO " << _id << endl;
}
int
PoGate::aig_under() const {
    if(_fanin[0] == NULL || _fanin[0]->marked())
        return 0;
    int sum = _fanin[0]->aig_under();
    _fanin[0]->mark_node();
    return sum;
}
void
PoGate::print_aig_under(ostream& outfile) const {
    if(_fanin[0] != NULL){
        _fanin[0]->print_aig_under(outfile);
        _fanin[0]->mark_node();
    }
}

/*======================================CONST GATE================================================*/
void
ConstGate::printGate() const {
    cout << "[" << cur_print_idx++ << "] " << "CONST0" << endl;
}
void
ConstGate::printFanin(bool is_neg, int self_level, int rest_level) const{
    for(int i = 0; i < self_level; i++)
        cout << "  ";
    cout << (is_neg? "!" : "") << "CONST " << _id << endl;

    // cout << "done ConstGate::printFanin()..." << endl;///
}
void
ConstGate::printFanout(unsigned previous_id, int self_level, int rest_level) const {
    if(rest_level < 0)
        return;
    cout << "CONST 0" << endl;
    if(rest_level > 0){
        for(size_t i = 0; i < _fanout.size(); i++){
            _fanout[i]->printFanout(0, 1, rest_level - 1);
            if(rest_level > 1)
                _fanout[i]->mark_node();
        }
    }
}