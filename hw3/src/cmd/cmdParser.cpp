/****************************************************************************
  FileName     [ cmdParser.cpp ]
  PackageName  [ cmd ]
  Synopsis     [ Define command parsing member functions for class CmdParser ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <cassert>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include "util.h"
#include "cmdParser.h"

using namespace std;

//----------------------------------------------------------------------
//    External funcitons
//----------------------------------------------------------------------
void mybeep();


//----------------------------------------------------------------------
//    Member Function for class cmdParser
//----------------------------------------------------------------------
// return false if file cannot be opened
// Please refer to the comments in "DofileCmd::exec", cmdCommon.cpp
bool
CmdParser::openDofile(const string& dof)
{
   // TODO...
   _dofileStack.push(_dofile);
   _dofile = new ifstream(dof.c_str());
   if(_dofileStack.size() > 1024) {
      delete _dofile;
      return false;
   }  else if(!(*_dofile)){
      closeDofile();
      return false;
   }
   else
      return true;
   
}

// Must make sure _dofile != 0
void
CmdParser::closeDofile()
{
   assert(_dofile != 0);
   // TODO...
   delete _dofile;
   if(!_dofileStack.empty()){
      _dofile = _dofileStack.top();
      _dofileStack.pop();
   } else{
      _dofile = 0;
   }
}

// Return false if registration fails
bool
CmdParser::regCmd(const string& cmd, unsigned nCmp, CmdExec* e)
{
   // Make sure cmd hasn't been registered and won't cause ambiguity
   string str = cmd;
   unsigned s = str.size();
   if (s < nCmp) return false;
   while (true) {
      if (getCmd(str)) return false;
      if (s == nCmp) break;
      str.resize(--s);
   }

   // Change the first nCmp characters to upper case to facilitate
   //    case-insensitive comparison later.
   // The strings stored in _cmdMap are all upper case
   //
   assert(str.size() == nCmp);  // str is now mandCmd
   string& mandCmd = str;
   for (unsigned i = 0; i < nCmp; ++i)
      mandCmd[i] = toupper(mandCmd[i]);
   string optCmd = cmd.substr(nCmp);
   assert(e != 0);
   e->setOptCmd(optCmd);

   // insert (mandCmd, e) to _cmdMap; return false if insertion fails.
   return (_cmdMap.insert(CmdRegPair(mandCmd, e))).second;
}

// Return false on "quit" or if excetion happens
CmdExecStatus
CmdParser::execOneCmd()
{
   bool newCmd = false;
   if (_dofile != 0)
      newCmd = readCmd(*_dofile);
   else
      newCmd = readCmd(cin);

   // execute the command

   if (newCmd) {
      string option;
      CmdExec* e = parseCmd(option);
      if (e != 0)
         return e->exec(option);
   }
   return CMD_EXEC_NOP;
}

// For each CmdExec* in _cmdMap, call its "help()" to print out the help msg.
// Print an endl at the end.
void
CmdParser::printHelps() const
{
   // TODO...
   CmdMap :: const_iterator it = _cmdMap.begin();
   for(; it != _cmdMap.end(); it++){
      it -> second -> help();
   }
}

void
CmdParser::printHistory(int nPrint) const
{  
   assert(_tempCmdStored == false);
   if (_history.empty()) {
      cout << "Empty command history!!" << endl;
      return;
   }
   int s = _history.size();
   if ((nPrint < 0) || (nPrint > s))
      nPrint = s;
   for (int i = s - nPrint; i < s; ++i)
      cout << "   " << i << ": " << _history[i] << endl;
}


//
// Parse the command from _history.back();
// Let string str = _history.back();
//
// 1. Read the command string (may contain multiple words) from the leading
//    part of str (i.e. the first word) and retrive the corresponding
//    CmdExec* from _cmdMap
//    ==> If command not found, print to cerr the following message:
//        Illegal command!! "(string cmdName)"
//    ==> return it at the end.
// 2. Call getCmd(cmd) to retrieve command from _cmdMap.
//    "cmd" is the first word of "str".
// 3. Get the command options from the trailing part of str (i.e. second
//    words and beyond) and store them in "option"
//
CmdExec*
CmdParser::parseCmd(string& option)
{
   assert(_tempCmdStored == false);
   assert(!_history.empty());
   string str = _history.back();

   // TODO...
   
   assert(str[0] != 0 && str[0] != ' ');
   CmdExec* e;
   int mainCmdLoc = str.find(' ') == -1 ? str.size() : str.find(' ');
   string mainCmd = str.substr(0, mainCmdLoc + 1);
   if(mainCmdLoc != str.size())
      option = str.substr(mainCmdLoc);

   for (unsigned i = 0; i < mainCmd.size(); ++i)
      mainCmd[i] = toupper(mainCmd[i]);

   for(int i = 0; i < mainCmd.size(); i++){
      string temp = mainCmd.substr(0, i + 1);
      e =  getCmd(temp);
      if(e) return e;
   }
   cout <<"Illegal command!! " << str.substr(0, mainCmdLoc + 1) << endl;
   return NULL;
}

// This function is called by pressing 'Tab'.
// It is to list the partially matched commands.
// "str" is the partial string before current cursor position. It can be 
// a null string, or begin with ' '. The beginning ' ' will be ignored.
//
// Several possibilities after pressing 'Tab'
// (Let $ be the cursor position)
// 1. LIST ALL COMMANDS
//    --- 1.1 ---
//    [Before] Null cmd
//    cmd> $
//    --- 1.2 ---
//    [Before] Cmd with ' ' only
//    cmd>     $
//    [After Tab]
//    ==> List all the commands, each command is printed out by:
//           cout << setw(12) << left << cmd;
//    ==> Print a new line for every 5 commands
//    ==> After printing, re-print the prompt and place the cursor back to
//        original location (including ' ')
//
// 2. LIST ALL PARTIALLY MATCHED COMMANDS
//    --- 2.1 ---
//    [Before] partially matched (multiple matches)
//    cmd> h$                   // partially matched
//    [After Tab]
//    HELp        HIStory       // List all the parially matched commands
//    cmd> h$                   // and then re-print the partial command
//    --- 2.2 ---
//    [Before] partially matched (multiple matches)
//    cmd> h$llo                // partially matched with trailing characters
//    [After Tab]
//    HELp        HIStory       // List all the parially matched commands
//    cmd> h$llo                // and then re-print the partial command
//
// 3. LIST THE SINGLY MATCHED COMMAND
//    ==> In either of the following cases, print out cmd + ' '
//    ==> and reset _tabPressCount to 0
//    --- 3.1 ---
//    [Before] partially matched (single match)
//    cmd> he$
//    [After Tab]
//    cmd> heLp $               // auto completed with a space inserted
//    --- 3.2 ---
//    [Before] partially matched with trailing characters (single match)
//    cmd> he$ahah
//    [After Tab]
//    cmd> heLp $ahaha
//    ==> Automatically complete on the same line
//    ==> The auto-expanded part follow the strings stored in cmd map and
//        cmd->_optCmd. Insert a space after "heLp"
//    --- 3.3 ---
//    [Before] fully matched (cursor right behind cmd)
//    cmd> hElP$sdf
//    [After Tab]
//    cmd> hElP $sdf            // a space character is inserted
//
// 4. NO MATCH IN FITST WORD
//    --- 4.1 ---
//    [Before] No match
//    cmd> hek$
//    [After Tab]
//    ==> Beep and stay in the same location
//
// 5. FIRST WORD ALREADY MATCHED ON FIRST TAB PRESSING
//    --- 5.1 ---
//    [Before] Already matched on first tab pressing
//    cmd> help asd$gh
//    [After] Print out the usage for the already matched command
//    Usage: HELp [(string cmd)]
//    cmd> help asd$gh
//
// 6. FIRST WORD ALREADY MATCHED ON SECOND AND LATER TAB PRESSING
//    ==> Note: command usage has been printed under first tab press
//    ==> Check the word the cursor is at; get the prefix before the cursor
//    ==> So, this is to list the file names under current directory that
//        match the prefix
//    ==> List all the matched file names alphabetically by:
//           cout << setw(16) << left << fileName;
//    ==> Print a new line for every 5 commands
//    ==> After printing, re-print the prompt and place the cursor back to
//        original location
//    --- 6.1 ---
//    [Before] if prefix is empty, print all the file names
//    cmd> help $sdfgh
//    [After]
//    .               ..              Homework_3.docx Homework_3.pdf  Makefile
//    MustExist.txt   MustRemove.txt  bin             dofiles         include
//    lib             mydb            ref             src             testdb
//    cmd> help $sdfgh
//    --- 6.2 ---
//    [Before] with a prefix and with mutiple matched files
//    cmd> help M$Donald
//    [After]
//    Makefile        MustExist.txt   MustRemove.txt
//    cmd> help M$Donald
//    --- 6.3 ---
//    [Before] with a prefix and with mutiple matched files,
//             and these matched files have a common prefix
//    cmd> help Mu$k
//    [After]
//    ==> auto insert the common prefix and make a beep sound
//    ==> DO NOT print the matched files
//    cmd> help Must$k
//    --- 6.4 ---
//    [Before] with a prefix and with a singly matched file
//    cmd> help MustE$aa
//    [After] insert the remaining of the matched file name followed by a ' '
//    cmd> help MustExist.txt $aa
//    --- 6.5 ---
//    [Before] with a prefix and NO matched file
//    cmd> help Ye$kk
//    [After] beep and stay in the same location
//    cmd> help Ye$kk
//
//    [Note] The counting of tab press is reset after "newline" is entered.
//
// 7. FIRST WORD NO MATCH
//    --- 7.1 ---
//    [Before] Cursor NOT on the first word and NOT matched command
//    cmd> he haha$kk
//    [After Tab]
//    ==> Beep and stay in the same location

void
CmdParser::listCmd(const string& str)
{
   // TODO...
   int num = 0;
   static int tabCount = 0;
   bool onlySpace = true;
   string temp;
   vector<string> temp_vector;
   vector<string> collect;
   vector<string> files;
   tabCount++;
   for(char* i = _readBuf; i != _readBufPtr; i++){
      if(*i != ' '){
         temp.push_back(*i);
         onlySpace = false;
      }
      if(*i == ' ' && !onlySpace){
         temp_vector.push_back(temp);
         temp.clear();
         onlySpace = true;
      }
   }
   if(!temp.empty()) temp_vector.push_back(temp);
  
   for(int i = 0; i < files.size(); i++){
      cout << files[i] << endl;
   }
   if(temp_vector.empty()){
      cout << endl;
      for(CmdMap :: iterator it = _cmdMap.begin(); it != _cmdMap.end(); it++){
         cout << setw(12) << left <<  it -> first + it -> second -> getOptCmd() ;
         num++;
         if(num == 5){
            cout << endl;
            num = 0;
         }
      }
      cout << endl;
      reBoard();
      tabCount = 0;
   } 
   else{
      CmdExec* e = createList(temp_vector[0], collect);

      if(collect.empty()){
         mybeep();
         tabCount = 0;
      }
      else if(collect.size() == 1 && (temp_vector.size() == 1) && !onlySpace){
         int dist = collect[0].size() - temp.size() + 1;
         for(char* i = _readBufEnd - 1 + dist; i != _readBufPtr + dist - 1; i--)
            *i = *(i - dist);
         *(_readBufPtr + dist - 1) = ' ';
         for(int i = 0; i < dist - 1; i++)
            *(_readBufPtr + i) = collect[0].at(temp_vector[0].size() + i);
         _readBufEnd += dist;
         for(char* i = _readBufPtr; i != _readBufEnd; i++)
            cout << *i;
         _readBufPtr += dist;
         for(char* i = _readBufEnd; i != _readBufPtr; i--)
            cout << '\b';
         tabCount = 0;
      }
      else if(temp_vector.size() == 1 && !onlySpace && !(temp_vector[0].size() == collect[0].size())){
         int num = 0;
         cout << endl;
         for(int i = 0; i < collect.size(); i++){
            cout << setw(12) << left <<  collect[i] ;
            num++;
            if(num == 5){
               cout << endl;
               num = 0;
            }
         }
         cout << endl;
         reBoard();
         tabCount = 0;
      }
      else if(tabCount == 1){
         cout << endl;
         e -> usage(cout);
         reBoard();
      }
      else if(onlySpace){
         listDir(files, "", ".");
         cout << endl;
         for(int i = 0; i <  files.size(); i++){
            cout << setw(16) << left << files[i];
            num++;
            if(num == 5){
               cout << endl;
               num = 0;
            }
         }
         cout << endl;
         reBoard();
         tabCount = 1;
      }
      else{
         listDir(files, temp_vector.back(), ".");
         if(files.size() == 1){
            int dist = files[0].size() - temp_vector.back().size() + 1;
            for(char* i = _readBufEnd - 1 + dist; i != _readBufPtr + dist - 1; i--)
               *i = *(i - dist);
            *(_readBufPtr + dist - 1) = ' ';
            for(int i = 0; i < dist - 1; i++)
               *(_readBufPtr + i) = files[0].at(temp_vector.back().size() + i);
            _readBufEnd += dist;
            for(char* i = _readBufPtr; i != _readBufEnd; i++)
               cout << *i;
            _readBufPtr += dist;
            for(char* i = _readBufEnd; i != _readBufPtr; i--)
               cout << '\b';
            tabCount = 1;
         }
         else if(files.empty()){
            mybeep();
            tabCount = 1;
         }
         else{
            int same_W = files[0].size() - temp_vector.back().size(), temp_W = 0;
            for(int i = 1; i < files.size(); i++){
               int com = files[0].size() < files[i].size() ? files[0].size()  : files[i].size() ;
               for(int j = temp_vector.back().size(); j < com; j++){
                  if(files[0][j] != files[i][j]) break;
                  temp_W++;
               }
               same_W = temp_W;
               temp_W = 0;
               if(same_W == 0) break; 
            }
            if(same_W == 0){
               cout << endl;
               for(int i = 0; i <  files.size(); i++){
                  cout << setw(16) << left << files[i];
                  num++;
                  if(num == 5){
                     cout << endl;
                     num = 0;
                  }
               }
               cout << endl;
               reBoard();
               tabCount = 1;
            }
            else{
               int dist = same_W;
               for(char* i = _readBufEnd - 1 + dist; i != _readBufPtr + dist - 1; i--)
                  *i = *(i - dist);
               for(int i = 0; i < dist; i++)
                  *(_readBufPtr + i) = files[0].at(temp_vector.back().size() + i);
               _readBufEnd += dist;
               for(char* i = _readBufPtr; i != _readBufEnd; i++)
                  cout << *i;
               _readBufPtr += dist;
               for(char* i = _readBufEnd; i != _readBufPtr; i--)
                  cout << '\b';
               tabCount = 1;
            }
         }
      }
   }
}

void CmdParser::reBoard(){
   cout << endl;
   cout << "mydb> ";
   for(char* i = _readBuf; i != _readBufEnd; i++){
      cout << *i ;
   }
   for(char* i = _readBufEnd; i != _readBufPtr; i--){
      cout << '\b';
   }
}

CmdExec* CmdParser::createList(string &input, vector<string> &collect){
   CmdExec* e;
   for(CmdMap :: iterator it = _cmdMap.begin(); it != _cmdMap.end(); it++){
      string cmd = (it -> first + it -> second -> getOptCmd());
      if(cmd.size() >= input.size()){
         if(myStrNCmp(cmd, input, input.size()) == 0){
            collect.push_back(cmd);
            e = it -> second;
         }   
      }
   }
   return e;
}

// cmd is a copy of the original input
//
// return the corresponding CmdExec* if "cmd" matches any command in _cmdMap
// return 0 if not found.
//
// Please note:
// ------------
// 1. The mandatory part of the command string (stored in _cmdMap) must match
// 2. The optional part can be partially omitted.
// 3. All string comparison are "case-insensitive".
//
CmdExec*
CmdParser::getCmd(string cmd)
{
   CmdExec* e = 0;
   CmdMap :: iterator it = _cmdMap.find(cmd);
   if(it != _cmdMap.end()) return it -> second;
   // TODO...
   return e;
}


//----------------------------------------------------------------------
//    Member Function for class CmdExec
//----------------------------------------------------------------------
// Return false if error options found
// "optional" = true if the option is optional XD
// "optional": default = true
//
bool
CmdExec::lexSingleOption
(const string& option, string& token, bool optional) const
{
   size_t n = myStrGetTok(option, token);
   if (!optional) {
      if (token.size() == 0) {
         errorOption(CMD_OPT_MISSING, "");
         return false;
      }
   }
   if (n != string::npos) {
      errorOption(CMD_OPT_EXTRA, option.substr(n));
      return false;
   }
   return true;
}

// if nOpts is specified (!= 0), the number of tokens must be exactly = nOpts
// Otherwise, return false.
//
bool
CmdExec::lexOptions
(const string& option, vector<string>& tokens, size_t nOpts) const
{
   string token;
   size_t n = myStrGetTok(option, token);
   while (token.size()) {
      tokens.push_back(token);
      n = myStrGetTok(option, token, n);
   }
   if (nOpts != 0) {
      if (tokens.size() < nOpts) {
         errorOption(CMD_OPT_MISSING, "");
         return false;
      }
      if (tokens.size() > nOpts) {
         errorOption(CMD_OPT_EXTRA, tokens[nOpts]);
         return false;
      }
   }
   return true;
}

CmdExecStatus
CmdExec::errorOption(CmdOptionError err, const string& opt) const
{
   switch (err) {
      case CMD_OPT_MISSING:
         cerr << "Error: Missing option";
         if (opt.size()) cerr << " after (" << opt << ")";
         cerr << "!!" << endl;
      break;
      case CMD_OPT_EXTRA:
         cerr << "Error: Extra option!! (" << opt << ")" << endl;
      break;
      case CMD_OPT_ILLEGAL:
         cerr << "Error: Illegal option!! (" << opt << ")" << endl;
      break;
      case CMD_OPT_FOPEN_FAIL:
         cerr << "Error: cannot open file \"" << opt << "\"!!" << endl;
      break;
      default:
         cerr << "Error: Unknown option error type!! (" << err << ")" << endl;
      exit(-1);
   }
   return CMD_EXEC_ERROR;
}

