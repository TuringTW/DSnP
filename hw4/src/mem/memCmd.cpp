/****************************************************************************
  FileName     [ memCmd.cpp ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include "memCmd.h"
#include "memTest.h"
#include "cmdParser.h"
#include "util.h"

using namespace std;

extern MemTest mtest;  // defined in memTest.cpp

bool
initMemCmd()
{
   if (!(cmdMgr->regCmd("MTReset", 3, new MTResetCmd) &&
         cmdMgr->regCmd("MTNew", 3, new MTNewCmd) &&
         cmdMgr->regCmd("MTDelete", 3, new MTDeleteCmd) &&
         cmdMgr->regCmd("MTPrint", 3, new MTPrintCmd)
      )) {
      cerr << "Registering \"mem\" commands fails... exiting" << endl;
      return false;
   }
   return true;
}


//----------------------------------------------------------------------
//    MTReset [(size_t blockSize)]
//----------------------------------------------------------------------
CmdExecStatus
MTResetCmd::exec(const string& option)
{
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token))
      return CMD_EXEC_ERROR;
   if (token.size()) {
      int b;
      if (!myStr2Int(token, b) || b < int(toSizeT(sizeof(MemTestObj)))) {
         cerr << "Illegal block size (" << token << ")!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
      }
      #ifdef MEM_MGR_H
      mtest.reset(toSizeT(b));
      #else
      mtest.reset();
      #endif // MEM_MGR_H
   }
   else
      mtest.reset();
   return CMD_EXEC_DONE;
}

void
MTResetCmd::usage(ostream& os) const
{
   os << "Usage: MTReset [(size_t blockSize)]" << endl;
}

void
MTResetCmd::help() const
{
   cout << setw(15) << left << "MTReset: "
        << "(memory test) reset memory manager" << endl;
}


//----------------------------------------------------------------------
//    MTNew <(size_t numObjects)> [-Array (size_t arraySize)]
//----------------------------------------------------------------------
CmdExecStatus
MTNewCmd::exec(const string& option)
{
   // TODO
   vector<string> options;
   lexOptions(option, options);
   int num_obj = 0;
   int arr_size = 0;
   if (options.size()<=0) return CmdExec::errorOption(CMD_OPT_MISSING, "");

   for (size_t i = 0; i < options.size(); i++) {
     int temp_num=0;
     if (i>=3) return CmdExec::errorOption(CMD_OPT_EXTRA, options[i]);
     if (myStr2Int(options[i], temp_num)&&temp_num>0) {
       if (num_obj==0) num_obj = temp_num;
       else return CmdExec::errorOption(CMD_OPT_EXTRA, options[i]);
     }else if(myStrNCmp("-Array", options[i], 2)==0){
       int temp_s_arr = 0;
        if (i+1>options.size()) return CmdExec::errorOption(CMD_OPT_MISSING, "");
        else if(myStr2Int(options[i+1], temp_s_arr)&&temp_s_arr>0)arr_size=temp_s_arr;
        else return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i+1]);
        i++;
     }else{
       if (num_obj>0) return CmdExec::errorOption(CMD_OPT_EXTRA, options[i]);
       else return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
     }
   }

   try{
     if (arr_size>0) {
       mtest.newArrs(num_obj, arr_size);
     }else{
       mtest.newObjs(num_obj);
     }
   }catch(bad_alloc b){
     return CMD_EXEC_ERROR;
   }

   return CMD_EXEC_DONE;
}

void
MTNewCmd::usage(ostream& os) const
{
   os << "Usage: MTNew <(size_t numObjects)> [-Array (size_t arraySize)]\n";
}

void
MTNewCmd::help() const
{
   cout << setw(15) << left << "MTNew: "
        << "(memory test) new objects" << endl;
}


//----------------------------------------------------------------------
//    MTDelete <-Index (size_t objId) | -Random (size_t numRandId)> [-Array]
//----------------------------------------------------------------------
CmdExecStatus
MTDeleteCmd::exec(const string& option)
{
   // TODO
   int idx_opt = -1, rnd_times = -1, rnd_idx;
   bool is_arr = false;
   vector<string> options;
   lexOptions(option, options);
   if (options.size()<=0) return CmdExec::errorOption(CMD_OPT_MISSING, "");

   for (size_t i = 0; i < options.size(); i++) {
     if(i>=3) return CmdExec::errorOption(CMD_OPT_EXTRA, options[i]);
     if (myStrNCmp("-Index", options[i], 2)==0||myStrNCmp("-Random", options[i], 2)==0) {
       int temp_num = 0;
       if (idx_opt>0||rnd_times>0) return CmdExec::errorOption(CMD_OPT_EXTRA, options[i]);
       if (i+1>options.size()) return CmdExec::errorOption(CMD_OPT_MISSING, "");
       if (!myStr2Int(options[i+1], temp_num)||temp_num<0) return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i+1]);
       if (myStrNCmp("-Index", options[i], 2)==0) idx_opt = temp_num;
       else if(temp_num>0) rnd_times = temp_num;
       else return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
       rnd_idx = i;
       i++;
     }else if(myStrNCmp("-Array", options[i], 2)==0){
       if (is_arr) return CmdExec::errorOption(CMD_OPT_EXTRA, options[i]);
       is_arr = true;
     }else return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
   }
   if (rnd_times==-1&&idx_opt==-1) return CmdExec::errorOption(CMD_OPT_MISSING, "");
   if (rnd_times>0) {
     for (size_t i = 0; i < (size_t)rnd_times; i++) {

       if (is_arr) {
         if (mtest.getArrListSize()==0) {
           cerr << "Size of array list is 0!!" << endl;
           return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[rnd_idx]);
         }
         int idx_rnd = rnGen(mtest.getArrListSize());
         mtest.deleteArr(idx_rnd);
       }else{
         if (mtest.getObjListSize()==0) {
           cerr << "Size of object list is 0!!" << endl;
           return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[rnd_idx]);
         }
         int idx_rnd = rnGen(mtest.getObjListSize());
         #ifdef MEM_DEBUG
         cout << "Rndn...(" << idx_rnd << ")" << endl;
         #endif // MEM_DEBUG
         mtest.deleteObj(idx_rnd);
       }
     }
   }else{
     if (is_arr) {
       if ((size_t)idx_opt>=mtest.getArrListSize()){
         cerr << "Size of array list ("<< mtest.getArrListSize() <<") is <= " << idx_opt << "!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);
       }
       mtest.deleteArr(idx_opt);
     }else{
       if ((size_t)idx_opt>=mtest.getObjListSize()){
         cerr << "Size of object list ("<< mtest.getObjListSize() <<") is <= " << idx_opt << "!!" << endl;;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);
       }
       mtest.deleteObj(idx_opt);
     }
   }
   return CMD_EXEC_DONE;
}

void
MTDeleteCmd::usage(ostream& os) const
{
   os << "Usage: MTDelete <-Index (size_t objId) | "
      << "-Random (size_t numRandId)> [-Array]" << endl;
}

void
MTDeleteCmd::help() const
{
   cout << setw(15) << left << "MTDelete: "
        << "(memory test) delete objects" << endl;
}


//----------------------------------------------------------------------
//    MTPrint
//----------------------------------------------------------------------
CmdExecStatus
MTPrintCmd::exec(const string& option)
{
   // check option
   if (option.size())
      return CmdExec::errorOption(CMD_OPT_EXTRA, option);
   mtest.print();

   return CMD_EXEC_DONE;
}

void
MTPrintCmd::usage(ostream& os) const
{
   os << "Usage: MTPrint" << endl;
}

void
MTPrintCmd::help() const
{
   cout << setw(15) << left << "MTPrint: "
        << "(memory test) print memory manager info" << endl;
}
