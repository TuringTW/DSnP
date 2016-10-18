/****************************************************************************
  FileName     [ dbTable.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database Table member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iomanip>
#include <string>
#include <cctype>
#include <cassert>
#include <set>
#include <algorithm>
#include "dbTable.h"

using namespace std;

/*****************************************/
/*          Global Functions             */
/*****************************************/
ostream& operator << (ostream& os, const DBRow& r)
{
   // TODO: to print out a row.
   // - Data are seperated by a space. No trailing space at the end.
   // - Null cells are printed as '.'
   for (size_t i = 0; i < r.size(); i++) {
     if (r[i] == INT_MAX) os << ".";
     else os << r[i];
     if (i != r.size()-1) { os << " "; }
   }

   return os;
}

ostream& operator << (ostream& os, const DBTable& t)
{
   // TODO: to print out a table
   // - Data are seperated by setw(6) and aligned right.
   // - Null cells are printed as '.'
   for (size_t i = 0; i < t.nRows(); i++) {
     for (size_t j = 0; j < t[i].size(); j++) {t.printData(os<<setw(6), t[i][j]);}
     cout << endl;
   }
   return os;
}

ifstream& operator >> (ifstream& ifs, DBTable& t)
{
   // TODO: to read in data from csv file and store them in a table
   // - You can assume all the data of the table are in a single line.
    while(ifs.good()){
      string values;
      getline( ifs, values, '\r');
      DBRow n_row;
      string temp;
      istringstream f(values);
      if(values.length()==0) break;
      while(getline(f, temp, ',')){
        if (temp.length()>0&&temp!=".") n_row.addData(atoi(temp.c_str()));
        else n_row.addData(INT_MAX);
      }
      if (values[values.length()-1]==',')n_row.addData(INT_MAX);

      while(n_row.size() < t.nCols()) n_row.addData(INT_MAX);
      t.addRow(n_row);
    }

   return ifs;
}

/*****************************************/
/*   Member Functions for class DBRow    */
/*****************************************/
void
DBRow::removeCell(size_t c)
{
   // TODO
   _data.erase(_data.begin()+c);
}

/*****************************************/
/*   Member Functions for struct DBSort  */
/*****************************************/
bool
DBSort::operator() (const DBRow& r1, const DBRow& r2) const
{
   // TODO: called as a functional object that compares the data in r1 and r2
   //       based on the order defined in _sortOrder
   for (size_t i = 0; i < _sortOrder.size(); i++) {
     int index = _sortOrder[i];
     if (r1[index]>r2[index]) {
       return false;
     }
   }
   return true;
}

/*****************************************/
/*   Member Functions for class DBTable  */
/*****************************************/
void
DBTable::reset()
{
   // TODO
   _table.clear();
}

void
DBTable::addCol(const vector<int>& d)
{
   // TODO: add a column to the right of the table. Data are in 'd'.
   for (size_t i = 0; i < nRows(); i++)
     _table[i].addData(d[i]);
}

void
DBTable::delRow(int c)
{
   // TODO: delete row #c. Note #0 is the first row.
   _table.erase(_table.begin()+c);
}

void
DBTable::delCol(int c)
{
   // delete col #c. Note #0 is the first row.
   for (size_t i = 0, n = _table.size(); i < n; ++i)
      _table[i].removeCell(c);
}

// For the following getXXX() functions...  (except for getCount())
// - Ignore null cells
// - If all the cells in column #c are null, return NAN
// - Return "float" because NAN is a float.
vector<int>
DBTable::getColVec(size_t col_id) const{
	vector<int>  colvec;
	for (size_t i = 0; i < nRows(); ++i){
		DBRow t_row = _table.at(i);
		if (t_row.size()>col_id&&t_row[col_id]!=INT_MAX){
			colvec.push_back(t_row[col_id]);
		}
	}
	return colvec;
}

float
DBTable::getMax(size_t c) const
{
    // TODO: get the max data in column #c
    vector<int> colvec = getColVec(c);
    if (colvec.size()==0 ) return NAN;
    int max = -INT_MAX;
 	  for (int i = 0; i < (int)colvec.size(); ++i){
 		   if (colvec.at(i)>max){ max = colvec.at(i); }
    }
 	  return (float)max;
}

float
DBTable::getMin(size_t c) const
{
   // TODO: get the min data in column #c
   vector<int> colvec = getColVec(c);
 	int min = INT_MAX;
  if (colvec.size()==0 ) return NAN;

 	for (int i = 0; i < (int)colvec.size(); ++i){
 		if (colvec.at(i)<min) { min = colvec.at(i); }
 	}
 	return (float)min;
}

float
DBTable::getSum(size_t c) const
{
   // TODO: compute the sum of data in column #c
   vector<int> colvec = getColVec(c);
   if (colvec.size()==0 ) return NAN;
  int sum = 0;
 	for (int i = 0; i < (int)colvec.size(); ++i){
 		sum += colvec.at(i);
 	}
 	return (float)sum;
}

int
DBTable::getCount(size_t c) const
{
   // TODO: compute the number of distinct data in column #c
   // - Ignore null cells
   vector<int> colvec = getColVec(c);
 	std::sort(colvec.begin(), colvec.end());
 	int count = (int)colvec.size();
 	for (int i = 0; i < (int)colvec.size()-1; ++i){
 		if (colvec.at(i)-colvec.at(i+1)==0){ count--; }
 	}
 	return count;
}

float
DBTable::getAve(size_t c) const
{
   // TODO: compute the average of data in column #c
   vector<int> colvec = getColVec(c);
   if (colvec.size()==0 ) return NAN;
   float result = (float)getSum(c)/(float)colvec.size();
 	return result;
}

void
DBTable::sort(const struct DBSort& s)
{
   // TODO: sort the data according to the order of columns in 's'
   std::sort(_table.begin(), _table.end(), s);
}

void
DBTable::printCol(size_t c) const
{
   // TODO: to print out a column.
   // - Data are seperated by a space. No trailing space at the end.
   // - Null cells are printed as '.'
   for (size_t i = 0; i < nRows(); ++i){
 		DBRow t_row = _table.at(i);
 		if (_table[i].size()>c&&t_row[c]!=INT_MAX){
 			cout << _table[i][c];
 		}else cout << ".";
    if(i!=nRows()-1) cout << " ";
 	}
}

void
DBTable::printSummary() const
{
   size_t nr = nRows(), nc = nCols(), nv = 0;
   for (size_t i = 0; i < nr; ++i)
      for (size_t j = 0; j < nc; ++j)
         if (_table[i][j] != INT_MAX) ++nv;
   cout << "(#rows, #cols, #data) = (" << nr << ", " << nc << ", "
        << nv << ")" << endl;
}
