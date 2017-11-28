/* Template for projects
 * Copyright Yuanjie "Duane" Ding (c) 2015
 * Any codes cannot be used for benifit
 *
 * Project: Dictation
 * Version: Beta 0.1.1
 * log:		
 *		I.	Bugs:
 *			-Extra special judge on editing word order (negatives, characters, etc.)
 *			-Arguments for dictation directory will be ignored by the argument checker
 *			-In checking mode, empty string will be recognised as ignore, aka. keep the origin answer
 *			-Empty string will not be regarded as incorrect length
 *
 *		II. New features:
 *			-New theme color will be set at the beginning
 *			-COMMAND:
 *			----CHANGE
 *				:f(inish) not change so much but you can use it in checking mode
 *				:edit [ORDER] NEW_WORDS
 *					To edit the word spelt.
 *					[ORDER]		is selective argument, which will be defaultly set to the last vaild word (aka, last word)
 *					NEW_WORDS	is required argument, which indicates the new word to replace
 *				:w(rite) [REPORT_MODE] FILE_NAME
 *					To write down the dictation reports.
 *					[REPORT_MODE]	is selective switch, which indicates the special mode of report, which are:
 *						/t	defalut switch which will write down the table
 *						/w	all words that are not correct will be write down
 *						the priority: /w > /t
 *					FILE_NAME		is a required argument that indicates the destiny of the report.
 *					*This command can only be used in finishing mode
 *			----NEW
 *				:che(ck) CHECKING_MODE
 *					To check the word might be spelt wrongly.
 *					CHECKING_MODE	is a series of switches which indicates the mode of checking, also the range of checking:
 *						/s	check the words with stars.
 *						/l	check the words with wrong length.
 *						/e	check all empty words.
 *						/a	check all the words.
 *						the priority of the switches will be /a > /s = /l = /e, if two switches have the same priority, 
 *							they will be both recognised, else the lower will be ignored.
 *					*This command can only be used in checking mode
 *			-COLOR system:
 *				When making the list or ending the test, error status and normal status will be set in different colors
 *			-MAIN menu
 *				-Starting the program with out valid argument will be turn to the main menu with a probable command list and basic hints
 *				-Which are:
 *					**********************************************
 *					* Dictation program by Yuanjie D. D. in 2015 *
 *					* Enter the command to start the dictation   *
 *					**********************************************
 *				-The main menu will be qeuried when:
 *					-Start the program without argument
 *					-End the dictation without exiting tag
 *					-End the list making without extra tag
 *
 *		III.Code Improvemet:
 *			-All functions have their declearation at the beginning of the program.
 *			-Waiting for command and set the last non-command string as input has been moved to function wait()
 *			-Looking in dict will not call the batch from outside anymore.
 *
 *		IV.	TODO:
 *			-The manual is required when the command is getting more and more.
 *
 */
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <cstdio>
#include <algorithm>
#include <cstdlib>
#include <vector>
#include <string>
#include <iomanip>
#define CUT "-----------------------------------------------"
#define DICT ".\\__dict.txt"
#define DICTA ".\\dicta.bat"
#define VERSION "Beta_0.1.1"
#define YEAR "2015.11"
#define YTHEME "2f"
#define NTHEME "4f"

using namespace std;

namespace dict
{
	//static variables
	vector<string> _wlst;
	vector<string> _wainfo;
	vector<int> _res;
	vector<char> _star;
	string _buff;

