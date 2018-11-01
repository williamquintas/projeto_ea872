#include <iostream>
#include <chrono>
#include <thread>
#include <vector>

#include <pthread.h>

#include <string.h>
#include "oo_model.hpp"
#include <ncurses.h>
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

#define MAX_CONEXOES 5
#ifdef __MACH__
#define MSG_NOSIGNAL SO_NOSIGPIPE 
#endif

int running = 1; //varivel global que controla a thread do accept

struct sockaddr_in myself, client;
socklen_t client_size;
int socket_fd;
int connection_fd[MAX_CONEXOES];
int conexao_usada[MAX_CONEXOES];

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

void *wait_connections(void *parameters) {
  int conn_fd;
  int user_id;
  while(running) {
    conn_fd = accept(socket_fd, (struct sockaddr*)&client, &client_size);
    user_id = adicionar_conexao(conn_fd);
    if (user_id != -1) {
     // printw("Novo usuario chegou! ID=%d\n", user_id);
    } else {
     // printw("Maximo de usuarios atingido!\n");
    }
  }
  return NULL;
}

int remover_conexao(int user) {
  if (conexao_usada[user]==1) {
  conexao_usada[user] = 0;
  close(connection_fd[user]);
  }
  return 1;
}

using namespace std::chrono;
uint64_t get_now_ms() {
  return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}

int main () {
  char input_teclado;
  pthread_t esperar_conexoes;
  int msglen;
  int user_iterator;
  char output_buffer[100];
  char input_buffer[100];

  /* Inicializando variaveis */
  client_size = (socklen_t)sizeof(client);
  for (int i=0; i<MAX_CONEXOES; i++) {
    conexao_usada[i] = 0;
  }
  running = 1;

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
//  printw("Socket criado\n");
  myself.sin_family = AF_INET;
  myself.sin_port = htons(3001);
  inet_aton("127.0.0.1", &(myself.sin_addr));
 // printw("Tentando abrir porta 3001\n");
  if (bind(socket_fd, (struct sockaddr*)&myself, sizeof(myself)) != 0) {
   // printw("Problemas ao abrir porta\n");
    return 0;
  }
 // printw("Abri porta 3001!\n");
  listen(socket_fd, 2);
  //printw("Estou ouvindo na porta 3001!\n");


  /* Dispara thread para ouvir conexoes */
  pthread_create(&esperar_conexoes, NULL, wait_connections, NULL);
            int num_vezes = 0;
  while (running) {

    for (user_iterator=0; user_iterator<MAX_CONEXOES; user_iterator++){
      // Atualiza timers (nao sei se aqui ou entre o for e o while)
      t0 = t1;
      t1 = get_now_ms();
      deltaT = t1-t0;

      // Atualiza modelo
      f->update_tiro(deltaT);

      // Atualiza tela
      tela->update();

      //Recebe a resposta
      msglen = recv(connection_fd[user_iterator], &input_teclado, 1, MSG_DONTWAIT);
      if (msglen > 0) {
            //printw("Recebi mensagem de %d\n", user_iterator);
            num_vezes++;
            if(input_teclado=='q'||input_teclado=='w'||input_teclado=='t'||input_teclado=='s'){
            //  printw("=====num_vezes: %d=====\n", num_vezes);
             // printw("Escrevi mensagem de %c!\n",input_teclado);

              char c = input_teclado;

              // Lê o teclado
              if (c=='q') {
                running = 0;
                break;
              }
              if (c=='s') {
                f->andar_nave(1);
              }
              if (c=='w') {
                f->andar_nave(-1);
              }
              if (c=='t') {
                if (n_tiro+1 == MAX_TIROS){
                  running = 0;
                  break;
                }
                asample->set_position(0);
                player->play(asample);
                f->disparar_tiro(n_tiro);
                n_tiro++;
              }


            }
            if (input_teclado=='q'){
              running=0;
            }
            // std::string buffer(sizeof(Nave), ' ');
            // nave1->serialize(buffer);
            // Nave *nave2 = new Nave(buffer);
            // std::cout << nave2->get_posicao() << '\n';
            // strcpy(output_buffer, buffer.c_str());
            const char *s = "5";
            strcpy(output_buffer, s);
            // sprintf(output_buffer, "USER %d: %c\n", user_iterator, input_teclado);
            //printw("%s\n", output_buffer);
            for (int ret=0; ret<MAX_CONEXOES; ret++) {
              if (conexao_usada[ret] == 1) {
               // printw("Avisando user %d\n", ret);
                if (send(connection_fd[ret], output_buffer, 100, MSG_NOSIGNAL) == -1) {
                 /* Usuario desconectou!?? */
                 // printw("Usuario %d desconectou!\n", ret);
                  remover_conexao(ret);
                }
              }
            }
      }  

//estava aqui o sleep
    }
    std::this_thread::sleep_for (std::chrono::milliseconds(100));//estava 100 antes
  }

  for (user_iterator=0; user_iterator<MAX_CONEXOES; user_iterator++){
    remover_conexao(user_iterator);
  }

  player->stop();
  tela->stop();
  pthread_join(esperar_conexoes, NULL);
  return 0;
}
