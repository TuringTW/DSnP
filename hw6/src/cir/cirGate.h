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
#include "cirDef.h"

using namespace std;
class CirGate;
class And;
class PI;
class PO;
class CG;
class Pin;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes
class Pin
{
  friend class CirGate;
  friend class CirMgr;
public:
  Pin(CirGate* from, CirGate* to, bool isposi): _fromGate(from), _toGate(to), _isposi(isposi){};
  ~Pin(){ };
  void goFanin(int, int, bool, bool);
  void goFanout(int, int, bool);
private:
  CirGate* _fromGate;
  CirGate* _toGate;
  bool _isposi;
};

class CirGate
{
  friend class CirMgr;
public:
   CirGate(GateType Gtype, int lineNo, int id, bool isdef):_type(Gtype), _lineNo(lineNo), _id(id), _flag(0), _isDef(isdef) {};
   virtual ~CirGate() {}

   // Basic access methods
   unsigned getLineNo() const { return _lineNo; }
   string getTypeStr() const { 
    switch(_type){
      case PI_GATE:
        return "PI";
      case PO_GATE:
        return "PO";
      case AIG_GATE:
        if (_isDef){return "AIG";}
        else{return "UNDEF";}
      case CONST_GATE:
        return "CONST";
      default:
        break;
    }
    return "";
  }
   GateType getType() const { 
    if(_type==AIG_GATE&&!_isDef){return UNDEF_GATE;}
    return _type; 
  }
   // Printing functions
   void printGate() const;

   void reportGate() const;
   void reportFanin(int level);
   void goFanin(int, int, bool, bool);
   void reportFanout(int level);
   void goFanout(int, int, bool, bool);

   void getDFSlist(bool, ostream&);
   void printDFSlist(bool);
   void addfanin(CirGate* cirg, bool isposi){
     Pin* con_pin = new Pin(cirg, this, isposi);
     _fanin.push_back(con_pin);
     cirg->addFanout(con_pin);
   }
   void addFanout(Pin* pinout){
     _fanout.push_back(pinout);
   }
   void setLineNo(int lineNo){
     _lineNo = lineNo;
   }
   virtual bool setFanIn(CirGate* cirg, bool isposi){
    if (_type==AIG_GATE){
      if (getfaninSize()<2) {
        addfanin(cirg, isposi);
      }else{
        return false;
      }
      return true;
    }else if(_type==PO_GATE){
      if (getfaninSize()<1) {
        addfanin(cirg, isposi);
      }else{
        return false;
      }
    }
    return true;
   }
   size_t getfaninSize(){return _fanin.size();}
   size_t getfanoutSize(){return _fanout.size();}
   int getId(){return _id;}
   void setDef(){_isDef = true;}
   bool getDef() const{return _isDef;}
private:
  GateType _type;
  int _lineNo, _id;
  string _alias;
  vector<Pin*> _fanin;
  vector<Pin*> _fanout;
  static int _cflag;
  static int plineNo;
  int _flag;
  bool _isDef;

};
#endif // CIR_GATE_H
