#include<iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
using namespace std;
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void fireman(int) {
	while (waitpid(-1, NULL, WNOHANG) > 0) {
		std::cout << "Child process ended" << std::endl;
	}
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno, clilen;
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0)
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		error("ERROR on binding");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
	 signal(SIGCHLD, fireman);
	 while(true)
	 {
		newsockfd = accept(sockfd, (struct sockaddr*) & cli_addr, (socklen_t*)& clilen);
		pid_t pid = fork();
		if (pid == 0)
		{
			char key;
			n = read(newsockfd, &key, 1);
			char buffer[256];
			bzero(buffer, 256);
			n = read(newsockfd, buffer, 256);
			//reference: https://stackoverflow.com/questions/26380695/how-to-correctly-converting-char-array-to-string-in-c
			string message(buffer);

			string newString = "";
			for (int j = 0; j < message.length(); j++)
			{
				if (buffer[j] == key)
				{
					newString += '1';
				}
				else {
					newString += '0';
				}
			}
			bzero(buffer, 256);
			for (int l = 0; l < newString.length(); l++)
				buffer[l] = newString[l];
			n = write(newsockfd,  buffer,  256);
			exit(0);
		}
		close(newsockfd);
	 }
     return 0;
}
