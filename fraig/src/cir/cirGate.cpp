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

// TODO: Keep "CirGate::reportGate()", "CirGate::reportFanin()" and
//       "CirGate::reportFanout()" for cir cmds. Feel free to define
//       your own variables and functions.

extern CirMgr *cirMgr;

// static variable
int CirGate::_cflag=0;

/**************************************/
/*   class CirGate member functions   */
/**************************************/
Pin::~Pin()
{
	for (size_t i = 0; i < _fromGate->_fanout.size(); ++i){
    if(_fromGate->_fanout[i]==this){
      _fromGate->_fanout.erase(_fromGate->_fanout.begin()+i);
      break;
    }
  }
  for (size_t i = 0; i < _toGate->_fanin.size(); ++i){
    if(_toGate->_fanin[i]==this){
      _toGate->_fanin.erase(_toGate->_fanin.begin()+i);
      break;
    }
  }
}
void
CirGate::reportGate() const
{
  ostringstream oss;
  oss << getTypeStr() << "(" << _id << ")";
  if (_alias.length()>0) oss <<"\"" << _alias << "\"";
  oss << ", line " << _lineNo;
  cout << "==================================================" << endl;
  cout << "= " << left  << setfill(' ')<< setw(47)<<  oss.str() << "=" << endl;
  cout << "==================================================" << endl;
}

void
CirGate::reportFanin(int level) const
{
   assert (level >= 0);
   _cflag++;
   goFaninPrint(level, 0, true);
}

void
CirGate::reportFanout(int level) const
{
   assert (level >= 0);
   _cflag++;
   goFanoutPrint(level, 0, true);
}

void 
CirGate::goFaninPrint(int level, int padding, bool isPosi) const {
	ostringstream oss;
	if (!isPosi) oss <<"!";
	oss << getTypeStr() << " " << _id;
	cout  <<  setw(2*padding) <<"" <<oss.str();
	if(_cflag == _flag&&(getType()!=PI_GATE&&getType()!=CONST_GATE)&&level!=0){ cout << " (*)"; }
	cout << endl;
  if (_cflag != _flag) {
    _flag = _cflag;
    if (level>0) {
      for (size_t i = 0; i < _fanin.size(); i++) {
        if(_fanin[i]->_fromGate!=0){_fanin[i]->_fromGate->goFaninPrint(level-1, padding+1, _fanin[i]->_isposi);}
      }
    }
  }
}

void 
CirGate::goFanoutPrint(int level, int padding, bool isPosi) const {
  
  ostringstream oss;
  if (!isPosi) {oss <<"!";}
  oss << getTypeStr() << " " << _id;
  cout  <<  setw(2*padding) <<"" <<oss.str();
  if(getType()!=PO_GATE&&level!=0&&_cflag==_flag){cout << " (*)";}
  cout << endl;

  if (_cflag != _flag) {
    _flag = _cflag;
    if (level>0) {
      for (size_t i = 0; i < _fanout.size(); i++) {
        if(_fanout[i]->_toGate!=0){_fanout[i]->_toGate->goFanoutPrint(level-1, padding+1, _fanout[i]->_isposi);}
      }
    }
  }
}
void 
CirGate::getDFS(GateList &_dfslist){
	if (_cflag != _flag) {
    _flag = _cflag;
		if(getType()==UNDEF_GATE)return;
    for (size_t i = 0; i < _fanin.size(); i++) {
      if(_fanin[i]->_fromGate!=0){_fanin[i]->_fromGate->getDFS(_dfslist);}
    }
    _dfslist.push_back(this);
  }
}

void 
CirGate::getDFSfanout() const{
	if (_cflag != _flag) {
    _flag = _cflag;
		if(getType()==UNDEF_GATE)return;
    for (size_t i = 0; i < _fanout.size(); i++) {
      if(_fanout[i]->_toGate!=0){_fanout[i]->_toGate->getDFSfanout();}
    }
  }	
}


void 
CirGate::printGate() const {
	ostringstream oss;
  for (size_t i = 0; i < _fanin.size(); i++) {
    oss << " ";
    if(_fanin[i]->_fromGate->getType()==UNDEF_GATE){ oss << "*"; }
    if(!_fanin[i]->_isposi) { oss << "!"; }
    oss << _fanin[i]->_fromGate->getId();
  }
  if (_alias.length()>0) oss << " (" << _alias << ")";
	cout << setw(4) << left << getTypeStr() << getId() << oss.str() << endl;
}