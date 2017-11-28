/* Template for projects
 * Copyright Yuanjie "Duane" Ding (c) 2015
 * Any codes cannot be used for benifit
 *
 * Project: Dictation
 * Version: Beta 0.1.0
 * log:		
 *		I.	Bugs:
 *			-I probably fixed some bugs, but I cannot remember them...
 *
 *		II. New features:
 *			-Program filter all the punctuation input, and recognize the '\' as mark
 *			-All mark will be shown in the table for user more easy to know the word unfamiliar
 *			-Not only AC, but also SE, EPT toll will be shown in statistic
 *			-One can make the dictionary list through the program
 *			-COMMAND SYSTEM:
 *				-New command system is added, you can set command when doing the dictation.
 *				-Command includes:
 *					:e(dit) WORD_ORDER NEW_WORD
 *						To change the word spelt wrong. To use the command, both of the arguments are required.
 *						WORD_ORDER is a order number before the current order
 *						NEW_WORD is the new word you want to change
 *					:! SHELL_COMMAND
 *						Set shell command in program. The string except front two character will be completely pass to the shell.
 *						Program will never exam your SHELL_COMMAND, so it may cause unknown crash. Do not do die.
 *					:f(inish) eps
 *						Explicitly finish the dictation with all words left empty.
 *						any arguments will be ignore.
 *					:c(hinese) [WORD_ORDER]
 *						Print the Chinese tag of the current word.
 *						WORD_ORDER is a selectable argument which indicates the order of word that user wants to print.
 *					:h(int) eps
 *						Print the front letter to hint (distinguish) the word.
 *						any arguments will be ignore.
 *					:w(rite) FILE_NAME
 *						Write down (save) the summary to the certain place.
 *						FILE_NAME argument requried, if the file already exists, permission of overwrite will be needed.
 *						This command can only be used after the dictation statistics
 *				-Command can be used in three place which are following:
 *					1.	in dictation;
 *					2.	after dictation, before the statistics, aka, checking time;
 *					3.	after the statistic, before the finish of the program;
 *
 *		III.Code Improvemet:
 *			-Many iosflags which is useless are removed
 *			-Namespace dict is added.
 *			-Many vectors are declared as static.
 *			-New code style
 *
 *		IV.	Completed on Saturday October 17th, 2015
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
#define VERSION "Beta_0.1.0"
#define YEAR "2015.10"

using namespace std;

namespace dict
{
	//static variables
	vector<string> _wlst;
	vector<string> _wainfo;
	vector<int> _res;
	vector<char> _star;

	//function declaration
	void copyright();
	void look_in_dict(const string &);
	void get_out(ofstream &, string &);
	void make_list(string);
	void argument(const int &, char **);
	void judge(const string &, const string &, vector<string> &, vector<int> &, vector<char> &);
	void result(const size_t &, const vector<int> &, const vector<string> &, const vector<string> &, int &, int &, int &, int &, int &, const vector<char> &);
	void shell(const string &);
	void getwords(string);
	void hint(const size_t &);
	void finish(size_t &);
	void chinese(const size_t &, const size_t &);
	void statistic(ostream &);
	int edit_distance(const string &, const string &);
	bool exist(const string &);
	string filename();
	bool openfile(const string &, ifstream &);
	bool print(const string &, const int &, const int &);
	bool preprocess(const string &, string &);
	bool dictation(const vector<string> &);
	bool permit();
	bool correct(const string &);

	//display the program infomations
	void copyright() {
		cout << CUT << endl;
		cout << "Dictaion (R) by Yuanjie Duane Ding (c)." << endl;
		cout << "Version: " VERSION << endl;
		cout << "	in " YEAR << endl;
		cout << CUT << endl << endl;
	}

	//to check if the file already exists
	bool exist(const string &name) {
		string cmd = "dir "+name+" > nul";
		return !system(cmd.c_str());
	}

	//read the wordlist name
	string filename() {
		string ret;
		cout << "Please input the File name: " << flush;
		getline(cin, ret);
		return ret;
	}

	//function for opening file
	bool openfile(const string &fname, ifstream &in) {
		in.open(fname);
		return in;	//true and valid, false and invalid
	}

	void getwords(string fname) {	//get all the words
#ifdef iDEBUG
		cout << "Reading words form " << fname << endl;
#endif
		ifstream in;
		string tmp;
		while(!exist(fname)) {		//loop check if the filename is vaild
			cout << fname+" cannot be found, please check." << endl;
			cout << "Please input another filename: " << flush;
			getline(cin, fname);	//getline to avoid the \n
		}
		in.open(fname);
		while(in >> tmp) {
			_wlst.push_back(tmp);
		}
#ifndef DEBUG
		random_shuffle(_wlst.begin(), _wlst.end());		//shuffle the wordlist
#endif
	}

	//look up the dict and print the Chinese
	void look_in_dict(const string &x) {
		string cmd = string(DICTA)+" "+x;
		system(cmd.c_str());
	}

	//print the function and check the existence of the word
	bool print(const string &word, const int &ord, const int &tot) {
		bool ret;
		if((ret = correct(word))) {										//ifthe word is exist
			cout << setw(3) << ord << "in" << setw(5) << tot << endl;	//print the number of the word
		}
		look_in_dict(word);
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
			_wainfo[i] = ("");
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
		if(word.size() == 0) {						//meaning the word is empty
			_res.push_back(2);
			_wainfo.push_back("$EMPTY$");
		} else if(word == std) {					//meaning the word is correct
			_res.push_back(1);
			_wainfo.push_back("");
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
#ifdef iDEBUG
		cout << cmd.front() << endl;
#endif
		return (cmd.front() == ':');
	}

	//to edit the words writen
	bool edit(const size_t &now, const size_t &i, const string &another) {
		if(i > now) {													//if the word has not been displayed
			cerr << "This word has not been displayed yet." << endl;
			return false;
		} else if(_res[i-1] == -1) {									//if the word is invaild
			cerr << "This word is invaild." << endl;
			return false;
		} else {														//condition satisfied, rejudge
			cout << "Old word \"" << _wainfo[i-1] << "\" has been replaced." << endl;
			judge(i-1, _wlst[i-1], another);
			return true;
		}
	}

	/* tflags:
	 *		1:	run command in dictation
	 *		2:	run command after dictation
	 *		3:	run command in checking
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
			if(tflag == 2) {
				cerr << "Dictation is already finished, no words can be edit now." << endl;
				return 0;
			} else {
				if(argv.empty()) {							//check the argument
					cerr << "Argument error." << endl;
					return 0;
				} else {
					size_t i;
					string another;
					i = atoi(argv[0].c_str());				//the number want to edit
					another = argv[1];						//the number want to change to
					edit(now, i, another);					//edit it
					return 0;
				}
			}
		} else if(sign == ":h" || sign == ":hint") {		//hint the first letter
			if(tflag == 2) {
				cerr << "Meaningless to hint after the dictation" << endl;
				return 0;
			} else {
				hint(now);
				return 0;
			}
		} else if(sign == ":c" || sign == ":chinese") {		//print the Chinese tag of a word
			if(argv.empty() && tflag != 2 && tflag != 3) {	//print current meaning
				chinese(now, now);
				return 0;
			} else if(argv.empty()) {
				cerr << "Dictation is already over, word order is required." << endl;
				return 0;
			} else {										//print another meaning
				size_t i = atoi(argv[0].c_str())-1;
				chinese(i, now);
				return 0;
			}
		} else if(sign == ":f" || sign == ":finish") {		//finish the dictation
			cout << "Finish the dictation with all rest of words remain empty, " << flush;
			if(permit()) {
				finish(now);
				return -1;
			} else {
				return 0;
			}
		} else if(sign == ":w" || sign == ":write") {
			if(tflag != 2) {
				cerr << "Incorrect time to write down the summary." << endl;
				return 0;
			} else if(argv.empty()) {
				cerr << "File name is required." << endl;
				return 0;
			} else {
				ofstream out;
				string name = argv[0];
				get_out(out, name);
				statistic(out);
				cout << "Statistic file established successfully." << endl;
				return 2;
			}
		}
		return 0;
	}

	//Dictation main function
	bool dictation() {
		int /*tot = 0, act = 0, wat = 0, st = 0, ept = 0,*/ ord = 1, all = _wlst.size();
		bool finish_flag = false;
		string read;
		for(size_t i = 0; i < _wlst.size() && !finish_flag; ++i) {	//for all words in wordlist
			if(print(_wlst[i], i+1, all)) {							//print the word itself
				ord++;
				for(size_t j = 0; j < _wlst[i].size(); ++j) {		//hint by size of word
					cout << '-';
				}
				cout << ": " << flush;
				getline(cin, read);									//read the word from user
				while(iscmd(read) && !finish_flag) {				//loop check if the word is a command
					if((finish_flag = (runcmd(i, read, 1) == -1))) {
						break;
					};
					for(size_t j = 0; j < _wlst[i].size(); ++j) {	//rehint by size of word
						cout << '-';
					}
					cout << ": " << flush;
					getline(cin, read);
				}
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
		do {
			int rflag;
			cout << "> " << flush;
			getline(cin, read);
			if(iscmd(read)) {
				size_t i = _wlst.size();
				if((rflag = runcmd(i, read, 3)) == -1) {
					break;
				}
			} else {
				break;
			}
		} while(true);
		//the end of the dictation, the start of statistics
		statistic(cout);
		do {
			int rflag;
			cout << "> " << flush;
			getline(cin, read);
			if(iscmd(read)) {
				size_t i = _wlst.size();
				if((rflag = runcmd(i, read, 2)) == -1) {
					break;
				}
			} else {
				break;
			}
		} while(true);
		system("pause");
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
	bool correct(const string &x) {
		string cmd = "findstr \"^"+x+"\\>\" " DICT+" > nul";
		return !system(cmd.c_str());
	}

	//to make wordlist
	void make_list(string name) {//name of file
		ofstream out;
		string word;
		get_out(out, name);
		unordered_set<string> exi;
		//print the filename
		//out << name << '_' << endl;
		//print the hint
		cout << "File establish successfully, now start making the list:" << endl;
		while(getline(cin, word)) {
			if(word.empty()) {											//empty line means end
				break;
			} else if(exi.find(word) != exi.end()) {					//if the word is already written
				cout << "The word had already been written." << endl;
			} else if(correct(word)) {									//word correct
				look_in_dict(word);
				out << word << endl;
				exi.insert(word);
			} else {													//word not correct
				cout << "Word not correct." << endl;
			}
		}
		//list made succesfully
		cout << "List made, start the dictation, " << flush;
		//permission required
		if(permit()) {
			//cls to... you know...
			shell("cls");
			getwords(name);
			dictation();
		}
	}

	//to process the arguments from the main() function
	void argument(const int &argc, char **argv) {
		bool _m, _d;																	//_m for make, _d for dictation
		string _mf, _df;																//for filenames
		_m = _d = false;
		if(argc == 1) {																	//no arguments means dictation
			getwords(filename());
			dictation();
		} else {
			for(int i = 1; i < argc; ++i) {												//no Error judgement //a loop in arguments to check the model
				if(string(argv[i]) == "/m") {											//if want to make the list
					_m = true;
					_mf = string(argv[++i]);
					continue;
				} else if(string(argv[i]) == "/d") {									//if want to dictation
					_d = true;
					_df = string(argv[i+1]);
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
			getwords(_df);
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
		for(size_t i = 1; i < _wlst[cur].size(); ++i) {	//fill the else plase with '-'
			cout << '-';
		}
		cout << endl;
	}

	//display the chinese meaning of the word i
	void chinese(const size_t &i, const size_t &cur) {
		if(i > cur) {							//if the word has not been displayed yet
			cerr << "This word has not been displayed yet!" << endl;
			return;
		} else if(i != cur && _res[i] == -1) {	//if the word is invaild
			cerr << "The word is invaild!" << endl;
			return;
		} else {								//if the word is vaild
			cout << i+1 << ":" << endl;
			look_in_dict(_wlst[i]);
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
}

int main(int argc, char **argv) {
	//reset the rand seed as the time of system
	srand(time(NULL));
	dict::copyright();
	dict::argument(argc, argv);

	return 0;
}
