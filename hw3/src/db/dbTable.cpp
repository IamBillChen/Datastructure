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
#include "util.h"

using namespace std;

/*****************************************/
/*          Global Functions             */
/*****************************************/
ostream& operator << (ostream& os, const DBRow& r)
{
   // TODO: to print out a row.
   // - Data are seperated by a space. No trailing space at the end.
   // - Null cells are printed as '.'
   for(int i = 0; i < r.size(); i++){
      if(r[i] == INT_MAX)
         os << setw(2) << right << '.';
      else
         os << setw(2) << right << r[i] ;
   }
   return os;
  
}

ostream& operator << (ostream& os, const DBTable& t)
{
   // TODO: to print out a table
   // - Data are seperated by setw(6) and aligned right.
   // - Null cells are printed as '.'
   for(int i = 0; i < t.nRows(); i++){
      for(int j = 0; j < t[i].size(); j++){
         if(t[i][j] == INT_MAX){
            os << setw(6) << right << '.' ;
         }
         else
            os << setw(6) << right << t[i][j]; 
      }
      os << endl;
   }
   return os;
}

ifstream& operator >> (ifstream& ifs, DBTable& t)
{
   // TODO: to read in data from csv file and store them in a table
   // - You can assume the input file is with correct csv file format
   // - NO NEED to handle error file format
   string temp;
   while(!ifs.eof()){
      DBRow r;
      getline(ifs, temp, '\n');
      int loc = 0, preloc = temp.find(',');
      while(preloc != -1){
         if(preloc - loc == 0) 
            r.addData(INT_MAX);
         else
            r.addData(stoi(temp.substr(loc, preloc - loc)));
         loc = preloc + 1;
         preloc = temp.find(',', preloc + 1);
      } 
      if(temp.substr(loc).size() == 1 && !ifs.eof()) 
               r.addData(INT_MAX);
      else 
         r.addData(stoi(temp.substr(loc)));

      t.addRow(r);
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
   _data.erase(_data.begin() + c);
}

/*****************************************/
/*   Member Functions for struct DBSort  */
/*****************************************/
bool
DBSort::operator() (const DBRow& r1, const DBRow& r2) const
{
   // TODO: called as a functional object that compares the data in r1 and r2
   //       based on the order defined in _sortOrder
   for (size_t i = 0; i < _sortOrder.size(); ++i) {
      if (r1[_sortOrder[i]] > r2[_sortOrder[i]]) {
         return false;
      } else if (r1[_sortOrder[i]] < r2[_sortOrder[i]]) {
         return true;
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
   for(int i = 0; i < this -> nRows(); i++){
      _table[i].addData(d[i]); 
   }
}

void
DBTable::delRow(int c)
{
   // TODO: delete row #c. Note #0 is the first row.
   _table.erase(_table.begin() + c);
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
float
DBTable::getMax(size_t c) const
{
   // TODO: get the max data in column #c
   float max = INT_MIN;
   int num = 0;
   for(size_t i = 0; i < nRows(); i++){
      if(_table[i][c] > max && _table[i][c] != INT_MAX) {
         max = _table[i][c];
         num++;
      }
   }
   return num == 0 ? NAN : max;
}

float
DBTable::getMin(size_t c) const
{
   // TODO: get the min data in column #c
   float min = INT_MAX;
   int num = 0;
   for(size_t i = 0; i < nRows(); i++){
      if(_table[i][c] < min && _table[i][c] != INT_MAX) {
         min = _table[i][c];
         num++;
      }
   }
   return num == 0 ? NAN : min;
}

float 
DBTable::getSum(size_t c) const
{
   // TODO: compute the sum of data in column #c
   float sum = 0;
   int num = 0;
   for(size_t i = 0; i < nRows(); i++){
      if(_table[i][c] != INT_MAX) {
         sum += _table[i][c];
         num++;
      }
   }
   return num == 0 ? NAN : sum;
}

int
DBTable::getCount(size_t c) const
{
   // TODO: compute the number of distinct data in column #c
   // - Ignore null cells
   set<int> s;
   int num = 0;
   for(size_t i = 0; i < nRows(); i++){
      if(_table[i][c] != INT_MAX) {
         s.insert(_table[i][c]);
         num++;
      }
   }
   return num == 0 ? NAN : s.size();
}

float
DBTable::getAve(size_t c) const
{
   // TODO: compute the average of data in column #c
   float sum = 0;
   int num = 0;
   for(size_t i = 0; i < nRows(); i++){
      if(_table[i][c] != INT_MAX){
         sum += _table[i][c];
         num++;
      }  
   }
   return num == 0 ? NAN : sum/num;
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
   for(int i = 0; i < nRows(); i++){
      if(_table[i][c] == INT_MAX)
         cout << setw(2) << right << '.';
      else
         cout << setw(2) << right << _table[i][c] ;
   }
   cout << endl;
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

