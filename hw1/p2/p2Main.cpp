/****************************************************************************
  FileName     [ p2Main.cpp ]
  PackageName  [ p2 ]
  Synopsis     [ Define main() function ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2016-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <string>
#include "p2Table.h"

using namespace std;

int find_Number(string input);

int main()
{
   Table table;

   // Read in the csv file. Do NOT change this part of code.
   string csvFile;
   cout << "Please enter the file name: ";
   cin >> csvFile;
   if (table.read(csvFile))
      cout << "File \"" << csvFile << "\" was read in successfully." << endl;
   else {
      cerr << "Failed to read in file \"" << csvFile << "\"!" << endl;
      exit(-1); // csvFile does not exist.
   }

   // TODO read and execute commands
   string input;
   while (getline(cin, input)) {
      size_t space_Loc = input.find(' ');
      size_t number_Loc = find_Number(input);
      string action = input.substr(0, space_Loc);
      if(action == "PRINT") 
        table.PRINT();
      else if(action == "SUM" && number_Loc != 0 && number_Loc < input.size())
        table.SUM(stoi(input.substr(number_Loc)));
      else if(action == "MAX" && number_Loc != 0 && number_Loc < input.size())
        table.MAX(stoi(input.substr(number_Loc)));
      else if(action == "MIN" && number_Loc != 0 && number_Loc < input.size())
        table.MIN(stoi(input.substr(number_Loc)));
      else if(action == "DIST" && number_Loc != 0 && number_Loc < input.size())
        table.DIST(stoi(input.substr(number_Loc)));
      else if(action == "AVE" && number_Loc != 0 && number_Loc < input.size())
        table.AVE(stoi(input.substr(number_Loc)));
      else if(action == "ADD" && number_Loc != 0 && number_Loc < input.size())
        table.ADD(input.substr(number_Loc));
      else if(action == "EXIT")
         break;
   }
}

int find_Number(string input){
   int loc = input.find(' ');
   while(1){
      if(input.find(' ', loc + 1) == -1)
         return loc + 1;
      else 
         loc = input.find(' ', loc + 1);
   }
}
