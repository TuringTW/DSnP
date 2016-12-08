/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
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
   FILE_CANT_OPEN,
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
              << ": Cannot redefine constant (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errInt << ")!!" << endl;
         break;
     case FILE_CANT_OPEN:
        cerr << "Cannot open design \"" << errMsg
             << "\"!!" << endl;
        break;
      default: break;
   }
   return false;
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
CirGate* CirMgr::getGate(unsigned gid) const {
  for (size_t i = 0; i < _totalGate.size(); i++) {
    if((unsigned)_totalGate[i]->getId() == gid) return _totalGate[i];
  }
  return 0;
}
bool
CirMgr::readCircuit(const string& fileName)
{
  ifstream file(fileName.c_str());
  string line;
  lineNo = 0;
  bool is_empty_line = false;
  init_vals.clear();
  if (file.fail()) {
    errMsg = fileName;
    return parseError(FILE_CANT_OPEN);
  }
  while(file.good()){
    colNo = 0;
    // getGate(0);

  	getline(file, line,  '\n');
    if (lineNo==0) {
      if(!parsing(line, true, "aag", 5, init_vals)) return false;
    }else if((int)lineNo>0&&(int)lineNo<=(init_vals[1])){
      vector<int> pi_vals;
      if(!parsing(line, false, "", 1, pi_vals)){return false;}
      not_found_then_new(pi_vals[0]/2, PI_GATE, true);
    }else if((int)lineNo>init_vals[1]&&(int)lineNo<=init_vals[1]+init_vals[3]){
      vector<int> po_vals;
      if(!parsing(line, false, "", 1, po_vals)){return false;}
      CirGate* gate = not_found_then_new(lineNo+1-init_vals[1]-1+init_vals[0], PO_GATE, true);
      CirGate* fanin = not_found_then_new(po_vals[0]/2, UNDEF_GATE, false);
      if(!gate->setFanIn(fanin, po_vals[0]%2==0))return parseError(REDEF_GATE);
    }else if((int)lineNo>init_vals[1]+init_vals[3]&&(int)lineNo<=init_vals[1]+init_vals[3]+init_vals[4]){
      vector<int> aig_vals;
      if(!parsing(line, false, "", 3, aig_vals)){return false;}
      CirGate* gate = not_found_then_new(aig_vals[0]/2, AIG_GATE, true);

      gate->setLineNo(lineNo+1);
      CirGate* fanin1 = not_found_then_new(aig_vals[1]/2, UNDEF_GATE, false);
      CirGate* fanin2 = not_found_then_new(aig_vals[2]/2, UNDEF_GATE, false);
      if(!gate->setFanIn(fanin1, aig_vals[1]%2==0))return parseError(REDEF_GATE);
      if(!gate->setFanIn(fanin2, aig_vals[2]%2==0))return parseError(REDEF_GATE);
    }else{
      colNo = 0;
      bool iscomment =false;
      if(line.length()==0){
        is_empty_line = true;
      }else if(!is_empty_line){
        if(!parseAlias(line, iscomment)){return false;}
      }else{
        return parseError(EXTRA_SPACE);
      }
      if(iscomment){break;}
    }
    lineNo++;
  }
  return true;
}
CirGate* CirMgr::not_found_then_new(int id, GateType type, bool is_defi){
  CirGate* gate = getGate(id);
  if(gate!=0){
    if(is_defi){
      if(gate->getDef()){return 0;}
      gate->setDef();
      if(type==AIG_GATE){_aigCounter++;}
    }
    return gate;
  }
  // for (size_t i = 0; i < _totalGate.size(); i++) {
  //   if(_totalGate[i]->getId() == id){
  //     if(is_defi){
  //       if(!_totalGate[i]->getDef()){
  //         _totalGate[i]->setDef();
  //         if(type==AIG_GATE){_aigCounter++;}
  //       }else{
  //         //TODO
  //       }
  //     }
  //     return _totalGate[i];
  //   }
  // }
  // not found
  if(id==0) type=CONST_GATE;
  CirGate* new_gate;
  switch (type) {
    case PI_GATE:
      new_gate = new PI(lineNo+1, id, is_defi);
      _piGate.push_back(new_gate);
      break;
    case PO_GATE:
      new_gate = new PO(lineNo+1, id, is_defi);
      _poGate.push_back(new_gate);
      break;
    case AIG_GATE:
      new_gate = new And(lineNo+1, id, is_defi);
      if(is_defi)_aigCounter++;
      break;
    case CONST_GATE:
      new_gate = new CG(lineNo+1, id, is_defi);
      break;
      case UNDEF_GATE:
        new_gate = new And(lineNo+1, id, false);
        break;
    default:
      break;
  }
  _totalGate.push_back(new_gate);
  return new_gate;
}
bool CirMgr::parsing(string values, bool is_prefix, string prefix, int n_cols, vector<int> &vals){
  if (!isalnum(values[0])) return parseError(EXTRA_SPACE);
  int pf_pos = values.find(prefix, 0);
  if (is_prefix) {
    if (pf_pos>0) return parseError(MISSING_IDENTIFIER);

    values.erase(0, prefix.length());
    colNo+=(prefix.length());
    if (values.find_first_not_of(" ")==0) return parseError(MISSING_SPACE);
    values.erase(0, 1);
  }
  string temp;
  istringstream f(values);
  int counter = 0;
  while(getline(f, temp, ' ')){
    if (temp.length()>0){
      int val;
      if(!myStr2Int(temp, val)){
        return parseError(EXTRA_SPACE);
      }
      vals.push_back(val);
    }
    else{ return parseError(EXTRA_SPACE); }
    colNo+=temp.length();
    counter++;
  }
  if(counter!=n_cols) return parseError(MISSING_NUM);
	return true;
}
bool CirMgr::parseAlias(string values, bool& istoBreak){
  if (values.find_first_not_of(" ")!=0) {return parseError(EXTRA_SPACE);}
  if (values.length()==0) {return parseError(ILLEGAL_SYMBOL_TYPE);}

  string temp;
  istringstream f(values);
  int index = 0;
  string prefix, alias;

  getline(f, temp, ' ');
  if (temp.length()<=0){ return parseError(EXTRA_SPACE); }
  if(temp == "c"){
    if(values.length()>1){colNo++;return parseError(MISSING_NEWLINE);}
    istoBreak = true;
    return true;
  }
  prefix = temp.substr(0,1);
  if(prefix != "i"&&prefix != "l"&&prefix != "o"){
    errMsg = prefix;
    return parseError(ILLEGAL_SYMBOL_TYPE);
  }
  if(temp.length()<2){ colNo++; errMsg = "symbol index"; return parseError(MISSING_NUM); }
  if(!myStr2Int(temp.substr(1), index)) {colNo++; errMsg = "symbol index("+temp.substr(1)+")"; return parseError(ILLEGAL_NUM);}
  colNo+=temp.length();

  CirGate* gate = 0;
  if (prefix=="i") {
    if(_piGate.size()<=(size_t)index){errMsg = "PI index"; errInt = index;return parseError(NUM_TOO_BIG);}
    if(0>index){errMsg = "PI index"; errInt = index;return parseError(NUM_TOO_SMALL);}
    if(_piGate[index]->_alias.length()>0) {return parseError(REDEF_SYMBOLIC_NAME);}
    gate = _piGate[index];
  }

  if (prefix=="o") {
    if(_poGate.size()<=(size_t)index){errMsg = "PO index"; errInt = index;return parseError(NUM_TOO_BIG);}
    if(0>index){errMsg = "PO index"; errInt = index;return parseError(NUM_TOO_SMALL);}
    gate = _poGate[index];
  }
// symbolic name
  getline(f, alias);
  if(alias.length()==0) {errMsg = "symbolic name"; return parseError(MISSING_IDENTIFIER);}

  gate->_alias = alias;
  return true;
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
  CirGate::_cflag++;
  size_t len = _totalGate.size();
  if(len>0){
    cout << endl;
    CirGate::plineNo=0;
    for (size_t i = 0; i < _poGate.size(); i++) {
      _poGate[i]->goFanin(len, 0, true, false, true);
    }
  }

}

