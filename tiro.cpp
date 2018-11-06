#include "tiro.hpp"
#include <vector> 

//Tiro
Tiro::Tiro(float posicao_x, float posicao_y, float velocidade, bool existe) {
  this->posicao_x = posicao_x;
  this->posicao_y = posicao_y;
  this->velocidade = velocidade;
  this->existe = existe;
}

float Tiro::get_posicao_x() {
  return this->posicao_x;
}

float Tiro::get_posicao_y() {
  return this->posicao_y;
}

float Tiro::get_velocidade() {
  return this->velocidade;
}

bool Tiro::get_existe() {
  return this->existe;
}

void Tiro::update_existe(bool e) {
  this->existe = e;
}

void Tiro::update(float nova_posicao_x, float nova_posicao_y, float nova_velocidade) {
  this->posicao_x = nova_posicao_x;
  this->posicao_y = nova_posicao_y;
  this->velocidade = nova_velocidade;
}

//Lista de Tiros
ListaDeTiros::ListaDeTiros() {
  this->tiros = new std::vector<Tiro *>(0);
}

void ListaDeTiros::hard_copy(ListaDeTiros *ldt) {
  std::vector<Tiro *> *tiros = ldt->get_tiros();
  for (int k=0; k < tiros->size(); k++) {
    Tiro *t = new Tiro( (*tiros)[k]->get_posicao_x(), 
                        (*tiros)[k]->get_posicao_y(), 
                        (*tiros)[k]->get_velocidade(), 
                        (*tiros)[k]->get_existe()     );
    this->add_tiro(t);
  }
}

void ListaDeTiros::add_tiro(Tiro *t) {
  (this->tiros)->push_back(t);
}

std::vector<Tiro*> *ListaDeTiros::get_tiros() {
  return (this->tiros);
}
