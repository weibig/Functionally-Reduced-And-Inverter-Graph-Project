/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir optimization functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::sweep()" and "CirMgr::optimize()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/**************************************************/
/*   Public member functions about optimization   */
/**************************************************/
// Remove unused gates
// DFS list should NOT be changed
// UNDEF, float and unused list may be changed
void
CirMgr::sweep()
{
	// cout << "In the sweep()..." << endl;
	genDFSList();
	

	// cout << "Size before sweep: " << _gates.size() << endl;////
	// cout << "_dfsList = ";
	// for(int i = 0; i < _dfsList.size(); i++)
	// 	cout << _dfsList[i]->_id << " " ;

	// cout << endl;

	for(size_t i = 1; i < _gates.size(); i++)
	{
		if(_gates[i] == 0)
			continue;
		if(_gates[i]->visited == false && _gates[i]->getTypeStr() != "PI")
		{
			cout << "Sweeping: " << _gates[i]->getTypeStr() << "(" << _gates[i]->_id << ") removed..." << endl;

			for(int k = 0; k < 2; k++)
			{
				if(_gates[i]->_fanin[k] == 0)
					continue;
				for(size_t j = 0; j < _gates[i]->_fanin[k]->_fanout.size(); j++)
				{
					if(_gates[i]->_fanin[k]->_fanout[j] == _gates[i])
						_gates[i]->_fanin[k]->_fanout.erase(_gates[i]->_fanin[k]->_fanout.begin()+j);
				}
			}

			_gates[i] = 0;

		}
		
	}
	// cout  << "Size after sweep: " << _gates.size() << endl;////
}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void
CirMgr::optimize()
{
	// for(int j = 0; j < 10; j++)
	// {
		for(size_t i = 0; i < _pos.size(); i++)
		{
			optHelper(_pos[i]); // AIG now, start with aig_before_po
		}
	// }
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
void
CirMgr::optHelper(CirGate* input)
{
	for(int l = 0; l < 2; l++)
	{
		// cout << "input = " << input->_id << endl;

		if(input->_fanin[l] == NULL)
		{
			// cout << "No fanin gate's id = " << input->_id << endl;
			return;
		}


		// cout << "l = " << l << "; id = " << input->_fanin[l]->_id << endl;

		CirGate* tmp = input->_fanin[l]; // AIG now, start with aig_before_po

		int case_switch = -1; // 0 is to_zero, 1 is to_a, 2 is to_invert_a
		CirGate* a = 0;
		bool a_invert;

		
		if(input->_fanin[l]->_fanin[0] == NULL)
		{
			optHelper(input->_fanin[l]);
			continue;
		}
		else if(tmp->_fanin_idx[0] == tmp->_fanin_idx[1])
		{
			// cout << "ha" << endl;
			if(tmp->_fanin_invert[0] == tmp->_fanin_invert[1])
			{
				if(tmp->_fanin_invert[0] == 0) // 0 0
					case_switch = 1;
				else // 1 1
					case_switch = 2;

				a = tmp->_fanin[0];
				a_invert = tmp->_fanin_invert[0];
			}
			else // 1 0
			{
				case_switch = 0;
				a = _gates[0];
			}
			//////////////// to gate 0!!!!

		}
		// aig_before_po 's fanin has only one const0 or const1
		else if(tmp->_fanin_idx[0] == 0 || tmp->_fanin_idx[1] == 0) 
		{
			// cout << "hi" << endl;
			for(int j = 0; j < 2; j++)  // 0 1 => case to_zero
			{
				if(tmp->_fanin_idx[j] == 0)
				{

					if(tmp->_fanin_invert[j] == 1) // Fanin has const 1 (case 1)
					{
						case_switch = 1;
						// cout << ":(" << endl;
						if(j == 1)
						{
							a = tmp->_fanin[0];
							a_invert = tmp->_fanin_invert[0];
						}
						
						else if(j == 0)
						{
							a = tmp->_fanin[1];
							a_invert = tmp->_fanin_invert[1];
						}					
						// cout << ":)" << endl;
					}
					else if(tmp->_fanin_invert[j] == 0)
					{
						case_switch = 0;
						a = _gates[0];
					}
				}
			}
		}
		else
		{
			optHelper(input->_fanin[l]);

			if(input->_fanin[l]->_changed == true)
			{
				input->_fanin[l]->_changed = false;
				l--;
			}
			continue;
		}

		// cout << "case " << case_switch << ", a = " << a->_id << endl;

		if(input->_fanin[l]->getTypeStr() == "PI")
			continue;

		cout << "Simplifying: " << a->_id << " merging " << (a_invert? "!" : "") << input->_fanin[l]->_id << "..." << endl;


		// 0 is to_zero, 1 is to_a, 2 is to_invert_a
		if(case_switch == 0)
		{
			for(int j = 0; j < tmp->_fanout.size(); j++)
			{
				a->_fanout.push_back(tmp->_fanout[j]);
				for(int i = 0; i < 2; i++)
				{
					if(tmp->_fanout[j]->_fanin_idx[i] == tmp->_id && tmp->_fanout[j] != input)
					{
						// cout << "check " << tmp->_id << "\'s fanout gate = " << tmp->_fanout[j]->_id << endl;
						tmp->_fanout[j]->_fanin[i] = a;
						tmp->_fanout[j]->_fanin_idx[i] = 0;
					}
				}
			}
			delete tmp;
			tmp = 0;
			input->_fanin[l] = a;


			size_t idx = input->_fanin_idx[l];
			_gates[idx] = 0;

			

			input->_fanin_invert[l] = a_invert;
			input->_fanin_idx[l] = 0;

		}
		else if(case_switch == 1)
		{
			
			// input => tmp
			

			for(int j = 0; j < tmp->_fanout.size(); j++)
			{
				a->_fanout.push_back(tmp->_fanout[j]);
				for(int i = 0; i < 2; i++)
				{
					if(tmp->_fanout[j]->_fanin_idx[i] == tmp->_id && tmp->_fanout[j] != input)
					{
						// cout << "check " << tmp->_id << "\'s fanout gate = " << tmp->_fanout[j]->_id << endl;
						tmp->_fanout[j]->_fanin[i] = a;
						tmp->_fanout[j]->_fanin_idx[i] = a->_id;
					}
				}
			}

			delete tmp;
			tmp = 0;
			input->_fanin[l] = a;

			size_t idx = input->_fanin_idx[l];
			_gates[idx] = 0;

			input->_fanin_invert[l] = a_invert;
			input->_fanin_idx[l] = a->_id;

		}
		else if(case_switch == 2)
		{
			for(int j = 0; j < tmp->_fanout.size(); j++)
			{
				a->_fanout.push_back(tmp->_fanout[j]);
				for(int i = 0; i < 2; i++)
				{
					if(tmp->_fanout[j]->_fanin_idx[i] == tmp->_id && tmp->_fanout[j] != input)
					{
						// cout << "check " << tmp->_id << "\'s fanout gate = " << tmp->_fanout[j]->_id << endl;
						tmp->_fanout[j]->_fanin[i] = a;
						tmp->_fanout[j]->_fanin_idx[i] = a->_id;
					}
				}
			}
			delete tmp;
			tmp = 0;
			input->_fanin[l] = a;

			size_t idx = input->_fanin_idx[l];
			_gates[idx] = 0;
			input->_fanin_invert[l] = !a_invert;
			input->_fanin_idx[l] = a->_id;
			
		}
		input->_changed = true;
		// cout << "changed gate = " << input->_id << endl;


		optHelper(input->_fanin[l]);
		
	}
	
}
