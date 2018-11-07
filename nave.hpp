#ifndef NAVE_HPP
#define NAVE_HPP
#include <string>
#include <vector>

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
class ListaDeNaves {
  private:
    std::vector<Nave*> *naves;
  public:
    ListaDeNaves();
    void hard_copy(ListaDeNaves *ldn);
    void add_nave(Nave *n);
    std::vector<Nave*> *get_naves();
};

#endif
