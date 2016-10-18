/****************************************************************************
  FileName     [ cmdReader.cpp ]
  PackageName  [ cmd ]
  Synopsis     [ Read the command from the standard input or dofile ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <cassert>
#include <cstring>
#include "util.h"
#include "cmdParser.h"

using namespace std;

//----------------------------------------------------------------------
//    Extrenal funcitons
//----------------------------------------------------------------------
void mybeep();


//----------------------------------------------------------------------
//    Member Function for class CmdParser
//----------------------------------------------------------------------
bool
CmdParser::readCmd(istream& istr)
{
    resetBufAndPrintPrompt();

    // THIS IS EQUIVALENT TO "readCmdInt()" in HW#2
    bool newCmd = false;
    while (!newCmd) {
        ParseChar pch = getChar(istr);
        if (pch == INPUT_END_KEY) {
            if (_dofile != 0)
                closeDofile();
            break;
        }
        switch (pch) {
            case LINE_BEGIN_KEY :
            case HOME_KEY       : moveBufPtr(_readBuf); break;
            case LINE_END_KEY   :
            case END_KEY        : moveBufPtr(_readBufEnd); break;
            case BACK_SPACE_KEY : // YOUR HW#2 TODO
            {
                if(moveBufPtr(_readBufPtr-1) == true)
                {
                    deleteChar();   
                }else
                {
                    mybeep();
                }
                break;
            }
            case DELETE_KEY     : deleteChar(); break;
            case NEWLINE_KEY    : newCmd = addHistory();
                               cout << char(NEWLINE_KEY);
                               if (!newCmd) resetBufAndPrintPrompt();
                               break;
            case ARROW_UP_KEY   : moveToHistory(_historyIdx - 1); break;
            case ARROW_DOWN_KEY : moveToHistory(_historyIdx + 1); break;
            case ARROW_RIGHT_KEY: /* YOUR HW#2 TODO*/ moveBufPtr(_readBufPtr + 1); break;
            case ARROW_LEFT_KEY : /* YOUR HW#2 TODO*/ moveBufPtr(_readBufPtr - 1); break;
            case PG_UP_KEY      : moveToHistory(_historyIdx - PG_OFFSET); break;
            case PG_DOWN_KEY    : moveToHistory(_historyIdx + PG_OFFSET); break;
            case TAB_KEY        : // THIS IS DIFFERENT FROM HW#2
                               { char tmp = *_readBufPtr; *_readBufPtr = 0;
                               string str = _readBuf; *_readBufPtr = tmp;
                               listCmd(str);
                               break; }
            case INSERT_KEY     : // not yet supported; fall through to UNDEFINE
            case UNDEFINED_KEY:   mybeep(); break;
            default:  // printable character
                insertChar(char(pch)); break;
        }
        #ifdef TA_KB_SETTING
        taTestOnly();
        #endif
    }
    return newCmd;
}


// This function moves _readBufPtr to the "ptr" pointer
// It is used by left/right arrowkeys, home/end, etc.
//
// Suggested steps:
// 1. Make sure ptr is within [_readBuf, _readBufEnd].
//    If not, make a beep sound and return false. (DON'T MOVE)
// 2. Move the cursor to the left or right, depending on ptr
// 3. Update _readBufPtr accordingly. The content of the _readBuf[] will
//    not be changed
//
// [Note] This function can also be called by other member functions below
//        to move the _readBufPtr to proper position.
bool
CmdParser::moveBufPtr(char* const ptr)
{
    // THIS IS YOUR HW#2 TODO
    if(ptr > _readBufEnd || ptr < _readBuf)
    {
        mybeep();
        return false;
    }
    else
    {
        if(ptr < _readBufPtr)
        {
            for(char* i=_readBufPtr; i>ptr; i--)
            {
                cout << "\b";
                _readBufPtr--;                
            }
        }
        else if(ptr > _readBufPtr)
        {
            for(char* i=_readBufPtr; i<ptr; i++)
            {
                cout << *_readBufPtr;
                _readBufPtr++;
            } 
        }
        return true;
    }
}


// [Notes]
// 1. Delete the char at _readBufPtr
// 2. mybeep() and return false if at _readBufEnd
// 3. Move the remaining string left for one character
// 4. The cursor should stay at the same position
// 5. Remember to update _readBufEnd accordingly.
// 6. Don't leave the tailing character.
// 7. Call "moveBufPtr(...)" if needed.
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling deleteChar()---
//
// cmd> This is he command
//              ^
//
bool
CmdParser::deleteChar()
{
    // THIS IS YOUR HW#2 TODO
    if(_readBufPtr == _readBufEnd)
    {
        mybeep();
        return false;
    }

    char* _readBufPtrTmp = _readBufPtr;

    for(char* _readBufTmp=_readBufPtr; _readBufTmp<=_readBufEnd; _readBufTmp++)
    {
        *_readBufTmp = *(_readBufTmp+1);
    }

    _readBufEnd = _readBufEnd - 1;

    moveBufPtr(_readBufEnd);
    cout << ' ' << '\b';
    moveBufPtr(_readBufPtrTmp);

    return true;
}

