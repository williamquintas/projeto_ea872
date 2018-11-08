
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "alvo.hpp"
#include "fisica.hpp"
#include "nave.hpp"
#include "teclado.hpp"
#include "tela.hpp"
#include "tiro.hpp"
#include <ncurses.h>

#define MAX_TIROS 101
#define ALTURA_TELA 20
#define LARGURA_TELA 40

int total_tiros = 0;
int pontos = 0;

int socket_fd;
float pos_nave0;


void *receber_respostas(void *parametros) {
  /* Recebendo resposta */
  char reply[50];
  char in_buffer_nave0[50];
  int msg_len;
  int msg_num;
  msg_num = 0;
  while(1) {
    //msg_len = recv(socket_fd, reply, 50, MSG_DONTWAIT);
   // if(msg_len>0){
      //printf("[01]: %s\n", reply);
    //}
    msg_len = recv(socket_fd, in_buffer_nave0, 50, MSG_DONTWAIT); //a segunda msg recebida sera a posicao da primeira nave do vetor
    if(msg_len>0){
      //printf("[02]: %s\n", in_buffer_nave0);
      sscanf(in_buffer_nave0,"%f", &pos_nave0);
      //printf("pos_nave0: %f\n", pos_nave0);
    }
    //if (msg_len > 0) {
      //printf("[%d][%d] RECEBI:\n%s\n", msg_num, msg_len, nave);
     // msg_num++;
    //}
  }
}

int main() {
  struct sockaddr_in target;
  pthread_t receiver;

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

  //Criando a fisica e a tela
  Fisica *f = new Fisica(alvo, n_lista, t_lista);
  Tela *tela = new Tela(alvo, n_lista, t_lista, 20, 20, 20, 20);
  tela->init();
  tela->draw();

  //Criando o teclado
  Teclado *teclado = new Teclado();
  teclado->init();

  //Inicializando o audio
  Audio::Sample *asample;
  asample = new Audio::Sample();
  asample->load("assets/gun.dat");
  Audio::Player *player;
  player = new Audio::Player();
  player->init();

  socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  // printf("Socket criado\n");

  target.sin_family = AF_INET;
  target.sin_port = htons(3001);
  inet_aton("127.0.0.1", &(target.sin_addr));
  // printf("Tentando conectar\n");
  if (connect(socket_fd, (struct sockaddr*)&target, sizeof(target)) != 0) {
    // printf("Problemas na conexao\n");
    return 0;
  }
  // printf("Conectei ao servidor\n");

  pthread_create(&receiver, NULL, receber_respostas, NULL);

  //Tentando atualizar o vetor de naves
  std::vector<Nave *> *n = n_lista->get_naves();

  while(1) {
    // Atualiza modelo
    //f->update_tiro(0, NULL);
    // Atualiza tela
    tela->update(&total_tiros, &pontos);
    (*n)[0]->update(pos_nave0);

    char c = teclado->getchar();
    std::this_thread::sleep_for (std::chrono::milliseconds(10));
    if(c=='q'||c=='w'||c=='t'||c=='s'){
      send(socket_fd, &c, 1, 0);
      //printw("Escrevi mensagem de %c!\n",c);
      if (c=='q') {
        break;
      }
    }
    std::this_thread::sleep_for (std::chrono::milliseconds(10));
  }

  teclado->stop();
  tela->stop();
  return 0;
}