	//function declaration
	void copyright();
	void get_out(ofstream &, string &);
	void make_list(string def = "");
	void argument(const int &, char **);
	void judge(const size_t, const string &, const string &);
	void judge(const string &, const string &);
	void result(ostream &, const size_t &, int &, int &, int &, int &, int &);
	void shell(const string &);
	void getwords(string);
	void hint(const size_t &);
	void finish(size_t &);
	void Chinese(const size_t &, const size_t & cur = _wlst.size());
	void statistic(ostream &);
	void cmdchar(int);
	void wait(int);
	void wait(string &, bool &, size_t &, const int &);
	void check(bool, bool, bool, bool);
	void print_wa(ostream &);
	void yescolor();
	void nocolor();
	void menu();
	size_t get_argv(const string &, vector<string> &);
	int edit_distance(const string &, const string &);
	int runcmd(size_t &, const string &, const int &);
	int edit(vector<string> &, size_t &, const int &);
	int cmd_hint(const size_t &, const int &);
	int cmd_Chinese(const vector<string> &, const size_t &, const int &);
	int cmd_finish(size_t &);
	int cmd_write(const int &, const vector<string> &argv);
	int cmd_print(const vector<string> &, const size_t &, const int &);
	int cmd_check(const int &, const vector<string> &);
	bool exist(const string &);
	bool openfile(const string &, ifstream &);
	bool look_in_dict(const string &, const string &dict = "__dict.txt");
	bool print(const string &, const int &, const int &);
	bool preprocess(const string &, string &);
	bool dictation();
	bool permit();
	bool correct(const string &, const string &from = DICT);
	bool iscmd(const string &);
	bool edit(const size_t &, const size_t &, const string &);
	bool inbuff(const string &);
	bool preprint(const string &);
	string get_in(const string &def = "");
	string first_word(const string &);

	//display the program infomations
	void copyright() {
		system("color " YTHEME);
		cout << CUT << endl;
		cout << "Dictaion (R) by Yuanjie Duane Ding (c)." << endl;
		cout << "Version: " VERSION << endl;
		cout << "	in " YEAR << endl;
		cout << CUT << endl << endl;
	}

	//to check if the file already exists
	bool exist(const string &name) {
		string cmd = "dir "+name+" >nul 2>nul";
		return !system(cmd.c_str());
	}

	//read the wordlist name
	string get_in(const string &def) {
		string ret = def;
		if(def == "" || !exist(ret)) {
			cout << "Please input the File name: " << flush;
			getline(cin, ret);
		}
		while(!exist(ret)) {
			cerr << "This file not exist, another one: " << flush;
			getline(cin, ret);
		}
		cout << "File read succesfully!" << endl;
		return ret;
	}

	//function for opening file
	bool openfile(const string &fname, ifstream &in) {
		in.open(fname);
		return in;	//true and valid, false and invalid
	}

	//get all words from the certain page
	void getwords(string fname) {	//get all the words
		_wlst.clear();				//clear the container first
#ifdef iDEBUG
		cout << "Reading words form " << fname << endl;
#endif
		ifstream in;
		string tmp;
		/*while(!exist(fname)) {		//loop check if the get_in is vaild
			cerr << fname+" cannot be found, please check." << endl;
			cout << "Please input another get_in: " << flush;
			getline(cin, fname);	//getline to avoid the \n
		}*/
		in.open(fname);
		while(in >> tmp) {
			_wlst.push_back(tmp);
		}
//		preprint(fname);
#ifndef DEBUG
		//cout << _buff << endl;
		random_shuffle(_wlst.begin(), _wlst.end());		//shuffle the wordlist
#endif
	}

	//look up the dict and print the Chinese
	bool look_in_dict(const string &x, const string &dict) {
		//string cmd = string(DICTA)+" "+x;
		//			  cmd /q /c for /f "usebackq tokens=2,3" %i in (`findstr "^x\>" "dict") do echo %j %k
		string cmd = "cmd /q /c for /f \"usebackq tokens=2,3\" %i in (`findstr \"^"+x+"\\>\" \""+dict+"\"`) do echo %i %j";
		//string cmd = string(DICTA)+" "+x+" "+dict;
		return !system(cmd.c_str()); //true means valid
	}

	//print the function and check the existence of the word
	bool print(const string &word, const int &ord, const int &tot) {
		bool ret;
		if((ret = correct(word))) {										//ifthe word is exist
			cout << setw(3) << ord << "in" << setw(5) << tot << endl;	//print the number of the word
		}
		look_in_dict(word, DICT);
		return ret;														//true and vaild
	}

