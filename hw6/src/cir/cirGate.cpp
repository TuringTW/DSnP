/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <stdarg.h>
#include <cassert>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

extern CirMgr *cirMgr;

// TODO: Implement memeber functions for class(es) in cirGate.h

// static variable
int CirGate::_cflag=0;
int CirGate::plineNo=0;
/**************************************/
/*   class CirGate member functions   */
/**************************************/
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
CirGate::reportFanin(int level)
{
   assert (level >= 0);
   _cflag++;
   goFanin(level, 0, true, true);

}
void
CirGate::reportFanout(int level)
{
  assert (level >= 0);
  _cflag++;
  goFanout(level, 0, true, true);

}
void CirGate::goFanin(int level, int padding, bool isPosi, bool isPrint) {
  if(isPrint){
    ostringstream oss;
    if (!isPosi) oss <<"!";
    oss << getTypeStr() << " " << _id;
    cout  <<  setw(2*padding) <<"" <<oss.str();
    if(_cflag == _flag&&(getType()!=PI_GATE&&getType()!=CONST_GATE)&&level!=0){ cout << " (*)"; }
    cout << endl;
  }
  if (_cflag != _flag) {

    _flag = _cflag;
    if (level>0) {
      for (size_t i = 0; i < _fanin.size(); i++) {
        if(_fanin[i]->_fromGate!=0){_fanin[i]->_fromGate->goFanin(level-1, padding+1, _fanin[i]->_isposi, isPrint);}
      }
    }
  }
}
void CirGate::goFanout(int level, int padding, bool isPosi, bool isPrint) {
  if(isPrint){
    ostringstream oss;
    if (!isPosi) {oss <<"!";}
    oss << getTypeStr() << " " << _id;
    cout  <<  setw(2*padding) <<"" <<oss.str();
    if(getType()!=PO_GATE&&level!=0&&_cflag==_flag){cout << " (*)";}
    cout << endl;
  }
  if (_cflag != _flag) {
    _flag = _cflag;
    if (level>0) {
      for (size_t i = 0; i < _fanout.size(); i++) {
        if(_fanout[i]->_toGate!=0){_fanout[i]->_toGate->goFanout(level-1, padding+1, _fanout[i]->_isposi, isPrint);}
      }
    }
  }
}
void CirGate::getDFSlist(bool isPosi,ostream& os){
  if (_cflag != _flag) {
    _flag = _cflag;
    if (getType()==AIG_GATE) {
      plineNo++;
    }
    for (size_t i = 0; i < _fanin.size(); i++) {
      _fanin[i]->_fromGate->getDFSlist(_fanin[i]->_isposi, os);
    }
    int fanin1=0, fanin2=0;
    if (getType()==AIG_GATE&&_cflag==_flag) {
      fanin1 = (_fanin[0]->_fromGate->getId())*2;
      fanin2 = (_fanin[1]->_fromGate->getId())*2;
      if(!_fanin[0]->_isposi){ fanin1++; }
      if(!_fanin[1]->_isposi){ fanin2++; }
      os << getId()*2 << " " << fanin1 << " " << fanin2 << endl;
    }
  }
}
void CirGate::printDFSlist(bool isPosi){
  if (_cflag != _flag) {
    _flag = _cflag;
    ostringstream oss;
    for (size_t i = 0; i < _fanin.size(); i++) {
      oss << " ";
      if(_fanin[i]->_fromGate->getType()==UNDEF_GATE){
        oss << "*";
      }else{
        _fanin[i]->_fromGate->printDFSlist(_fanin[i]->_isposi);
      }
      if(!_fanin[i]->_isposi) {oss << "!";}
      oss << _fanin[i]->_fromGate->getId();
    }
    if (_alias.length()>0) oss <<" (" << _alias << ")";
    cout << "[" << plineNo << "] " << setw(4) << left<< getTypeStr() << getId() << oss.str() << endl;
    plineNo++;
  }
}
