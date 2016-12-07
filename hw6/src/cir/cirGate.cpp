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
   goFanin(level, 0, true);

}
void
CirGate::reportFanout(int level)
{
  assert (level >= 0);
  _cflag++;
  goFanout(level, 0, true);

}
void CirGate::goFanin(int level, int padding, bool isPosi) {
  ostringstream oss;
  if (!isPosi) oss <<"!";
  oss << getTypeStr() << " " << _id;

  cout  <<  setw(2*padding) <<"" <<oss.str();
  if (_cflag == _flag) {
    cout << " (*)" << endl;
  }else{
    cout << endl;
    _flag = _cflag;
    if (level>0) {
      for (size_t i = 0; i < _fanin.size(); i++) {
        _fanin[i]->goFanin(level-1, padding+1);
      }
    }
  }
}
void Pin::goFanin(int level, int padding){
  if(_fromGate!=0)_fromGate->goFanin(level, padding, _isposi);
}
void CirGate::goFanout(int level, int padding, bool isPosi) {
  ostringstream oss;
  if (!isPosi) oss <<"!";
  oss << getTypeStr() << " " << _id;

  cout  <<  setw(2*padding) <<"" <<oss.str();
  if (_cflag == _flag) {
    cout << " (*)" << endl;
  }else{
    cout << endl;
    _flag = _cflag;
    if (level>0) {
      for (size_t i = 0; i < _fanout.size(); i++) {
        _fanout[i]->goFanout(level-1, padding+1);
      }
    }
  }
}
void Pin::goFanout(int level, int padding){
  if(_toGate!=0)_toGate->goFanout(level, padding, _isposi);
}