	//to calculate the edit distance between aa and bb in order to check if the word was spelled wrong
	//implementation in dp
	int edit_distance(const string &aa, const string &bb) {
		unordered_map<int, int> dp[2];
		string a = "$"+aa;
		string b = "$"+bb;
		int na = a.size(), nb = b.size();
		for(int i = 0; i < nb; ++i) {
			dp[0][i] = i;
		}
		for(int i = 1; i < na; ++i) {
			dp[i&1][0] = i;
			for(int j = 1; j < nb; ++j) {
				dp[i&1][j] = min(min(dp[(i&1)^1][j]+1, dp[i&1][j-1]+1), dp[(i&1)^1][j-1]+1-(a[i] == b[j]));
			}
		}
		return dp[(na&1)^1][nb-1];
	}

	//used to check the mark '\' and clean the punctuations
	bool preprocess(const string &wrd, string &ret) {
		ret.clear();
		for(auto i : wrd) {			//for all i in wrd
			if(isalpha(i)) {		//if is in alphabete
				ret.push_back(i);	//add it asusual
			}						//else ignore
		}
		if(wrd.back() == '\\') {	//the tail signal aka. star checking
			return true;			//true means marked
		}
		return false;				//ture means unmarked
	}

	//an overloaded version of judge
	void judge(const size_t i, const string &std, const string &wrd) {
		string word;
		bool mark = preprocess(wrd, word);
		if(word.size() == 0) {					//meaning the word is empty
			_res[i] = (2);
			_wainfo[i] = ("$EMPTY$");
		} else if(word == std) {				//meaning the word is correct
			_res[i] = (1);
			_wainfo[i] = (word);
		}
		else if(edit_distance(word, std) < 3) {	//meaning the word is error-spelled
			_res[i] = (3);
			_wainfo[i] = (word);
		} else {								//meaning the word is incorrect
			_res[i] = (0);
			_wainfo[i] = (word);
		}
		if(mark) {								//meaning the word is marked
			_star[i] = ('*');
		} else {
			_star[i] = ('\0');
		}
	}

	//to judge if the word is right and put infos in _wainfo and _res
	void judge(const string &std, const string &wrd) {
		string word;
		bool mark = preprocess(wrd, word);
		yescolor();
		if(!wrd.empty() && std.size() != wrd.size()) {
			nocolor();
		}
		if(word.size() == 0) {						//meaning the word is empty
			_res.push_back(2);
			_wainfo.push_back("$EMPTY$");
		} else if(word == std) {					//meaning the word is correct
			_res.push_back(1);
			_wainfo.push_back(word);
		} else if(edit_distance(word, std) < 3) {	//meaning the word is error-spelled
			_res.push_back(3);
			_wainfo.push_back(word);
		} else {									//meaning the word is incorrect
			_res.push_back(0);
			_wainfo.push_back(word);
		}
		if(mark) {									//meaning the word is marked
			_star.push_back('*');
		} else {									//meanint the word is unmarked
			_star.push_back('\0');
		}
	}

	//to display the information
	void result(ostream &out, const size_t &i, int &ac, int &wat, int &se, int &ept, int &tot) {
		if(_res[i] == -1) {			//the word is invaild
			out << setiosflags(ios::left) << setw(4) << "" << setw(16) << _wlst[i] << flush;
			out << setiosflags(ios::left) << setw(7) << "NAN" << endl;
		} else if(_res[i] == 0) {	//wrong answer
			out << setiosflags(ios::left) << setw(4) << tot+1 << setw(16) << _wlst[i] << flush;
			out << setiosflags(ios::left) << setw(7) << "WA" << setw(16) << _wainfo[i] << setw(2) << _star[i] << endl;
			tot++;
			wat++;
		} else if(_res[i] == 1) {	//accepted
			out << setiosflags(ios::left) << setw(4) << tot+1 << setw(16) << _wlst[i] << flush;
			out << setiosflags(ios::left) << setw(7) << "AC" << setw(16) << "" << setw(2) << _star[i] << endl;
			tot++;
			ac++;
		} else if(_res[i] == 2) {	//empty
			out << setiosflags(ios::left) << setw(4) << tot+1 << setw(16) << _wlst[i] << flush;
			out << setiosflags(ios::left) << setw(7) << "EPT" << setw(16) << _wainfo[i] << setw(2) << _star[i] << endl;
			tot++;
			ept++;
		} else if(_res[i] == 3) {	//spelling error
			out << setiosflags(ios::left) << setw(4) << tot+1 << setw(16) << _wlst[i] << flush;
			out << setiosflags(ios::left) << setw(7) << "SE" << setw(16) << _wainfo[i] << setw(2) << _star[i] << endl;
			tot++;
			se++;
		}
	}

