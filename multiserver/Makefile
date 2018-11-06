FLAGS=-lportaudio -lncurses -lpthread -std=c++11

all: server client

server: server.cpp alvo.cpp audio.cpp fisica.cpp nave.cpp teclado.cpp tela.cpp tiro.cpp
	g++ -oserver server.cpp alvo.cpp audio.cpp fisica.cpp nave.cpp teclado.cpp tela.cpp tiro.cpp $(FLAGS)

client: client.cpp alvo.cpp audio.cpp fisica.cpp nave.cpp teclado.cpp tela.cpp tiro.cpp
	g++ -oclient client.cpp alvo.cpp audio.cpp fisica.cpp nave.cpp teclado.cpp tela.cpp tiro.cpp $(FLAGS)

remove:
	rm server client
