#ifndef TIRO_HPP
#define TIRO_HPP
#include <string>
#include <vector>

class Tiro {
  private:
    float posicao_x;
    float posicao_y;
    float velocidade;
    bool existe; //0 se nao existe e 1 se existe o tiro
    int quem_atirou;
  public:
    Tiro(float posicao_x, float posicao_y, float velocidade, bool existe, int quem_atirou);
    Tiro(std::string buffer_in);
    void serialize(std::string &buffer_out);
    void unserialize(std::string buffer_in);
    float get_posicao_x();
    float get_posicao_y();
    float get_velocidade();
    int get_quem_atirou();
    bool get_existe();
    void update_existe(bool e);
    void update_quem_atirou(int q);
    void update(float nova_posicao_x, float nova_posicao_y, float nova_velocidade);
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

#endif