	//to check if cmd is a command by the sign :
	bool iscmd(const string &cmd) {
#ifdef DEBUG
		cout << cmd.front() << endl;
#endif
		return (cmd.front() == ':');
	}

	//to edit the words writen
	bool edit(const size_t &now, const size_t &i, const string &another) {
		if(i >= now) {													//if the word has not been displayed
			nocolor();
			cerr << "This word has not been displayed yet." << endl;
			return false;
		} else if(_res[i] == -1) {									//if the word is invaild
			nocolor();
			cerr << "This word is invaild." << endl;
			return false;
		} else {														//condition satisfied, rejudge
			yescolor();
			cout << "Old word \"" << _wainfo[i] << "\" has been replaced." << endl;
			judge(i, _wlst[i], another);
			return true;
		}
	}

	//edit function has been removed from the runcmd(), for it's too long.
	int edit(vector<string> &argv, size_t &now, const int &tflag) {
		if(tflag == 2) {
			nocolor();
			cerr << "Dictation is already finished, no words can be edit now." << endl;
			return 0;
		} else {
			if(argv.empty()) {
				nocolor();
				cerr << "At least one argument is required!" << endl;
				return 0;
			} else if(argv.size() == 1) {				//no order argument means the former
				size_t i;
				for(i = now-1; i <= now; --i) {			//for all the word before the current one
					if(_res[i] != -1) {					//if the word is vaild
						break;							//break to return the first vaild
					}
				}
				if(i > now) {							//i is unsigned so always greater or equal than 0
					cerr << "No vaild word before the current one!" << endl;
				} else {
					edit(now, i, argv[0]);				//second augment of edit is the number seeing in dict, instead of in program
				}
				return 0;
			} else {
				size_t i;
				string another;
				i = atoi(argv[0].c_str());				//the number want to edit
				if(i == 0) {
					cerr << "Word order should be a integer!" << endl;
					return 0;
				}
				another = argv[1];						//the number want to change to
				edit(now, i-1, another);				//edit it
				return 0;
			}
		}
	}

	//cmdhint
	int cmd_hint(const size_t &now, const int &tflag) {
		if(tflag == 2) {
			cerr << "Meaningless to hint after the dictation" << endl;
			return 0;
		} else {
			hint(now);
			return 0;
		}
	}

	//cmd_Chinese here
	int cmd_Chinese(const vector<string> &argv, const size_t &now, const int &tflag) {
		if(argv.empty() && tflag != 2 && tflag != 3) {	//print current meaning
			Chinese(now, now);
			return 0;
		} else if(argv.empty()) {
			cerr << "Dictation is already over, word order is required." << endl;
			return 0;
		} else {										//print another meaning
			size_t i = atoi(argv[0].c_str())-1;
			Chinese(i, now);
			return 0;
		}
	}

	//cmd_finish here
	int cmd_finish(size_t &now) {
		cout << "Finish the dictation with all rest of words remain empty, " << flush;
		if(permit()) {		//big command, permission is required
			finish(now);	//finish the dictation by change the iterator
			return -1;		//return -1 means quit
		} else {
			return 0;
		}
	}

