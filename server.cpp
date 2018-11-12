#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <chrono>
#include <thread>
#include <ncurses.h>
#include <iostream>   

#include "alvo.hpp"
#include "nave.hpp"
#include "tiro.hpp"
#include "fisica.hpp"
#include "tela.hpp"
#include "audio.hpp"

#define MAX_TIROS 51
#define ALTURA_TELA 20
#define LARGURA_TELA 40
#define MAX_CONEXOES 4

#ifdef __MACH__
#define MSG_NOSIGNAL SO_NOSIGPIPE 
#endif

/* Variaveis globais do servidor */
struct sockaddr_in myself, client;
socklen_t client_size;
int socket_fd;
int connection_fd[MAX_CONEXOES];
int conexao_usada[MAX_CONEXOES];
int running;

int total_tiros = 0;
int pontos = 0;

/* Funcoes do servidor */
int adicionar_conexao(int new_connection_fd) {
  int i;
  for (i=0; i<MAX_CONEXOES; i++) {
    if (conexao_usada[i] == 0) {
      conexao_usada[i] = 1;
      connection_fd[i] = new_connection_fd;
      return i;
    }
  }
  return -1;
}

int remover_conexao(int user) {
  if (conexao_usada[user]==1) {
  conexao_usada[user] = 0;
  close(connection_fd[user]);
  }
  return 1;
}

void *wait_connections(void *parameters) {
  int conn_fd;
  int user_id;
  while(running) {
    conn_fd = accept(socket_fd, (struct sockaddr*)&client, &client_size);
    user_id = adicionar_conexao(conn_fd);
    if (user_id != -1) {
      // printf("Novo usuario chegou! ID=%d\n", user_id);
    } else {
      // printf("Maximo de usuarios atingido!\n");
    }
  }
  return NULL;
}

using namespace std::chrono;
uint64_t get_now_ms() {
  return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}

