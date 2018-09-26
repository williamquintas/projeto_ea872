#include <iostream>
#include <chrono>
#include <thread>
#include <vector>

#include "oo_model.hpp"

#include <stdlib.h>
#include <time.h>       /* time */

#define MAX_TIROS 51
#define ALTURA_TELA 20
#define LARGURA_TELA 40

using namespace std::chrono;
uint64_t get_now_ms() {
  return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}

int main () {

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

  Fisica *f = new Fisica(n_lista, t_lista);


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

  while (1) {
    // Atualiza timers
    t0 = t1;
    t1 = get_now_ms();
    deltaT = t1-t0;

    // Atualiza modelo
    f->update_tiro(deltaT);

    // Atualiza tela
    tela->update();

    // Lê o teclado
    char c = teclado->getchar();
    if (c=='q') {
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
        break;
      }
      f->disparar_tiro(n_tiro);
      n_tiro++;
    }

    std::this_thread::sleep_for (std::chrono::milliseconds(100));
  }
  tela->stop();
  teclado->stop();
  return 0;
}
