/* File : transmitter.cpp */

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fstream>

int main(int argc, char *argv[])
{
	struct sockaddr_in serv_addr;
    struct hostent *server;

	if (argc < 4) {
		cerr << "Masukan Salah" << endl;
	}
	else { //argc == 3
		int portno = atoi(argv[2]);
		string file_teks = argv[3];

		// Membuat socket
		int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    	if (sockfd < 0) 
        	cout << "ERROR opening socket" << endl;
    		server = gethostbyname(argv[1]);
    	if (server == NULL) {
        	fprintf(stderr,"ERROR, no such host\n");
        	exit(0);
    	}

		// Membuat fork
    	int counter = 0;
    	pid_t pid = fork();

    	if (pid == 0)
    	{
        	/* repeat  
        		ch <- read a character from socket  
        		last received char <- ch
        	until connection is terminated */
    	}
    	else if (pid > 0)
    	{
    		ifstream fin;
    		char current;
    		fin.open(argv[3], ios::in);
    		while (!fin.eof()) {
    			if (/*ini gimana*/) {
    				fin.get(current);
    				//send current to socket
    			}
    		}
    	}
    	else
    	{
    	    // fork failed
    	    printf("fork() failed!\n");
    	    return 1;
    	}

	}
	return 0;
}
