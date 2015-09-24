all: receiver transmitter

receiver:
	g++ -W -Wall -Wextra -pedantic -std=c++11 receiver.cpp -o receiver -lpthread

transmitter:
	g++ -W -Wall -Wextra -pedantic -std=c++11 transmitter.cpp -o transmitter -lpthread
