all: receiver transmitter

receiver:
	g++ -W -Wall -Wextra -pedantic -std=c++11 receiver.cpp -o receiver -lboost_system

transmitter:
	g++ -W -Wall -Wextra -pedantic -std=c++11 transmitter.cpp -o transmitter -lboost_system -lpthread
