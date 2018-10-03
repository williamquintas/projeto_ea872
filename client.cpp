#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "oo_model.hpp"

int main() {
    int socket_fd;
    struct sockaddr_in target;

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


    Teclado *teclado = new Teclado();
    teclado->init();

    char c = 0;
    
    while(c != 'q'){
        /* Agora, meu socket funciona como um descritor de arquivo usual */
        c = teclado->getchar();
        send(socket_fd, &c, 1, 0);
        printf("Escrevi mensagem de %c!\n", c);
        sleep(1);
        /* Recebendo resposta */
        char reply[10];
        recv(socket_fd, reply, 10, 0);
        printf("Resposta:\n%s\n", reply);
    }
    close(socket_fd);
    teclado->stop();
    return 0;
}

