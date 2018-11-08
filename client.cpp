
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

int socket_fd;

//Variaveis de send e recv
float pos_nave0;
float alvo_x;
float alvo_y;
float tiro1_x;
float tiro1_y;
float tiro1_vel;
float deltaT_global;
int pontos;
int total_tiros;


void *receber_respostas(void *parametros) {
  /* Recebendo resposta */
  char reply[50];
  int msg_len;
  int msg_num;
  msg_num = 0;

  //Variaveis de input
  char in_flag[100];
  char input_buffer[50];

  while(1) {
    //A flag de qual informacao o buffer contem encontra-se na posicao 45

    //Nave 0
    msg_len = recv(socket_fd, input_buffer, 50, MSG_DONTWAIT);
    if(msg_len>0 && input_buffer[45] == '0'){
      input_buffer[45] = ' ';
      sscanf(input_buffer,"%f", &pos_nave0);
    }

    //Alvo x e y
    if(msg_len>0 && input_buffer[45] == '1'){
      input_buffer[45] = ' ';
      sscanf(input_buffer,"%f", &alvo_x);
    }
    if(msg_len>0 && input_buffer[45] == '2'){
      input_buffer[45] = ' ';
      sscanf(input_buffer,"%f", &alvo_y);
    }

   //deltaT, pontos, total de tiros
    if(msg_len>0 && input_buffer[45] == '3'){
      input_buffer[45] = ' ';
      sscanf(input_buffer,"%f", &deltaT_global);
    }
    if(msg_len>0 && input_buffer[45] == '4'){
      input_buffer[45] = ' ';
      sscanf(input_buffer,"%d", &pontos);
    }
    if(msg_len>0 && input_buffer[45] == '5'){
      input_buffer[45] = ' ';
      sscanf(input_buffer,"%d", &total_tiros);
    }

    //Tiro da nave 0
   // if(msg_len>0 && input_buffer[45] == '6'){
   //  input_buffer[45] = ' ';
   //   sscanf(input_buffer,"%f", &tiro1_x);
   // }
   // if(msg_len>0 && input_buffer[45] == '7'){
   //   input_buffer[45] = ' ';
   //   sscanf(input_buffer,"%f", &tiro1_y);
   // }
   // if(msg_len>0 && input_buffer[45] == '8'){
   //   input_buffer[45] = ' ';
    //  sscanf(input_buffer,"%f", &tiro1_vel);
   // }
 
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

  //Colocando o vetor de naves em uma variavel auxiliar
  std::vector<Nave *> *n = n_lista->get_naves();

  float deltaT = deltaT_global;

  while(1) {
    // Atualiza modelo
    f->update_tiro(deltaT, &pontos);
    // Atualiza tela
    tela->update(&total_tiros, &pontos);

    //Atualizando os parametros que chegaram pelo recv
    (*n)[0]->update(pos_nave0);
    alvo->update(alvo_x, alvo_y);

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

