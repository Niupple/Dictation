/* Template for projects
 * Copyright Yuanjie "Duane" Ding (c) 2015
 * Any codes cannot be used for benifit
 *
 * Project: Dictation
 * Version: Beta 0.0.1
 * log:		
 *		I.	Bugs:
 *			-Invaild filename check
 *			-Invaild word nodisplay
 *			-Wrong order of the invaild words
 *			-"__dict.txt" and "dicta.bat" will be local version instead of cmd %cd% version
 *
 *		II. New features:
 *			-Invaild words will not be display
 *			-Passing argument to the execution will be accepted
 *			-The order of the word will be displayed in the result table
 *			-The program info will be displayed at the beginning
 *			-Word can be skipped by the empty line
 *			-Program will be paused by the system at last
 *			-Table will show if you skipped the words
 *			-Program can simply check the answer wrong type as:
 *				-AC:	ACcepted:		completely right
 *				-EPT:	EmPTy:			skipped this
 *				-SE:	Spelling Error:	almost right but a little wrong, based on the edit distance model
 *				-WA:	Wrong Answer:	Far from right
 *
 *		III.Code Improvemet
 *			-The two outer files involved are defined as marco
 *
 *		IV.	Completed on Oct.11 2015
 */
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <cstdio>
#include <algorithm>
#include <cstdlib>
#include <vector>
#include <string>
#include <iomanip>
#define CUT "------------------------------------------"
#define DICT ".\\__dict.txt"
#define DICTA ".\\dicta.bat"
#define VERSION "Beta_0.0.1"
#define YEAR "2015.10"

using namespace std;

//display the program infomations
void copyright()
{
	cout << CUT << endl;
	cout << "Dictaion (R) by Yuanjie Duane Ding (c)." << endl;
	cout << "Version: " VERSION << endl;
	cout << "	in " YEAR << endl;
	cout << CUT << endl << endl;
}

//read the wordlist name
string filename()
{
	string ret;
	cout << "Please input the File name: " << flush;
	getline(cin, ret);
	return ret;
}

//function for opening file
bool openfile(const string &fname, ifstream &in)
{
	in.open(fname);
	return in;	//true and valid, false and invalid
}

//get all the words
vector<string> getwords(string fname)
{
	ifstream in;
	string tmp;
	//words all are saved in a vector
	vector<string> wordlist;
	//loop check if the filename is vaild
	while(!openfile(fname, in))
	{
		cout << fname+" cannot be found, please check." << endl;
		cout << "Please input another filename: " << flush;
		getline(cin, fname);	//getline to avoid the \n
	}
	while(in >> tmp)
	{
		wordlist.push_back(tmp);
	}
	//shuffle the wordlist
	random_shuffle(wordlist.begin(), wordlist.end());
	return wordlist;
}

//print the function and check the existence of the word
bool print(const string &word, const int &ord, const int &tot)
{
	bool ret;
	string cmd;
	cmd = "findstr \"^"+word+"\\>\" " DICT " > nul";
	ret = system(cmd.c_str());
	if(!ret)
	{
		cout << setw(3) << ord << "in" << setw(5) << tot << endl;	//print the number of the word
	}
	cmd = string(DICTA)+" "+word;
	system(cmd.c_str());
	return !ret;			//true and vaild
}

//to calculate the edit distance between aa and bb in order to check if the word was spelled wrong
int edit_distance(const string &aa, const string &bb)
{
	unordered_map<int, int> dp[2];
	//int dp[2][10000];
	string a = "$"+aa;
	string b = "$"+bb;
	int na = a.size(), nb = b.size();
	for(int i = 0; i < nb; ++i)
	{
		dp[0][i] = i;
	}
	for(int i = 1; i < na; ++i)
	{
		dp[i&1][0] = i;
		for(int j = 1; j < nb; ++j)
		{
			dp[i&1][j] = min(min(dp[(i&1)^1][j]+1, dp[i&1][j-1]+1), dp[(i&1)^1][j-1]+1-(a[i] == b[j]));
		}
	}
	return dp[(na&1)^1][nb-1];
}

