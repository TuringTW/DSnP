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
#include <climits>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Keep "CirMgr::randimSim()" and "CirMgr::fileSim()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/
#define size_of_sizet sizeof(size_t);
/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static bool
MyGateListCmp (GateList i,GateList j) { return (i[0]->getId()<j[0]->getId()); }
/************************************************/
/*   Public member functions about Simulation   */
/************************************************/
void
CirMgr::randomSim()
{
  int counter = 0;
  unsigned max_fail = log(_totalGate.size());
  cout << "MAX_FAILS = " << max_fail << endl;
  while(_falseCount<max_fail){
    Rndsimulation();
    cout << _falseCount << " " << _fecGrps.size()<< endl;
    counter++;
  }
  cout << counter*32 << " patterns simulated" << endl;

  sort(_fecGrps.begin(), _fecGrps.end(), MyGateListCmp);
  registerFECGrp();
}

void
CirMgr::fileSim(ifstream& patternFile)
{
  string line;
  vector<size_t> patterns;
  patterns.resize(_piGate.size());
  patterns.clear();
  while(patternFile.good()){
    patternFile >> line;
    if(line.length()!=_piGate.size()){
      cerr << "Error: Pattern(" << line << ") length("<< line.length() <<") does not match the number of inputs(" << _piGate.size() << ") in a circuit!!" << endl;
      continue;
    }
    size_t i = 0;
    for (; i < line.size(); i++) {
      if(line[i]=='0'){
        _piGate[i]->simPattern = 0;
      }else if(line[i]=='1'){
        _piGate[i]->simPattern = ~0;
      }else{
        cerr << "Error: Pattern("<< line <<") contains a non-0/1 character('" << line[i] << "')." << endl;
        break;
      }
    }
    if(i!=line.size()){ continue; }

    simulation();

    *_simLog << line << " ";
    for (size_t i = 0; i < _poGate.size(); i++) {
      *_simLog << (bool)_poGate[i]->simPattern;
    }
    *_simLog << endl;
  }
  sort(_fecGrps.begin(), _fecGrps.end(), MyGateListCmp);
  registerFECGrp();
}

/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/
void
CirMgr::simulation() {
  for (size_t i = 0; i < _dfsList.size(); i++) {
    if(_dfsList[i]->getType()==PI_GATE||_dfsList[i]->getType()==CONST_GATE||_dfsList[i]->getType()==UNDEF_GATE){continue;}
    _dfsList[i]->simulate();
  }
  updateFECgrps();
}
void
CirMgr::Rndsimulation() {
  for (size_t i = 0; i < _totalGate.size(); i++) {
    if(_totalGate[i]->getType()==PI_GATE){ _totalGate[i]->simPattern = rnGen(INT_MAX)*2349809821%INT_MAX;  continue;}
    if(_totalGate[i]->getType()==AIG_GATE){
      _totalGate[i]->simulate();
    }
  }
  updateFECgrps();
}
void
CirMgr::updateFECgrps(){
  if(!_isSim){
    HashMap<HashKey4Ptn, GateList> _hashFECGrp(getHashSize(5*log(_aigList.size())*log(_aigList.size()))); //=====================<<<<<<=
    for (size_t j = 0; j < _totalGate.size(); j++) {
      if(_totalGate[j]==0){ continue; }
      GateList grp;
      HashKey4Ptn HK4P(_totalGate[j]->simPattern);
      HashKey4Ptn HK4PI(~_totalGate[j]->simPattern);
      if(_hashFECGrp.query(HK4P, grp)){
        _totalGate[j]->invEqvlnt = false;
        grp.push_back(_totalGate[j]);
        _hashFECGrp.update(HK4P, grp);
      }else if(_hashFECGrp.query(HK4PI, grp)){
        _totalGate[j]->invEqvlnt = true;
        grp.push_back(_totalGate[j]);
        _hashFECGrp.update(HK4PI, grp);
      }else{
        _totalGate[j]->invEqvlnt = false;
        grp.push_back(_totalGate[j]);
        _hashFECGrp.force_insert(HK4P, grp);
      }
    }
    for (HashMap<HashKey4Ptn, GateList>::iterator itr = _hashFECGrp.begin(); itr!=_hashFECGrp.end(); itr++) {
      if((*itr).second.size()<=1){continue;}
      _fecGrps.push_back((*itr).second);
    }
    _isSim = true;
    _falseCount = 0;
    return;
  }

  vector<GateList> _tempFECGrps;
  for (size_t i = 0; i < _fecGrps.size(); i++) {
    HashMap<HashKey4Ptn, GateList> _hashFECGrp(5*log(_fecGrps[i].size())*log(_fecGrps[i].size())); //=====================<<<<<<=
    for (size_t j = 0; j < _fecGrps[i].size(); j++) {
      GateList grp;
      HashKey4Ptn HK4P(_fecGrps[i][j]->simPattern);
      if(_fecGrps[i][j]->invEqvlnt){
        HK4P.set_pattern(~_fecGrps[i][j]->simPattern);
      }
      if(_hashFECGrp.query(HK4P, grp)){
        grp.push_back(_fecGrps[i][j]);
        _hashFECGrp.update(HK4P, grp);
        continue;
      }else{
        grp.push_back(_fecGrps[i][j]);
        _hashFECGrp.force_insert(HK4P, grp);
      }
    }
    for (HashMap<HashKey4Ptn, GateList>::iterator itr = _hashFECGrp.begin(); itr!=_hashFECGrp.end(); itr++) {
      if((*itr).second.size()<=1){continue;}
      _tempFECGrps.push_back((*itr).second);
    }
  }
  if (_fecGrps.size()==_tempFECGrps.size()) {
    _falseCount++;
  }else{
    _falseCount = 0;
  }
  _fecGrps = _tempFECGrps ;
}
void
CirMgr::registerFECGrp(){
  for (size_t i = 0; i < _totalGate.size(); i++) {
    if(_totalGate[i]==0){continue;}
    _totalGate[i]->_selfFECgroup.clear();
    _totalGate[i]->_selfFECgroup.resize(0);
  }
  for (size_t i = 0; i < _fecGrps.size(); i++) {
    for (size_t j = 0; j < _fecGrps[i].size(); j++) {
      _fecGrps[i][j]->_selfFECgroup = _fecGrps[i];
    }
  }
}
