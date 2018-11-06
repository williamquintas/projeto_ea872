#include "nave.hpp"

//Nave
Nave::Nave(float posicao) {
  this->posicao = posicao;
}

Nave::Nave(std::string buffer_in) {
  this->unserialize(buffer_in);
}

void Nave::serialize(std::string &buffer_out) {
  std::memcpy((void*)buffer_out.c_str(), &(this->posicao), sizeof(float));
}

void Nave::unserialize(std::string buffer_in) {
  std::memcpy(&(this->posicao), (void*)buffer_in.c_str(), sizeof(float));
}

void Nave::update(float nova_posicao) {
  this->posicao = nova_posicao;
}

float Nave::get_posicao() {
  return this->posicao;
}

//Lista de Naves
ListaDeNaves::ListaDeNaves() {
  this->naves = new std::vector<Nave *>(0);
}

void ListaDeNaves::hard_copy(ListaDeNaves *ldn) {
  std::vector<Nave *> *naves = ldn->get_naves();
  for (int k=0; k<naves->size(); k++) {
    Nave *n = new Nave( (*naves)[k]->get_posicao());
    this->add_nave(n);
  }
}

void ListaDeNaves::add_nave(Nave *n) {
  (this->naves)->push_back(n);
}

std::vector<Nave*> *ListaDeNaves::get_naves() {
  return (this->naves);
}
