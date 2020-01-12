#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<vector>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
using namespace std;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t parent_child = PTHREAD_COND_INITIALIZER;
static pthread_cond_t child_child = PTHREAD_COND_INITIALIZER;

struct thread
{
	//** CHANGE **
	string bincode;
	char char_c;
	int thread_id;

	string prev_string = "";

	//** CONDITION **
	int turn;
	int data_saved = 0;
	void get_thread(string b, char l, int i)
	{
		bincode = b; char_c = l; thread_id = i;
	}
};

void* child_decompression(void* shared_data)
{
	// Stop parents until child finishes saving data
	pthread_mutex_lock(&mutex);

	//save to local
	thread local_data = *((thread*)shared_data);
	((thread*)shared_data)->data_saved = 1;
	pthread_cond_signal(&parent_child);
	pthread_mutex_unlock(&mutex);


	pthread_mutex_lock(&mutex);

	// Stop other childs until finishes his decompression
	while (local_data.thread_id != ((thread*)shared_data)->turn)
		pthread_cond_wait(&child_child, &mutex);


	// start decompression
	(local_data.char_c == '\n') ? cout << "<EOL>" : cout << local_data.char_c;
	cout << " Binary code = " << local_data.bincode << endl;
	string prev_code = ((thread*)shared_data)->prev_string;

	// *********** DECOMPRESSION PROCESS **************
	string temp;
	int track = 0;
	for (int j = 0; j < local_data.bincode.length(); j++)
	{
		if (local_data.bincode[j] == '1')
			temp += local_data.char_c;
		else {
			temp += prev_code[track];
			track++;
		}
	}
	((thread*)shared_data)->prev_string = temp;
	((thread*)shared_data)->turn--;

	// *********** FINISH DECOMPRESSION **************

	// WAKE UP THE REST CHILDREN
	pthread_cond_broadcast(&child_child);
	pthread_mutex_unlock(&mutex);
}
int main()
{
	vector<char> letter;
	vector<string> code;
	vector<int> thread_ids;

	// Parsing the input
	string temp_line;
	int i = 0;
	while (getline(cin, temp_line)) {
		if (temp_line != "") {
			char c;
			string s;
			if (temp_line.find("<EOL>") == 0)
			{
				c = '\n';
				s = temp_line.substr(6, temp_line.size());
			}
			else
			{
				c = temp_line[0];
				s = temp_line.substr(2, temp_line.size());

			}
			letter.push_back(c); code.push_back(s);
		}
		thread_ids.push_back(i); i++;
	}
	int size = letter.size();
	static struct thread child;

	// Set turn to execute child in right order
	child.turn = size-1;


	pthread_t tids[100];
	for (int i = 0; i < size; i++)
	{
		// Assign new data to pass to child thread
		child.get_thread(code[i], letter[i], thread_ids[i]);


		if(pthread_create(&tids[i], NULL, child_decompression, (void*)&child))
		{
			fprintf(stderr, "Error creating thread\n");
			return 1;
		}
		pthread_mutex_lock(&mutex);

		// parent wait for child to finish saving data
		if (child.data_saved == 0)
			pthread_cond_wait(&parent_child, &mutex);

		//reset data_saved for new thread
		child.data_saved = 0;
		pthread_mutex_unlock(&mutex);

	}

	for (int i = 0; i < size; i++)
		pthread_join(tids[i], NULL);

	cout << "Decompressed file contents:" << endl;
	cout << child.prev_string << endl;

	return 0;
}