void
CirMgr::printPIs() const
{
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
  CirGate::_cflag++;
  ostringstream oss;
  size_t len = _poGate.size();
  size_t depth = _totalGate.size();
  for (size_t i = 0; i < len; i++) {
    _poGate[i]->goFanin(depth, 0, true, false, false);
  }
  CirGate::_cflag++;
  len = _piGate.size();
  for (size_t i = 0; i < len; i++) {
    _piGate[i]->goFanout(depth, 0, true, false);
  }
  CirGate::_cflag++;
  if (getGate(0)!=0) {
    getGate(0)->goFanout(depth, 0, true, false);
  }
  CirGate::_cflag++;
  len = _totalGate.size();
  for (size_t i = 0; i < len; i++) {
    if(_totalGate[i]->getTypeStr()=="UNDEF"){
      _totalGate[i]->goFanout(1, 0, true, false);
      _totalGate[i]->_flag = CirGate::_cflag-1;
    }
  }
  int counter = 0;
  for (size_t i = 0; i < len; i++) {
    if(_totalGate[i]->_flag>=CirGate::_cflag||_totalGate[i]->_flag<CirGate::_cflag-4){
      oss << " " << _totalGate[i]->getId();
      counter++;
    }
  }
  if (counter>0) {
    cout << "Gates with floating fanin(s):" << oss.str() << endl;
  }

  oss.str("");
  counter = 0;
  for (size_t i = 0; i < _totalGate.size(); i++) {
    if(_totalGate[i]->getfanoutSize()==0&&_totalGate[i]->getTypeStr()!="PO"){
      oss << " " << _totalGate[i]->getId();
      counter++;
    }
  }
  if (counter>0) {
    cout << "Gates defined but not used  :" << oss.str() << endl;
  }

}

