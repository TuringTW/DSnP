/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;

enum CirParseError {
   EXTRA_SPACE,
   MISSING_SPACE,
   ILLEGAL_WSPACE,
   ILLEGAL_NUM,
   ILLEGAL_IDENTIFIER,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   CANNOT_INVERTED,
   MAX_LIT_ID,
   REDEF_GATE,
   REDEF_SYMBOLIC_NAME,
   REDEF_CONST,
   NUM_TOO_SMALL,
   NUM_TOO_BIG,

   DUMMY_END
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned lineNo = 0;  // in printint, lineNo needs to ++
static unsigned colNo  = 0;  // in printing, colNo needs to ++
static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;

static bool
parseError(CirParseError err)
{
   switch (err) {
      case EXTRA_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Extra space character is detected!!" << endl;
         break;
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing space character!!" << endl;
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal white space char(" << errInt
              << ") is detected!!" << endl;
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
              << errMsg << "!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
              << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errInt
              << ")!!" << endl;
         break;
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing " << errMsg << "!!" << endl;
         break;
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
              << errMsg << "\"!!" << endl;
         break;
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": A new line is expected here!!" << endl;
         break;
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
              << " definition!!" << endl;
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": " << errMsg << " " << errInt << "(" << errInt/2
              << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
              << endl;
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo()
              << "!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
              << errMsg << errInt << "\" is redefined!!" << endl;
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Cannot redefine const (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errInt << ")!!" << endl;
         break;
      default: break;
   }
   return false;
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
CirGate* CirMgr::getGate(unsigned gid) const {
  if(gid>=_totalGate.size()) return 0;
  return _totalGate[gid];
}

bool
CirMgr::readCircuit(const string& fileName)
{
  // init
  string line;
  lineNo = 0;
  initV.resize(5);
  initV.clear();
  _piGate.clear();
  _poGate.clear();
  _totalGate.clear();
  _aigCounter = 0;

  ifstream file(fileName.c_str());
  if (file.fail()) {
    // delete file;
    cerr << "Cannot open design \"" << fileName << "\"!!" << endl;
    return false;
  }

  while(file.good()){
    colNo = 0;
    getline(file, line,  '\n');

    if(lineNo == 0){
      if(5 != sscanf(line.c_str(), "aag %d %d %d %d %d", &initV[0], &initV[1], &initV[2], &initV[3], &initV[4])){return parseError(ILLEGAL_IDENTIFIER); }
      _totalGate.resize(initV[0]+initV[3]+1);
    }else if((int)lineNo>0&&(int)lineNo<=(initV[1])){
      int PiNum;
      if(1 != sscanf(line.c_str(), "%d", &PiNum)){return parseError(ILLEGAL_IDENTIFIER); }
      if(PiNum%2==1){ return parseError(ILLEGAL_NUM); }
      CirGate* gate;
      if(!not_found_then_new(gate, PiNum/2, PI_GATE, true)){return false;}
    }else if((int)lineNo>initV[1]&&(int)lineNo<=initV[1]+initV[3]){
      int faninNum;
      if(1 != sscanf(line.c_str(), "%d", &faninNum)){return parseError(ILLEGAL_IDENTIFIER); }
      CirGate* gate;
      CirGate* fanin;
      if(!not_found_then_new(gate, lineNo+1-initV[1]-1+initV[0], PO_GATE, true)){return false;}
      if(!not_found_then_new(fanin, faninNum/2, UNDEF_GATE, false)){return false;}
      if(!gate->setFanIn(fanin, faninNum%2==0))return parseError(REDEF_GATE);
    }else if((int)lineNo>initV[1]+initV[3]&&(int)lineNo<=initV[1]+initV[3]+initV[4]){
      int gid, fid1, fid2;
      if(3 != sscanf(line.c_str(), "%d %d %d", &gid, &fid1, &fid2)){return parseError(ILLEGAL_IDENTIFIER); }
      if(gid%2==1){return parseError(ILLEGAL_NUM);}
       CirGate* gate;
       CirGate* fanin1;
       CirGate* fanin2;
      if(!not_found_then_new(gate, gid/2, AIG_GATE, true)){return false;}
      gate->setLineNo(lineNo+1);

      if(!not_found_then_new(fanin1, fid1/2, UNDEF_GATE, false)){return false;}
      if(!not_found_then_new(fanin2, fid2/2, UNDEF_GATE, false)){return false;}

      if(!gate->setFanIn(fanin1, fid1%2==0)){return parseError(REDEF_GATE);};
      if(!gate->setFanIn(fanin2, fid2%2==0)){return parseError(REDEF_GATE);};
    }else{
      char prefix;
      int index;
      char a[200];
      CirGate* gate = 0;
      int counter = sscanf(line.c_str(), "%c%d %[^\n]", &prefix, &index, a);
      if(line.length()==0){ continue; }
      if(prefix == 'c'){ break; }
      if(prefix != 'i'&&prefix != 'l'&&prefix != 'o'){ errMsg = prefix;return parseError(ILLEGAL_SYMBOL_TYPE); }
      if(counter<2){ colNo++; errMsg = "symbol index"; return parseError(MISSING_NUM); }

      if (prefix=='i') {
        if(_piGate.size()<=(size_t)index){errMsg = "PI index"; errInt = index;return parseError(NUM_TOO_BIG);}
        if(0>index){errMsg = "PI index"; errInt = index;return parseError(NUM_TOO_SMALL);}
        if(_piGate[index]->_alias.length()>0) {return parseError(REDEF_SYMBOLIC_NAME);}
        gate = _piGate[index];
      }else if (prefix=='o'){
        if(_poGate.size()<=(size_t)index){ errMsg = "PO index"; errInt = index;return parseError(NUM_TOO_BIG); }
        if(0>index){ errMsg = "PO index"; errInt = index;return parseError(NUM_TOO_SMALL); }
        gate = _poGate[index];
      }
      if(gate!=0){
        gate->_alias = string(a);
      }
    }
    lineNo++;
  }
  getDFSlist();
  return true;
}

bool
CirMgr::not_found_then_new(CirGate* &gate, int id, GateType type, bool is_defi){
  gate = getGate(id);
  if(gate!=0){
    if(is_defi){
      if(gate->getDef()){ errGate = gate; return parseError(REDEF_GATE); }
      gate->setDef();
      if(type==AIG_GATE){ _aigCounter++; }
    }
    return true;
  }
  if(id==0) type=CONST_GATE;
  switch (type) {
    case PI_GATE:
      gate = new PiGate(lineNo+1, id);
      _piGate.push_back(gate);
      break;
    case PO_GATE:
      gate = new PoGate(lineNo+1, id);
      _poGate.push_back(gate);
      break;
    case AIG_GATE:
      gate = new AigGate(lineNo+1, id, is_defi);
      if(is_defi)_aigCounter++;
      break;
    case CONST_GATE:
      gate = new ConstGate(lineNo+1, id);
      break;
    case UNDEF_GATE:
      gate = new AigGate(lineNo+1, id, false);
      break;
    default:
      return false;
      break;
  }
  _totalGate[id] = gate;
  return true;
}

void
CirMgr::getDFSlist(){
  _dfsList.clear();
  _unusedList.clear();
  _floatingList.clear();

  CirGate::_cflag++;
  for (size_t i = 0; i < _poGate.size(); ++i){
    _poGate[i]->getDFS(_dfsList);
  }
  CirGate::_cflag++;
  for (size_t i = 0; i < _piGate.size(); ++i){
    _piGate[i]->getDFSfanout();
  }
  CirGate::_cflag++;
  for (size_t i = 0; i < _totalGate.size(); i++) {
    if(_totalGate[i]!=0&&_totalGate[i]->getType()==UNDEF_GATE){
      for (size_t j = 0; j < _totalGate[i]->_fanout.size(); j++) {
        if(_totalGate[i]->_fanout[j]->_toGate!=0){
          _totalGate[i]->_fanout[j]->_toGate->_flag = CirGate::_cflag;
        }
      }
    }
  }
  // CirGate::_cflag++;
  // if (getGate(0)!=0) {
  //   getGate(0)->goFanout(depth, 0, true, false);
  // }

  for (size_t i = 0; i < _totalGate.size(); i++) {
    if(_totalGate[i]==0)continue;
    if(_totalGate[i]->_flag>=CirGate::_cflag||_totalGate[i]->_flag<CirGate::_cflag-2){
      _floatingList.push_back(_totalGate[i]);
    }
    if(_totalGate[i]->getfanoutSize()==0&&_totalGate[i]->getType()!=PO_GATE){
      _unusedList.push_back(_totalGate[i]);
    }
  }
}

/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      162
*********************/
void
CirMgr::printSummary() const
{
  size_t pi_count = _piGate.size();
  size_t po_count = _poGate.size();
  if (pi_count+po_count+_aigCounter==0) return;

  cout << endl;
  cout << "Circuit Statistics" << endl;
  cout << "==================" << endl;
  cout << setw(2) << "" << left << setw(5) << "PI" << right << setw(9) <<  pi_count << endl;
  cout << setw(2) << "" << left << setw(5) << "PO" << right << setw(9) <<  po_count << endl;
  cout << setw(2) << "" << left <<  setw(5) << "AIG" << right <<  setw(9) <<  _aigCounter << endl;
  cout <<  "------------------" << endl;
  cout << setw(2) << "" << left <<  setw(5) << "Total" << right <<  setw(9) << (_aigCounter+pi_count+po_count)  << endl;
}

void
CirMgr::printNetlist() const
{
  cout << endl;
  for (unsigned i = 0, n = _dfsList.size(); i < n; ++i) {
    cout << "[" << i << "] ";
    _dfsList[i]->printGate();
  }
}

void
CirMgr::printPIs() const{
  ostringstream oss;
  int counter = 0;
  for (size_t i = 0; i < _piGate.size(); i++) {
    oss  << " "<< _piGate[i]->getId();
    counter++;
  }
  if (counter>0) {
    cout << "PIs of the circuit:" << oss.str() << endl;
  }
}

void
CirMgr::printPOs() const
{
  ostringstream oss;
  int counter = 0;
  for (size_t i = 0; i < _poGate.size(); i++) {
    oss  << " "<< _poGate[i]->getId();
    counter++;
  }
  if (counter>0) {
    cout << "POs of the circuit:" << oss.str() << endl;
  }
}

void
CirMgr::printFloatGates() const
{
  if (_floatingList.size()>0) {
    cout << "Gates with floating fanin(s):";
    for (size_t i = 0; i < _floatingList.size(); ++i){
      cout << " " << _floatingList[i]->getId();
    }
    cout << endl;
  }
  if (_unusedList.size()>0) {
    cout << "Gates defined but not used  :";
    for (size_t i = 0; i < _unusedList.size(); ++i){
      cout << " " << _unusedList[i]->getId();
    }
    cout << endl;
  }
}

void
CirMgr::printFECPairs() const
{
}

void
CirMgr::writeAag(ostream& outfile) const
{
  ostringstream oss;
  int numAig;

  for (size_t i = 0; i < _piGate.size(); i++) {
    oss << _piGate[i]->getId()*2 << endl;
  }

  for (size_t i = 0; i < _poGate.size(); i++) {
    oss << (_poGate[i]->_fanin[0]->_fromGate->getId()*2+(int)!_poGate[i]->_fanin[0]->_isposi) << endl;
  }
  for (size_t i = 0; i < _dfsList.size(); ++i){
    if(_dfsList[i]->getType()==AIG_GATE){
      numAig++;
      int fanin1 = (_dfsList[i]->_fanin[0]->_fromGate->getId())*2;
      int fanin2 = (_dfsList[i]->_fanin[1]->_fromGate->getId())*2;
      if(!_dfsList[i]->_fanin[0]->_isposi){ fanin1++; }
      if(!_dfsList[i]->_fanin[1]->_isposi){ fanin2++; }
      oss << _dfsList[i]->getId()*2 << " " << fanin1 << " " << fanin2 << endl;
    }
  }

  outfile << "aag " << initV[0] << " " << initV[1] << " " << initV[2] << " " << initV[3] << " " << numAig << endl;
  outfile << oss.str();
  for (size_t i = 0; i < _piGate.size(); i++) {
    if(_piGate[i]->_alias.length()>0){
      outfile <<"i"<< i << " " << _piGate[i]->_alias << endl;
    }
  }
  for (size_t i = 0; i < _poGate.size(); i++) {
    if(_poGate[i]->_alias.length()>0){
      outfile <<"o"<< i << " " << _poGate[i]->_alias << endl;
    }
  }
}

void
CirMgr::writeGate(ostream& outfile, CirGate *g) const
{
  GateList _tempDfs;
  CirGate::_cflag++;
  ostringstream ossAig;
  ostringstream ossPI;
  ostringstream ossPIa;
  unsigned maxId = 0;
  int numPi = 0, numAig = 0;

  g->getDFS(_tempDfs);

  for (size_t i = 0; i < _tempDfs.size(); ++i){
    if(_tempDfs[i]->getType()==PI_GATE){
      ossPI << _tempDfs[i]->getId()*2 << endl;
      ossPIa << "i" << numPi << " " << _tempDfs[i]->_alias << endl;
      numPi++;
    }
    if(_tempDfs[i]->getType()==AIG_GATE){
      int fanin1 = (_tempDfs[i]->_fanin[0]->_fromGate->getId())*2;
      int fanin2 = (_tempDfs[i]->_fanin[1]->_fromGate->getId())*2;
      if(!_tempDfs[i]->_fanin[0]->_isposi){ fanin1++; }
      if(!_tempDfs[i]->_fanin[1]->_isposi){ fanin2++; }
      ossAig << _tempDfs[i]->getId()*2 << " " << fanin1 << " " << fanin2 << endl;
      numAig++;
    }
    if(_tempDfs[i]->getId() > maxId){ maxId = _tempDfs[i]->getId(); }
  }

  outfile << "aag " << maxId << " " << numPi << " 0 1 " << numAig << endl;
  outfile << ossPI.str();
  outfile << g->getId()*2 << endl;
  outfile << ossAig.str();
  outfile << ossPIa.str();
  outfile << "o0 " << g->getId() << endl;
}
