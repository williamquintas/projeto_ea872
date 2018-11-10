#ifndef TELA_HPP
#define TELA_HPP
#include "alvo.hpp"
#include "nave.hpp"
#include "tiro.hpp"

#define VELOCIDADE_TIRO 10
#define ACELERACAO_TIRO 0
#define ALTURA_TELA 20
#define LARGURA_TELA 40
#define POSICAO_X_NAVE_0 3
#define POSICAO_X_NAVE_1 17

class Tela {
  private:
    Alvo *alvo, *alvo_antigo;
    ListaDeNaves *lista_nave, *lista_anterior_nave;
    ListaDeTiros *lista_tiro, *lista_anterior_tiro;
    int maxI, maxJ;
    float maxX, maxY;
  public:
    Tela(Alvo *alvo, ListaDeNaves *ldn, ListaDeTiros *ldt, int maxI, int maxJ, float maxX, float maxY);
    ~Tela();
    void stop();
    void init();
    void draw();
    void update(int *total_tiros, int *pontos);
};

#endif
