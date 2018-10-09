FLAGS=-lportaudio -lncurses -lpthread -std=c++11

all: model client

model: model_mainloop.cpp oo_model.cpp oo_model.hpp
	g++ -omodel model_mainloop.cpp oo_model.cpp $(FLAGS)

client: client.cpp oo_model.cpp
	g++ -oclient client.cpp oo_model.cpp $(FLAGS)

remove:
	rm model client
