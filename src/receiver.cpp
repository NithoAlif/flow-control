/* Nama/NIM  : Nitho Alif Ibadurrahman/13513072	*/
/* File		 : receiver.cpp						*/
/* Deskripsi : Deklarasi objek receiver 		*/
#include <iostream>
#include <thread>
#include <cstring>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "dcomm.h"

using namespace std;

#define DELAY 500 // Delay to adjust speed of consuming buffer, in milliseconds
#define RXQSIZE 8 // Define receive buffer size
#define UPLIMIT 6 // Define minimum upper limit
#define LOWLIMIT 2 // Define maximum lower limit

Byte sent_xonxoff[1]; // XON/XOFF signal container

class buffer{
public:
	/* CREATE NEW BUFFER */
	buffer() : maxsize(RXQSIZE){
		count = 0;
		data = new Byte [maxsize];
		for (int i = 0; i < maxsize; ++i){
			data[i] = 0;
		}
	}

	/* DELETE BUFFER REGION */
	~buffer(){
		delete [] data;
	}

	/* ADD LAST ELEMENT IN BUFFER */
	void add(Byte c){
		/* */
		if (!isFull()){
			data[count] = c;
			count++;
		}
	}

	/* CONSUME FIRST ELEMENT IN BUFFER */
	void consume(Byte *c){
		if (!isEmpty()){
			*c = data[0];
			count--;
			for (int i = 0; i < count; ++i){
				data[i] = data[i+1];
			}
			data[count] = 0;
		}
	}

	/* GETTER CURRENT BUFFER SIZE */
	int getCount(){
		return count;
	}

	/* GET BUFFER FULL STATUS */
	bool isFull(){
		return (count == maxsize);
	}

	/* GET BUFFER EMPTY STATUS */
	bool isEmpty(){
		return (count == 0);
	}

private:
	int count; // Current buffer size
	int maxsize; // Maximum buffer size
	Byte *data; // Buffer memory region
};


class receiver{
public:
	/* CREATE NEW RECEIVER OBJECT */
	receiver(const char* arg) : port(arg){
		createSocket(); // Create new socket

		bindSocket(); // Bind socket to IP/Port

		sent_xonxoff[0] = XON; // XON/XOFF state initialization

		doReceive(); // Do the receiving

		closeSocket(); // Close socket
	}

	/* CLOSE RECEIVER SOCKET */
	~receiver(){
		closeSocket();
	}

	/* CREATE SOCKET */
	void createSocket(){
		if ((socket_ = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
			throw "Error opening socket!";
		}
	}

	/* BIND SOCKET */
	void bindSocket(){
		bzero(&receiver_endpoint, sizeof(receiver_endpoint));
		receiver_endpoint.sin_family = AF_INET;
		receiver_endpoint.sin_addr.s_addr = htonl(INADDR_ANY);
		receiver_endpoint.sin_port = htons(atoi(port));
		if (bind(socket_, (struct sockaddr *)&receiver_endpoint, sizeof(receiver_endpoint)) < 0){
			throw "Error binding. Try different port!";
		} else{
			inet_ntop(AF_INET, &(receiver_endpoint.sin_addr), address, INET_ADDRSTRLEN);
			cout << "Binding pada " << getAddress() << ":" << getPort() << " ..." << endl;
		}
	}

	/* RECEIVE DATA FROM TRANSMITTER */
	void doReceive(){
		socklen_t addrlen = sizeof(transmitter_endpoint);
		Byte c[1];
		int received = 1;

		thread consume_t(doConsume, &rxbuf, &socket_, &transmitter_endpoint); // Create new thread for consuming the buffer data
		do{
			if(sent_xonxoff[0] == XON){ // Receive data while current state is XON
				int recvlen = recvfrom(socket_, c, 1, 0, (struct sockaddr *)&transmitter_endpoint, &addrlen);
				if (recvlen > 0){
					if (c[0]>32 || c[0]==CR || c[0]==LF || c[0]==Endfile){
						cout << "Menerima byte ke-" << received << "." << endl;
						received++;
						rxbuf.add(c[0]);						
					}
				}	
				if (rxbuf.getCount()>=UPLIMIT){ // Minimum upper limit has been reached; send the XOFF signal
					sent_xonxoff[0] = XOFF;
					cout << "Buffer > minimum upperlimit. Mengirim XOFF." << endl;
					if (sendto(socket_, sent_xonxoff, 1, 0, (struct sockaddr *)&transmitter_endpoint, addrlen) < 0){
						throw "Error sending XOFF signal";
					}
				} 
			}
			usleep(DELAY * 1000);
		} while(c[0] != Endfile);
		consume_t.join(); // Join the buffer-consumer thread to this thread
	}

	/* CONSUME DATA IN BUFFER */
	static void doConsume(buffer *buf, int *sockfd, sockaddr_in *transmitter){
		socklen_t addrlen = sizeof(*transmitter);
		Byte c = 0;
		int consumed = 1;
		while(c != Endfile){
			if (!buf->isEmpty()){ // Consume the buffer data unless empty
				buf->consume(&c);
				if (c == Endfile){
					cout << "Data has been successfully received!" << endl;
				} else{
					cout << "Mengkonsumsi byte ke-" << consumed << ": '" << c << "'" << endl;
					consumed++;					
				}
			}

			if (buf->getCount()<LOWLIMIT && sent_xonxoff[0]==XOFF){ // Maximum lower limit has been reached; send the XON signal
				sent_xonxoff[0] = XON;
				cout << "Buffer < maximum lowerlimit. Mengirim XON." << endl;
				if (sendto(*sockfd, sent_xonxoff, 1, 0, (struct sockaddr *)transmitter, addrlen) < 0){
					throw "Error sending XON signal";
				}
			}
			usleep(DELAY * 3000);
		}
	}

	/* GETTER RECEIVER BOUND ADDRESS */
	string getAddress(){
		return address;
	}

	/* GETTER RECEIVER BOUND PORT */
	string getPort(){
		return port;
	}

	/* CLOSE SOCKET */
	void closeSocket(){
		close(socket_);
	}
	
private:
	int socket_; // Opened socket
	struct sockaddr_in receiver_endpoint; // Receiver endpoint
	struct sockaddr_in transmitter_endpoint; // Transmitter endpoint
	char address[INET_ADDRSTRLEN]; // Receiver bound address
	const char* port; // Receiver bound port
	buffer rxbuf; // Buffer
};


int main(int argc, char const *argv[]){
	try{
		if (argc < 2){ // Parameter checking
			throw "Usage: receiver <port>";
		} 
		
		receiver rx(argv[1]); // Create receiver object

	} catch (const char* msg) { // Exception handling
		cerr << msg << endl;
	} catch(std::exception& e){
		cerr << "Exception: " << e.what() << endl;  // Unhandled exception
	}

	return 0;
}