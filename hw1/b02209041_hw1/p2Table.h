#ifndef P2_TABLE_H
#define P2_TABLE_H

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <iomanip>
#include <climits>
#include <algorithm>


using namespace std;

class Row
{
public:
	Row();
	Row(int, int*);
	Row(const Row&);
	~Row();
	const int operator[] (size_t i) const; // TODO
	int& operator[] (size_t i); // TODO

	void print(void);
	int get_size(void);
private:
   int  *_data;
   int _num;
};

class Table
{
public:
   const Row& operator[] (size_t i) const;
   Row& operator[] (size_t i);
   bool read(const string&);
   void parsing(string values, int method);
   void new_rows(int, int*);

   size_t get_size(void);
   vector<int> getColVec(int);
   void print(void);
   int count_ele(int col_id);
   int sum(int col_id);
   float ave(int col_id);
   int max(int col_id);
   int min(int col_id);
   int count(int col_id);


private:
   vector<Row>  _rows;

};

#endif // P2_TABLE_H