// 1. Insert character 'ch' for "repeat" times at _readBufPtr
// 2. Move the remaining string right for "repeat" characters
// 3. The cursor should move right for "repeats" positions afterwards
// 4. Default value for "repeat" is 1. You should assert that (repeat >= 1).
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling insertChar('k', 3) ---
//
// cmd> This is kkkthe command
//                 ^
//
void
CmdParser::insertChar(char ch, int repeat)
{
    // THIS IS YOUR HW#2 TODO
    assert(repeat >= 1);
    int back_counter = 0; // Use temporary pointer to save _readBufPtr is easier. Use in deleteChar() part.
    _readBufEnd = _readBufEnd + repeat;

    for(char* _readBufTmp=_readBufEnd; _readBufTmp>=_readBufPtr+repeat; _readBufTmp--)
    {
        *_readBufTmp = *(_readBufTmp-repeat);
        // cout << *_readBufTmp;
        // *_readBufPtr = *_readBufTmp;
        back_counter++;
    }

    for(char* _readBufTmp=_readBufPtr; _readBufTmp<_readBufPtr+repeat; _readBufTmp++)
    {
        *_readBufTmp = ch;
    }

    // cout << *_readBufPtr;
    moveBufPtr(_readBufEnd);
    moveBufPtr(_readBufEnd-back_counter+1); 
}

// 1. Delete the line that is currently shown on the screen
// 2. Reset _readBufPtr and _readBufEnd to _readBuf
// 3. Make sure *_readBufEnd = 0
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling deleteLine() ---
//
// cmd>
//      ^
//
void
CmdParser::deleteLine()
{
    // THIS IS YOUR HW#2 TODO
    moveBufPtr(_readBuf);
    while(_readBuf != _readBufEnd)
    {
        deleteChar();
    }
    *_readBufEnd = 0;
}


// Reprint the current command to a newline
// cursor should be restored to the original location
void
CmdParser::reprintCmd()
{
    // THIS IS NEW IN HW#3
    cout << endl;
    char *tmp = _readBufPtr;
    _readBufPtr = _readBufEnd;
    printPrompt(); cout << _readBuf;
    moveBufPtr(tmp);
}


// This functions moves _historyIdx to index and display _history[index]
// on the screen.
//
// Need to consider:
// If moving up... (i.e. index < _historyIdx)
// 1. If already at top (i.e. _historyIdx == 0), beep and do nothing.
// 2. If at bottom, temporarily record _readBuf to history.
//    (Do not remove spaces, and set _tempCmdStored to "true")
// 3. If index < 0, let index = 0.
//
// If moving down... (i.e. index > _historyIdx)
// 1. If already at bottom, beep and do nothing
// 2. If index >= _history.size(), let index = _history.size() - 1.
//
// Assign _historyIdx to index at the end.
//
// [Note] index should not = _historyIdx
//
void
CmdParser::moveToHistory(int index)
{
    // THIS IS YOUR HW#2 TODO
    if(_history.size()==0)
    {
        mybeep();
    }

    // Moving up.
    if(index < _historyIdx)
    {
        if(_historyIdx == 0)
        {
            mybeep();
            return;
        }else 
        {
            if(_tempCmdStored==false)
            {
                _tempCmdStored = true;
                _history.push_back(_readBuf);
                _historyIdx ++;          
            }
            if(index < 0)
            {
                _historyIdx = 0;
            }else
            {
                _historyIdx = index;
            }
        }
        
        retrieveHistory();
    }

    // Moving down.
    if(index > _historyIdx)
    {
        if(_historyIdx == int(_history.size()) || _historyIdx == int(_history.size()-1))
        {
            mybeep();
            return;
        }else if(index > int(_history.size()))
        {
            _historyIdx = int(_history.size()-1);
        }else
        {
            _historyIdx = index;
        }

        retrieveHistory();

        if(_tempCmdStored == true && _historyIdx == int(_history.size()-1))
        {
            _history.pop_back();
            _tempCmdStored = false;
            // _historyIdx --;
        }
    }
}


// This function adds the string in _readBuf to the _history.
// The size of _history may or may not change. Depending on whether 
// there is a temp history string.
//
// 1. Remove ' ' at the beginning and end of _readBuf
// 2. If not a null string, add string to _history.
//    Be sure you are adding to the right entry of _history.
// 3. If it is a null string, don't add anything to _history.
// 4. Make sure to clean up "temp recorded string" (added earlier by up/pgUp,
//    and reset _tempCmdStored to false
// 5. Reset _historyIdx to _history.size() // for future insertion
// 6. Reset _readBufPtr and _readBufEnd to _readBuf
// 7. Make sure *_readBufEnd = 0 ==> _readBuf becomes null string
//
bool
CmdParser::addHistory()
{
    // THIS IS SIMILAR TO addHistory in YOUR HW#2 TODO
    // HOWEVER, return true if a new command is entered
    // return false otherwise
    bool newCmd = false;

    if(_tempCmdStored == true)
    {
        _history.pop_back();
        _tempCmdStored = false;
    }
    
    int space_front = 0;
    int space_back = 0;
    for(char* _readSpace = _readBuf; _readSpace<=_readBufEnd; _readSpace++)
    {
        if(*_readSpace==' ')
        {
            space_front++;
        }else
        {
            break;
        }
    }

    for(char * _readSpace = _readBufEnd-1; _readSpace>=_readBuf; _readSpace--)
    {
        
        if(*_readSpace == ' ')
        {
            space_back++;
        }else
        {
            break;
        }
    }

    string _readBuf_string(_readBuf, space_front, _readBufEnd - _readBuf - space_back - space_front);

    if(_readBuf_string != "")
    {
        _history.push_back(_readBuf_string);
        newCmd = true;
    }

    _historyIdx = _history.size();

    return newCmd;
}


// 1. Replace current line with _history[_historyIdx] on the screen
// 2. Set _readBufPtr and _readBufEnd to end of line
//
// [Note] Do not change _history.size().
//
void
CmdParser::retrieveHistory()
{
    deleteLine();
    strcpy(_readBuf, _history[_historyIdx].c_str());
    cout << _readBuf;
    _readBufPtr = _readBufEnd = _readBuf + _history[_historyIdx].size();
}