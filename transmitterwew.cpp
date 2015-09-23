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
#include <pthread.h>

#define NUM_THREADS 2;

/* XON/XOFF protocol */
#define XON (0x11)
#define XOFF (0x13)

void *signalXonXoff(int fd, char buf, (struct sockaddr *)&remaddr, int &slen)
{
	do {
		recvlen = recvfrom(fd, buf, 1, 0, (struct sockaddr *)&remaddr, &slen);
	} while (!done);
	pthread_exit(NULL);
}

void *sendData(string file_teks, int fd, char buf, char bufData, (struct sockaddr *)&remaddr, int &slen)
{
	ifstream fin;
   	char current;
	fin.open(file_teks, ios::in);
	while (!fin.eof()) {
		if (buf != XOFF)
		{
			fin.get(current);
			bufData = current;
			if (sendto(fd, bufData, strlen(bufData), 0, (struct sockaddr *)&remaddr, slen)==-1) {
				perror("sendto");
			}
			{
			else
				//print data ini kekirim
			}
		}
		else {
			// print kena XOFF
			while (buf != XON) do {
				// print menunggu XON
			}
			// sudah XON
		}
	}
	done = true;
	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	struct sockaddr_in serv_addr, remaddr;
    struct hostent *server;
    int slen = sizeof(remaddr);
    char buf[1], bufData[1];
    bool done = false;

	if (argc < 4) {
		perror("Masukan salah\n");
	}
	else { //argc == 3
		int portno = atoi(argv[2]);
		string file_teks = argv[3];

		// Membuat socket
		int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    	if (sockfd < 0) 
        	fprintf(stderr,"ERROR, opening socket\n");
    		server = gethostbyname(argv[1]);
    	if (server == NULL) {
        	fprintf(stderr,"ERROR, no such host\n");
        	exit(0);
    	}

    	// Membuat bind
    	memset((void *)&serv_addr, 0, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		serv_addr.sin_port = htons(0);

		if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
			perror("bind failed");
			return 1;
		}

		alen = sizeof(serv_addr);
		if (getsockname(sockfd, (struct sockaddr *)&serv_addr, &alen) < 0) {
			perror("getsockname failed");
			return 1;
		}

		printf("bind complete. Port number = %d\n", ntohs(serv_addr.sin_port));

    	memset((char *) &remaddr, 0, sizeof(remaddr));
		remaddr.sin_family = AF_INET;
		remaddr.sin_port = htons(portno);
		if (inet_aton(server, &remaddr.sin_addr)==0) {
			fprintf(stderr, "inet_aton() failed\n");
			exit(1);
		}


    	// Membuat thread
    	pthread_t threads[NUM_THREADS];

	}

	close(sockfd);
	return 0;
}