	//cmd_write here
	int cmd_write(const int &tflag, const vector<string> &argv) {
		bool _w = false, _t = true;
		if(tflag != 2) {			//command after dictation
			cerr << "Incorrect time to write down the summary." << endl;
			return 0;
		} else if(argv.empty()) {	//arguments error
			cerr << "File name is required." << endl;
			return 0;
		} else {					//arguments correct
			ofstream out;
			string temp;
			for(auto i : argv) {
				if(i[0] == '/') {
					if(i == "/t") {
						_t = true;
					} else if(i == "/w") {
						_w = true;
					}
				} else {
					get_out(out, i);
					break;
				}
			}
			//string name = argv[0];
			//get_out(out, name);
			if(_w) {
				print_wa(out);
			} else if(_t) {
				statistic(out);
			}
			//statistic(out);
			cout << "Statistic file established successfully." << endl;
			return 2;
		}
	}
	
	//cmd_print here
	int cmd_print(const vector<string> &argv, const size_t &now, const int &tflag) {
		if(tflag == 2) {			//wrong time
			cerr << "You can see your spelling in the table." << endl;
			return 0;
		} else if(argv.empty())	{	//arguments error
			cerr << "Word order is required." << endl;
			return 0;
		} else {
			size_t t = atoi(argv[0].c_str());
			if(t >= now+1) {		//time error
				cerr << "You haven't even spell this yet." << endl;
				return 0;
			} else if(t == 0) {		//invalid order
				cerr << "This order is invaild." << endl;
				return 0;
			} else if(_res[t-1] == -1) {	//invalid word
				cerr << "The word is invaild." << endl;
				return 0;
			} else {						//valid word, run
				Chinese(t-1, now);
				cout << _wainfo[t-1] << endl;		//print the word you spelt
				return 0;
			}
		}
	}

	//checking main
	void check(bool _s, bool _l, bool _e, bool _a) {
		string another;
		bool finish_flag = false;
		for(size_t i = 0; i < _wlst.size() && !finish_flag; ++i) {	//for each words in the wordlist
			if(_a) {	//check all mode
				if(_res[i] != -1) {		//the word is valid
					Chinese(i);			//display the Chinese
					if(_res[i] != 2 && _wainfo[i].size() != _wlst[i].size()) {	//wrong info print
						cout << "LENGTH wrong!" << endl;
					} else if (_res[i] == 2) {
						cout << "EMPTY WORD!" << endl;
					}
					cout << _wainfo[i] << endl;
					cmdchar(_wlst[i].size());
					wait(another, finish_flag, i, 3);	//wait for further command, in 3tflag
					if(finish_flag) {					//finish flag check
						break;
					}
					if(!another.empty()) {				//***differ from the dictation, empty input will be ignored
						judge(i, _wlst[i], another);
					}
				}
			} else {	//check curtain switches
				if((_s && _star[i] == '*') || (_e && _res[i] == 2) || (_res[i] != 2 && _l && _wlst[i].size() != _wainfo[i].size())) {
					Chinese(i);
					cout << _wainfo[i] << endl;
					cmdchar(_wlst[i].size());
					wait(another, finish_flag, i, 3);
					if(!another.empty()) {		//also ignore the empty input
						judge(i, _wlst[i], another);
					}
				}
			}
		}
	}

	//cmd_check here
	int cmd_check(const int &tflag, const vector<string> &argv)
	{
		//_s: stars; _l: length; _e: empty; _a: all;
		bool _s, _l, _e, _a;
		_s = _l = _e = _a = false;
		if(tflag == 1) {			//in dictation
			cerr << "It is too early to check!" << endl;
			return 0;
		} else if(tflag == 2) {		//after dictation
			cerr << "It is too late to check!" << endl;
			return 0;
		} else if(argv.empty()) {	//exception
			cerr << "At least one switch is required!" << endl;
			return 0;
		} else {					//normal direction
			for(auto i : argv) {	//check each switch
				if(i == "/s") {
					_s = true;
				} else if(i == "/l") {
					_l = true;
				} else if(i == "/e") {
					_e = true;
				} else if(i == "/a") {
					_a = true;
				} else {		//unexpected switch
					cerr << "\"" << i << "\" not recognised!" << endl;
				}
			}
			if(_s || _l || _e || _a) {	//any switch
				check(_s, _l, _e, _a);
				cout << "The end of recheck." << endl;
			} else {					//no switches
				cerr << "No valid switch recognised!" << endl;
			}
		}
		return 0;
	}

