/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
using namespace std;

#include "cirDef.h"
class PI;
class PO;
class And;
class CG;
extern CirMgr *cirMgr;

// TODO: Define your own data members and member functions
class CirMgr
{
public:
   CirMgr(){
     _aigCounter = 0;
   }
   ~CirMgr() {}

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const;

   // Member functions about circuit construction
   bool readCircuit(const string&);
   bool not_found_then_new(CirGate*&, int, GateType, bool);
   bool parsing(string, bool, string, int, vector<int>&);
   bool parseAlias(string, bool &);

   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void writeAag(ostream&) const;

private:
  IdList  _gateId;
  GateList _totalGate;
  GateList _poGate;
  GateList _piGate;
  int _aigCounter;
  vector<int> init_vals;
};

#endif // CIR_MGR_H
