#ifndef OO_MODEL_HPP
#define OO_MODEL_HPP

#include <thread>

class Nave {
  private:
  float posicao;

  public:
  Nave(float posicao);
  float get_posicao();
  void update(float nova_posicao);
};

class Tiro {
  private:
  float posicao_x;
  float posicao_y;
  float velocidade;
  int existe; //0 se nao existe e 1 se existe o tiro

  public:
  Tiro(float posicao_x, float posicao_y, float velocidade, int existe);
  float get_posicao_x();
  float get_posicao_y();
  float get_velocidade();
  int get_existe();
  void update_existe(int e);
  void update(float nova_posicao_x, float nova_posicao_y, float nova_velocidade);
};

class ListaDeNaves {
 private:
    std::vector<Nave*> *naves;

  public:
    ListaDeNaves();
    void hard_copy(ListaDeNaves *ldn);
    void add_nave(Nave *n);
    std::vector<Nave*> *get_naves();
};

class ListaDeTiros {
 private:
    std::vector<Tiro*> *tiros;

  public:
    ListaDeTiros();
    void hard_copy(ListaDeTiros *ldt);
    void add_tiro(Tiro *t);
    std::vector<Tiro*> *get_tiros();
};

class Fisica {
  private:
    ListaDeNaves *lista_nave;
    ListaDeTiros *lista_tiro;

  public:
    Fisica(ListaDeNaves *ldn, ListaDeTiros *ldt);
    void add_nave(Nave *n);
    void add_tiro(Tiro *t);
    void andar_nave(int deslocamento);
    void disparar_tiro(int i_tiro);
    void update_tiro(float deltaT);
    void destruir_tiro(int i_tiro);
};

class Tela {
  private:
    ListaDeNaves *lista_nave, *lista_anterior_nave;
    ListaDeTiros *lista_tiro, *lista_anterior_tiro;
    int maxI, maxJ;
    float maxX, maxY;

  public:
    Tela(ListaDeNaves *ldn, ListaDeTiros *ldt, int maxI, int maxJ, float maxX, float maxY);
    ~Tela();
    void stop();
    void init();
    void draw();
    void update();
};

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
