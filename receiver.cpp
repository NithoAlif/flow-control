#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "dcomm.h"


using namespace std;

#define DELAY 500 // Delay to adjust speed of consuming buffer, in milliseconds
#define RXQSIZE 8 // Define receive buffer size

Byte rxbuf[RXQSIZE];
QTYPE rcvq = { 0, 0, 0, RXQSIZE, rxbuf };
QTYPE *rxq = &rcvq;
Byte sent_xonxoff = XON;
bool send_xon = false,
send_xoff = false;


int sockfd, length, n;
socklen_t fromlen;
struct sockaddr_in transmitter_endpoint;
char buf[1024];

/* Functions declaration */
static Byte *rcvchar(int sockfd, QTYPE *queue);
static Byte *q_get(QTYPE *, Byte *);


static Byte *q_get(QTYPE *queue, Byte *){

}


class receiver{
public:
	receiver(const char* arg) : port(arg){
		/* CREATE SOCKET */
		createSocket();

		/* BIND SOCKET */
		bindSocket();

	}

	void createSocket(){
		if ((socket_ = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
			throw "Error opening socket!";
		}
	}

	void bindSocket(){
		bzero(&receiver_endpoint, RXQSIZE);
		receiver_endpoint.sin_family = AF_INET;
		receiver_endpoint.sin_addr.s_addr = INADDR_ANY;
		receiver_endpoint.sin_port = htons(atoi(port));
		if (bind(socket_, (struct sockaddr *)&receiver_endpoint, sizeof(receiver_endpoint)) < 0){
			throw "Error binding. Try different port!";
		} else{
			inet_ntop(AF_INET, &(receiver_endpoint.sin_addr), address, INET_ADDRSTRLEN);
		}
	}

	string getAddress(){
		return address;
	}

	string getPort(){
		return port;
	}

	void closeSocket(){
		close(socket_);
	}
	
private:
	int socket_;
	struct sockaddr_in receiver_endpoint;
	char address[INET_ADDRSTRLEN];
	const char* port;
};


void* buff_consumer(void *arg){

}



int main(int argc, char const *argv[]){
	try{
		if (argc < 2){
			throw "Usage: receiver <port>";
		} 
		
		receiver rx(argv[1]);
		cout << "Binding pada " << rx.getAddress() << ":" << rx.getPort() << " ..." << endl;

		pthread_t buff_t;

		//pthread_create (buff_t, NULL, start_routine, NULL);


		
		/* Initialize XON/XOFF flags */
		/* Create child process */
		/*** IF PARENT PROCESS ***/
//		while (true) {
//			c = *(rcvchar(sockfd, rxq));
			/* Quit on end of file */
//			if (c == Endfile) {
//				exit(0);
//			}
//		}
		/*** ELSE IF CHILD PROCESS ***/
//		while (true) {
			/* Call q_get */
			/* Can introduce some delay here. */
//		}

		rx.closeSocket();
	} catch (const char* msg) {
		cerr << msg << endl;
	} catch(std::exception& e){
		cerr << "Exception: " << e.what() << endl;
	}

	return 0;
}