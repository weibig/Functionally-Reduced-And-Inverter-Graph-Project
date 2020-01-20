/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir simulation functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Keep "CirMgr::randimSim()" and "CirMgr::fileSim()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/************************************************/
/*   Public member functions about Simulation   */
/************************************************/
void
CirMgr::randomSim()
{
	//initialize

	for(int i = 0; i < _gates.size(); i++)
	{
		if(_gates[i] != 0)
		{
			if(_gates[i]->_signalList.length() != 0 )
				_gates[i]->_signalList = "";
		}
	}

	// make _baseSignal
	_gates[0]->_baseSignal = '0';

	for(int i = 0; i < _pis_idx.size(); i++)
	{
		_gates[_pis_idx[i]]->_baseSignal = '0';
	}
	while(true)
	{
		bool somethingChanged = false;
		for(int i = 0; i < _gates.size(); i++)
		{
			if(_gates[i] == 0)
				continue;
			if(_gates[i]->isAig() && _gates[i]->_baseSignal == NULL)
			{
				if(_gates[i]->_fanin[0]->_baseSignal != NULL && _gates[i]->_fanin[1]->_baseSignal != NULL)
				{
					bool in01 = (_gates[i]->_fanin[0]->_baseSignal == '1') ? true : false;
					if(_gates[i]->_fanin_invert[0])
						in01 = !in01;

					bool in02 = (_gates[i]->_fanin[1]->_baseSignal == '1') ? true : false;
					if(_gates[i]->_fanin_invert[1])
						in02 = !in02;

					bool result = (in01 && in02);

					_gates[i]->_baseSignal = (result? '1' : '0');
					somethingChanged = true;

					// cout << "Gate " << _gates[i]->_fanin[0]->_id << " & " << _gates[i]->_fanin[1]->_id << ", " << _gates[i]->_fanin[0]->_baseSignal << " " << _gates[i]->_fanin[1]->_baseSignal;
					// cout << " change the gate " << _gates[i]->_id << " to " << _gates[i]->_baseSignal << endl;
					// cout << "Size = " << _gates[i]->_signalList.size() << endl;
				}

			}
		}

		if(!somethingChanged)
			break;
	}

	// other simulation
	_simList = new HashMap<SignalKey, vector<size_t>>(getHashSize(_gates.size()));

	simulationCnt = 64;
	int patternCnt = 0;

	for(int i = 0; i < simulationCnt; i++)
	{
		for(int j = 0; j < _pis_idx.size(); j++)
		{
			_gates[_pis_idx[j]]->_signalList += rnGen(1);
		}
		_gates[0]->_signalList += '0';

		simulateHelper(i); // store the result in the _siinalList

	}

	for(int i = 0; i < _gates.size(); i++)
	{
		if(_gates[i] != 0)
		{
			// cout << "Gate " << _gates[i]->_id << " _signalList.length() = " << _gates[i]->_signalList.length() << endl;
			if((_gates[i]->_signalList.length() == 0 || !_gates[i]->isAig()) && i != 0)
				continue;
			// if(_gates[i]->_signalList.length() != simulationCnt)
				// cout << "Error: there are bugsssss" << endl;

			// cout << _gates[i]->_id << " is here" << endl;
			string tmp = _gates[i]->_signalList;

			SignalKey key(tmp, '\0');

			if(_simList->add(key,_gates[i]->_id))
				patternCnt++;
		}
	}

	
	cout << simulationCnt << " patterns simulated." << endl;

}

