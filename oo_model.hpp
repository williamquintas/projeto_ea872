#ifndef OO_MODEL_HPP
#define OO_MODEL_HPP

#include <thread>
#include "portaudio.h"

namespace Audio {
  class Sample {
    private:
      std::vector<float> data;
      unsigned int position;
    public:
      Sample();
      ~Sample();
      void load(const char *filename);
      std::vector<float> get_data();
      unsigned int get_position();
      void set_position(unsigned int pos);
      bool finished();
  };
  class Player {
    private:
      Sample *audio_sample;
      bool playing;
      PaStreamParameters  outputParameters;
      PaStream*           stream;
      PaError             err;
      PaTime              streamOpened;
    public:
      Player();
      ~Player();
      void init();
      void pause();
      void stop();
      void play(Sample *audiosample);
      Sample *get_data();
  };
}

class Nave {
  private:
    float posicao;
  public:
    Nave(float posicao);
    Nave(std::string buffer_in);
    void serialize(std::string &buffer_out);
    void unserialize(std::string buffer_in);
    float get_posicao();
    void update(float nova_posicao);
};
class Alvo {
  private:
    float posicao_x;
    float posicao_y;
  public:
    Alvo(float posicao_x, float posicao_y);
    Alvo(std::string buffer_in);
    void serialize(std::string &buffer_out);
    void unserialize(std::string buffer_in);
    float get_posicao_x();
    float get_posicao_y();
    void update(float nova_posicao_x, float nova_posicao_y);
};
class Tiro {
  private:
    float posicao_x;
    float posicao_y;
    float velocidade;
    bool existe; //0 se nao existe e 1 se existe o tiro
  public:
    Tiro(float posicao_x, float posicao_y, float velocidade, bool existe);
    Tiro(std::string buffer_in);
    void serialize(std::string &buffer_out);
    void unserialize(std::string buffer_in);
    float get_posicao_x();
    float get_posicao_y();
    float get_velocidade();
    bool get_existe();
    void update_existe(bool e);
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
    Alvo *alvo;
    ListaDeNaves *lista_nave;
    ListaDeTiros *lista_tiro;
    Audio::Player *player;
    Audio::Sample *asample;
  public:
    Fisica(Alvo *alvo, ListaDeNaves *ldn, ListaDeTiros *ldt);
    void add_nave(Nave *n);
    void add_tiro(Tiro *t);
    void andar_nave(int deslocamento);
    void disparar_tiro(int i_tiro);
    void update_tiro(float deltaT);
    void destruir_tiro(int i_tiro);
};
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

enum TypeOfData { NAVE, LISTA_DE_NAVES, ALVO, TIRO, LISTA_DE_TIROS };
struct DataContainer {
  TypeOfData t;
  float posicao;
  float posicao_x;
  float posicao_y;
  float velocidade;
  bool existe; //0 se nao existe e 1 se existe o tiro
};
#endif