/* FUNCAO PRINCIPAL */
int main() {
  //Declaraco de variaveis
  pthread_t esperar_conexoes;
  int msglen;
  int user_iterator;
  char input_buffer[50];
  char input_teclado;

  //Variaveis de buffer
  char output_buffer[50];

  /* Inicializando variaveis do servidor*/
  client_size = (socklen_t)sizeof(client);
  for (int i=0; i<MAX_CONEXOES; i++) {
    conexao_usada[i] = 0;
  }
  running = 1;

  //Criando os tiros
  ListaDeTiros *t_lista = new ListaDeTiros();
  for (int k=1; k<MAX_TIROS; k++){
    Tiro *tiro = new Tiro(0, 0, 0, 0, 0);
    t_lista->add_tiro(tiro);
  }

  //Criando a Nave
  Nave *nave1 = new Nave(3);
  Nave *nave2 = new Nave(3);
  Nave *nave3 = new Nave(3);
  Nave *nave4 = new Nave(3);
  ListaDeNaves *n_lista = new ListaDeNaves();
  n_lista->add_nave(nave1);
  n_lista->add_nave(nave2);
  n_lista->add_nave(nave3);
  n_lista->add_nave(nave4);


  //Criando o alvo
  srand (time(NULL));
  Alvo *alvo = new Alvo((float)(rand() % (LARGURA_TELA/2) + LARGURA_TELA/2), (float)(rand() % ALTURA_TELA));

  //Crinado a Tela
  Fisica *f = new Fisica(alvo, n_lista, t_lista);
  Tela *tela = new Tela(alvo, n_lista, t_lista, 20, 20, 20, 20);
  tela->init();
  tela->draw();

  //Variaveis para fisica
  uint64_t t0;
  uint64_t t1;
  uint64_t deltaT;
  uint64_t T;
  int n_tiro = 0;
  T = get_now_ms();
  t1 = T;

  //Inicializando o audio
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

  /* socket, bind, listen */
  socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  // printf("Socket criado\n");
  myself.sin_family = AF_INET;
  myself.sin_port = htons(3001);
  inet_aton("127.0.0.1", &(myself.sin_addr));
  // printf("Tentando abrir porta 3001\n");
  if (bind(socket_fd, (struct sockaddr*)&myself, sizeof(myself)) != 0) {
    // printf("Problemas ao abrir porta\n");
    return 0;
  }
  // printf("Abri porta 3001!\n");
  listen(socket_fd, 2);
  // printf("Estou ouvindo na porta 3001!\n");

  /* Dispara thread para ouvir conexoes */
  pthread_create(&esperar_conexoes, NULL, wait_connections, NULL);

  while (running) {
    // Atualiza timers
    t0 = t1;
    t1 = get_now_ms();
    deltaT = t1-t0;
    // Atualiza modelo
    f->update_tiro(deltaT, &pontos);
    // Atualiza tela
    tela->update(&total_tiros, &pontos);

    for (int ret=0; ret<MAX_CONEXOES; ret++) {
       if (conexao_usada[ret] == 1) { //sem opcao de desconectar
          //Jah usados: 0-9, a-c ([45])

          std::vector<Nave *> *n = n_lista->get_naves();
          //Mandando a posicao da primeira nave
          sprintf(output_buffer, "%f", (*n)[0]->get_posicao());
          output_buffer[45] = '0';
          send(connection_fd[ret], output_buffer, 50, 0);
          //Mandando a posicao da segunda nave
          sprintf(output_buffer, "%f", (*n)[1]->get_posicao());
          output_buffer[45] = 'a';
          send(connection_fd[ret], output_buffer, 50, 0);
          //Mandando a posicao da terceira nave
          sprintf(output_buffer, "%f", (*n)[2]->get_posicao());
          output_buffer[45] = 'b';
          send(connection_fd[ret], output_buffer, 50, 0);
          //Mandando a posicao da quarta nave
          sprintf(output_buffer, "%f", (*n)[3]->get_posicao());
          output_buffer[45] = 'c';
          send(connection_fd[ret], output_buffer, 50, 0);

          //Mandando a posicao dos alvos
          sprintf(output_buffer, "%lf", alvo->get_posicao_x());
          output_buffer[45] = '1';
          send(connection_fd[ret], output_buffer, 50, 0);

          sprintf(output_buffer, "%lf", alvo->get_posicao_y());
          output_buffer[45] = '2';
          send(connection_fd[ret], output_buffer, 50, 0);

          //Mandando as informacoes dos pontos, deltaT e total de tiros
          sprintf(output_buffer, "%f", (float)deltaT);
          output_buffer[45] = '3';
          send(connection_fd[ret], output_buffer, 50, 0);

          sprintf(output_buffer, "%d", pontos);
          output_buffer[45] = '4';
          send(connection_fd[ret], output_buffer, 50, 0);

          sprintf(output_buffer, "%d", total_tiros);
          output_buffer[45] = '5';
          send(connection_fd[ret], output_buffer, 50, 0);

          //Mandando as informacoes dos tiros
          std::vector<Tiro *> *t = t_lista->get_tiros();
          int tiro_i = 0;
          for (int i = 48; i<97; i++){
            output_buffer[46] = i;

            sprintf(output_buffer, "%f", (*t)[tiro_i]->get_posicao_x());
            output_buffer[45] = '6';
            send(connection_fd[ret], output_buffer, 50, 0);

            sprintf(output_buffer, "%f", (*t)[tiro_i]->get_posicao_y());
            output_buffer[45] = '7';
            send(connection_fd[ret], output_buffer, 50, 0);

            sprintf(output_buffer, "%f", (*t)[tiro_i]->get_velocidade());
            output_buffer[45] = '8';
            send(connection_fd[ret], output_buffer, 50, 0);

            sprintf(output_buffer, "%d", (*t)[tiro_i]->get_existe());
            output_buffer[45] = '9';
            send(connection_fd[ret], output_buffer, 50, 0);
            
            tiro_i++;
          }

       }
    }

    for (user_iterator=0; user_iterator<MAX_CONEXOES; user_iterator++) {
      if (conexao_usada[user_iterator] == 1) {
        msglen = recv(connection_fd[user_iterator], &input_teclado, 1, MSG_DONTWAIT);
        if (msglen > 0) {
          // printf("Recebi mensagem de %d\n", user_iterator);
          // Lê o teclado
          if (input_teclado=='q') {
            running = 0;
            break;
          }
          if (input_teclado=='s') {
            f->andar_nave(1, user_iterator);
          }
          if (input_teclado=='w') {
            f->andar_nave(-1, user_iterator);
          }
          if (input_teclado=='t') {
            if (n_tiro+1 == MAX_TIROS){
              running = 0;
              break;
            }
            asample->set_position(0);
            player->play(asample);
            f->disparar_tiro(n_tiro, &total_tiros, user_iterator);
            n_tiro++;
          }
        }
      }
    //estava aqui o sleep
    }
    std::this_thread::sleep_for (std::chrono::milliseconds(100));
  }

  // printf("Encerrando server...\n");
  for (user_iterator=0; user_iterator<MAX_CONEXOES; user_iterator++)
    remover_conexao(user_iterator);
  
  pthread_join(esperar_conexoes, NULL);
  player->stop();
  tela->stop();

  return 0;
}
