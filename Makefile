FLAGS=-lportaudio -lncurses -lpthread -std=c++11

all: model client

model: server.cpp oo_model.cpp oo_model.hpp
	g++ -oserver server.cpp oo_model.cpp $(FLAGS)

client: client.cpp oo_model.cpp
	g++ -oclient client.cpp oo_model.cpp $(FLAGS)

remove:
	rm server client
