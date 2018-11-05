FLAGS=-lportaudio -lncurses -lpthread -std=c++11

all: server client

server: server.cpp
	g++ -oserver server.cpp $(FLAGS)

client: client.cpp teclado.cpp
	g++ -oclient client.cpp teclado.cpp $(FLAGS)

remove:
	rm server client
