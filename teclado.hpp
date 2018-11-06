#ifndef TECLADO_HPP
#define TECLADO_HPP
#include <thread>

void threadfun (char *keybuffer, int *control);
class Teclado {
  private:
    char ultima_captura;
    int rodando;
    std::thread kb_thread;
  public:
    Teclado();
    ~Teclado();
    void stop();
    void init();
    char getchar();
};
#endif