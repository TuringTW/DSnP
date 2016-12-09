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
   CirGate(int lineNo, int id, bool isdef):_lineNo(lineNo), _id(id), _flag(0), _isDef(isdef) {}
   virtual ~CirGate() {}

   // Basic access methods
   unsigned getLineNo() const { return _lineNo; }
   virtual string getTypeStr() const { return ""; }
   virtual GateType getType() const { return TOT_GATE; }
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
   virtual bool setFanIn(CirGate* cirg, bool isposi)=0;
   size_t getfaninSize(){return _fanin.size();}
   size_t getfanoutSize(){return _fanout.size();}
   int getId(){return _id;}
   void setDef(){_isDef = true;}
   bool getDef() const{return _isDef;}
private:
protected:
  int _lineNo, _id;
  string _alias;
  vector<Pin*> _fanin;
  vector<Pin*> _fanout;
  static int _cflag;
  static int plineNo;
  int _flag;
  bool _isDef;

};


class And : public CirGate
{
public:
  And(int lineNo, int id, bool isdef):CirGate(lineNo, id, isdef){};
  ~And(){}
  string getTypeStr() const {
    if(getDef())return "AIG";
    else return "UNDEF";
  }
  GateType getType() const {
    if(getDef())return AIG_GATE;
    else return UNDEF_GATE;
  }

  bool setFanIn(CirGate* cirg, bool isposi){
      if (getfaninSize()<2) {
        addfanin(cirg, isposi);
      }else{
        return false;
      }
      return true;
  }
};
class PO : public CirGate
{
public:
  PO(int lineNo, int id, bool isdef):CirGate(lineNo, id, isdef) {}
  ~PO(){}
  string getTypeStr() const { return "PO"; }
  GateType getType() const {return PO_GATE; }

  bool setFanIn(CirGate* cirg, bool isposi){
      if (getfaninSize()<1) {
        addfanin(cirg, isposi);
      }else{
        //TODO
      }
      return true;
  }
};
class PI : public CirGate
{
public:
  PI(int lineNo, int id, bool isdef):CirGate(lineNo, id, isdef) {}
  ~PI(){};
  string getTypeStr() const { return "PI"; };
  GateType getType() const {return PI_GATE; }
  bool setFanIn(CirGate* cirg, bool isposi){
    //TODO
    return true;
  }
};
class CG : public CirGate
{
public:
  CG(int lineNo, int id, bool isdef):CirGate(lineNo, id, isdef) {}
  ~CG(){}
  string getTypeStr() const { return "CONST"; }
  GateType getType() const {return CONST_GATE; }

  bool setFanIn(CirGate* cirg, bool isposi){
    //TODO
    return true;
  }
};
class Undef : public And
{
public:
  Undef(int lineNo, int id):And(lineNo, id, false) {}
  ~Undef(){}
  string getTypeStr() const { return "UNDEF"; }
};
#endif // CIR_GATE_H
