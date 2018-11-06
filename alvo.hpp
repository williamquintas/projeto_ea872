#ifndef ALVO_HPP
#define ALVO_HPP
#include <string>

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

#endif