#include <iostream>
#include <thread>
#include <vector>
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
#define UPLIMIT 6 // Define minimum upper limit
#define LOWLIMIT 2 // Define maximum lower limit

Byte sent_xonxoff[1];


int sockfd, length, n;
socklen_t fromlen;
struct sockaddr_in transmitter_endpoint;
char buf[RXQSIZE];


class buffer{
public:
	buffer() : maxsize(RXQSIZE){
		count = 0;
		data = new Byte [maxsize];
		for (int i = 0; i < maxsize; ++i){
			data[i] = 0;
		}
	}

	~buffer(){
		delete [] data;
	}

	void add(Byte c){
		if (!isFull()){
			data[count] = c;
			count++;
		}
	}

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

	Byte getLast(){
		return data[count-1];
	}

	int getCount(){
		return count;
	}

	bool isFull(){
		return (count == maxsize);
	}

	bool isEmpty(){
		return (count == 0);
	}

private:
	int count;
	int maxsize;
	Byte *data;
};


class receiver{
public:
	receiver(const char* arg) : port(arg){
		/* CREATE SOCKET */
		createSocket();

		sent_xonxoff[0] = XON;

		/* BIND SOCKET */
		bindSocket();

		/* DO THE RECEIVING */
		doReceive();

	}

	~receiver(){
		closeSocket();
	}

	void createSocket(){
		if ((socket_ = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
			throw "Error opening socket!";
		}
	}

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

	void doReceive(){
		socklen_t addrlen = sizeof(transmitter_endpoint);
		Byte c[1];
		int received = 1;

		thread consume_t(doConsume, &rxbuf, socket_);
		do{
			int recvlen = recvfrom(socket_, c, 1, 0, (struct sockaddr *)&transmitter_endpoint, &addrlen);
			if (recvlen > 0){
				cout << "Menerima byte ke-" << received << "." << endl;
				received++;

				if (rxbuf.getCount() < UPLIMIT){
					rxbuf.add(c[0]);
					//cout << "count = " << rxbuf.getCount() << "|| current char = " << rxbuf.getLast() << endl;
				} else{
					if (sent_xonxoff[0] == XON){
						sent_xonxoff[0] = XOFF;
						cout << "Buffer > minimum upperlimit. Mengirim XOFF." << endl;
						if (sendto(socket_, sent_xonxoff, 1, 0, (struct sockaddr *)&transmitter_endpoint, addrlen) < 0){
							//cout << "sendto" << endl;
							//throw "Error sending XOFF signal";
						}
					}
				}
			}
			
			usleep(DELAY * 1000);
		} while(c[1] != Endfile);

		consume_t.join();
	}

	static void doConsume(buffer *buf, int sockfd){
		socklen_t addrlen = sizeof(sockfd);
		Byte c = 0;
		int consumed = 1;
		do{
			if(!buf->isEmpty()){
				buf->consume(&c);
				if (c>32 && c!=CR){
					cout << "Mengkonsumsi byte ke-" << consumed << ":'" << c << "'" << endl;
					consumed++;
					if (buf->getCount()<LOWLIMIT && sent_xonxoff[0]==XOFF){
						sent_xonxoff[0] = XON;
						cout << "Buffer < maximum lowerlimit. Mengirim XON." << endl;
						if (sendto(sockfd, sent_xonxoff, 1, 0, (struct sockaddr *)&sockfd, addrlen) < 0){
							throw "Error sending XON signal";
						}
						cout << "LOLOLO" << endl;
					}
				}
			}
			usleep(DELAY * 3000);
		} while(c != Endfile);

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
	struct sockaddr_in transmitter_endpoint;
	char address[INET_ADDRSTRLEN];
	const char* port;
	buffer rxbuf;
};




int main(int argc, char const *argv[]){
	try{
		if (argc < 2){
			throw "Usage: receiver <port>";
		} 
		
		receiver rx(argv[1]);

		
		//rx.closeSocket();
	} catch (const char* msg) {
		cerr << msg << endl;
	} catch(std::exception& e){
		cerr << "Exception: " << e.what() << endl;
	}

	return 0;
}