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
	CirGate::_cflag++;
	for (size_t i = 0; i < _dfsList.size(); i++) {
		_dfsList[i]->_flag = CirGate::_cflag;
	}
	for (size_t i = 0; i < _totalGate.size(); i++) {
		if(_totalGate[i]==0||_totalGate[i]->getType()==PI_GATE||_totalGate[i]->_flag==CirGate::_cflag){ continue; }
		cout << "Sweeping: " << _totalGate[i]->getTypeStr() << "(" << _totalGate[i]->getId() << ") removed..." << endl;
		delete _totalGate[i];
		_totalGate[i] = 0;
	}
	getDFSlist();
}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void
CirMgr::optimize()
{
	for (size_t i = 0; i < _totalGate.size(); i++) {
		if(_totalGate[i]==0||_totalGate[i]->getType()!=AIG_GATE){ continue; }
		CirGate* const_zero;
		not_found_then_new(const_zero, 0, CONST_GATE, false);
		if(_totalGate[i]->_fanin[0]->_fromGate->getType()==CONST_GATE&&_totalGate[i]->_fanin[1]->_fromGate->getType()!=CONST_GATE){
			if(_totalGate[i]->_fanin[0]->_isposi){
				while(_totalGate[i]->_fanout.size()>0){
					_totalGate[i]->_fanout[0]->_fromGate = _totalGate[i]->_fanin[1]->_fromGate;
					_totalGate[i]->_fanout[0]->_isposi = !(_totalGate[i]->_fanin[1]->_isposi ^ _totalGate[i]->_fanout[0]->_isposi);
					_totalGate[i]->_fanin[1]->_fromGate->_fanout.push_back(_totalGate[i]->_fanout[0]);
					_totalGate[i]->_fanout.erase(_totalGate[i]->_fanout.begin());
				}
			}else{
				while(_totalGate[i]->_fanout.size()>0){
					_totalGate[i]->_fanout[0]->_fromGate = const_zero;
					const_zero->_fanout.push_back(_totalGate[i]->_fanout[0]);
					_totalGate[i]->_fanout.erase(_totalGate[i]->_fanout.begin());
				}
			}
		}else if(_totalGate[i]->_fanin[0]->_fromGate->getType()!=CONST_GATE&&_totalGate[i]->_fanin[1]->_fromGate->getType()==CONST_GATE){
			if(_totalGate[i]->_fanin[1]->_isposi){
				while(_totalGate[i]->_fanout.size()>0){
					_totalGate[i]->_fanout[0]->_fromGate = _totalGate[i]->_fanin[0]->_fromGate;
					_totalGate[i]->_fanout[0]->_isposi = !(_totalGate[i]->_fanin[0]->_isposi ^ _totalGate[i]->_fanout[0]->_isposi);
					_totalGate[i]->_fanin[0]->_fromGate->_fanout.push_back(_totalGate[i]->_fanout[0]);
					_totalGate[i]->_fanout.erase(_totalGate[i]->_fanout.begin());
				}
			}else{
				while(_totalGate[i]->_fanout.size()>0){
					_totalGate[i]->_fanout[0]->_fromGate = const_zero;
					const_zero->_fanout.push_back(_totalGate[i]->_fanout[0]);
					_totalGate[i]->_fanout.erase(_totalGate[i]->_fanout.begin());
				}
			}
		}else if(_totalGate[i]->_fanin[0]->_fromGate==_totalGate[i]->_fanin[1]->_fromGate){
			if(_totalGate[i]->_fanin[0]->_isposi==_totalGate[i]->_fanin[1]->_isposi){
				while(_totalGate[i]->_fanout.size()>0){
					_totalGate[i]->_fanout[0]->_fromGate = _totalGate[i]->_fanin[0]->_fromGate;
					_totalGate[i]->_fanout[0]->_isposi = !(_totalGate[i]->_fanin[0]->_isposi ^ _totalGate[i]->_fanout[0]->_isposi);
					_totalGate[i]->_fanin[0]->_fromGate->_fanout.push_back(_totalGate[i]->_fanout[0]);
					_totalGate[i]->_fanout.erase(_totalGate[i]->_fanout.begin());
				}
			}else{
				while(_totalGate[i]->_fanout.size()>0){
					_totalGate[i]->_fanout[0]->_fromGate = const_zero;
					_totalGate[i]->_fanout[0]->_isposi = !_totalGate[i]->_fanout[0]->_isposi;
					const_zero->_fanout.push_back(_totalGate[i]->_fanout[0]);
					_totalGate[i]->_fanout.erase(_totalGate[i]->_fanout.begin());
				}
			}
		}else{
			continue;
		}
		unsigned tempid = _totalGate[i]->getId();
		delete _totalGate[i];
		_totalGate[tempid] = 0;
	}
	getDFSlist();
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
