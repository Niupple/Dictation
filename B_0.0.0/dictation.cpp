/* Template for projects
 * Copyright Yuanjie "Duane" Ding (c) 2015
 * Any codes cannot be used for benifit
 *
 * Project: Dictation
 * Version: Beta 0.0.0
 * log:		Completed on Oct.10 2015
 */
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <algorithm>
#include <cstdlib>
#include <vector>
#include <string>
#include <iomanip>

using namespace std;

string filename()
{
	string ret;
	cout << "Please input the File name: " << flush;
	cin >> ret;
	return ret;
}

vector<string> getwords(const string &fname)
{
	ifstream in;
	string tmp;
	vector<string> wordlist;
	in.open(fname);
	while(in >> tmp)
	{
		wordlist.push_back(tmp);
	}
	random_shuffle(wordlist.begin(), wordlist.end());
	return wordlist;
}

bool print(const string &word)
{
	string cmd;
	cmd = "dicta "+word;
	if(system(cmd.c_str()))
	{
		return false;
	}
	return true;
}

bool dictation(const vector<string> &wlst)
{
	int tot = 0, cnt = 0;
	string read;
	vector<int> res;
	vector<string> wa;
	for(auto i : wlst)
	{
		cout << endl;
		if(print(i))
		{
			cout << "";
			for(int j = 0; j < i.size(); ++j)
			{
				cout << '-';
			}
			cout << ": " << flush;
			cin >> read;
			wa.push_back(read);
			res.push_back(read == i);
		}
		else
		{
			res.push_back(-1);
			wa.push_back("0");
		}
	}
	cout << "\n	Dictation finished.\nTable:\n--------------------------------------\nword            status your_spelling\n--------------------------------------" << endl;
	for(int i = 0; i < wlst.size(); ++i)
	{
		cout << setiosflags(ios::left) << setw(16) << wlst[i] << flush;
		if(res[i] == -1)
		{
			cout << setiosflags(ios::left) << setw(7) << "NAN" << endl;
		}
		else if(res[i] == 0)
		{
			cout << setiosflags(ios::left) << setw(7) << "WA" << setiosflags(ios::left) << setw(16) << wa[i] << endl;
			tot++;
		}
		else
		{
			cout << setiosflags(ios::left) << setw(7) << "AC" << endl;
			tot++;
			cnt++;
		}
	}
	cout << "\n--------------------------------------\nTotal:\n"
		 << setiosflags(ios::left) << setw(16) << "Valid:" << setiosflags(ios::left) << setw(5) << tot << endl
		 << setiosflags(ios::left) << setw(16) << "Passed:" << setiosflags(ios::left) << setw(5) << cnt << endl
		 << setiosflags(ios::left) << setw(16) << "Precision:" << setiosflags(ios::left) << fixed << setprecision(2) << setw(5) << (double)cnt/tot*100.0 << '%' << endl;
	return true;
}

int main()
{
	dictation(getwords(filename()));

	return 0;
}
