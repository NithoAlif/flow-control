all: receiver transmitter

receiver:
	g++ -W -Wall -Wextra -pedantic -std=c++11 src/receiver.cpp -o bin/receiver -lpthread

transmitter:
	g++ -W -Wall -Wextra -pedantic -std=c++11 src/transmitter.cpp -o bin/transmitter -lpthread
