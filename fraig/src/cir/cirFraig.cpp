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
  HashMap<HashKey, CirGate*> _hashmap(getHashSize(5*log(_aigList.size())*log(_aigList.size())));
  for (size_t i = 0; i < _dfsList.size(); i++) {
    if(_dfsList[i]->getType()!=AIG_GATE){ continue; }
    unsigned lfin1 = 2*_dfsList[i]->_fanin[0]->_fromGate->getId()+(unsigned)!_dfsList[i]->_fanin[0]->_isposi;
    unsigned lfin2 = 2*_dfsList[i]->_fanin[1]->_fromGate->getId()+(unsigned)!_dfsList[i]->_fanin[1]->_isposi;
    HashKey key(lfin1, lfin2);
    CirGate* target;
    if(_hashmap.query(key, target)){
      cout << "Strashing: " << target->getId() << " merging " << _dfsList[i]->getId() << "..." << endl;
      merge(target, _dfsList[i], false);
    }else{
      _hashmap.force_insert(key, _dfsList[i]);
    }
  }
  getDFSlist();
}

void
CirMgr::fraig()
{
  SatSolver solver;
  solver.initialize();

  for (size_t i = 0; i < _totalGate.size(); ++i)
  {
    if(_totalGate[i]==0){continue;}
    Var v = solver.newVar();
    _totalGate[i]->setVar(v);

    if(_totalGate[i]->getType()==AIG_GATE){
      solver.addAigCNF(_totalGate[i]->getVar(), _totalGate[i]->_fanin[0]->_fromGate->getVar(), !_totalGate[i]->_fanin[0]->_isposi, _totalGate[i]->_fanin[1]->_fromGate->getVar(), !_totalGate[i]->_fanin[1]->_isposi); 
    }    
  }
  bool result;


  while(_fecGrps.size()>0)
  {
    while(_fecGrps[0].size()>1)
    {
      Var newV = solver.newVar();
      solver.addXorCNF(newV, _fecGrps[0][0]->getVar(), false, _fecGrps[0][1]->getVar(), !(_fecGrps[0][0]->invEqvlnt==_fecGrps[0][1]->invEqvlnt));
      // 
      solver.assumeRelease();  // Clear assumptions
      solver.assumeProperty(newV, true);  // k = 1
      result = solver.assumpSolve();
      if(!result){  //"UNSAT"
        cout << "Fraig: " << _fecGrps[0][0]->getId() << " merging " << _fecGrps[0][1]->getId() << "..." << endl;
        merge(_fecGrps[0][0], _fecGrps[0][1], !(_fecGrps[0][0]->invEqvlnt==_fecGrps[0][1]->invEqvlnt));

        
      }
      _fecGrps[0].erase(_fecGrps[0].begin()+1);
    } 
    _fecGrps.erase(_fecGrps.begin());
  }
  getDFSlist();
}

/********************************************/
/*   Private member functions about fraig   */
/********************************************/

void
CirMgr::merge(CirGate* target, CirGate* gate, bool invEqvlnt) {
  
  while(gate->_fanout.size()>0){
    gate->_fanout[0]->_fromGate = target;
    if(invEqvlnt){
      gate->_fanout[0]->_isposi = (! gate->_fanout[0]->_isposi);
    }
    target->_fanout.push_back(gate->_fanout[0]);
    gate->_fanout.erase(gate->_fanout.begin());
  }
  _totalGate[gate->getId()] = 0;
  delete gate;
}
