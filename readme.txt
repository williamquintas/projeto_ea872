// William Quintas de Melo - RA 188684
// Palmira Sara Aranovich Florentino - RA 185306

Compilar:
-Para compilar, escreva make remove, depois make, e rode primeiro o ./model (o server) em um terminal e o ./client em outro terminal
-O teclado será reconhecido no segundo terminal, que roda o client, e será exibido no primeiro terminal (o server).

Jogo:
-t para atirar;
-w para andar para cima;
-s para andar para baixo;
-q para sair do jogo.

Modificações:
model_mainloop.cpp: 
-inserção dos códigos para abrir e fechar o server com ajuda da biblioteca socket.h (linhas 30 a 45, 110 a 123 e 152);
-na linha 113, será recebido do client a variável "input_teclado", que será usada para controlar o jogo.
client.cpp:
-inserção do código para o client se conectar ao servidos (linhas 24 e 25, 56 a 70 e 77);
-inserção da inicialização da Tela do ncurses para fazer funcionar a espera de teclado (linhas 27 a 50, 78 e 79);
-o caracter do teclado será identificado na linha 55 com teclado->getchar, e será mandado ao server com o send da linha 66.