	/* tflags:
	 *		1:	run command in dictation
	 *		2:	run command after dictation
	 *		3:	run command in checking
	 *		4:	run command in the main menu
	 */
	//to run the command in program
	int runcmd(size_t &now, const string &cmd, const int &tflag) {
		string sign, tmp;
		vector<string> argv;								//to store the arguments
		stringstream os;									//to convert the long line into departed arguments
		os << cmd;
		os >> sign;
		if(sign.size() >= 2 && sign[1] == '!') {			//to judge the shell command specially
			sign = ":!";
		}
		while(os >> tmp) {									//to store the arguments
			argv.push_back(tmp);
		}
		if(sign == ":") {									//meaning nothing
			return 1;
		} else if(sign == ":!") {							//meaning shell
			shell(string(cmd.begin()+2, cmd.end()));
		} else if(sign == ":e" || sign == ":edit") {		//meaning edit
			return edit(argv, now, tflag);
		} else if(sign == ":h" || sign == ":hint") {		//hint the first letter
			return cmd_hint(now, tflag);
		} else if(sign == ":c" || sign == ":Chinese") {		//print the Chinese tag of a word
			return cmd_Chinese(argv, now, tflag);
		} else if(sign == ":f" || sign == ":finish") {		//finish the dictation
			return cmd_finish(now);
		} else if(sign == ":w" || sign == ":write") {		//write down the sign
			return cmd_write(tflag, argv);
		} else if(sign == ":p" || sign == ":print") {		//print the Chinese and the spelling
			return cmd_print(argv, now, tflag);
		} else if(sign == ":che" || sign == ":check") {		//check the word in special order
			return cmd_check(tflag, argv);
		}
		return 0;
	}

	//Dictation main function
	bool dictation() {
		_wainfo.clear();
		_res.clear();
		_star.clear();
		int ord = 1, all = _wlst.size();
		bool finish_flag = false;
		string read;
		for(size_t i = 0; i < _wlst.size() && !finish_flag; ++i) {	//for all words in wordlist
			if(print(_wlst[i], i+1, all)) {							//print the word itself
				ord++;
				cmdchar(_wlst[i].size());
				wait(read, finish_flag, i, 1);
				if(!finish_flag){
					judge(_wlst[i], read);							//record the correction of the input
					cout << endl;
				}
			} else {												//else the word is invaild
				all--;
				_res.push_back(-1);
				_wainfo.push_back("0");
			}
		}
		//checking time
		cout << "Dictation finished, now checking time." << endl;
		wait(3);			//wait for the further command
		//the end of the dictation, the start of statistics
		statistic(cout);
		wait(2);			//wait for the further command
		//system("pause");
		menu();
		return true;
	}

	//to check yes or no
	bool permit() {
		string in;
		do {
			cout << "permit or not? (Y/N): " << flush;
			getline(cin, in);
			if(in == "y" || in == "Y") {
				return true;
			} else if(in == "n" || in == "N") {
				return false;
			}
		}while(true);
	}

	//get out file stream
	void get_out(ofstream &ret, string &name) {
		if(name.empty()) {
			cout << "Please input a file name: " << flush;
			getline(cin, name);
		}
		do {															//loop check if the name is valid.
			if(exist(name)) {											//if the name already exists, permission to overwrite is required
				cout << "File already exists, overwrite, " << flush;
				if(permit()) {											//get permission
					ret.open(name);
					return;
				}
			} else {													//name doesn't exist, stablish directly
				ret.open(name);
				return;
			}
		}while(getline(cin, name));										//while not eof
	}

	//to check if the word spelling is correct
	bool correct(const string &x, const string &dict) {
		//string cmd = "findstr \"^"+x+"\\>\" "+from+" >> "+file;
		//return !system(cmd.c_str());
		string cmd = "cmd /q /c for /f \"usebackq tokens=2,3\" %i in (`findstr \"^"+x+"\\>\" \""+dict+"\"`) do echo %i %j > nul";
		return !system(cmd.c_str());
	}

