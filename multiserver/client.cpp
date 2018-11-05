
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "teclado.hpp"

int socket_fd;

void *receber_respostas(void *parametros) {
  /* Recebendo resposta */
  char reply[60];
  int msg_len;
  int msg_num;
  msg_num = 0;
  while(1) {
  msg_len = recv(socket_fd, reply, 50, MSG_DONTWAIT);
  if (msg_len > 0) {
    printf("[%d][%d] RECEBI:\n%s\n", msg_num, msg_len, reply);
    msg_num++;
  }
  }
}

int main() {
  Teclado *teclado = new Teclado();
  teclado->init();

  struct sockaddr_in target;
  pthread_t receiver;

  socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  printf("Socket criado\n");

  target.sin_family = AF_INET;
  target.sin_port = htons(3001);
  inet_aton("127.0.0.1", &(target.sin_addr));
  printf("Tentando conectar\n");
  if (connect(socket_fd, (struct sockaddr*)&target, sizeof(target)) != 0) {
    printf("Problemas na conexao\n");
    return 0;
  }
  printf("Conectei ao servidor\n");

  pthread_create(&receiver, NULL, receber_respostas, NULL);

  while(1) {
    char c = teclado->getchar();
    std::this_thread::sleep_for (std::chrono::milliseconds(10));
    printf("%c", c);
    if (c == 's') {
      send(socket_fd, &c, 1, 0);
      printf("Escrevi mensagem de %c!\n", c);
    }
    if (c=='q') {
      break;
    }
  }

  teclado->stop();
  return 0;
}

