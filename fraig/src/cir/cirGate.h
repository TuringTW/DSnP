/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include <iostream>
#include <cmath>
#include <climits>
#include "cirDef.h"
#include "sat.h"

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

class CirGate;
class AigGate;
class PiGate;
class PoGate;
class ConstGate;
class CirGateS;
class Pin;
//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
class Pin
{
  friend class CirGate;
  friend class CirMgr;
public:
  Pin(CirGate* from, CirGate* to, bool isposi): _fromGate(from), _toGate(to), _isposi(isposi){};
  ~Pin();

  // bool operator == (const Pin& other) const { return other._fromGate==_fromGate&&other._isposi==_isposi; }
private:
  CirGate* _fromGate;
  CirGate* _toGate;
  bool _isposi;
};

class CirGate
{
  friend class CirMgr;
  friend class Pin;
public:
  CirGate(int lineNo, int id):_lineNo(lineNo), _id(id), _flag(0), simPattern(0), invEqvlnt(false){}
  virtual ~CirGate() {
    while(_fanin.size()>0){
      delete _fanin[0];
    }
    while(_fanout.size()>0){
      delete _fanout[0];
    }
  }
  // Basic access methods
  virtual string getTypeStr() const =0;
  unsigned getLineNo() const { return _lineNo; }
  virtual bool isAig() const = 0;
   // self def
  void setLineNo(int lineNo){_lineNo = lineNo;}
  virtual GateType getType() const =0;
  // void setDef(){_isDef = true;}
  // const bool getDef() const{return _isDef;}
  virtual void setDef() = 0;
  virtual const bool getDef() const = 0;
  const unsigned getId() const{return _id;}

    // fanin/out
  void addfanin(CirGate* cirg, bool isposi){
    Pin* con_pin = new Pin(cirg, this, isposi);
    _fanin.push_back(con_pin);
    cirg->_fanout.push_back(con_pin);
  }
  virtual bool setFanIn(CirGate* cirg, bool isposi)=0;
  const size_t getfaninSize(){return _fanin.size();}
  const size_t getfanoutSize(){return _fanout.size();}


   // Printing functions
  virtual void printGate() const;
  void reportGate() const;
  void reportFanin(int level) const;
  void reportFanout(int level) const;

   // print fanin/out
  void goFanout(int, int, bool) const;
  void goFanoutPrint(int, int, bool) const ;
  void goFanin(int, int, bool) const ;
  void goFaninPrint(int, int, bool) const;
  //DFS
  void getDFS(GateList &_dfslist);
  void getDFSfanout() const;
  // fraig
  Var getVar() const { return _var; }
  void setVar(const Var& v) { _var = v; }

  bool simulate() const {
    size_t result = ~0;
    for (size_t i = 0; i < _fanin.size(); i++) {
      if(_fanin[i]->_isposi){
        result &= _fanin[i]->_fromGate->simPattern;
      }else{
        result &= (~_fanin[i]->_fromGate->simPattern);
      }
    }
    bool status = (simPattern == result);
    simPattern = result;
    return status;
  }

private:

  int _lineNo, _id;
  mutable int _flag;
  string _alias;
  vector<Pin*> _fanin;
  vector<Pin*> _fanout;
  // signal
  mutable unsigned simPattern;
  bool invEqvlnt;
  GateList _selfFECgroup;

  // fraig
  Var _var;

  static int _cflag;
};
class AigGate : public CirGate
{
public:
  AigGate(int lineNo, int id, bool isdef):CirGate(lineNo, id), _isDef(isdef){};
  ~AigGate(){}
  string getTypeStr() const {
    if(getDef())return "AIG";
    else return "UNDEF";
  }
  GateType getType() const {
    if(getDef())return AIG_GATE;
    else return UNDEF_GATE;
  }
  bool isAig() const { return true; }
  bool setFanIn(CirGate* cirg, bool isposi){
    if (getfaninSize()<2) {
      addfanin(cirg, isposi);
      return true;
    }
    return false;
  }

  void setDef(){
    _isDef = true;
  }
  const bool getDef() const{
    return _isDef;
  }
private:
  bool _isDef;
};
class CirGateS : public CirGate
{
public:
  CirGateS(int lineNo, int id):CirGate(lineNo, id){};
  ~CirGateS(){}
  bool isAig() const { return false; }
  void setDef(){}
  const bool getDef() const { return true; }
};

class PoGate : public CirGateS
{
public:
  PoGate(int lineNo, int id):CirGateS(lineNo, id) {}
  ~PoGate(){}
  string getTypeStr() const { return "PO"; }
  GateType getType() const {return PO_GATE; }
  bool setFanIn(CirGate* cirg, bool isposi){
      if (getfaninSize()<1) {
        addfanin(cirg, isposi);
        return true;
      }
      return false;
  }
};
class PiGate : public CirGateS
{
public:
  PiGate(int lineNo, int id):CirGateS(lineNo, id) {}
  ~PiGate(){};
  string getTypeStr() const { return "PI"; };
  GateType getType() const {return PI_GATE; }
  bool setFanIn(CirGate* cirg, bool isposi){ return false; }
};
class ConstGate : public CirGateS
{
public:
  ConstGate(int lineNo, int id):CirGateS(lineNo, id) {}
  ~ConstGate(){}
  string getTypeStr() const { return "CONST"; }
  GateType getType() const {return CONST_GATE; }
  bool setFanIn(CirGate* cirg, bool isposi){ return false; }
};

#endif // CIR_GATE_H
