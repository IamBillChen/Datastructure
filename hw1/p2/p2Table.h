/****************************************************************************
  FileName     [ p2Table.h]
  PackageName  [ p2 ]
  Synopsis     [ Header file for class Row and Table ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2016-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#ifndef P2_TABLE_H
#define P2_TABLE_H

#include <vector>
#include <string>

using namespace std;

class Row
{
public:
   // TODO: define constructor and member functions on your own
   Row(size_t col_Len) {
      length = col_Len;
      _data = new int[col_Len];
   }
   
   Row(const Row &row){
      length = row.length;
      _data = new int[length];
      for(int i = 0; i < length; i++){
         _data[i] = row._data[i];
      }
   }

   ~Row(){
      delete [] _data;
      _data = NULL;
   }   
   // Basic access functions
   const int operator[] (size_t i) const { return _data[i]; }
   int& operator[] (size_t i) { return _data[i]; }

private:
   int     length;
   int    *_data; // DO NOT change this definition. Use it to store data.
};

class Table
{
public:
   // TODO: define constructor and member functions on your own
   bool read(const string&);

   // Basic access functions
   size_t nCols() const { return _nCols; }
   const Row& operator[] (size_t i) const { return _rows[i]; }
   Row& operator[] (size_t i) { return _rows[i]; }

   void PRINT();
   void SUM(size_t col);
   void MAX(size_t col);
   void MIN(size_t col);
   void DIST(size_t col);
   void AVE(size_t col);
   void ADD(string input);
private:
   size_t       _nCols; // You should record the number of columns, from 1.
   vector<Row>  _rows;  // DO NOT change this definition. Use it to store rows.
};

#endif // P2_TABLE_H
