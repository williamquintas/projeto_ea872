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

using namespace std::chrono;
uint64_t get_now_ms() {
  return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}

int main () {

  int socket_fd, connection_fd;
  struct sockaddr_in myself, client;
  socklen_t client_size = (socklen_t)sizeof(client);
  char input_teclado;

  socket_fd = socket(AF_INET, SOCK_STREAM, 0);

  myself.sin_family = AF_INET;
  myself.sin_port = htons(3001);
  inet_aton("127.0.0.1", &(myself.sin_addr));

  if (bind(socket_fd, (struct sockaddr*)&myself, sizeof(myself)) != 0) {
    return 0;
  }

  listen(socket_fd, 2);

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
  tela->draw();

  Teclado *teclado = new Teclado();
  teclado->init();

  uint64_t t0;
  uint64_t t1;
  uint64_t deltaT;
  uint64_t T;

  int n_tiro = 0;

  T = get_now_ms();
  t1 = T;

  Audio::Sample *asample;
  asample = new Audio::Sample();
  asample->load("assets/gun.dat");

  Audio::Player *player;
  player = new Audio::Player();
  player->init();

  //Espera
  while (1) {
    std::this_thread::sleep_for (std::chrono::milliseconds(1));
    t1 = get_now_ms();
    if (t1-t0 > 500) break;
  }

  int n_resp = 0;
  while (1) {
    // Atualiza timers
    t0 = t1;
    t1 = get_now_ms();
    deltaT = t1-t0;

    // Atualiza modelo
    f->update_tiro(deltaT);

    // Atualiza tela
    tela->update();


    //Espera resposta
    move(100,100);
    connection_fd = accept(socket_fd, (struct sockaddr*)&client, &client_size);
    recv(connection_fd, &input_teclado, 1, 0);

    /* Identificando cliente */
    char ip_client[INET_ADDRSTRLEN];
    inet_ntop( AF_INET, &(client.sin_addr), ip_client, INET_ADDRSTRLEN );

    /* Respondendo */
    if (send(connection_fd, "PONG", 5, 0) < 0) {
    } else {
      n_resp++;
    }
  
    char c = input_teclado;
    // Lê o teclado
    if (c=='q') {
      break;
    }
    if (c=='s') {
      move(100,100);
      f->andar_nave(1);
    }
    if (c=='w') {
      f->andar_nave(-1);
    }
    if (c=='t') {
      if (n_tiro+1 == MAX_TIROS){
        break;
      }
      asample->set_position(0);
      player->play(asample);
      f->disparar_tiro(n_tiro);
      n_tiro++;
    }
    std::this_thread::sleep_for (std::chrono::milliseconds(100));
  }

  player->stop();
  tela->stop();
  teclado->stop();
  close(socket_fd);
  return 0;
}
