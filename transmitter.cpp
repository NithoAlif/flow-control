/* 	Filename	: transmitter.cpp
	Author		: Ahmad Darmawan (13513096)
	Description	: Transmitter berfungsi untuk mengirimkan paket data ke dalam socket UDP */

#include <stdio.h>	/* defines printf */
#include <stdlib.h>	/* defines exit and other sys calls */
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>	/* needed for memset */
#include <netinet/in.h>	/* needed for sockaddr_in */
#include <netdb.h>
#include <iostream>
#include <fstream>
#include <pthread.h>

/* XON/XOFF protocol */
#define XON (0x11)
#define XOFF (0x13)

using namespace std;

/* Variabel Global */
char* filename;
int sock_fd;
struct sockaddr_in myaddr, remaddr;
unsigned int alen;	/* length of address (for getsockname) */
int i, recvlen;
unsigned int slen=sizeof(remaddr);
char buf[1], bufData[1];
struct hostent *hp;
bool done = false;
pthread_t tid[2];

void *sendData(void *) {
	// Mengirim pesan
	ifstream fin;
	char current;
	fin.open(filename, ios::in);
	int i = 0;
	while (fin.get(current)) {
		if (bufData[0] != XOFF)
		{
			i++;
			sprintf(buf, "%c", current);
			if (sendto(sock_fd, buf, 1, 0, (struct sockaddr *)&remaddr, slen)==-1) {
				perror("sendto");
			}
			else {
				printf("Mengirim byte ke-%d: '%s'\n", i, buf);
				sleep(1);
			}
		}
		else {
			printf("Menunggu XON...\n");
			sleep(1);
		}
	}
	fin.close();
	done = true;
	close(sock_fd);
	pthread_detach(tid[0]);
	pthread_detach(tid[1]);
	pthread_exit(NULL);
}

void *signalXonXoff(void *)
{
	// Memberi sinyal XON-XOFF
	do {
		recvlen = recvfrom(sock_fd, bufData, 1, 0, (struct sockaddr *)&remaddr, &slen);
		if (recvlen >= 0) {
			if (bufData[0] == XOFF) {
				printf("XOFF diterima.\n");
			}
			else {
				printf("XON diterima.\n");
			}
		}
	} while (done == false);
	pthread_exit(NULL);
}


int main(int argc, char *argv[])
{
	/* Verifikasi Masukan */
	if ((argc > 4) || (argc <= 3))
	{
		printf("Usage: ./transmitter <ip_address> <port_no> <filename>\n");
	}
	else {

		/* Algoritma */
		/* Assign filename dari argumen */
		filename = argv[3];

		/* Membuat socket UDP/IP */
		printf("Membuat socket untuk koneksi ke %s:%s ...\n", (argv[1]), (argv[2]));
		if ((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
			perror("cannot create socket");
			return 0;
		}

		/* Melakukan bind pada socket */		

		memset((char *)&myaddr, 0, sizeof(myaddr));
		myaddr.sin_family = AF_INET;
		myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
		myaddr.sin_port = htons(0);

		if (bind(sock_fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
			perror("bind failed");
			return 0;
		}       

		hp = gethostbyname(argv[1]);

		memset((char *) &remaddr, 0, sizeof(remaddr));
		remaddr.sin_family = AF_INET;
		remaddr.sin_port = htons(atoi(argv[2]));
		memcpy((void *)&remaddr.sin_addr, hp->h_addr_list[0], hp->h_length);

		
		pthread_create(&(tid[0]), NULL, sendData, NULL);
		pthread_create(&(tid[1]), NULL, signalXonXoff, NULL);
		
		pthread_join(tid[0], NULL);
		pthread_join(tid[1], NULL);

		//close(sock_fd);
	}
	return 0;
}
