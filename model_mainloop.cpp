#include <iostream>
#include <chrono>
#include <thread>
#include <vector>

#include "oo_model.hpp"

using namespace std::chrono;
uint64_t get_now_ms() {
  return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}

int main () {
  Tiro *tiro1 = new Tiro(4, 10, 6);
  Tiro *tiro2 = new Tiro(4, 12, 7);
  Tiro *tiro3 = new Tiro(4, 14, 8);

  ListaDeTiros *t_lista = new ListaDeTiros();
  t_lista->add_tiro(tiro1);
  t_lista->add_tiro(tiro2);
  t_lista->add_tiro(tiro3);

  Nave *nave1 = new Nave(1);
  ListaDeNaves *n_lista = new ListaDeNaves();
  n_lista->add_nave(nave1);


  Fisica *f = new Fisica(n_lista, t_lista);


  Tela *tela = new Tela(n_lista, t_lista, 20, 20, 20, 20);
  tela->init();
  tela->draw();

  Teclado *teclado = new Teclado();
  teclado->init();

  uint64_t t0;
  uint64_t t1;
  uint64_t deltaT;
  uint64_t T;

  int i = 0;

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

    // Condicao de parada
    if ( (t1-T) > 10000 ) break;

    std::this_thread::sleep_for (std::chrono::milliseconds(100));
    i++;
  }
  tela->stop();
  teclado->stop();
  return 0;
}
