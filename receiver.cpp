#include <iostream>
//#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include "dcomm.h"


using namespace std;

#define DELAY 500 // Delay to adjust speed of consuming buffer, in milliseconds
#define RXQSIZE 8 // Define receive buffer size



int sockfd, length, n;
socklen_t fromlen;
struct sockaddr_in receiver_endpoint;
struct sockaddr_in transmitter_endpoint;
char buf[1024];












int main(int argc, char const *argv[]){
	try{
		if (argc < 2){
			cerr << "Usage: receiver <port>" << endl;
			return 1;
		} 

		/* CREATE SOCKET */
		sockfd = socket(AF_INET, SOCK_DGRAM, 0);
		if (sockfd < 0){
			cerr << "Error opening socket" << endl;
		}
		length = sizeof(receiver_endpoint);
		bzero(&receiver_endpoint, length);
		
		receiver_endpoint.sin_family = AF_INET;
		receiver_endpoint.sin_addr.s_addr = INADDR_ANY;
		receiver_endpoint.sin_port = htons(atoi(argv[1]));

		if (bind(sockfd, (struct sockaddr *)&receiver_endpoint,length)<0) 
			cerr << "Error binding. Try different port" << endl;
		fromlen = sizeof(struct sockaddr_in);


	} catch(std::exception& e){
		cerr << "Exception: " << e.what() << endl;
	}

	return 0;
}