void
CirMgr::writeAag(ostream& outfile) const
{
  size_t len = _poGate.size();
  CirGate::_cflag++;
  CirGate::plineNo=0;
  for (size_t i = 0; i < len; i++) {
    _poGate[i]->getDFSlist();
  }

  outfile << "aag " << init_vals[0] << " " << init_vals[1] << " " << init_vals[2] << " " << init_vals[3] << " " << CirGate::plineNo << endl;
  for (size_t i = 0; i < _piGate.size(); i++) {
    outfile << _piGate[i]->getId()*2 << endl;
  }
  for (size_t i = 0; i < _poGate.size(); i++) {
    outfile << (_poGate[i]->_fanin[0]->_fromGate->getId()*2+(int)!_poGate[i]->_fanin[0]->_isposi) << endl;
  }
  for (size_t i = 0; i < _totalGate.size(); i++) {
    if (_totalGate[i]->getTypeStr()=="AIG"&&CirGate::_cflag==_totalGate[i]->_flag) {
      int fanin1 = (_poGate[i]->_fanin[0]->_fromGate->getId()*2+(int)!_poGate[i]->_fanin[0]->_isposi);
      int fanin2 = (_poGate[i]->_fanin[1]->_fromGate->getId()*2+(int)!_poGate[i]->_fanin[1]->_isposi);
      outfile << (_totalGate[i]->getId()*2) << " " << fanin1 << " " << fanin2 << endl;
    }
  }
  for (size_t i = 0; i < _piGate.size(); i++) {
    outfile <<"i"<< i << " " << _piGate[i]->_alias << endl;
  }
  for (size_t i = 0; i < _poGate.size(); i++) {
    outfile <<"o"<< i << " " << _piGate[i]->_alias << endl;
  }

}
