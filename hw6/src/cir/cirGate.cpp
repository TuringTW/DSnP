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
   goFanin(level, 0, true, true, false);

}
void
CirGate::reportFanout(int level)
{
  assert (level >= 0);
  _cflag++;
  goFanout(level, 0, true, true);

}
void CirGate::goFanin(int level, int padding, bool isPosi, bool isPrint, bool isDFS = false) {
  if(isPrint){
    ostringstream oss;
    if (!isPosi) oss <<"!";
    oss << getTypeStr() << " " << _id;
    cout  <<  setw(2*padding) <<"" <<oss.str();
    if(_cflag == _flag&&getTypeStr()!="PI"&&level!=0){ cout << " (*)"; }
    cout << endl;
  }
  if (_cflag != _flag) {

    _flag = _cflag;
    if (level>0) {
      for (size_t i = 0; i < _fanin.size(); i++) {
        _fanin[i]->goFanin(level-1, padding+1, isPrint,  isDFS);
      }
    }
    if(isDFS) {
      ostringstream oss;
      for (size_t i = 0; i < _fanin.size(); i++) {
        oss << " ";
        if(!_fanin[i]->_isposi) {oss << "!";}
        oss << _fanin[i]->_fromGate->getId();
      }
      if (_alias.length()>0) oss <<" (" << _alias << ")";
      cout << "[" << plineNo << "] " << setw(4) << left<< getTypeStr() << getId() << oss.str() << endl;
      plineNo++;
    }
  }
}
void Pin::goFanin(int level, int padding, bool isPrint, bool isDFS){
  if(_fromGate!=0)_fromGate->goFanin(level, padding, _isposi, isPrint, isDFS);
}
void CirGate::goFanout(int level, int padding, bool isPosi, bool isPrint) {
  if(isPrint){
    ostringstream oss;
    if (!isPosi) oss <<"!";
    oss << getTypeStr() << " " << _id;
    cout  <<  setw(2*padding) <<"" <<oss.str();
    if(getTypeStr()!="PO"&&level!=0&&_cflag==_flag){cout << " (*)";}
    cout << endl;
  }
  if (_cflag != _flag) {
    _flag = _cflag;
    if (level>0) {
      for (size_t i = 0; i < _fanout.size(); i++) {
        _fanout[i]->goFanout(level-1, padding+1, isPrint);
      }
    }
  }
}
void Pin::goFanout(int level, int padding, bool isPrint){
  if(_toGate!=0)_toGate->goFanout(level, padding, _isposi, isPrint);
}
void CirGate::getDFSlist(){
  if (_cflag != _flag) {
    _flag = _cflag;
    if (getTypeStr()=="AIG") {
      plineNo++;
    }
    for (size_t i = 0; i < _fanout.size(); i++) {
      _fanin[i]->_fromGate->getDFSlist();
    }
  }
}