void
CirMgr::fileSim(ifstream& patternFile)
{
	//initialize
	for(int i = 0; i < _gates.size(); i++)
	{
		if(_gates[i] != 0)
		{
			if(_gates[i]->_signalList.length() != 0 )
				_gates[i]->_signalList = "";
		}
	}


	// make _baseSignal
	
	_gates[0]->_baseSignal = '0';

 	// cout << "make _baseSignal" << endl;
 	for(int i = 0; i < _pis_idx.size(); i++)
	{
		_gates[_pis_idx[i]]->_baseSignal = '0';
	}

	while(true)
	{
		bool somethingChanged = false;
		for(int i = 0; i < _gates.size(); i++)
		{
			if(_gates[i] == 0)
				continue;
			if(_gates[i]->isAig() && _gates[i]->_baseSignal == NULL)
			{
				if(_gates[i]->_fanin[0]->_baseSignal != NULL && _gates[i]->_fanin[1]->_baseSignal != NULL )
				{
					bool in01 = (_gates[i]->_fanin[0]->_baseSignal == '1') ? true : false;
					if(_gates[i]->_fanin_invert[0])
						in01 = !in01;

					bool in02 = (_gates[i]->_fanin[1]->_baseSignal == '1') ? true : false;
					if(_gates[i]->_fanin_invert[1])
						in02 = !in02;

					bool result = (in01 && in02);

					_gates[i]->_baseSignal = (result? '1' : '0');
					somethingChanged = true;

					// cout << "Gate " << _gates[i]->_fanin[0]->_id << " & " << _gates[i]->_fanin[1]->_id << ", " << _gates[i]->_fanin[0]->_baseSignal << " " << _gates[i]->_fanin[1]->_baseSignal;
					// cout << " change the gate " << _gates[i]->_id << " to " << _gates[i]->_baseSignal << endl;
					// cout << "Size = " << _gates[i]->_signalList.size() << endl;
				}

			}
		}

		if(!somethingChanged)
			break;
	}

	// other simulation

	string str;
	int patternCnt = 0;
	simulationCnt = 0;
	_simList = new HashMap<SignalKey, vector<size_t>>(getHashSize(_gates.size())); 
	// <string bitwisekey, literal gateID>


	while (patternFile >> str) 
	{
		// cout << "No: " << simulationCnt << endl;
		if(str.length() != _pis_idx.size())
		{
			cerr << "Error: Pattern(" << str << ") length(" << str.length() << ") does not match the number of inputs(" << _pis_idx.size() << ") in a circuit!!" << endl << endl;
			cout << "0 patterns simulated." << endl;
			return;
		}
		for(int i = 0; i < str.length(); i++)
		{
			_gates[_pis_idx[i]]->_signalList += str[i];
		}

		//gate0
		
		_gates[0]->_signalList += '0';

		simulateHelper(simulationCnt); // store the result in the _siinalList

		simulationCnt++;

	}

	for(int i = 0; i < _gates.size(); i++)
	{
		if(_gates[i] != 0)
		{
			// cout << "Gate " << _gates[i]->_id << " _signalList.length() = " << _gates[i]->_signalList.length() << endl;
			if((_gates[i]->_signalList.length() == 0 || !_gates[i]->isAig()) && i != 0)
				continue;
			// if(_gates[i]->_signalList.length() != simulationCnt)
			// 	cout << "Error: there are bugsssss" << endl;

			// cout << _gates[i]->_id << " is here" << endl;
			string tmp = _gates[i]->_signalList;

			char base;
			if(cirMgr->simulationCnt % 64 == 0)
		       base = '\0';
		    else  
		       base = _gates[i]->_baseSignal;

			SignalKey key(tmp, base);

			if(_simList->add(key,_gates[i]->_id))
				patternCnt++;
		}
	}

	

	cout << simulationCnt << " patterns simulated." << endl;
	
}

/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/
void
CirMgr::simulateHelper(int idx)
{
	// cout << "In the simulateHelper()..." << endl;

	while(true)
	{
		bool somethingChanged = false;
		for(int i = 0; i < _gates.size(); i++)
		{
			if(_gates[i] == 0)
				continue;
			if(_gates[i]->isAig() && _gates[i]->_signalList[idx] == NULL)
			{
				if(_gates[i]->_fanin[0]->_signalList[idx] != NULL && _gates[i]->_fanin[1]->_signalList[idx] != NULL && (_gates[i]->_fanout.size() != 0 || !_gates[i]->isAig()))
				{
					bool in01 = (_gates[i]->_fanin[0]->_signalList[idx] == '1') ? true : false;
					if(_gates[i]->_fanin_invert[0])
						in01 = !in01;

					bool in02 = (_gates[i]->_fanin[1]->_signalList[idx] == '1') ? true : false;
					if(_gates[i]->_fanin_invert[1])
						in02 = !in02;

					bool result = (in01 && in02);

					_gates[i]->_signalList += (result? '1' : '0');
					somethingChanged = true;

					// cout << "Gate " << _gates[i]->_fanin[0]->_id << " & " << _gates[i]->_fanin[1]->_id << ", " << _gates[i]->_fanin[0]->_signalList[idx] << " " << _gates[i]->_fanin[1]->_signalList[idx];
					// cout << " change the gate " << _gates[i]->_id << " to " << _gates[i]->_signalList[idx] << endl;
					// cout << "Size = " << _gates[i]->_signalList.size() << endl;
				}

			}
		}

		if(!somethingChanged)
			break;
	}
	

}
