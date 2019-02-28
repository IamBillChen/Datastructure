/****************************************************************************
  FileName     [ p2Table.cpp ]
  PackageName  [ p2 ]
  Synopsis     [ Define member functions of class Row and Table ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2016-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include "p2Table.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <istream>
#include <iomanip>
#include <climits>
#include <string>
#include <set>

using namespace std;

// Implement member functions of class Row and Table here
istream& csvGetline(istream& is, string& t) {
	t.clear();

	istream::sentry se(is, true);
	streambuf *sb = is.rdbuf();

	while(1){
		int c = sb->sbumpc();
		switch(c) {
			case '\n':
				return is;
			case '\r':
				if(sb->sgetc() == '\n')
					sb->sbumpc();
				return is;
			case EOF: // last line
				if(t.empty())
					is.setstate(std::ios::eofbit);
				return is;
			default:
				t += (char)c;
		}
	}
}

bool
Table::read(const string& csvFile)
{
   // TODO
	//initial the number of col and row
	fstream file(csvFile);
	size_t _nRows = 1;
	_nCols = 1;

	if(file.is_open() == false) return false;
	char ch;
	while(file.get(ch)){
		if(ch == '\n' || ch == '\r')  // count rows
			_nRows++;
		if(ch == ','  && _nRows == 1) // count columns
			_nCols++;
	}
	file.close();

	//read csv
	string line;
	file.open(csvFile);

	while(!csvGetline(file, line).eof()){
		Row temp_Row(_nCols);
		int i = 0;
		int Loc = 0;
		int preLoc = line.find(',');
		while(preLoc != -1){
			if(preLoc == Loc){

				temp_Row[i] = INT_MAX;
			}
			else{
				temp_Row[i] = stoi(line.substr(Loc, preLoc - Loc));
			}
			Loc = preLoc + 1;
			preLoc = line.find(',', Loc);
			i++;
		}	
		
		if(Loc == line.size()){
			temp_Row[i] = INT_MAX;
		}	else{
			temp_Row[i] = stoi(line.substr(Loc, line.size() - Loc + 1));
		}	

		_rows.push_back(temp_Row);

	}
	file.close();
	return true;
}

void Table::PRINT(){
	for(size_t i = 0; i < _rows.size(); i++){
		for(size_t j = 0; j < _nCols; j++){
			if(_rows[i][j] == INT_MAX){
				cout << setw(4) << '.' ;
			}
			else{
				cout << setw(4) << _rows[i][j];
			}
		}
		cout << endl;
	}
}

void Table::SUM(size_t col){
	if(col < 0 || col >= _nCols) {
		cout << "Error: This is a NULL column!!" << endl ;
		return;
	}

	int sum = 0;
	for(size_t i = 0; i < _rows.size(); i++){
		if(_rows[i][col] != INT_MAX)
			sum += _rows[i][col];
	}
	cout << "The summation of data in column #" << col << " is " << sum << endl;		
}

void Table::MAX(size_t col){
	if(col < 0 || col >= _nCols) {
		cout << "Error: This is a NULL column!!" << endl ;
		return;
	}

	int max = INT_MIN;
	for(size_t i = 0; i < _rows.size(); i++){
		if(_rows[i][col] != INT_MAX)
			max = max > _rows[i][col]? max : _rows[i][col];
	}
	cout << "The maximum of data in column #" << col << " is " << max << endl;
}

void Table::MIN(size_t col){
	if(col < 0 || col >= _nCols) {
		cout << "Error: This is a NULL column!!" << endl ;
		return;
	}

	int min = INT_MAX;
	for(size_t i = 0; i < _rows.size(); i++){
		if(_rows[i][col] != INT_MAX)
			min = min < _rows[i][col]? min : _rows[i][col];
	}
	cout << "The minimun of data in column #" << col << " is " << min << endl;
}

void Table::DIST(size_t col){
	if(col < 0 || col >= _nCols) {
		cout << "Error: This is a NULL column!!" << endl ;
		return;
	}

	set<int> dist;
	for(size_t i = 0; i < _rows.size(); i++){
		if(_rows[i][col] != INT_MAX)
			dist.insert(_rows[i][col]);
	}
	cout << "The distinct count of data in column #" << col << " is " << dist.size() << endl;
}

void Table::AVE(size_t col){
	if(col < 0 || col >= _nCols) {
		cout << "Error: This is a NULL column!!" << endl ;
		return;
	}

	int num = 0;
	int sum = 0;
	for(size_t i = 0; i < _rows.size(); i++){
		if(_rows[i][col] != INT_MAX){
			sum += _rows[i][col];
			num++;
		}
	}

	cout << setprecision(2)<< "The average of data in column #" << col << " is " << (double)sum/ (double)(num) << endl;
}

void Table::ADD(string input){
	int preloc = input.find(","), loc = 0, num = 0;
	Row temp(_nCols);
	while(preloc != -1){	
		temp[num++] = preloc == loc ? INT_MAX : stoi(input.substr(loc, preloc - loc));
		loc = preloc + 1;
		preloc = input.find(",", loc);
		if(num >= _nCols) return;
	}
	temp[num++] = loc == input.size() ? INT_MAX : stoi(input.substr(loc));
	if(num == _nCols)
		_rows.push_back(temp);
}