	//to make wordlist
	void make_list(string name) {//name of file
		ofstream out;
		string word;
		get_out(out, name);
		unordered_set<string> exi;
		//print the hint
		cout << "File establish successfully, now start making the list:" << endl;
		while(getline(cin, word)) {
			word = first_word(word);
			if(word.empty()) {											//empty line means end
				yescolor();
				break;
			} else if(exi.find(word) != exi.end()) {					//if the word is already written
				nocolor();
				cout << "The word had already been written." << endl;
			} else if(correct(word)) {									//word correct
				yescolor();
				look_in_dict(word, DICT);
				out << word << endl;
				exi.insert(word);
			} else {													//word not correct
				nocolor();
				cout << "Word not correct." << endl;
			}
		}
		//list made succesfully
		cout << "List made, start the dictation, " << flush;
		//permission required
		if(permit()) {
			//cls to... you know...
			shell("cls");
			getwords(get_in(name));
			dictation();
		} else {
			menu();
		}
	}

	//to process the arguments from the main() function
	void argument(const int &argc, char **argv) {
		bool _m, _d;																	//_m for make, _d for dictation
		string _mf, _df;																//for filenames
		_m = _d = false;
		if(argc == 1) {																	//no arguments means dictation
			/*getwords(get_in());
			dictation();*/
			menu();
		} else {
			for(int i = 1; i < argc; ++i) {												//no Error judgement //a loop in arguments to check the model
				if(string(argv[i]) == "/m") {											//if want to make the list
					_m = true;
					_mf = string(argv[++i]);
					continue;
				} else if(string(argv[i]) == "/d") {									//if want to dictation
					_d = true;
					_df = string(argv[++i]);
					continue;
				} else {																//argument not recognized
					cerr << '\"' << argv[i] << "\" Argument not recognized." << endl;
				}
			}
		}
		if(_m) {																		//if switch _m is open
			make_list(_mf);
		}
		if(_d) {																		//if switch _d is open
			getwords(get_in(_df));
			dictation();
		}
	}

	//function to set shell command
	void shell(const string &cmd) {
		system(cmd.c_str());
	}

	//hint the very beginning letter of current word
	void hint(const size_t &cur) {
		cout << _wlst[cur].front();
		cmdchar((int)_wlst[cur].size()-1);
		cout << endl;
	}

	//display the Chinese meaning of the word i
	void Chinese(const size_t &i, const size_t &cur) {
		if(i > cur) {							//if the word has not been displayed yet
			cerr << "This word has not been displayed yet!" << endl;
			return;
		} else if(i != cur && _res[i] == -1) {	//if the word is invaild
			cerr << "The word is invaild!" << endl;
			return;
		} else {								//if the word is vaild
			cout << i+1 << ":" << endl;
			look_in_dict(_wlst[i], DICT);
			return;
		}
	}

	//finish the dictation for fun
	void finish(size_t &i) {
		for(; i < _wlst.size(); ++i) {
			judge(_wlst[i], "");
		}
	}

	//print the statistic of the dictaion result to ostream out
	void statistic(ostream &out) {
		int act, wat, st, ept, tot;
		act = wat = st = ept = tot = 0;
		out << "Table:\n" CUT "\n#   word            status your_spelling   star\n" CUT << endl;
		for(size_t i = 0; i < _wlst.size(); ++i) {
			result(out, i, act, wat, st, ept, tot);
		}
		out << CUT "\n\nStatistics:\n"
			 << setiosflags(ios::left) << setw(16) << "Valid:" << setw(5) << tot << endl
			 << setiosflags(ios::left) << setw(16) << "Passed:" << setw(5) << act << endl
			 << setiosflags(ios::left) << setw(16) << "Spelling Error:" << setw(5) << st << endl
			 << setiosflags(ios::left) << setw(16) << "Empty:" << setw(5) << ept << endl
			 << setiosflags(ios::left) << setw(16) << "Precision:" << fixed << setprecision(2) << setw(5) << (double)act/tot*100.0 << '%' << endl;
	}

	//print the hint chars
	void cmdchar(int t) {
		for(int i = 0; i < t; ++i) {
			cout << '-';
		}
		cout << '>' << flush;
	}

