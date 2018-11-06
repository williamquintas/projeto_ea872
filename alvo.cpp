#include "alvo.hpp"

//Alvo
Alvo::Alvo(float posicao_x, float posicao_y) {
  this->posicao_x = posicao_x;
  this->posicao_y = posicao_y;
}

float Alvo::get_posicao_x() {
  return this->posicao_x;
}

float Alvo::get_posicao_y() {
  return this->posicao_y;
}

void Alvo::update(float nova_posicao_x, float nova_posicao_y) {
  this->posicao_x = nova_posicao_x;
  this->posicao_y = nova_posicao_y;
}