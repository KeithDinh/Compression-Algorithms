#include <iostream>
#include <string>
#include <map>
#include <algorithm>  
#include <queue>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fstream>
using namespace std;


int main()
{
	///////////////////INPUT EVERYTHING INTO THE STRING/////////////////////////
	string input{(istreambuf_iterator<char>(cin)), istreambuf_iterator<char>()};
	
	
	priority_queue<int> main_queue,sub_queue;			///////second queue-sub_queue and second map-freq_print are for printing frequency only////////
	map<char, int> freq,freq_print;
	pid_t pid;

	//find frequency for each character and insert to map
	for (char i : input)
	{
		if (i == '\n')
			freq['\n']++;
		else 
			freq[i]++;
	}
	

	int size = freq.size();

	/////////////PUSH FREQ TO P////////////////////
	map<char, int>::iterator itr;
	for (itr = freq.begin(); itr != freq.end(); ++itr)
	{
		main_queue.push(itr->second);
	}

	///////////////PRINT FREQ////////////////////
	sub_queue = main_queue;
	freq_print = freq;

	while (!sub_queue.empty())
	{
		itr = freq_print.begin();
		while (true)
		{
			if (itr->second == sub_queue.top())
			{
				if (itr->first == '\n')
					cout << "<EOL> ";
				else if (itr->first != ' ')
					cout << itr->first << ' ';

				cout << "frequency = " << itr->second << endl;

				sub_queue.pop(); freq_print.erase(itr->first);
				break;
			}
			itr++;
		}
	}
	///////////////////////////////////////////////


	cout << "Original Message:\t";
	for (int i = 0; i < input.length(); i++)
	{
		if (input[i] == '\n')
			cout << "<EOL>";
		else cout << input[i];
	}
	cout << endl;

	for (int i = 0; i < size; i++)
	{
		itr = freq.begin();

		///////HIGHEST FREQUENCY = TOP OF QUEUE///////////
		int fr = main_queue.top();
		main_queue.pop();
		string code = "";

		/////////////GET THE CHAR WITH HIGHEST FREQ///////////////
		char c;
		while (true)
		{
			if (itr->second == fr)
			{
				c = itr->first;
				freq.erase(c);
				break;
			}
			itr++;
		}
		//////////////////////////////////////////////////

		if ((pid = fork()) == 0)
		{
			string after_remove;
			/////////ENCODE PROCESS///////////////
			for (char letter : input)
			{
				if (letter == c)
				{
					code += "1";
				}
				else 
				{
					code += "0";
					after_remove += letter;
				}
			}
			///////////OUTPUT TO FILE/////////////
			ofstream output(to_string(i) + ".txt");

			output << "Symbol ";
			if (c == '\n')
				output << "<EOL>" << " code:\t" << code;
			else
				output << c << " code:\t\t" << code;
			output << endl;

			if (after_remove != "")
			{
				output << "Remaining Message:\t";
				for (int i = 0; i < after_remove.length(); i++)
				{
					if (after_remove[i] == '\n')
						output << "<EOL>";
					else output << after_remove[i];
				}
				output << endl;
			}
			output.close();


			//////NO MORE TASK FOR CHILD, EXIT////
			exit(0);
		}

		input.erase(remove(input.begin(), input.end(), c), input.end());

	}
		////////////WAIT FOR THE CHILD///////////
	for(int m =0; m<size; m++)
	{
		wait(0);
	}

	for (int i = 0; i < size; i++)
	{
		////////////READ FILE FROM TXT/////////////
		ifstream inputFile(to_string(i) + ".txt");
		string child_code;
		getline(inputFile, child_code, '\0');
		cout << child_code;
		inputFile.close();
		///////////////////////////////////////////

	}
	return 0;
}

