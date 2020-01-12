#include<iostream>
#include<map>
#include<string>
#include <vector>
#include<algorithm>
#include<fstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <strings.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>

using namespace std;


struct storeThreadUse
{
	char key;
	string shortMessage;
	char** argv;
	string newString;
	string delMessage;
};
void error(const char* e)
{
	perror(e);
	exit(0);
}
void* convertNum(void *current) //Keith change function to void(no need to return), pass string by ref, and add an argument (child iteration)
{
	struct storeThreadUse* temp = (struct storeThreadUse*)current;
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent* server;
	char buffer[256];
	portno = atoi(temp->argv[2]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		error("ERROR opening socket");
	server = gethostbyname(temp->argv[1]);
	if (server == NULL) {
		fprintf(stderr, "ERROR, no such host\n");
		exit(0);
	}
	bzero((char*)& serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char*)server->h_addr, (char*)& serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(portno);
	if (connect(sockfd, (struct sockaddr*) & serv_addr, sizeof(serv_addr)) < 0)
		error("ERROR connecting");
	bzero(buffer, 256);
	char keytosend = temp->key;
	n = write(sockfd, &keytosend, 1);
	bzero(buffer, 256);
	for (int k = 0; k < temp->shortMessage.size(); k++)
		buffer[k] = temp->shortMessage[k];
	n = write(sockfd, buffer, 256);
	bzero(buffer, 256);
	n = read(sockfd, buffer, 256);
	temp->newString = *new string(buffer);
}

int main(int argc, char* argv[])
{
	static struct storeThreadUse storeToUse[256];
	string letter;
	getline(cin, letter, '\0');


	map<int, int> mp;
	map<int, int> nmp;

	// Traverse through array elements and
	// count frequencies
	for (int i = 0; i < letter.size(); i++)
	{
		//cout << letter[i] << "ascii value=" << int(letter[i]) << endl;
		if ((int(letter[i]) >= 65 && int(letter[i] <= 90)) ||
			(int(letter[i]) >= 97 && int(letter[i] <= 122)) ||
			(int(letter[i]) >= 48 && int(letter[i] <= 57)) ||
			letter[i] == '\n' || letter[i] == ' ') //Keith
			mp[letter[i]]++;
	}

	map<int, int> sortedMap;
	vector<int> sortedKey;
	vector<int> sortedValue;

	for (auto x = mp.begin(); x != mp.end(); x++)
	{
		sortedValue.push_back(x->second);
	}
	sort(sortedValue.begin(), sortedValue.end(), greater<int>());



	for (unsigned i = 0; i < sortedValue.size(); i++) {
		for (auto x = mp.begin(); x != mp.end(); x++) {
			if (x->second == sortedValue.at(i)) {
				sortedKey.push_back(x->first);
				x->second = -1;
			}
		}
	}


	for (unsigned i = 0; i < sortedValue.size(); i++)
	{
		if (char(sortedKey.at(i)) == '\n') //Keith
			cout << "<EOL> frequency = " << sortedValue.at(i) << endl; //Keith
		else
		{
			if (char(sortedKey.at(i)) != ' ')
				cout << char(sortedKey.at(i)) << " ";
			cout << "frequency = " << sortedValue.at(i) << endl;
		}
	}


	cout << "Original Message:       ";
	for (int k = 0; k < letter.length(); k++)
		if (letter[k] != '\n')
			cout << letter[k];
		else cout << "<EOL>";
	cout << endl;

	string shortMessage;
	shortMessage = letter;
	pthread_t tid[256];


	for (int i = 0; i < sortedKey.size(); i++)
	{
		storeToUse[i].key = sortedKey.at(i);
		storeToUse[i].shortMessage = shortMessage;
		storeToUse[i].delMessage = "";
		for (int m = 0; m < shortMessage.size(); m++)
			if (shortMessage[m] != sortedKey.at(i))
				storeToUse[i].delMessage += shortMessage[m];

		storeToUse[i].argv = argv;

		if (pthread_create(&tid[i], NULL, convertNum, &storeToUse[i]))
		{
			fprintf(stderr, "Error creating thread\n");
			return 1;
		}
		shortMessage = storeToUse[i].delMessage;
	}
	for (int i = 0; i < sortedKey.size(); i++)
		pthread_join(tid[i], NULL);

	for (int i = 0; i < sortedKey.size(); i++)
	{
		cout << "Symbol ";
		if (storeToUse[i].key == '\n')
			cout << "<EOL>" << " code:	" << storeToUse[i].newString;
		else
			cout << storeToUse[i].key << " code:		" << storeToUse[i].newString;
		cout << endl;

		if (storeToUse[i].delMessage.length() > 0)
		{
			cout << "Remaining Message:	";
			for (int j = 0; j < storeToUse[i].delMessage.length(); j++)
			{
				if (storeToUse[i].delMessage[j] == '\n')
					cout << "<EOL>";
				else cout << storeToUse[i].delMessage[j];
			}
			cout << endl;
		}
	}
	return 0;
}