//to judge if the word is right and put infos in wa and res
void judge(const string &std, const string &wrd, vector<string> &wa, vector<int> &res)
{
	//meaning the word is empty
	if(wrd.size() == 0)
	{
		res.push_back(2);
		wa.push_back("$EMPTY$");
	}
	//meaning the word is correct
	else if(wrd == std)
	{
		res.push_back(1);
		wa.push_back("");
	}
	//meaning the word is error-spelled
	else if(edit_distance(wrd, std) < 3)
	{
		res.push_back(3);
		wa.push_back(wrd);
	}
	//meanning the word is incorrect
	else
	{
		res.push_back(0);
		wa.push_back(wrd);
	}
}

void result(const size_t &i, const vector<int> &res, const vector<string> &wlst, const vector<string> &wa, int &cnt, int &tot)
{
	//the word is invaild
	if(res[i] == -1)
	{
		cout << setiosflags(ios::left) << setw(4) << "" << setw(16) << wlst[i] << flush;
		cout << setiosflags(ios::left) << setw(7) << "NAN" << endl;
	}
	//wrong answer
	else if(res[i] == 0)
	{
		cout << setiosflags(ios::left) << setw(4) << tot+1 << setw(16) << wlst[i] << flush;
		cout << setiosflags(ios::left) << setw(7) << "WA" << setiosflags(ios::left) << setw(16) << wa[i] << endl;
		tot++;
	}
	//accepted
	else if(res[i] == 1)
	{
		cout << setiosflags(ios::left) << setw(4) << tot+1 << setw(16) << wlst[i] << flush;
		cout << setiosflags(ios::left) << setw(7) << "AC" << endl;
		tot++;
		cnt++;
	}
	//empty
	else if(res[i] == 2)
	{
		cout << setiosflags(ios::left) << setw(4) << tot+1 << setw(16) << wlst[i] << flush;
		cout << setiosflags(ios::left) << setw(7) << "EPT" << setiosflags(ios::left) << setw(16) << wa[i] << endl;
		tot++;
	}
	//spelling error
	else if(res[i] == 3)
	{
		cout << setiosflags(ios::left) << setw(4) << tot+1 << setw(16) << wlst[i] << flush;
		cout << setiosflags(ios::left) << setw(7) << "SE" << setiosflags(ios::left) << setw(16) << wa[i] << endl;
		tot++;
	}
}

//Dictation main function
bool dictation(const vector<string> &wlst)
{
	int tot = 0, cnt = 0, ord = 1, all = wlst.size();
	string read;
	vector<int> res;
	vector<string> wa;
	for(auto i : wlst)					//for all words in wordlist
	{
		//print the word itself
		if(print(i, ord, all))	//then the word is valid
		{
			ord++;
			//hint by size of word
			for(size_t j = 0; j < i.size(); ++j)
			{
				cout << '-';
			}
			cout << ": " << flush;
			//read the word from user
			getline(cin, read);
			//cin >> read;
			//record the correction of the input
			judge(i, read, wa, res);
			cout << endl;
		}
		else							//else the word cannot be found in the dictionary
		{
			all--;
			res.push_back(-1);
			wa.push_back("0");
		}
	}
	//the end of the dictation, the start of statistics
	cout << "\n	Dictation finished.\nTable:\n" CUT "\n#   word            status your_spelling\n" CUT << endl;
	for(size_t i = 0; i < wlst.size(); ++i)
	{
		result(i, res, wlst, wa, cnt, tot);
	}
	cout << CUT "\n\nTotal:\n"
		 << setiosflags(ios::left) << setw(16) << "Valid:" << setiosflags(ios::left) << setw(5) << tot << endl
		 << setiosflags(ios::left) << setw(16) << "Passed:" << setiosflags(ios::left) << setw(5) << cnt << endl
		 << setiosflags(ios::left) << setw(16) << "Precision:" << setiosflags(ios::left) << fixed << setprecision(2) << setw(5) << (double)cnt/tot*100.0 << '%' << endl;
	system("pause");
	return true;
}

int main(int argc, char **argv)
{
	//reset the rand seed as the time of system
	srand(time(NULL));
	copyright();
	//if not get a filename
	if(argc == 1)
	{
		dictation(getwords(filename()));
	}
	//else if have a filename
	else if(argc == 2)
	{
		dictation(getwords(argv[1]));
	}

	return 0;
}
