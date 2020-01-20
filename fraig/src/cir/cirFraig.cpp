/****************************************************************************
  FileName     [ cirFraig.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir FRAIG functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "sat.h"
#include "myHashMap.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::strash()" and "CirMgr::fraig()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/*******************************************/
/*   Public member functions about fraig   */
/*******************************************/
// _floatList may be changed.
// _unusedList and _undefList won't be changed
void
CirMgr::strash()
{
	// cout << "In the strash()..." << endl;
	
	genDFSList();

	HashMap<HashKey, size_t> *_hashTable = new HashMap<HashKey, size_t>(getHashSize(_dfsList.size()));


	for(int i = 0; i < _dfsList.size(); i++)
	{
		if(_dfsList[i]->isAig())
		{

			HashKey key(_dfsList[i]->_fanin_idx[0]*2+int(_dfsList[i]->_fanin_invert[0]), _dfsList[i]->_fanin_idx[1]*2+int(_dfsList[i]->_fanin_invert[1]));
			

			size_t data; //gate_id
			if(_hashTable->query(key, data)) // already have identical fanins gate
			{
				for(int j = 0; j < _dfsList[i]->_fanout.size(); j++)
				{
					_gates[data]->_fanout.push_back(_dfsList[i]->_fanout[j]); // store to the old gate

					for(int k = 0; k < 2; k++)
					{
						if(_dfsList[i]->_fanout[j]->_fanin[k] == _dfsList[i])
						{
							_dfsList[i]->_fanout[j]->_fanin[k] = _gates[data];
							_dfsList[i]->_fanout[j]->_fanin_idx[k] = data;
						}
					}
				}
				cout << "Strashing: " << data << " merging " << _dfsList[i]->_id << "..." << endl;
				_gates[_dfsList[i]->_id] = 0;

			}
			else
			{
				_hashTable->insert(key,_dfsList[i]->_id);
			}
		}
	}
}


void
CirMgr::printFEC() const
{
}

void
CirMgr::fraig()
{
}

/********************************************/
/*   Private member functions about fraig   */
/********************************************/
