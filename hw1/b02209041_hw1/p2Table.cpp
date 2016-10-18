#include "p2Table.h"

using namespace std;

// Implement member functions of class Row and Table here
// row
const int Row::operator[] (size_t i) const {
	return _data[i];
} 
int& Row::operator[] (size_t i) {
	return _data[i];
} 
Row::Row(int n_col, int *data){
	_data = new int[n_col];
	_data = data;
	_num = n_col;
}
Row::Row(const Row& row){
	_num = row._num;
	_data = new int[_num];
	for (int i = 0; i < _num; ++i){
		_data[i] = row._data[i];
	}
}
Row::~Row(void){}

void Row::print(){
	for (int i = 0; i < _num; ++i){
		if (_data[i]==INT_MAX){
			cout << setw(4) << right << "";
		}else{
			cout << setw(4) << right << _data[i];
		}
	}
}
int Row::get_size(){
	return _num;
}

// table
const Row& Table::operator[] (size_t i) const{
	return _rows.at(i);
}
Row& Table::operator[] (size_t i){
	return _rows.at(i);
}
bool Table::read(const string& csvFile){
	ifstream file(csvFile.c_str());
	if (file.fail()){
		return false;
	}else{
		string values;
		while(file.good()){
			getline ( file, values,  '\r');
			parsing(values, 1);
		}
		return true; // TODO
	}
}
void Table::parsing(string values, int method){
	vector<int> vals;
	string temp;
	istringstream f(values);
	if (method==1)
	{
		while(getline(f, temp, ',')){
			if (temp.length()>0){ vals.push_back(atoi(temp.c_str()));	}
			else{ vals.push_back(INT_MAX); }
		}
		if (values[values.length()-1]==','){vals.push_back(INT_MAX);}
	}else{ //2
		while(getline(f, temp, ' ')){
			if (temp=="-"){ vals.push_back(INT_MAX);}
			else{vals.push_back(atoi(temp.c_str()));}
		}
	}
	new_rows(vals.size(), vals.data());
}
void Table::new_rows(int size_row, int* data){
	Row *n_row = new Row(size_row, data);
	_rows.push_back(*n_row);
}
vector<int> Table::getColVec(int col_id){
	vector<int>  colvec;
	for (int i = 0; i < (int)this->_rows.size(); ++i){
		Row t_row = this->_rows.at(i);
		if (t_row.get_size()>col_id&&t_row[col_id]!=INT_MAX){
			colvec.push_back(t_row[col_id]);
		}
	}
	return colvec;
}
size_t Table::get_size(void){
	return _rows.size();
}
void Table::print(){
	for (size_t i = 0; i < _rows.size(); ++i){
		_rows.at(i).print();
		cout << endl;
	}
}
int Table::sum(int col_id){
	vector<int> colvec = getColVec(col_id);
	int sum = 0;
	for (int i = 0; i < (int)colvec.size(); ++i){
		sum += colvec.at(i);
	}
	return sum;
}
int Table::count_ele(int col_id){
	vector<int> colvec = getColVec(col_id);
	return colvec.size();
}
float Table::ave(int col_id){
	float result = (float)sum(col_id)/(float)count_ele(col_id);
	return result;
}
int Table::max(int col_id){
	vector<int> colvec = getColVec(col_id);
	int max = -INT_MAX;
	for (int i = 0; i < (int)colvec.size(); ++i){
		if (colvec.at(i)>max){ max = colvec.at(i); }
	}
	return max;
}
int Table::min(int col_id){
	vector<int> colvec = getColVec(col_id);
	int min = INT_MAX;
	for (int i = 0; i < (int)colvec.size(); ++i){
		if (colvec.at(i)<min) { min = colvec.at(i); }
	}
	return min;
}
int Table::count(int col_id){
	vector<int> colvec = getColVec(col_id);
	sort(colvec.begin(), colvec.end());
	int count = (int)colvec.size();
	for (int i = 0; i < (int)colvec.size()-1; ++i){
		if (colvec.at(i)-colvec.at(i+1)==0){ count--; }
	}
	return count;
}