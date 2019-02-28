/****************************************************************************
  FileName     [ test.cpp ]
  PackageName  [ test ]
  Synopsis     [ Test program for simple database db ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <cstdlib>
#include "dbTable.h"

using namespace std;

extern DBTable dbtbl;

class CmdParser;
CmdParser* cmdMgr = 0; // for linking purpose

int
main(int argc, char** argv)
{
   if (argc != 2) {  // testdb <cvsfile>
      cerr << "Error: using testdb <cvsfile>!!" << endl;
      exit(-1);
   }

   ifstream inf(argv[1]);

   if (!inf) {
      cerr << "Error: cannot open file \"" << argv[1] << "\"!!\n";
      exit(-1);
   }

   if (dbtbl) {
      cout << "Table is resetting..." << endl;
      dbtbl.reset();
   }
   if (!(inf >> dbtbl)) {
      cerr << "Error in reading csv file!!" << endl;
      exit(-1);
   }

   cout << "========================" << endl;
   cout << " Print table " << endl;
   cout << "========================" << endl;
   cout << dbtbl << endl;

   // TODO
   // Insert what you want to test here by calling DBTable's member functions
   dbtbl.delCol(0);
   cout << dbtbl << endl;

   dbtbl.delRow(0);
   cout << dbtbl << endl;

   vector<int> temp;
   for(int i = 0; i < dbtbl.nRows(); i++) temp.push_back(i);
   dbtbl.addCol(temp);
   cout << dbtbl << endl;

   cout << dbtbl.getMax(4) << ',' << dbtbl.getMin(4) << ',' << dbtbl.getSum(4) << ',' << dbtbl.getCount(4) << ',' << dbtbl.getAve(4) <<  endl;
   dbtbl.printSummary();
   dbtbl.printCol(4);
   return 0;
}
