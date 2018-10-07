#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <ncurses.h>

#include "oo_model.hpp"

#include <stdlib.h>
#include <time.h>       /* time */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_TIROS 101
#define ALTURA_TELA 20
#define LARGURA_TELA 40

int main() {
  int socket_fd;
  struct sockaddr_in target;

  //Criando a Tela para que o Teclado funcione

  //Criando os tiros
  ListaDeTiros *t_lista = new ListaDeTiros();
  for (int k=1; k<MAX_TIROS; k++){
    Tiro *tiro = new Tiro(0, 0, 0, 0);
    t_lista->add_tiro(tiro);
  }

  //Criando a Nave
  Nave *nave1 = new Nave(1);
  ListaDeNaves *n_lista = new ListaDeNaves();
  n_lista->add_nave(nave1);

  //Criando o alvo
  srand (time(NULL));
  Alvo *alvo = new Alvo((float)(rand() % (LARGURA_TELA/2) + LARGURA_TELA/2), (float)(rand() % ALTURA_TELA));
  Fisica *f = new Fisica(alvo, n_lista, t_lista);

  Tela *tela = new Tela(alvo, n_lista, t_lista, 20, 20, 20, 20);
  tela->init();

  Teclado *teclado = new Teclado();
  teclado->init();


  //Espera
  while(1){
    char c = teclado->getchar();
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    target.sin_family = AF_INET;
    target.sin_port = htons(3001);
    inet_aton("127.0.0.1", &(target.sin_addr));
    if (connect(socket_fd, (struct sockaddr*)&target, sizeof(target)) != 0) {
        return 0;
    }

    /* Agora, meu socket funciona como um descritor de arquivo usual */
    send(socket_fd, &c, 1, 0);
    
    /* Recebendo resposta */
    char reply[10];
    recv(socket_fd, reply, 10, 0);

    if (c=='q') {
      break;
    }
  }

  close(socket_fd);
  teclado->stop();
  tela->stop();

  return 0;
}