	//wait for the further command
	void wait(int tflag) {
		while(true) {
			int rflag;			//to store the flag of the return
			string read;
			cmdchar(0);
			getline(cin, read);
			if(iscmd(read)) {	//if read is command
				size_t i = _wlst.size();
				if((rflag = runcmd(i, read, tflag)) == -1) {
					break;
				}
			} else {
				break;
			}
		}
	}

	void wait(string &another, bool &finish_flag, size_t &i, const int &tflag) {
		getline(cin, another);
		while(iscmd(another) && !finish_flag) {				//loop check if the word is a command
			if((finish_flag = (runcmd(i, another, tflag) == -1))) {
				break;
			};
			cmdchar(_wlst[i].size());
			getline(cin, another);
		}
	}

	//print wa word list to the out
	void print_wa(ostream &out) {
		for(size_t i = 0; i < _wlst.size(); ++i) {
			if(_res[i] == 2 || _res[i] == 3 || _res[i] == 0) {
				out << _wlst[i] << endl;
			}
		}
	}

	//set color to yes
	void yescolor() {
		system("color " YTHEME);
	}

	//set color to no
	void nocolor() {
		system("color " NTHEME);
	}

	//useless now
/*	bool inbuff(const string &wrd) {
		return correct(wrd, _buff, "nul");
	}

	bool preprint(const string &name) {
		bool flag = false;
		if(!exist('$'+name)) {
			for(size_t i = 0; i < _wlst.size(); ++i) {
				if((flag = !correct(_wlst[i], "nul", '$'+name))) {
					correct(_wlst[i], '$'+name, DICT);
				}
			}
		}
		//_buff = '$'+name;	//the _buff will be set anyway
		_buff = DICT;
		return flag;
	}*/

	//convert a string line to a string vector
	size_t get_argv(const string &cmd, vector<string> &argv) {
		argv.clear();
		string tmp;
		stringstream os;	//a little slow but...
		os << cmd;
		while(os >> tmp) {
			argv.push_back(tmp);
		}
		return argv.size();
	}

	//main menu
	void menu() {
		//help();
		string in, sign;
		int argc;
		vector<string> argv;
		//loop until finish
		while(true) {
			cout << "$" << flush;	//$ is the sign of the menu
			cmdchar(0);
			getline(cin, in);
			if((argc = get_argv(in, argv)) == 0) {	//ignore the empty input like shell
				continue;
			}
			sign = argv[0];
			if(sign == "dictation" || sign == "dict") {			//if is dictation
				if(argc == 1) {			//1 argv means input the filename
					getwords(get_in());
					dictation();
				} else if(argc == 2) {	//2 argv means there is filename
					getwords(get_in(argv[1]));
					dictation();
				} else {				//exception
					cerr << "More arguments than expected!" << endl;
				}
			} else if(sign == "make" || sign == "makelist") {	//if is makelist
				if(argc == 1) {			//1 argv means input the filename
					make_list();
				} else if(argc == 2) {	//2 argv means the filename is inputed
					make_list(argv[1]);
				} else {				//exception
					cerr << "More arguments than expected!" << endl;
				}
			} else if(sign == "q" || sign == "quit") {			//if quit
				exit(0);	//TODO not good!
			} else {					//command not recognized
				cerr << "Command not recognized!" << endl;
			}
		}
	}
	
	//to get the first word of the line
	string first_word(const string &line) {
		size_t i;
		char c;
		string ret;
		for(i = 0; i < line.size(); ++i) {
			c = line[i];
			if(isalpha(c)) {
				break;
			}
		}
		for(; i < line.size(); ++i) {
			c = line[i];
			if(isalpha(c)) {
				ret.push_back(c);
			} else {
				break;
			}
		}
		return ret;
	}
	//end of the namspace
}

//mainfunction
int main(int argc, char **argv) {
	//reset the rand seed as the time of system
	srand(time(NULL));
	dict::copyright();
	dict::argument(argc, argv);
	//system("color 07");

	return 0;
}
